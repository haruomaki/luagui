#include "BulletPhysics.hpp"
#include <GUI/WorldObject.hpp>
#include <GUI/bullet_physics.hpp>

static btVector3 bt3(std::vector<float> &&vec) {
    float x = vec.size() > 0 ? vec[0] : 0;
    float y = vec.size() > 1 ? vec[1] : 0;
    float z = vec.size() > 2 ? vec[2] : 0;
    return {x, y, z};
}

void register_bullet_physics(sol::state &lua) {
    lua.new_usertype<Rigidbody>(
        "Rigidbody",
        "mass", sol::property([](Rigidbody *r) { return r->rigid_body->getMass(); }, [](Rigidbody *r, float mass) { r->rigid_body->setMassProps(mass, r->rigid_body->getLocalInertia()); }),
        "inertia", sol::property([](Rigidbody *r) { return r->rigid_body->getLocalInertia(); }, [](Rigidbody *r, std::vector<float> inertia) { r->rigid_body->setMassProps(r->rigid_body->getMass(), bt3(std::move(inertia))); }),
        "restitution", sol::property([](Rigidbody *r) { return r->rigid_body->getRestitution(); }, [](Rigidbody *r, float rest) { r->rigid_body->setRestitution(rest); }),
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
}
