#pragma once

#include "GUI.hpp"
#include <Camera.hpp>

class World : public WorldObject {
  public:
    Window &window; // update_task_id_初期化に必要なので前に書く

  private:
    set<function<void(const Camera &)> *> draws_;
    FunctionSet<void()> updates_;
    Camera *active_camera_ = nullptr;
    const TaskId update_task_id_;

    friend class DrawableWorldObject;
    friend class Update;

    friend class Window;
    int draw_priority_ = 0;

  public:
    World(Window &window, int draw_priority)
        : WorldObject(*this) // Worldにのみ許されたプライベートコンストラクタ
        , window(window)
        , update_task_id_(
              // 更新処理を定期的に呼ぶタイマーをセットする
              this->window.gui.timer.task(1. / 60, [this] { this->master_update(); }))
        , draw_priority_(draw_priority) {}

    ~World() override {
        print("Worldのデストラクタ");
        this->window.gui.timer.erase(this->update_task_id_);
        this->children_.clear(); // draws_やupdates_が消える前にUpdate等のデストラクタを呼ぶ
    }

    World(const World &) = delete;
    World &operator=(const World &) const = delete;
    World(World &&) = delete;
    World &operator=(World &&) const = delete;

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

    Camera *&active_camera() {
        return this->active_camera_;
    }

    [[nodiscard]] int get_draw_priority() const {
        return this->draw_priority_;
    }
    void set_draw_priority(int priority) {
        this->draw_priority_ = priority;
        this->window.refresh_world_order();
    }
    PropertyGetSet<&World::get_draw_priority, &World::set_draw_priority> draw_priority{this};
};
