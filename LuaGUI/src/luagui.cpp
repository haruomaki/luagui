#include <GUI/GUI.hpp>
#include <GUI/Text.hpp>
#include <GUI/Window.hpp>
#include <GUI/utility.hpp>
#include <SumiGL/Context.hpp>
#include <luagui.hpp>

#include "Box2D.hpp"
#include "BulletPhysics.hpp"
#include "Camera.hpp"
#include "Text.hpp"
#include "Window.hpp"
#include "World.hpp"
#include "WorldObject.hpp"
#include "mesh.hpp"
#include "sound.hpp"
#include "vec.hpp"

// 時刻関連の関数を登録
static void register_chrono(sol::state &lua) {
    lua.set_function("get_time", []() -> double {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
        return seconds;
    });
}

static void run_window(sol::state &lua, int width, int height, const std::string &title, sol::function func) {
    std::cout << "Creating window: " << title << " (" << width << "x" << height << ")" << '\n';

    sol::thread runner_thread = sol::thread::create(lua);
    sol::state_view runner_thread_state = runner_thread.state();
    sol::coroutine co(runner_thread_state, func);

    // C++側でウィンドウを作成し、Luaのグローバル変数に保持する
    print("ウィンドウ作成開始");
    GUI &gui = lua["__GUI"];
    Window window(gui, width, height, title);
    print("ウィンドウ作成完了");
    lua["__CurrentWindow"] = &window;

    debug(gui.dpi());

    // デフォルトのフォントを生成
    lunchbox::Storage &storage = lua["__Storage"];
    auto text_shader = GL::ProgramObject{
        GL::create_shader(GL_VERTEX_SHADER, storage.get_text("shaders/font.vsh")),
        GL::create_shader(GL_FRAGMENT_SHADER, storage.get_text("shaders/font.fsh"))};
    auto font = storage.get_font("fonts/main.ttf");
    auto &default_font = gui.resources.append<Font>(std::move(text_shader), std::move(font)).get();
    lua["__CurrentFont"] = &default_font;

    bool coroutine_finished = false;

    // フレームごとにコルーチンを進める。コルーチンが最後まで到達したら速やかにウィンドウを閉じる。
    // ウィンドウ一つごとに一つのメインループ。
    gui.mainloop([&co, &coroutine_finished, &window]() {
        if (!coroutine_finished) {
            auto result = co();
            auto status = result.status();

            if (status == sol::call_status::yielded) {
                // コルーチンが yield した
            } else if (status == sol::call_status::ok) {
                // コルーチンが終了した
                std::cout << "Game loop finished" << '\n';
                coroutine_finished = true;
                window.close();
            } else {
                // エラーが発生した
                sol::error err = result;
                std::cerr << "Error in Lua coroutine: " << err.what() << '\n';
                coroutine_finished = true;
                window.close();
            }
        }
    });

    std::cout << "Window closed: " << title << '\n';
}

// "modules.hoge"といったモジュール名を「modules/hoge.lua」の形に変換する。
static Path convert_module_to_path(const std::optional<Path> &cwd, const std::string &module_name) {
    std::string path_str = cwd.value_or("").string() + "/" + module_name;
    std::replace(path_str.begin(), path_str.end(), '.', '/');
    path_str += ".lua";
    return path_str;
}

// assetsからluaスクリプトを取得し、冒頭に「自身のファイルパス」を保持するコードを付加する。
static std::string get_code_injected(const lunchbox::Storage &storage, const Path &file_path) {
    auto code = storage.get_text(file_path);
    auto cwd = file_path.parent_path().string();

    // エラー行通知の際にズレないように、すべてを1行に収める
    std::string header =
        "local __CWD__ = '" + cwd + "';" +
        "local __original_require = require;"
        "local function require(module)"
        "    __CWD_global__ = __CWD__"
        "    return __original_require(module)"
        "end;";

    return header + code;
}

// assetsディレクトリからの相対パスで検索できるようにする。
static void add_custom_searcher(sol::state &lua, const lunchbox::Storage &storage) {
    sol::table searchers = lua["package"]["searchers"];
    searchers.add([&](const std::string &module_name) -> sol::object {
        // requireを呼ぶ直前に、`__CWD_global__`という変数にパスを格納しておくと、そこを基準とした相対パスでアセット内を検索する。
        std::string cwd = lua["__CWD_global__"];
        // モジュール名をスクリプトアセットのフルパスに変換。
        auto module_path = convert_module_to_path(cwd, module_name);

        try {
            // スクリプトのテキストを取得し、ロードする。
            std::string script_content = get_code_injected(storage, module_path);
            auto module = lua.load(script_content, "@" + module_path.string());
            // ロードに成功すればモジュールを返す。
            return module;
        } catch (const std::exception & /*e*/) {
            // ロードに失敗すればエラーメッセージを返す。
            return sol::make_object(lua, "no asset '" + module_path.string() + "'");
        }
    });
}

LuaGUI::LuaGUI() {
    // print("LuaGUIのコンストラクタ");
    lua["__GUI"] = static_cast<GUI *>(this);
    lua["__Storage"] = &this->storage;

    lua.open_libraries(
        sol::lib::base,
        sol::lib::os,
        sol::lib::string,
        sol::lib::math,
        sol::lib::package,
        sol::lib::coroutine);

    // アセット内からモジュールを検索する機能を追加
    add_custom_searcher(lua, storage);

    lua.set_function("run_window", [this](int width, int height, const std::string &title, sol::function func) {
        run_window(this->lua, width, height, title, std::move(func));
    });

    register_vec(lua);
    register_chrono(lua);
    register_window(lua);
    register_world_object(lua);
    register_world(lua);
    register_camera(lua);
    register_box2d(lua);
    register_bullet_physics(lua);
    register_text(lua);
    register_sound(lua);
    register_mesh(lua);

    // scripts/modules以下にある全てのファイルは自動でロードされる。
    for (auto &file : storage.list("scripts/modules")) {
        auto full_path = "scripts/modules" / file;
        auto content = storage.get_text(full_path);
        lua.script(content, "@" + full_path.string());
    }
}

LuaGUI::~LuaGUI() {
    // ワールドの各物体はluaステートに依存している場合があるため、ワールドが先に消えることを保証する。
    // NOTE: GUIのデストラクタで二重クリーンになってしまっていることには注意。瑕疵なく動くので黙殺。
    cleanup();
}

void LuaGUI::run(const Path &file_path) {
    print("LuaGUIのrun開始");
    auto scr = get_code_injected(storage, file_path);
    lua.script(scr, "@" + file_path.string()); // @を付けるとsolがうまく表示してくれる
    print("LuaGUIのrun終了");
}
