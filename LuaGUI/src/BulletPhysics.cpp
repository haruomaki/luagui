#include "BulletPhysics.hpp"
#include <GUI/WorldObject.hpp>
#include <GUI/bullet_physics.hpp>

void register_bullet_physics(sol::state &lua) {
    lua.new_usertype<Rigidbody>(
        "Rigidbody",
        "mass", sol::property(&Rigidbody::get_mass, &Rigidbody::set_mass),
        "linear_velocity", sol::property(&Rigidbody::get_linear_velocity, &Rigidbody::set_linear_velocity),
        "angular_factor", sol::property(&Rigidbody::get_angular_factor, &Rigidbody::set_angular_factor),
        "inertia", sol::property(&Rigidbody::get_inertia, &Rigidbody::set_inertia),
        "restitution", sol::property([](Rigidbody *r) { return r->rigid_body->getRestitution(); }, [](Rigidbody *r, float rest) { r->rigid_body->setRestitution(rest); }),
        "box_shape", &Rigidbody::box_shape,
        "plane_shape", &Rigidbody::plane_shape,
        sol::base_classes, sol::bases<Component>());

    lua["WorldObject"]["add_rigidbody"] = [](WorldObject *obj) -> Rigidbody * {
        return &obj->add_component<Rigidbody>();
    };

    lua.new_usertype<Collider>(
        "Collider",
        sol::base_classes, sol::bases<Component>());

    lua["WorldObject"]["add_collider"] = [](WorldObject *obj) -> Collider * {
        return &obj->add_component<Collider>();
    };

    lua.new_usertype<RaycastHit>(
        "RaycastHit",
        "hitObject", &RaycastHit::hit_object,
        "hitPoint", sol::readonly_property([](RaycastHit h) { return bt_to_glm(h.hit_point); }),
        "hitNormal", sol::readonly_property([](RaycastHit h) { return bt_to_glm(h.hit_normal); }),
        "hitFraction", &RaycastHit::hit_fraction);
}
