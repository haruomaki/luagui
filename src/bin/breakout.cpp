#include <LuaGUI/luagui.hpp>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    setUTF8();

    fs::path current_path = fs::current_path();
    std::cout << "Current Directory: " << current_path << std::endl;

    LuaGUI luagui;
    luagui.run("assets/scripts/breakout.lua");
}
