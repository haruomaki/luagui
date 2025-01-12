#pragma once

#include "Mesh.hpp"
#include "Rigidbody.hpp"
#include "Timer.hpp"
#include "WorldObject.hpp"

struct CameraInterface;
class Rigidbody;

class World : public WorldObject {
    // ステートフルに目まぐるしく変わる。
    CameraInterface *rendering_camera_ = nullptr;

    // メッシュ描画を一元管理するクラス
    friend class MeshComponent;
    MeshDrawManager mesh_draw_manager_;
    friend class WorldObject; // mesh_draw_manager_にアクセスするため

  public:
    GUI &gui;
    Timer timer;
    BufferedSet<std::function<void()> *> draws;
    BufferedSet<std::function<void()> *> updates;
    BufferedSet<Rigidbody *> rigidbodies;
    BufferedSet<RigidbodyComponent *> rigidbody_components;
    b2::World b2world;

    World(GUI &gui)
        : WorldObject(*this) // Worldにのみ許されたプライベートコンストラクタ
        , gui(gui) {
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
        info("Worldのデストラクタおわり");
    }

    World(const World &) = delete;
    World &operator=(const World &) const = delete;
    World(World &&) = delete;
    World &operator=(World &&) const = delete;

    void master_update() {
        trace("World::master_update開始:", this);
        updates.flush(); // フラッシュはここだけ
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

    void master_draw(CameraInterface &camera) {
        // 描画処理中だけrrendering_cameraが有効になる。
        rendering_camera_ = &camera;

        // ビューポートを設定
        GL::viewport(camera.viewport_provider());

        this->draws.foreach ([&](const auto *draw) {
            (*draw)();
        });

        // メッシュを描画
        this->mesh_draw_manager_.step();
        this->mesh_draw_manager_.draw_all_registered_objects(camera);

        // 描画処理が終わるとrendering_cameraは無効になる。
        rendering_camera_ = nullptr;
    }

    // void register_to_draw(const MeshObject &obj) {
    //     this->mesh_draw_manager_.register_to_draw(obj);
    // }

    // 描画処理の最中は、現在描画に用いているカメラへのポインタを返す。
    // 描画処理外ではnullptrを返す。
    CameraInterface *rendering_camera() {
        return this->rendering_camera_;
    }
};
