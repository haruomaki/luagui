#pragma once

#include "Shader.hpp"
#include "Window.hpp"

struct Material : public Resource {
    const ProgramObject &shader;
    int color; // 今のところはただのテスト用ダミー

    Material(const ProgramObject *shader, int color)
        : shader(shader == nullptr ? *this->get_window().default_shader : *shader)
        , color(color) {
    }
};

class MaterialBuilder {
    const ProgramObject *shader_ = nullptr;
    int color_ = 0;

  public:
    MaterialBuilder shader(const ProgramObject &shader) {
        this->shader_ = &shader;
        return *this;
    }

    MaterialBuilder color(int color) {
        this->color_ = color;
        return *this;
    }

    Material &build(Window &window) {
        auto &material = window.append_child<Material>(this->shader_, this->color_);
        return material;
    }
};
