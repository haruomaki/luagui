#include <luagui.hpp>

int main() {
    b2SetLengthUnitsPerMeter(0.02f);

    LuaGUI luagui;
    luagui.run("assets/scripts/luatest2.lua");
}
