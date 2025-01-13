#include <btBulletDynamicsCommon.h>
#include <memory>

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
    btRigidBody::btRigidBodyConstructionInfo ground_rigid_body_ci(0, ground_motion_state.get(), ground_shape.get(), btVector3(0, 0, 0));
    auto ground_rigid_body = std::make_unique<btRigidBody>(ground_rigid_body_ci);
    dynamics_world->addRigidBody(ground_rigid_body.get());

    // シミュレーションのステップ
    for (int i = 0; i < 150; i++) {
        dynamics_world->stepSimulation(1 / 60.f, 10);
    }
}
