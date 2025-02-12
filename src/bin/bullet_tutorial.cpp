#include <btBulletDynamicsCommon.h>
#include <iostream>
#include <memory>

// ボールが地面落ちて弾む様子のシミュレーション。

int main() {
    // Bulletの初期化
    auto collision_configuration = std::make_unique<btDefaultCollisionConfiguration>();
    auto dispatcher = std::make_unique<btCollisionDispatcher>(collision_configuration.get());
    auto overlapping_pair_cache = std::make_unique<btDbvtBroadphase>();
    auto solver = std::make_unique<btSequentialImpulseConstraintSolver>();
    auto dynamics_world = std::make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), overlapping_pair_cache.get(), solver.get(), collision_configuration.get());

    dynamics_world->setGravity(btVector3(0, -10, 0));

    // 地面の作成
    auto ground_shape = std::make_unique<btStaticPlaneShape>(btVector3(0, 1, 0), 1);
    auto ground_motion_state = std::make_unique<btDefaultMotionState>(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo ground_rigid_body_ci(1, ground_motion_state.get(), ground_shape.get(), btVector3(0, 0, 0));
    auto ground_rigid_body = std::make_unique<btRigidBody>(ground_rigid_body_ci);
    // ground_rigid_body->setRestitution(0.5f); // 地面の反発係数を設定
    dynamics_world->addRigidBody(ground_rigid_body.get());
    ground_rigid_body->setMassProps(0, {0, 0, 0});

    // 動く物体の作成
    auto fall_shape = std::make_unique<btSphereShape>(1);
    auto fall_motion_state = std::make_unique<btDefaultMotionState>(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 20, 0)));
    btScalar mass = 1;
    btVector3 fall_inertia(0, 0, 0);
    fall_shape->calculateLocalInertia(mass, fall_inertia);
    btRigidBody::btRigidBodyConstructionInfo fall_rigid_body_ci(mass, fall_motion_state.get(), fall_shape.get(), fall_inertia);
    auto fall_rigid_body = std::make_unique<btRigidBody>(fall_rigid_body_ci);
    // fall_rigid_body->setRestitution(0.5f); // ボールの反発係数を設定
    dynamics_world->addRigidBody(fall_rigid_body.get());

    // シミュレーションのステップ
    for (int i = 0; i < 150; i++) {
        dynamics_world->stepSimulation(1 / 60.f, 10);

        // 物体の位置を取得して出力
        btTransform trans;
        fall_rigid_body->getMotionState()->getWorldTransform(trans);
        std::cout << "Step " << i << ": Sphere at ("
                  << trans.getOrigin().getX() << ", "
                  << trans.getOrigin().getY() << ", "
                  << trans.getOrigin().getZ() << ")" << '\n';
    }
}
