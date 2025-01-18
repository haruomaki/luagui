#include "Text.hpp"
#include <GUI/GUI.hpp>
#include <GUI/Text.hpp>
#include <GUI/WorldObject.hpp>
#include <Lunchbox/Storage.hpp>

void register_text(sol::state &lua) {
    // Fontクラス
    lua.new_usertype<Font>(
        "Font",
        "load", [&lua](const std::string &file_path) -> Font * {
            GUI &gui = lua["__GUI"];
            lunchbox::Storage &storage = lua["__Storage"];
            auto text_shader = GL::ProgramObject{
                GL::create_shader(GL_VERTEX_SHADER, storage.get_text("shaders/font.vsh")),
                GL::create_shader(GL_FRAGMENT_SHADER, storage.get_text("shaders/font.fsh"))};
            auto font = storage.get_font(file_path);
            auto &default_font = gui.resources.append<Font>(std::move(text_shader), font).get();
            lua["__CurrentFont"] = &default_font;
            return &default_font;
        },
        sol::base_classes, sol::bases<Resource>());

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
