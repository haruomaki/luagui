#include <GUI/GUI.hpp>
#include <GUI/Text.hpp>
#include <GUI/Window.hpp>
#include <GUI/utility.hpp>
#include <SumiGL/Context.hpp>
#include <filesystem>
#include <luagui.hpp>

#include "Box2D.hpp"
#include "Camera.hpp"
#include "Text.hpp"
#include "Window.hpp"
#include "World.hpp"
#include "WorldObject.hpp"
#include "mesh.hpp"
#include "sound.hpp"

namespace fs = std::filesystem;

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
    auto &default_font = gui.resources.append<Font>(std::move(text_shader), font).get();
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
static fs::path convert_module_to_path(const std::string &module_name) {
    std::string path_str = module_name;
    std::replace(path_str.begin(), path_str.end(), '.', '/');
    path_str += ".lua";
    return path_str;
}

static void add_custom_searcher(sol::state &lua, const lunchbox::Storage &storage) {
    sol::table searchers = lua["package"]["searchers"];
    searchers.add([&](const std::string &module_name) -> sol::object {
        // モジュール名をスクリプトファイル名に変換
        std::string cwd = lua["_CWD"];
        auto module_path = convert_module_to_path(module_name);
        std::string file_path = cwd / module_path;

        try {
            // スクリプトのテキストを取得し、ロードする。
            std::string script_content = storage.get_text(file_path);
            auto module = lua.load(script_content);
            // ロードに成功すればモジュールを返す。
            return module;
        } catch (const std::exception & /*e*/) {
            // ロードに失敗すればエラーメッセージを返す。
            return sol::make_object(lua, "no asset '" + file_path + "'");
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

    // コルーチンまわりの関数を読み込み
    // lua.script(storage.get_text("assets/scripts/coroutines.lua"));

    // その他ユーティリティ関数などを読み込み
    lua.script(storage.get_text("scripts/misc.lua"));

    // アセット内からモジュールを検索する機能を追加
    add_custom_searcher(lua, storage);

    lua.set_function("run_window", [this](int width, int height, const std::string &title, sol::function func) {
        run_window(this->lua, width, height, title, std::move(func));
    });

    register_chrono(lua);
    register_window(lua);
    register_world_object(lua);
    register_world(lua);
    register_camera(lua);
    register_box2d(lua);
    register_text(lua);
    register_sound(lua);
    register_mesh(lua);
}

LuaGUI::~LuaGUI() {
    // ワールドの各物体はluaステートに依存している場合があるため、ワールドが先に消えることを保証する。
    // NOTE: GUIのデストラクタで二重クリーンになってしまっていることには注意。瑕疵なく動くので黙殺。
    cleanup();
}

void LuaGUI::run(const std::string &file_path) {
    print("LuaGUIのrun開始");
    fs::path path(file_path);
    lua["_CWD"] = path.parent_path().c_str();
    lua.script(storage.get_text(file_path));
    print("LuaGUIのrun終了");
}
