#pragma once

#include <Camera.hpp>

class World {
    set<function<void(const Camera &)> *> draws_;
    std::set<std::function<void()> *> updates_;

    friend class DrawableWorldObject;
    friend class Update;

  public:
    Window &window;

    World(Window &window)
        : window(window) {}

    void masterDraw(const Camera &camera) {
        for (auto *draw : draws_) {
            (*draw)(camera);
        }
    }

    void master_update() {
        for (auto *update : this->updates_) {
            (*update)();
        }
    }
};
