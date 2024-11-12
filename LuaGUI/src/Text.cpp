#include "Text.hpp"
#include <GUI/Text.hpp>

void register_text(sol::state &lua) {
    lua["WorldObject"]["draw_text"] = [&](WorldObject *obj, const std::string &text, const sol::table &options) {
        Font &font = lua["__CurrentFont"];
        auto &tt = obj->append_child<Text>(font, text, RGBA{0.5, 0.8, 0.2, 0.4});

        vector<float> p = options.get_or<vector<float>>("position", {0, 0});
        tt.set_position({p.at(0), p.at(1), 0});
    };
}
