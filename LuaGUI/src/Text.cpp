#include "Text.hpp"
#include <GUI/Text.hpp>

void register_text(sol::state &lua) {
    lua.set_function("put_text", [&] {
        Window &window = lua["__CurrentWindow"];
        World &world = lua["__CurrentWorld"];
        auto &migmix_font = window.append_resource<Font>();
        world.append_child<Text>(migmix_font, "This is sample text 123456789", RGBA{0.5, 0.8, 0.2, 0.4});
    });
}
