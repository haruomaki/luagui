#include <luagui.hpp>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    setUTF8();

    fs::path currentPath = fs::current_path();
    std::cout << "Current Directory: " << currentPath << std::endl;

    LuaGUI luagui;
    luagui.run("assets/scripts/breakout.lua");
}
