#pragma once

#include "Shader.hpp"
#include "Window.hpp"

struct Material : public Resource {
    const ProgramObject &shader;
    int color; // 今のところはただのテスト用ダミー
    std::optional<GLuint> texture;

    Material(const ProgramObject *shader, int color, std::optional<GLuint> texture)
        : shader(shader == nullptr ? *this->get_window().default_shader : *shader)
        , color(color)
        , texture(texture) {
    }
};

class MaterialBuilder {
    const ProgramObject *shader_ = nullptr;
    int color_ = 0;
    std::optional<GLuint> texture_ = std::nullopt;

  public:
    MaterialBuilder shader(const ProgramObject &shader) {
        this->shader_ = &shader;
        return *this;
    }

    MaterialBuilder color(int color) {
        this->color_ = color;
        return *this;
    }

    MaterialBuilder texture(GLuint tex_id) {
        this->texture_ = tex_id;
        return *this;
    }

    Material &build(Window &window) {
        auto &material = window.append_resource<Material>(shader_, color_, texture_);
        return material;
    }
};
