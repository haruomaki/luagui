#pragma once

#include <GUI/GUI.hpp>
#include <lunchbox.hpp>
#include <sol/sol.hpp>

using Path = std::filesystem::path;

class LuaGUI : public GUI {

  public:
    sol::state lua;
    lunchbox::Storage storage;
    OpenAL::Context openal_ctx;

    LuaGUI();
    ~LuaGUI();

    void run(const Path &file_path);
};
