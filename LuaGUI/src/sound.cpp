#include "sound.hpp"
#include <GUI/GUI.hpp>
#include <GUI/WorldObject.hpp>
#include <GUI/sound.hpp>
#include <lunchbox.hpp>
#include <utility>

void register_sound(sol::state &lua) {
    lua.new_usertype<Music>(
        "Music",
        "load", [&lua](const std::string &file_path) -> Music * {
            GUI &gui = lua["__GUI"];
            lunchbox::Storage &storage = lua["__Storage"];
            auto sound = storage.get_sound(file_path);
            auto music = gui.resources.append<Music>(sound);
            return &music.get();
        },
        sol::base_classes, sol::bases<Resource>());

    lua.new_usertype<SoundSource>(
        "SoundSource",
        "group_name", &SoundSource::group_name,
        "play", [](SoundSource *source) { source->play(); },
        "is_playing", [](SoundSource *source) -> bool { return source->is_playing(); },
        sol::base_classes, sol::bases<Component>());

    lua["WorldObject"]["add_soundsource_component"] = [](WorldObject *obj, std::string group_name, Music *music) -> SoundSource & {
        auto &ss = obj->add_component<SoundSource>(*music);
        ss.group_name = std::move(group_name);
        return ss;
    };
}
