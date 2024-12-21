#pragma once

#include "Mesh.hpp"
#include "Rigidbody.hpp"
#include "Timer.hpp"

struct CameraInterface;
class Rigidbody;

class World : public WorldObject {
    CameraInterface *active_camera_ = nullptr;

    friend class Window;
    int draw_priority_ = 0;

    // メッシュ描画を一元管理するクラス
    friend class MeshComponent;
    MeshDrawManager mesh_draw_manager_;
    friend class WorldObject; // mesh_draw_manager_にアクセスするため

  public:
    Window &window;
    Timer timer;
    std::function<GL::Viewport()> viewport_provider;
    BufferedSet<std::function<void()> *> draws;
    BufferedSet<std::function<void()> *> updates;
    BufferedSet<Rigidbody *> rigidbodies;
    BufferedSet<RigidbodyComponent *> rigidbody_components;
    b2::World b2world;

    World(Window &window, int draw_priority)
        : WorldObject(*this) // Worldにのみ許されたプライベートコンストラクタ
        , draw_priority_(draw_priority)
        , window(window)
        , viewport_provider([this] {
            auto [w, h] = this->window.fbsize_cache;
            return GL::Viewport{0, 0, w, h};
        }) {
        // Box2Dの世界を生成
        b2::World::Params world_params;
        world_params.gravity = b2Vec2{};
        b2world = b2::World(world_params);
    }

    ~World() override {
        info("Worldのデストラクタ");

        // WARNING: バグ応急処置で、worldに直接付いているコンポーネントの削除。
        // こんな場当たり的でなく、Worldのシステムから改善したい。
        components_.foreach ([](auto &comp) {
            trace("component iteration ", comp->id);
            comp->erase();
        });
        components_.flush();

        children_.foreach ([&](std::unique_ptr<WorldObject> &obj) {
            obj->erase(); // drawsやupdatesが消える前にUpdate等のデストラクタを呼ぶ
        });
        children_.flush(); // 即時flushしないと子オブジェクトがメモリから消えない
    }

    World(const World &) = delete;
    World &operator=(const World &) const = delete;
    World(World &&) = delete;
    World &operator=(World &&) const = delete;

    void master_update() {
        trace("World::master_update開始:", this);
        this->updates.foreach ([&](const auto update) {
            (*update)();
        });

        trace("World::master_update途中1:", this);

        // 子（および子孫）オブジェクトの追加／削除を再帰的に適用
        this->flush_components_children();

        trace("World::master_update途中2:", this);

        this->timer.step(); // タイマーを進める
        trace("World::master_update終了:", this);
    }

    void master_physics();

    void master_draw() {
        // ビューポートを設定
        GL::viewport(viewport_provider());

        if (this->active_camera_ == nullptr) {
            print("警告: アクティブなカメラが存在しません");
        }
        this->draws.foreach ([&](const auto *draw) {
            (*draw)();
        });

        // メッシュを描画
        this->mesh_draw_manager_.step();
        this->mesh_draw_manager_.draw_all_registered_objects(*active_camera());
    }

    // void register_to_draw(const MeshObject &obj) {
    //     this->mesh_draw_manager_.register_to_draw(obj);
    // }

    CameraInterface *&active_camera() {
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
