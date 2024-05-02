#include "World.hpp"
#include "Physics.hpp"

void World::master_physics() {
    // まず各Rigidbodyの更新処理を呼び出す
    this->rigidbodies.foreach_flush([](Rigidbody *rb) {
        rb->tick();
    });

    this->rigidbodies.foreach_flush_combination([](auto *rb1, auto *rb2) {
        (*rb1).collide(*rb2);
    });
}
