#include "Text.hpp"
#include <GUI/Text.hpp>

void register_text(sol::state &lua) {
    lua.set_function("put_text", [&] {
        World &world = lua["__CurrentWorld"];
        Font &font = lua["__CurrentFont"];
        world.append_child<Text>(font, "This is sample text 123456789", RGBA{0.5, 0.8, 0.2, 0.4});
    });
}
