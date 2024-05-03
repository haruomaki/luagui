#include "Physics.hpp"
#include "World.hpp"

Rigidbody::Rigidbody() {
    this->get_world().rigidbodies.request_set(this);
}

Rigidbody::~Rigidbody() {
    this->get_world().rigidbodies.request_erase(this);
}

void AABB2d::collide_aabb2d(AABB2d &rb) {
    info("AABB2Dのcollide_aabb2dです");
    auto obj1 = this->current_abs_;
    auto obj2 = rb.current_abs_;

    bool judge_x = test(obj1.x1, obj1.x2, obj2.x1, obj2.x2);
    bool judge_y = test(obj1.y1, obj1.y2, obj2.y1, obj2.y2);
    bool judge = judge_x && judge_y;

    if (judge) {
        // どちらの方向にぶつかったのか検査
        auto obj1_prev = this->previous_abs_;
        auto obj2_prev = rb.previous_abs_;
        bool judge_x_prev = test(obj1_prev.x1, obj1_prev.x2, obj2_prev.x1, obj2_prev.x2);
        bool judge_y_prev = test(obj1_prev.y1, obj1_prev.y2, obj2_prev.y1, obj2_prev.y2);

        if (!judge_x_prev && judge_x) {
            print("x軸方向に衝突！");
            std::swap(this->velocity.x, rb.velocity.x); // TODO: 質量を考慮
        }
        if (!judge_y_prev && judge_y) {
            print("y軸方向に衝突！");
            std::swap(this->velocity.y, rb.velocity.y);
        }
    }

    if (judge) {
        invoke_callbacks(*this, rb);
    }
}
