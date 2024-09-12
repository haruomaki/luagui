// #include <lua_load.hpp>
#include <luagui.hpp>

int main() {
    LuaGUI luagui;
    luagui.run("assets/scripts/luatest2.lua");

    // testfunc();

    // run_lua("assets/scripts/luatest.lua");
}

// #include <sol/sol.hpp>

// #include <iostream>

// int main() {
//     std::cout << "=== coroutine ===" << std::endl;

//     sol::state lua;

//     lua.open_libraries(sol::lib::base, sol::lib::coroutine);

//     lua.set_function("start_task",
//                      [&lua](
//                          sol::function f) {
//                          // You must ALWAYS get the current state
//                          sol::thread runner_thread = sol::thread::create(lua);
//                          sol::state_view runner_thread_state = runner_thread.state();
//                          sol::coroutine f_on_runner_thread(runner_thread_state, f);

//                          f_on_runner_thread();
//                          f_on_runner_thread();
//                      });

//     lua.script(
//         R"(
// function main()
// 	print("りんご")
// 	coroutine.yield(20)
// 	print("みかん")
// end

// 	start_task(main)
// )");

//     return 0;
// }
