#pragma once

#include "Component.hpp"
#include <btBulletDynamicsCommon.h>
#include <memory>

class BulletWorld {
  public:
    std::unique_ptr<btDefaultCollisionConfiguration> collision_configuration;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btDbvtBroadphase> overlapping_pair_cache;
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
    std::unique_ptr<btDiscreteDynamicsWorld> dynamics_world;

    BulletWorld() {
        collision_configuration = std::make_unique<btDefaultCollisionConfiguration>();
        dispatcher = std::make_unique<btCollisionDispatcher>(collision_configuration.get());
        overlapping_pair_cache = std::make_unique<btDbvtBroadphase>();
        solver = std::make_unique<btSequentialImpulseConstraintSolver>();
        dynamics_world = std::make_unique<btDiscreteDynamicsWorld>(
            dispatcher.get(),
            overlapping_pair_cache.get(),
            solver.get(),
            collision_configuration.get());
    }

    // 必要に応じて初期化やリソースの追加用メソッドを追加
    void add_rigidbody(const std::unique_ptr<btRigidBody> &body) const {
        dynamics_world->addRigidBody(body.get());
    }

    void step_simulation(float delta_time, int max_sub_steps = 1) const {
        dynamics_world->stepSimulation(delta_time, max_sub_steps);
    }
};

/// 一つの剛体を表すコンポーネント。
/// オブジェクトに対してRigidbodyComponentは一つだけ付与できる。
class Rigidbody : public Component, public btMotionState {
  public:
    std::unique_ptr<btSphereShape> shape;
    std::unique_ptr<btRigidBody> rigid_body;

    Rigidbody();

    void getWorldTransform(btTransform &world_trans) const override;
    void setWorldTransform(const btTransform &world_trans) override;
};
