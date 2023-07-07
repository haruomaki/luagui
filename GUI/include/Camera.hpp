#pragma once

#include <Viewport.hpp>
#include <WorldObject.hpp>

// ビュー行列と射影行列を与える機能を持つ抽象クラス
class Camera {
  public:
    virtual ~Camera() = default;
    [[nodiscard]] virtual glm::mat4 getViewMatrix() const = 0;
    [[nodiscard]] virtual glm::mat4 getProjectionMatrix() const = 0;
};

class NormalCamera : public Camera, public WorldObject {
    const Viewport &viewport_;

  public:
    NormalCamera(World &world, const Viewport &viewport)
        : WorldObject(world)
        , viewport_(viewport) {}

    [[nodiscard]] glm::mat4 getViewMatrix() const override {
        return SCALE({-1, 1, -1}) * glm::inverse(getAbsoluteTransform());
    }

    [[nodiscard]] glm::mat4 getProjectionMatrix() const override {
        auto size = viewport_.getSize();
        auto width = float(size.x);
        auto height = float(size.y);

        const auto aspect_ratio = width / height;
        // projection_matrix_ = glm::ortho<float>(-width / 2, width / 2, -height / 2, height / 2, 1000, 0);
        const auto projection_matrix = glm::perspective(glm::radians(60.0F), aspect_ratio, 0.1F, 3000.F);
        // debug(projection_matrix_);
        return projection_matrix;
    }
};

// 1ピクセルがfloat値1のスケールの正射影カメラ
class OrthoCamera : public Camera, public WorldObject {
    const Viewport &viewport_;

  public:
    OrthoCamera(World &world, const Viewport &viewport)
        : WorldObject(world)
        , viewport_(viewport) {}

    [[nodiscard]] glm::mat4 getViewMatrix() const override {
        return glm::inverse(getAbsoluteTransform());
    }

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
