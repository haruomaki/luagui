#include <GUI.hpp>
#include <luagui.hpp>

int testfunc() {
    sol::state lua;
    int x = 0;
    lua.set_function("beep", [&x] { x += 234; });
    lua.script("beep()");
    debug(x);

    GUI gui;
    gui.create_window(200, 200, "ぴえん");
    gui.mainloop();

    return 7;
}

static void create_window(sol::state &lua, int width, int height, const std::string &title, sol::function func) {
    std::cout << "Creating window: " << title << " (" << width << "x" << height << ")" << std::endl;

    sol::thread runner_thread = sol::thread::create(lua);
    sol::state_view runner_thread_state = runner_thread.state();
    sol::coroutine co(runner_thread_state, func);

    // C++側でウィンドウを作成し、Luaのグローバル変数に保持する
    print("ウィンドウ作成開始");
    GUI gui;
    Window &window = gui.create_window(width, height, title);
    print("ウィンドウ作成完了");
    lua["window"] = &window;

    debug(gui.dpi());

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
                std::cout << "Game loop finished" << std::endl;
                coroutine_finished = true;
                window.close();
            } else {
                // エラーが発生した
                sol::error err = result;
                std::cerr << "Error in Lua coroutine: " << err.what() << std::endl;
                coroutine_finished = true;
                window.close();
            }
        }
    });

    std::cout << "Window closed: " << title << std::endl;
}

LuaGUI::LuaGUI() {
    lua.open_libraries(sol::lib::base, sol::lib::coroutine);

    lua.set_function("create_window", [this](int width, int height, const std::string &title, sol::function func) {
        create_window(this->lua, width, height, title, std::move(func));
    });
}
