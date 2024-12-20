#pragma once

#include <sol/sol.hpp>
#include <sumigl.hpp>

class LuaGUI {

  public:
    GL::Context gui;
    sol::state lua;

    LuaGUI();

    void run(const std::string &filename) {
        print("LuaGUIのrun開始");
        lua.script_file(filename);
        print("LuaGUIのrun終了");
    }
};
