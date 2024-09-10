#include <GUI.hpp>
#include <luagui.hpp>

#include <sol/sol.hpp>

int testfunc() {
    sol::state lua;
    int x = 0;
    lua.set_function("beep", [&x] { x += 234; });
    lua.script("beep()");
    debug(x);

    GUI gui;
    gui.create_window(200, 200, "ぴえん");
    gui.mainloop();

    return 7;
}
