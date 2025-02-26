#pragma once

#include "Component.hpp"
#include "WorldObject.hpp"
#include <btBulletDynamicsCommon.h>
#include <memory>

inline glm::vec3 bt_to_glm(const btVector3 &v) { return {v.x(), v.y(), v.z()}; }
inline btVector3 glm_to_bt(const glm::vec3 &v) { return {v.x, v.y, v.z}; }

// デバッグビルド時にも静的オブジェクト同士の衝突を完全に無視する。
class CustomCollisionDispatcher : public btCollisionDispatcher {
  public:
    CustomCollisionDispatcher(btCollisionConfiguration *config)
        : btCollisionDispatcher(config) {}

    bool needsCollision(const btCollisionObject *body0, const btCollisionObject *body1) override {
        if ((body0->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT) &&
            (body1->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT)) {
            return false; // 静的オブジェクト同士の衝突を完全に無視
        }
        return btCollisionDispatcher::needsCollision(body0, body1);
    }
};

class BulletWorld {
  public:
    std::unique_ptr<btDefaultCollisionConfiguration> collision_configuration;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btDbvtBroadphase> overlapping_pair_cache;
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
    std::unique_ptr<btDiscreteDynamicsWorld> dynamics_world;

    BulletWorld() {
        collision_configuration = std::make_unique<btDefaultCollisionConfiguration>();
        dispatcher = std::make_unique<CustomCollisionDispatcher>(collision_configuration.get());
        overlapping_pair_cache = std::make_unique<btDbvtBroadphase>();
        solver = std::make_unique<btSequentialImpulseConstraintSolver>();
        dynamics_world = std::make_unique<btDiscreteDynamicsWorld>(
            dispatcher.get(),
            overlapping_pair_cache.get(),
            solver.get(),
            collision_configuration.get());
    }

    // 必要に応じて初期化やリソースの追加用メソッドを追加
    void add_rigidbody(const std::unique_ptr<btRigidBody> &body, int group, int mask) const { dynamics_world->addRigidBody(body.get(), group, mask); }
    void remove_rigidbody(const std::unique_ptr<btRigidBody> &body) const { dynamics_world->removeRigidBody(body.get()); }

    void step_simulation(float delta_time, int max_sub_steps = 1) const {
        dynamics_world->stepSimulation(delta_time, max_sub_steps);
    }

    // 各種プロパティ
    [[nodiscard]] glm::vec3 get_gravity() const { return bt_to_glm(dynamics_world->getGravity()); }
    void set_gravity(glm::vec3 v) const { dynamics_world->setGravity(glm_to_bt(v)); }
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

    void create_rigidbody();
    void delete_rigidbody();
    void regenerate_rigidbody();

    // グループとマスクはbtRigidBody内で変えられないため、ここで保持する。
    int group_ = 1;
    int mask_ = -1;

  public:
    std::unique_ptr<btRigidBody> rigid_body;
    std::unique_ptr<btCompoundShape> shapes;

    Rigidbody();
    ~Rigidbody() override;

    // グループとマスクの設定
    [[nodiscard]] int get_group() const { return group_; }
    void set_group(int group) {
        group_ = group;
        regenerate_rigidbody();
    }
    [[nodiscard]] int get_mask() const { return mask_; }
    void set_mask(int mask) {
        mask_ = mask;
        regenerate_rigidbody();
    }

    // ゲッターとセッター
    [[nodiscard]] float get_mass() const { return rigid_body->getMass(); }
    void set_mass(float mass) const;
    [[nodiscard]] glm::vec3 get_linear_velocity() const { return bt_to_glm(rigid_body->getLinearVelocity()); }
    void set_linear_velocity(glm::vec3 v) const { rigid_body->setLinearVelocity(glm_to_bt(v)); }
    [[nodiscard]] glm::vec3 get_linear_factor() const { return bt_to_glm(rigid_body->getLinearFactor()); }
    void set_linear_factor(glm::vec3 v) const { rigid_body->setLinearFactor(glm_to_bt(v)); }
    [[nodiscard]] glm::vec3 get_angular_velocity() const { return bt_to_glm(rigid_body->getAngularVelocity()); }
    void set_angular_velocity(glm::vec3 v) const { rigid_body->setAngularVelocity(glm_to_bt(v)); }
    [[nodiscard]] glm::vec3 get_angular_factor() const { return bt_to_glm(rigid_body->getAngularFactor()); }
    void set_angular_factor(glm::vec3 v) const { rigid_body->setAngularFactor(glm_to_bt(v)); }
    [[nodiscard]] glm::vec3 get_inertia() const { return bt_to_glm(rigid_body->getLocalInertia()); }
    void set_inertia(glm::vec3 v) const { rigid_body->setMassProps(get_mass(), glm_to_bt(v)); }
    [[nodiscard]] float get_friction() const { return rigid_body->getFriction(); }
    void set_friction(float frict) const { rigid_body->setFriction(frict); }
    [[nodiscard]] float get_restitution() const { return rigid_body->getRestitution(); }
    void set_restitution(float rest) const { rigid_body->setRestitution(rest); }

    void activate(std::optional<bool> force_activation) const { rigid_body->activate(force_activation.value_or(false)); }

    // void refresh() const;
    // void switch_type();

    Collider &box_shape(float x, float y, float z);
    Collider &plane_shape(float x, float y, float z, float distance);
    Collider &cylinder_shape(float x, float y, float z);
    Collider &capsule_shape(float radius, float height);

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

// レイキャストの結果を格納する構造体
struct RaycastHit {
    // NOTE: Bulletではどの子シェイプに当たったかどうかの情報を取得できない。
    Rigidbody *hit_object{}; // 衝突したオブジェクト
    btVector3 hit_point;     // 衝突位置
    btVector3 hit_normal;    // 衝突法線
    float hit_fraction{};    // 衝突位置の割合（0.0f ～ 1.0f）
};
