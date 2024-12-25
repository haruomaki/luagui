#include "GUI.hpp"
#include "World.hpp"

GUI::GUI()
    : resources(*this) {
    // デフォルトシェーダの設定
    GL::ProgramObject pg{GL::create_shader(GL_VERTEX_SHADER, load_string("assets/shaders/default.vsh")),
                         GL::create_shader(GL_FRAGMENT_SHADER, load_string("assets/shaders/default.fsh"))};
    auto &default_shader = this->resources.append<Shader>(std::move(pg));
    default_shader.name = "default_shader";

    // デフォルトマテリアルの設定
    auto &default_material = MaterialBuilder().build(*this);
    default_material.name = "default_material";
}

GUI::~GUI() {
    info("GUIのデストラクタ");
    this->resources.clear(); // resource_updatesが消える前にResourceUpdateのデストラクタを呼ぶ
    this->worlds_.clear();   // key_callbacksが消える前にKeyCallbackObjectが消えないといけない
    info("GUIのデストラクタおわり");
}

World &GUI::create_world() {
    auto world = std::make_unique<World>(*this);
    worlds_.emplace_back(std::move(world));

    return *worlds_.back();
}

void GUI::default_routine1() {
    windows.flush();
    windows.foreach ([](Window *window) {
        // 閉じるべきウィンドウは閉じる
        if (window->should_close()) {
            window->destroy();
            return;
        }

        // フレームバッファサイズの更新
        window->update_routine();
    });

    // 各ワールドの更新処理
    trace("[update] resource->《world》");
    for (const auto &world : this->worlds_) {
        world->master_update();
    }

    // 各ワールドの物理演算処理
    for (const auto &world : this->worlds_) {
        world->master_physics();
    }

    // 描画と後処理
    windows.foreach ([](Window *window) {
        window->draw_routine();
        window->post_process();
    });
}

void GUI::default_routine2() {
    // rigidbody_components_（物理演算結果を物体の位置に反映するために管理）をフラッシュしておく。
    // 次フレームに同一アドレスの別コンポーネントが作られたときに、競合しないようにするため。
    for (std::unique_ptr<World> &world : this->worlds_) {
        world->rigidbody_components.flush();

        // 各種フラッシュ TODO: 場所はここでいい？
        world->draws.flush();
        world->updates.flush();
        world->rigidbodies.flush();
    }
}
