#include <filesystem>
#include <luagui.hpp>

#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

namespace fs = std::filesystem;

int main() {
    fs::path current_path = fs::current_path();
    std::cout << "Current Directory: " << current_path << '\n';

    LuaGUI luagui;
    luagui.run("assets/scripts/breakout.lua");
}
