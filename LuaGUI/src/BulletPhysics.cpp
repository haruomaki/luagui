#include "BulletPhysics.hpp"
#include <GUI/WorldObject.hpp>
#include <GUI/bullet_physics.hpp>

void register_bullet_physics(sol::state &lua) {
    lua.new_usertype<Rigidbody>(
        "Rigidbody",
        sol::base_classes, sol::bases<Component>());

    lua["WorldObject"]["add_rigidbody"] = [](WorldObject *obj) -> Rigidbody * {
        return &obj->add_component<Rigidbody>();
    };
}
