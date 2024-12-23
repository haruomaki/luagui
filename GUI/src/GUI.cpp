#include "GUI.hpp"
#include "Material.hpp"
#include "base.hpp"

GUI::GUI() {
    // デフォルトシェーダの設定
    this->default_shader.emplace({GL::create_shader(GL_VERTEX_SHADER, load_string("assets/shaders/default.vsh")),
                                  GL::create_shader(GL_FRAGMENT_SHADER, load_string("assets/shaders/default.fsh"))});

    // デフォルトマテリアルの設定
    this->default_material = &MaterialBuilder().build(*this);
}
