#pragma once

#include "GUI.hpp"
#include "Shader.hpp"
#include <SumiGL/Texture.hpp>

struct Material : public Resource {
    const Shader &shader;
    int priority;
    glm::vec4 base_color;
    GL::Texture texture;
    double point_size;
    double line_width;

    Material(const Shader *shader, int priority, const glm::vec4 &color, GL::Texture &&texture, double point_size, double line_width)
        : shader(shader == nullptr ? *gui().resources.find<Shader>("default_shader") : *shader)
        , priority(priority)
        , base_color(color)
        , texture(std::move(texture))
        , point_size(point_size)
        , line_width(line_width) {
    }
};

class MaterialBuilder {
    const Shader *shader_ = nullptr;
    int priority_ = 0;
    glm::vec4 base_color_ = {1, 1, 1, 1};
    GL::Texture texture_;
    double point_size_ = 4;
    double line_width_ = 4;

  public:
    MaterialBuilder() = default;
    MaterialBuilder(Material &src)
        : shader_(&src.shader)
        , priority_(src.priority)
        , base_color_(src.base_color)
        , texture_(std::move(src.texture))
        , point_size_(src.point_size)
        , line_width_(src.line_width) {}

    MaterialBuilder shader(const Shader &shader) {
        this->shader_ = &shader;
        return std::move(*this);
    }

    MaterialBuilder priority(int priority) {
        this->priority_ = priority;
        return std::move(*this);
    }

    MaterialBuilder base_color(const glm::vec4 &color) {
        this->base_color_ = color;
        return std::move(*this);
    }

    MaterialBuilder texture(GL::Texture &&tex) {
        this->texture_ = std::move(tex);
        return std::move(*this);
    }

    MaterialBuilder point_size(double size) {
        this->point_size_ = size;
        return std::move(*this);
    }

    MaterialBuilder line_width(double width) {
        this->line_width_ = width;
        return std::move(*this);
    }

    Material &build(GUI &gui) {
        auto material = gui.resources.append<Material>(shader_, priority_, base_color_, std::move(texture_), point_size_, line_width_);
        return material.get();
    }
};
