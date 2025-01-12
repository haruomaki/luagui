#pragma once

#include <GUI/GUI.hpp>
#include <lunchbox.hpp>
#include <sol/sol.hpp>

class LuaGUI : public GUI {

  public:
    sol::state lua;
    lunchbox::Storage storage;
    OpenAL::Context openal_ctx;

    LuaGUI();
    ~LuaGUI();

    void run(const std::string &file_path) {
        print("LuaGUIのrun開始");
        lua["_CWD"] = file_path;
        lua.script(storage.get_text(file_path));
        print("LuaGUIのrun終了");
    }
};
