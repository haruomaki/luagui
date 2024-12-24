#include "World.hpp"
#include "Window.hpp"

void World::master_physics() {
    // Box2Dの毎フレーム更新処理
    // ディスプレイのリフレッシュレートに依存する
    float dt = 1 / float(gui.ctx().video_mode().refreshRate); // FIXME: refresh_rateメソッドがほしい
    b2world.Step(dt, 4);

    // 物理演算結果をWorldObjectに反映
    rigidbody_components.foreach ([](RigidbodyComponent *rbc) {
        const auto physics_position = rbc->b2body.GetPosition();
        rbc->owner().position = {physics_position.x, physics_position.y, 0};

        // b2Rot（サインコサイン）からglm::quatに変換する
        const auto physics_rotation = rbc->b2body.GetRotation();
        const float angle = atan2(physics_rotation.s, physics_rotation.c);
        const auto quat = glm::angleAxis(angle, glm::vec3{0, 0, 1}); // Z軸回転
        rbc->owner().rotate = quat;
    });

    // 衝突を検出してコールバックを実行
    auto contact_events = b2world.GetContactEvents();
    for (int i = 0; i < contact_events.beginCount; i++) {
        // print("衝突かいし");
        auto event = contact_events.beginEvents[i]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        auto &cca = dereference<ColliderComponent>(b2Shape_GetUserData(event.shapeIdA));
        auto &ccb = dereference<ColliderComponent>(b2Shape_GetUserData(event.shapeIdB));
        auto chain_id_a = b2Shape_GetParentChain(event.shapeIdA);
        auto chain_id_b = b2Shape_GetParentChain(event.shapeIdB);

        // AとBのどちらかがChainなら(Chain,Shape)の衝突、どちらもShapeなら(A,B)＆(B,A)の衝突。どちらもChainはあり得ないはず？
        if (b2Chain_IsValid(chain_id_a)) {
            auto &ccca = dereference<ChainColliderComponent>(b2Shape_GetUserData(event.shapeIdA));
            // NOTE: ここでもしccbがchainだったらクラッシュする
            if (ccca.on_collision_enter.has_value()) ccca.on_collision_enter.value()(ccca, ccb);
        } else if (b2Chain_IsValid(chain_id_b)) {
            auto &cccb = dereference<ChainColliderComponent>(b2Shape_GetUserData(event.shapeIdB));
            if (cccb.on_collision_enter.has_value()) cccb.on_collision_enter.value()(cccb, cca);
        } else {
            if (cca.on_collision_enter.has_value()) cca.on_collision_enter.value()(cca, ccb);
            // FIXME: コールバック中で片側のオブジェクトを消したときエラーになる
            if (ccb.on_collision_enter.has_value()) ccb.on_collision_enter.value()(ccb, cca);
        }
        // print("衝突おわり");
    }
}
