#include "Text.hpp"
#include <GUI/Text.hpp>
#include <GUI/WorldObject.hpp>

void register_text(sol::state &lua) {
    // Textクラス
    lua.new_usertype<Text>(
        "Text",
        "message", sol::property([](Text *t) { return t->text; }, [](Text *t, std::string msg) { t->text = std::move(msg); }),
        sol::base_classes, sol::bases<Component>());

    lua["WorldObject"]["child_text"] = [&](WorldObject &obj, sol::optional<const char *> text_opt, const sol::optional<sol::table> &options_opt) -> Text & {
        std::string text = text_opt.value_or("");
        sol::table options = options_opt.value_or(lua.create_table());

        Font &font = lua["__CurrentFont"];
        auto &tt = obj.child_component<Text>(font, text, RGBA{0.5, 0.8, 0.2, 0.4});

        vector<float> p = options.get_or<vector<float>>("position", {0, 0});
        tt.owner().set_position({p.at(0), p.at(1), 0});

        float s = options.get_or<float>("scale", 1);
        tt.owner().set_scale_prop(s);

        return tt;
    };
}
