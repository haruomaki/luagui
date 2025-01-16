#include "bullet_physics.hpp"
#include "World.hpp"

Rigidbody::Rigidbody() {
    // 動く物体の作成
    shape = std::make_unique<btSphereShape>(1);
    btScalar mass = 1;
    btVector3 fall_inertia(0, 0, 0);
    shape->calculateLocalInertia(mass, fall_inertia);
    btRigidBody::btRigidBodyConstructionInfo fall_rigid_body_ci(mass, this, shape.get(), fall_inertia);
    rigid_body = std::make_unique<btRigidBody>(fall_rigid_body_ci);
    world().bullet_world.add_rigidbody(rigid_body);
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
