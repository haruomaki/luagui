#include "BulletPhysics.hpp"
#include <GUI/WorldObject.hpp>
#include <GUI/bullet_physics.hpp>

void register_bullet_physics(sol::state &lua) {
    lua.new_usertype<Rigidbody>(
        "Rigidbody",
        "group", sol::property(&Rigidbody::get_group, &Rigidbody::set_group),
        "mask", sol::property(&Rigidbody::get_mask, &Rigidbody::set_mask),
        "mass", sol::property(&Rigidbody::get_mass, &Rigidbody::set_mass),
        "linear_velocity", sol::property(&Rigidbody::get_linear_velocity, &Rigidbody::set_linear_velocity),
        "linear_factor", sol::property(&Rigidbody::get_linear_factor, &Rigidbody::set_linear_factor),
        "angular_velocity", sol::property(&Rigidbody::get_angular_velocity, &Rigidbody::set_angular_velocity),
        "angular_factor", sol::property(&Rigidbody::get_angular_factor, &Rigidbody::set_angular_factor),
        "inertia", sol::property(&Rigidbody::get_inertia, &Rigidbody::set_inertia),
        "friction", sol::property(&Rigidbody::get_friction, &Rigidbody::set_friction),
        "restitution", sol::property(&Rigidbody::get_restitution, &Rigidbody::set_restitution),
        "box_shape", &Rigidbody::box_shape,
        "plane_shape", &Rigidbody::plane_shape,
        "cylinder_shape", &Rigidbody::cylinder_shape,
        "capsule_shape", &Rigidbody::capsule_shape,
        "activate", &Rigidbody::activate,
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

    // ワールドの設定
    lua.new_usertype<BulletWorld>(
        "BulletWorld",
        "gravity", sol::property(&BulletWorld::get_gravity, &BulletWorld::set_gravity));

    lua.new_usertype<RaycastHit>(
        "RaycastHit",
        "hitObject", &RaycastHit::hit_object,
        "hitPoint", sol::readonly_property([](RaycastHit h) { return bt_to_glm(h.hit_point); }),
        "hitNormal", sol::readonly_property([](RaycastHit h) { return bt_to_glm(h.hit_normal); }),
        "hitFraction", &RaycastHit::hit_fraction);
}
