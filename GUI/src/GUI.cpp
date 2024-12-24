#include "GUI.hpp"
#include "World.hpp"

GUI::GUI() {
    // デフォルトシェーダの設定
    this->default_shader.emplace({GL::create_shader(GL_VERTEX_SHADER, load_string("assets/shaders/default.vsh")),
                                  GL::create_shader(GL_FRAGMENT_SHADER, load_string("assets/shaders/default.fsh"))});

    // デフォルトマテリアルの設定
    this->default_material = &MaterialBuilder().build(*this);
}

World &GUI::create_world() {
    auto world = std::make_unique<World>(*this);
    current_window->worlds_.emplace_back(std::move(world));

    return *current_window->worlds_.back();
}
