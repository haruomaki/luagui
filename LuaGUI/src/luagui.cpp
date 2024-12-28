#include <GUI/GUI.hpp>
#include <GUI/Text.hpp>
#include <GUI/Window.hpp>
#include <GUI/utility.hpp>
#include <SumiGL/Context.hpp>
#include <luagui.hpp>

#include "Box2D.hpp"
#include "Camera.hpp"
#include "Text.hpp"
#include "Window.hpp"
#include "World.hpp"
#include "WorldObject.hpp"

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
    auto &default_font = gui.resources.append<Font>().get();
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

LuaGUI::LuaGUI() {
    // print("LuaGUIのコンストラクタ");
    lua["__GUI"] = static_cast<GUI *>(this);

    lua.open_libraries(
        sol::lib::base,
        sol::lib::os,
        sol::lib::string,
        sol::lib::math,
        sol::lib::package,
        sol::lib::coroutine);

    // requireで検索するパスを追加
    lua["package"]["path"] = lua["package"]["path"].get<std::string>() + ";./assets/scripts/?.lua";

    // コルーチンまわりの関数を読み込み
    lua.script_file("assets/scripts/coroutines.lua");

    // その他ユーティリティ関数などを読み込み
    lua.script_file("assets/scripts/misc.lua");

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
}

LuaGUI::~LuaGUI() {
    // ワールドの各物体はluaステートに依存している場合があるため、ワールドが先に消えることを保証する。
    // NOTE: GUIのデストラクタで二重クリーンになってしまっていることには注意。瑕疵なく動くので黙殺。
    cleanup();
}
