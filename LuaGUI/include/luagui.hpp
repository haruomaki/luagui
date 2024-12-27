#pragma once

#include <GUI/GUI.hpp>
#include <sol/sol.hpp>

class LuaGUI : public GUI {

  public:
    sol::state lua;

    LuaGUI();

    void run(const std::string &filename) {
        print("LuaGUIのrun開始");
        lua.script_file(filename);
        print("LuaGUIのrun終了");
    }
};
