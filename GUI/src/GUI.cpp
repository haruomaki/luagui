#include "GUI.hpp"
#include "World.hpp"

GUI::GUI() {
    // デフォルトシェーダの設定
    this->default_shader.emplace({GL::create_shader(GL_VERTEX_SHADER, load_string("assets/shaders/default.vsh")),
                                  GL::create_shader(GL_FRAGMENT_SHADER, load_string("assets/shaders/default.fsh"))});

    // デフォルトマテリアルの設定
    this->default_material = &MaterialBuilder().build(*this);
}

GUI::~GUI() {
    info("GUIのデストラクタ");
    this->resources_.clear(); // resource_updatesが消える前にResourceUpdateのデストラクタを呼ぶ
    this->worlds_.clear();    // key_callbacksが消える前にKeyCallbackObjectが消えないといけない
    info("GUIのデストラクタおわり");
}

World &GUI::create_world() {
    auto world = std::make_unique<World>(*this);
    worlds_.emplace_back(std::move(world));

    return *worlds_.back();
}

void GUI::default_routine1() {
    // 各ワールドの更新処理
    trace("[update] resource->《world》");
    for (const auto &world : this->worlds_) {
        world->master_update();
    }

    // 各ワールドの物理演算処理
    for (const auto &world : this->worlds_) {
        world->master_physics();
    }
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
