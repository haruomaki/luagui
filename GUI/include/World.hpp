#pragma once

#include <Camera.hpp>

// テスト用。Worldのデストラクタ、Worldのメンバ変数解放などの順序を確認するため
struct Namahage {
    int x{};
    Namahage() { print("なまはげ登場"); }
    ~Namahage() { print("なまはげ退散"); }
};

class World : public WorldObject {
    set<function<void(const Camera &)> *> draws_;
    FunctionSet<void()> updates_;
    Namahage namahage();
    Camera *active_camera_ = nullptr;

    friend class DrawableWorldObject;
    friend class Update;

  public:
    Window &window;

    World(Window &window)
        : WorldObject(*this, true)
        , window(window) {}

    ~World() override {
        print("Worldのデストラクタ");
        this->children_.clear();
    }

    void master_draw() {
        if (this->active_camera_ == nullptr) {
            print("警告: アクティブなカメラが存在しません");
        }
        for (auto *draw : draws_) {
            (*draw)(*this->active_camera_);
        }
    }

    void master_update() {
        for (const auto &[id, update] : this->updates_) {
            update();
        }
    }

    void set_active_camera(Camera &camera) {
        this->active_camera_ = &camera;
    }
};
