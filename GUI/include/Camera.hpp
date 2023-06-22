#pragma once

#include <Viewport.hpp>
#include <WorldObject.hpp>

class Camera : public WorldObject {
  protected:
    const Viewport &viewport_;
    float aspect_ratio_ = 1;

  public:
    Camera(const Viewport &viewport)
        : viewport_(viewport) {}
    // glm::mat4 view_matrix_ = glm::mat4(1);
    // glm::mat4 projection_matrix_ = glm::perspective(glm::radians(90.0F), 1.f, 0.1F, 1000.f);
    // glm::mat4 projection_matrix_ = glm::ortho(-1, 1, -1, 1, 1, -1);

    [[nodiscard]] glm::mat4 getViewMatrix() const {
        return glm::inverse(getAbsoluteTransform());
    }

    [[nodiscard]] virtual glm::mat4 getProjectionMatrix() const = 0;
};

class NormalCamera : public Camera {
  public:
    NormalCamera(const Viewport &viewport)
        : Camera(viewport) {}

    [[nodiscard]] glm::mat4 getProjectionMatrix() const override {
        auto size = viewport_.getSize();
        int width = size.x;
        int height = size.y;

        const auto aspect_ratio = (float)width / height;
        // projection_matrix_ = glm::ortho<float>(-width / 2, width / 2, -height / 2, height / 2, 1000, 0);
        const auto projection_matrix = glm::perspective(glm::radians(60.0F), aspect_ratio, 0.1F, 3000.F);
        // debug(projection_matrix_);
        return projection_matrix;
    }
};

class OrthoCamera : public Camera {
  public:
    OrthoCamera(const Viewport &viewport)
        : Camera(viewport) {}

    [[nodiscard]] glm::mat4 getProjectionMatrix() const override {
        auto size = viewport_.getSize();
        int width = size.x;
        int height = size.y;

        const auto w = float(width);
        const auto h = float(height);
        const auto projection_matrix = glm::ortho<float>(-w / 2, w / 2, -h / 2, h / 2, 1000.F, -1000.F);
        return projection_matrix;
    }
};
