#pragma once

#include <GUI/GUI.hpp>
#include <lunchbox.hpp>
#include <sol/sol.hpp>

class LuaGUI : public GUI {

  public:
    sol::state lua;
    lunchbox::Storage storage;

    LuaGUI();
    ~LuaGUI();

    void run(const std::string &filename) {
        print("LuaGUIのrun開始");
        lua.script(storage.get_text(filename));
        print("LuaGUIのrun終了");
    }
};
