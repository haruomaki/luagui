#include "World.hpp"
#include "Physics.hpp"

void World::master_physics() {
    // まず各Rigidbodyの更新処理を呼び出す
    this->rigidbodies.foreach_flush([](Rigidbody *rb) {
        rb->tick();
    });

    this->rigidbodies.foreach_flush_combination([](auto *rb1, auto *rb2) {
        (*rb1).collide(*rb2);
    });

    // Box2Dの毎フレーム更新処理
    b2world.Step(1 / 60.f, 4);

    // 物理演算結果をWorldObjectに反映
    rigidbody_components.foreach_flush([](RigidbodyComponent *rbc) {
        const auto physics_position = rbc->b2body.GetPosition();
        rbc->get_owner()->position = {physics_position.x, physics_position.y, 0};

        // b2Rot（サインコサイン）からglm::quatに変換する
        const auto physics_rotation = rbc->b2body.GetRotation();
        const float angle = atan2(physics_rotation.s, physics_rotation.c);
        const auto quat = glm::angleAxis(angle, glm::vec3{0, 0, 1}); // Z軸回転
        rbc->get_owner()->rotate = quat;
    });

    // 衝突を検出してコールバックを実行
    auto contact_events = b2world.GetContactEvents();
    for (int i = 0; i < contact_events.beginCount; i++) {
        auto event = contact_events.beginEvents[i]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        auto &cca = dereference<ColliderComponent>(b2Shape_GetUserData(event.shapeIdA));
        auto &ccb = dereference<ColliderComponent>(b2Shape_GetUserData(event.shapeIdB));
        if (cca.on_collision_enter.has_value()) cca.on_collision_enter.value()(cca, ccb);
        if (ccb.on_collision_enter.has_value()) ccb.on_collision_enter.value()(ccb, cca);
    }
}
