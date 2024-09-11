#include <lua_load.hpp>
#include <luagui.hpp>

int main() {
    LuaGUI luagui;
    luagui.mainloop("assets/scripts/luatest2.lua");

    testfunc();

    run_lua("assets/scripts/luatest.lua");
}
