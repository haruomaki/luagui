// // #include <lua_load.hpp>
// #include <luagui.hpp>

// int main() {
//     LuaGUI luagui;
//     luagui.run("assets/scripts/luatest2.lua");

//     // testfunc();

//     // run_lua("assets/scripts/luatest.lua");
// }

#include <sol/sol.hpp>

int main() {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::coroutine);

    lua.set_function("run_coroutine", [&lua](sol::function f) {
        sol::thread runner_thread = sol::thread::create(lua);
        sol::state_view runner_thread_state = runner_thread.state();
        sol::coroutine co(runner_thread_state, f);

        co();
        co();
        co();
    });

    lua.script(R"(
        function story()
            print("1")
            coroutine.yield()
            print("2")
            coroutine.yield()
            print("3")
        end

        run_coroutine(story)
    )");
}
