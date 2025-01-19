#include "World.hpp"
#include "Window.hpp"

void World::master_physics() {
    trace("master_physics: ", this);

    // Box2Dの毎フレーム更新処理
    // ディスプレイのリフレッシュレートに依存する
    float dt = 1 / float(gui.refresh_rate());
    b2world.Step(dt, 4);

    // 物理演算結果をWorldObjectに反映
    rigidbody_components.flush(); // flushはここのみ
    rigidbody_components.foreach ([](Rigidbody2D *rbc) {
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
        auto id1 = event.shapeIdA;
        auto id2 = event.shapeIdB;

        // ループ中にShapeが削除されることで、idが無効になっている場合がある。
        if (!b2Shape_IsValid(id1) || !b2Shape_IsValid(id2)) {
            print("無効なShapeらしいよ");
            continue;
        }

        auto &cca = dereference<Collider2D>(b2Shape_GetUserData(id1));
        auto &ccb = dereference<Collider2D>(b2Shape_GetUserData(id2));
        auto chain_id_a = b2Shape_GetParentChain(id1);
        auto chain_id_b = b2Shape_GetParentChain(id2);

        // AとBのどちらかがChainなら(Chain,Shape)の衝突、どちらもShapeなら(A,B)＆(B,A)の衝突。どちらもChainはあり得ないはず？
        if (b2Chain_IsValid(chain_id_a)) {
            auto &ccca = dereference<ChainCollider2D>(b2Shape_GetUserData(event.shapeIdA));
            // NOTE: ここでもしccbがchainだったらクラッシュする
            if (ccca.on_collision_enter.has_value()) ccca.on_collision_enter.value()(ccca, ccb);
        } else if (b2Chain_IsValid(chain_id_b)) {
            auto &cccb = dereference<ChainCollider2D>(b2Shape_GetUserData(event.shapeIdB));
            if (cccb.on_collision_enter.has_value()) cccb.on_collision_enter.value()(cccb, cca);
        } else {
            if (cca.on_collision_enter.has_value()) cca.on_collision_enter.value()(cca, ccb);
            // FIXME: コールバック中で片側のオブジェクトを消したときエラーになる
            if (ccb.on_collision_enter.has_value()) ccb.on_collision_enter.value()(ccb, cca);
        }
        // print("衝突おわり");
    }

    // -------------------------------
    // Bullet Physicsによる3D物理演算
    // -------------------------------

    bullet_world.step_simulation(dt, 10);
}

std::vector<RaycastHit> World::raycast(const btVector3 &origin, const btVector3 &direction) {
    // 結果を格納するリスト
    std::vector<RaycastHit> results;

    // コールバックを作成
    struct CustomRayResultCallback : public btCollisionWorld::AllHitsRayResultCallback {
        CustomRayResultCallback(const btVector3 &from, const btVector3 &to)
            : btCollisionWorld::AllHitsRayResultCallback(from, to) {}

        // ヒットしたオブジェクトごとに情報を記録
        [[nodiscard]] std::vector<RaycastHit> get_results() const {
            std::vector<RaycastHit> hits;

            // fractionでソート
            std::vector<std::pair<float, int>> tmp_fractions(m_collisionObjects.size());
            for (int i = 0; i < m_collisionObjects.size(); i++) {
                tmp_fractions[i] = {m_hitFractions[i], i};
            }
            std::sort(tmp_fractions.begin(), tmp_fractions.end());

            for (auto [_, i] : tmp_fractions) {
                RaycastHit hit;
                if (void *p = m_collisionObjects[i]->getUserPointer()) {
                    hit.hit_object = static_cast<Rigidbody *>(p);
                } else {
                    warn("レイキャストで当たった物体はRigidbodyコンポーネント由来でありません。");
                    hit.hit_object = nullptr;
                }
                hit.hit_point = m_hitPointWorld[i];
                hit.hit_normal = m_hitNormalWorld[i];
                hit.hit_fraction = m_hitFractions[i];
                hits.push_back(hit);
            }

            return hits;
        }
    };

    // レイキャストを実行
    btVector3 from = origin;
    btVector3 to = origin + direction;
    CustomRayResultCallback callback(from, to);
    bullet_world.dynamics_world->rayTest(from, to, callback);

    // 衝突したオブジェクトがあれば、結果を取得
    if (callback.hasHit()) {
        results = callback.get_results();
    }

    return results;
}
