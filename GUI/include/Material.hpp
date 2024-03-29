#pragma once

#include "Shader.hpp"
#include "Window.hpp"

struct Material : public Resource {
    const ProgramObject &shader;
    int color; // 今のところはただのテスト用ダミー
    std::optional<GLuint> texture;
    double point_size;
    double line_width;

    Material(const ProgramObject *shader, int color, std::optional<GLuint> texture, double point_size, double line_width)
        : shader(shader == nullptr ? *this->get_window().default_shader : *shader)
        , color(color)
        , texture(texture)
        , point_size(point_size)
        , line_width(line_width) {
    }
};

class MaterialBuilder {
    const ProgramObject *shader_ = nullptr;
    int color_ = 0;
    std::optional<GLuint> texture_ = std::nullopt;
    double point_size_ = 4;
    double line_width_ = 4;

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

    MaterialBuilder point_size(double size) {
        this->point_size_ = size;
        return *this;
    }

    MaterialBuilder line_width(double width) {
        this->line_width_ = width;
        return *this;
    }

    Material &build(Window &window) {
        auto &material = window.append_resource<Material>(shader_, color_, texture_, point_size_, line_width_);
        return material;
    }
};
