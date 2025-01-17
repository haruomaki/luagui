#pragma once

#include "Component.hpp"
#include "WorldObject.hpp"
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
    void add_rigidbody(const std::unique_ptr<btRigidBody> &body) const { dynamics_world->addRigidBody(body.get()); }
    void remove_rigidbody(const std::unique_ptr<btRigidBody> &body) const { dynamics_world->removeRigidBody(body.get()); }

    void step_simulation(float delta_time, int max_sub_steps = 1) const {
        dynamics_world->stepSimulation(delta_time, max_sub_steps);
    }
};

inline glm::mat4 bt_to_glm(const btTransform &transform) {
    btScalar matrix[16];
    transform.getOpenGLMatrix(matrix);
    return glm::make_mat4(matrix);
}

inline btTransform glm_to_bt(const glm::mat4 &mat) {
    btTransform transform;
    btScalar matrix[16];
    const float *mat_ptr = glm::value_ptr(mat);
    for (int i = 0; i < 16; ++i) {
        matrix[i] = mat_ptr[i];
    }
    transform.setFromOpenGLMatrix(matrix);
    return transform;
}

class Collider;

/// 一つの剛体を表すコンポーネント。
/// オブジェクトに対してRigidbodyComponentは一つだけ付与できる。
class Rigidbody : public Component, public btMotionState {
    friend Collider;
    void attach_shape(const Collider &cc) const;
    void detach_shape(const Collider &cc) const;

  public:
    std::unique_ptr<btRigidBody> rigid_body;
    std::unique_ptr<btCompoundShape> shapes;

    Rigidbody();
    ~Rigidbody() override;

    void set_mass(float mass) const;

    // void refresh() const;
    // void switch_type();

    Collider &box_shape(float x, float y, float z);
    Collider &plane_shape(float x, float y, float z, float distance);

    void getWorldTransform(btTransform &world_trans) const override;
    void setWorldTransform(const btTransform &world_trans) override;
};

class Collider : public Component {
  public:
    std::unique_ptr<btCollisionShape> shape;

    Collider() = default;
    ~Collider() override { clear_shape(); }
    Collider(Collider &) = delete;
    Collider &operator=(Collider &) = delete;
    Collider(Collider &&) = delete;
    Collider &operator=(Collider &&) = delete;

    template <std::derived_from<btCollisionShape> T, typename... Args>
        requires std::constructible_from<T, Args...>
    void set_shape(Args... args) {
        clear_shape();

        T *s = new T(std::forward<Args>(args)...); // NOLINT(cppcoreguidelines-owning-memory)
        btScalar mass = 0;
        btVector3 inertia(0, 0, 0);
        s->calculateLocalInertia(mass, inertia);
        shape = std::unique_ptr<btCollisionShape>(static_cast<btCollisionShape *>(s));

        // 既にRigidbodyが物体に付いていれば、そこにShapeを追加。
        if (auto *rbc = owner().get_component<Rigidbody>(false)) {
            rbc->attach_shape(*this);
        }
    }

    void clear_shape() {
        // Rigidbodyが物体に付いていれば、そこからShapeを削除。
        // Rigidbodyが無い場合は、Shapeの実体もどこにも無いはずなので何もしなくてok。
        if (auto *rbc = owner().get_component<Rigidbody>(false)) {
            rbc->detach_shape(*this);
        }
        shape.reset();
    }

    [[nodiscard]] bool is_valid() const { return shape != nullptr; }
};
