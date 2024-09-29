#include <luagui.hpp>

int main() {
    setUTF8();

    LuaGUI luagui;
    luagui.run("assets/scripts/breakout.lua");
}
