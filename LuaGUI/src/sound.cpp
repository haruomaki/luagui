#include "sound.hpp"
#include <GUI/GUI.hpp>
#include <GUI/SoundSource.hpp>
#include <GUI/WorldObject.hpp>
#include <lunchbox.hpp>

void register_sound(sol::state &lua) {
    lua.new_usertype<Music>(
        "Music",
        sol::base_classes, sol::bases<Resource>());

    lua.new_usertype<SoundSource>(
        "SoundSource",
        "play", [](SoundSource *source) { source->play(); },
        "is_playing", [](SoundSource *source) -> bool { return source->is_playing(); },
        sol::base_classes, sol::bases<Component>());

    lua.set_function("load_music", [&lua](const std::string &file_path) -> Music * {
        GUI &gui = lua["__GUI"];
        lunchbox::Storage &storage = lua["__Storage"];
        auto sound = storage.get_sound(file_path);
        auto music = gui.resources.append<Music>(sound);
        return &music.get();
    });

    lua["WorldObject"]["add_soundsource_component"] = [](WorldObject *obj, Music *music) -> SoundSource & {
        return obj->add_component<SoundSource>(*music);
    };
}
