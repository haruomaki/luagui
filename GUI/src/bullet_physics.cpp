#include "bullet_physics.hpp"
#include "World.hpp"

// ------------------------
// Rigidbodyコンポーネント
// ------------------------

void Rigidbody::attach_shape(const Collider &cc) const {
    info("attach shape: ", cc.id);
    shapes->addChildShape(btTransform::getIdentity(), cc.shape.get());
}
void Rigidbody::detach_shape(const Collider &cc) const {
    info("detach shape: ", cc.id);
    shapes->removeChildShape(cc.shape.get());
}

Rigidbody::Rigidbody() {
    assert(owner().get_components<Rigidbody>().empty());

    // 剛体の作成
    shapes = std::make_unique<btCompoundShape>();
    btRigidBody::btRigidBodyConstructionInfo rb_info(1, this, shapes.get());
    rigid_body = std::make_unique<btRigidBody>(rb_info);
    world().bullet_world.add_rigidbody(rigid_body);

    // 既にColliderがあれば、それを遅延的に実体化する。
    for (auto *cc : owner().get_components<Collider>()) {
        print("既にColliderがありました。");
        attach_shape(*cc);
    }
}

Rigidbody::~Rigidbody() {
    // Colliderがあれば実体が存在するので、それを削除。
    for (auto *cc : owner().get_components<Collider>()) {
        detach_shape(*cc);
    }
}

static glm::mat4 bt_to_glm(const btTransform &transform) {
    btScalar matrix[16];
    transform.getOpenGLMatrix(matrix);
    return glm::make_mat4(matrix);
}

static btTransform glm_to_bt(const glm::mat4 &mat) {
    btTransform transform;
    btScalar matrix[16];
    const float *mat_ptr = glm::value_ptr(mat);
    for (int i = 0; i < 16; ++i) {
        matrix[i] = mat_ptr[i];
    }
    transform.setFromOpenGLMatrix(matrix);
    return transform;
}

void Rigidbody::getWorldTransform(btTransform &world_trans) const {
    auto btt = owner().get_absolute_transform();
    world_trans = glm_to_bt(btt);
}

void Rigidbody::setWorldTransform(const btTransform &world_trans) {
    auto transform = bt_to_glm(world_trans);
    owner().set_transform(transform);
}

// -----------------------
// Colliderコンポーネント
// -----------------------

Collider::Collider() {
    auto *s = new btSphereShape(1); // NOLINT
    btScalar mass = 1;
    btVector3 inertia(0, 0, 0);
    s->calculateLocalInertia(mass, inertia);
    shape = std::unique_ptr<btCollisionShape>(static_cast<btCollisionShape *>(s));

    // 既にRigidbodyが物体に付いていれば、そこにShapeを追加。
    if (auto *rbc = owner().get_component<Rigidbody>(false)) {
        rbc->attach_shape(*this);
    }
}

Collider::~Collider() {
    // Rigidbodyが物体に付いていれば、そこからShapeを削除。
    // Rigidbodyが無い場合は、Shapeの実体もどこにも無いはずなので何もしなくてok。
    if (auto *rbc = owner().get_component<Rigidbody>(false)) {
        rbc->detach_shape(*this);
    }
}
