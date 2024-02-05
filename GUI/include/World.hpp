#pragma once

#include <Camera.hpp>

class World {
    set<function<void(const Camera &)> *> draws_;
    std::set<std::function<void()> *> updates_;
    Camera *active_camera_ = nullptr;

    friend class DrawableWorldObject;
    friend class Update;

  public:
    Window &window;

    World(Window &window)
        : window(window) {}

    void master_draw() {
        if (this->active_camera_ == nullptr) {
            print("警告: アクティブなカメラが存在しません");
        }
        for (auto *draw : draws_) {
            (*draw)(*this->active_camera_);
        }
    }

    void master_update() {
        for (auto *update : this->updates_) {
            (*update)();
        }
    }

    void set_active_camera(Camera &camera) {
        this->active_camera_ = &camera;
    }
};
