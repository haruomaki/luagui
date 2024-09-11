#include <sol/sol.hpp>

int testfunc();

class LuaGUI {
  public:
    GUI gui;
    sol::state lua;

    LuaGUI() {
        lua.open_libraries(sol::lib::base);
    }

    void mainloop(const std::string &filename) {
        print("LuaGUIのmainloop開始");
        lua.script_file(filename);

        gui.mainloop();
        print("LuaGUIのmainloop終了");
    }
};
