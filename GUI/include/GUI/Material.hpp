#pragma once

#include "GUI.hpp"
#include <SumiGL/Shader.hpp>

struct Material : public Resource {
    const GL::ProgramObject &shader;
    int priority;
    glm::vec4 base_color;
    std::optional<GLuint> texture;
    double point_size;
    double line_width;

    Material(const GL::ProgramObject *shader, int priority, const glm::vec4 &color, std::optional<GLuint> texture, double point_size, double line_width)
        : shader(shader == nullptr ? *this->get_gui().default_shader : *shader)
        , priority(priority)
        , base_color(color)
        , texture(texture)
        , point_size(point_size)
        , line_width(line_width) {
    }
};

class MaterialBuilder {
    const GL::ProgramObject *shader_ = nullptr;
    int priority_ = 0;
    glm::vec4 base_color_ = {1, 1, 1, 1};
    std::optional<GLuint> texture_ = std::nullopt;
    double point_size_ = 4;
    double line_width_ = 4;

  public:
    MaterialBuilder() = default;
    MaterialBuilder(Material &src)
        : shader_(&src.shader)
        , priority_(src.priority)
        , base_color_(src.base_color)
        , texture_(src.texture)
        , point_size_(src.point_size)
        , line_width_(src.line_width) {}

    MaterialBuilder shader(const GL::ProgramObject &shader) {
        this->shader_ = &shader;
        return *this;
    }

    MaterialBuilder priority(int priority) {
        this->priority_ = priority;
        return *this;
    }

    MaterialBuilder base_color(const glm::vec4 &color) {
        this->base_color_ = color;
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

    Material &build(GUI &gui) {
        auto &material = gui.append_resource<Material>(shader_, priority_, base_color_, texture_, point_size_, line_width_);
        return material;
    }
};
