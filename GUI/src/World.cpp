#include "World.hpp"
#include "GUI.hpp"
#include "Physics.hpp"

void World::master_physics() {
    // まず速度や加速度を適用して時間を1tick進める
    this->rigidbodies.foreach_flush([this](Rigidbody *rb) {
        int fps = this->window.gui.video_mode().refreshRate;
        rb->position += rb->velocity / fps;
        rb->velocity *= 0.99;
    });

    this->rigidbodies.foreach_flush_combination([](auto *rb1, auto *rb2) {
        (*rb1).collide(*rb2);
    });
}
