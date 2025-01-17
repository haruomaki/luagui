#include "bullet_physics.hpp"
#include "World.hpp"

// ------------------------
// Rigidbodyコンポーネント
// ------------------------

void Rigidbody::attach_shape(const Collider &cc) const {
    if (cc.shape) {
        info("attach shape: ", cc.id);
        shapes->addChildShape(btTransform::getIdentity(), cc.shape.get());
    }
}
void Rigidbody::detach_shape(const Collider &cc) const {
    if (cc.shape) {
        info("detach shape: ", cc.id);
        shapes->removeChildShape(cc.shape.get());
    }
}

Rigidbody::Rigidbody() {
    assert(owner().get_components<Rigidbody>().empty());

    // 剛体の作成
    // HACK: massを0にするとstaticな剛体が作れる。しかし初めにConstructionInfoに与えるものは非負にしておかないと、その後の質量の変更によるstatic/dynamicの切り替えがうまく働かない。
    shapes = std::make_unique<btCompoundShape>();
    btRigidBody::btRigidBodyConstructionInfo rb_info(1, this, shapes.get());
    rigid_body = std::make_unique<btRigidBody>(rb_info);
    world().bullet_world.add_rigidbody(rigid_body);
    set_mass(0);

    // 既にColliderがあれば、それを遅延的に実体化する。
    for (auto *cc : owner().get_components<Collider>()) {
        attach_shape(*cc);
    }
}

Rigidbody::~Rigidbody() {
    // Colliderがあれば実体が存在するので、それを削除。
    for (auto *cc : owner().get_components<Collider>()) {
        detach_shape(*cc);
    }
    world().bullet_world.remove_rigidbody(rigid_body);
}

void Rigidbody::set_mass(float mass) const {
    btVector3 inertia;
    rigid_body->getCollisionShape()->calculateLocalInertia(mass, inertia);
    rigid_body->setMassProps(mass, inertia);
}

// void Rigidbody::refresh() const {
//     world().bullet_world.remove_rigidbody(rigid_body);
//     world().bullet_world.add_rigidbody(rigid_body);
// }

// void Rigidbody::switch_type() {
//     world().bullet_world.remove_rigidbody(rigid_body);
//     btVector3 v;
//     rigid_body->getCollisionShape()->calculateLocalInertia(1, v);
//     rigid_body->setMassProps(1, v);
//     rigid_body->updateInertiaTensor();
//     rigid_body->setGravity({1, 1, 1});

//     // btRigidBody::btRigidBodyConstructionInfo rb_info(0.1, this, shapes.get());
//     // rigid_body = std::make_unique<btRigidBody>(rb_info);
//     world().bullet_world.add_rigidbody(rigid_body);
//     // refresh();
// }

Collider &Rigidbody::box_shape(float x, float y, float z) {
    auto &cc = owner().add_component<Collider>();
    cc.set_shape<btBoxShape>(btVector3{x, y, z});
    return cc;
}
Collider &Rigidbody::plane_shape(float x, float y, float z, float distance) {
    auto &cc = owner().add_component<Collider>();
    cc.set_shape<btStaticPlaneShape>(btVector3{x, y, z}, distance);
    return cc;
}

void Rigidbody::getWorldTransform(btTransform &world_trans) const {
    auto btt = owner().get_absolute_transform();
    world_trans = glm_to_bt(btt);
}

void Rigidbody::setWorldTransform(const btTransform &world_trans) {
    auto transform = bt_to_glm(world_trans);
    owner().set_transform(transform, true);
}
