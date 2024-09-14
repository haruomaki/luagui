#include "WorldObject.hpp"
#include <utility.hpp>

void register_world_object(sol::state &lua) {
    lua.new_usertype<WorldObject>(
        "WorldObject",
        "position",
        sol::property([](WorldObject *obj) { return obj->get_position(); }, [](WorldObject *obj, std::vector<float> pos) { obj->set_position({pos[0], pos[1], 0}); }));

    lua.new_usertype<MeshObject>("MeshObject", sol::base_classes, sol::bases<WorldObject>());
}
