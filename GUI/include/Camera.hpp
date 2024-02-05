#pragma once

#include <Viewport.hpp>
#include <WorldObject.hpp>

// ビュー行列と射影行列を与える機能を持つワールドオブジェクト
class Camera : public WorldObject {
  public:
    Camera(World &world)
        : WorldObject(world) {}
    virtual ~Camera() = default;
    [[nodiscard]] virtual glm::mat4 get_view_matrix() const = 0;
    [[nodiscard]] virtual glm::mat4 get_projection_matrix() const = 0;

    // このカメラをそれが属するワールドのアクティブカメラとする
    void set_active();
};

class NormalCamera : public Camera {
    const Viewport &viewport_;

  public:
    NormalCamera(World &world, const Viewport &viewport)
        : Camera(world)
        , viewport_(viewport) {}

    [[nodiscard]] glm::mat4 get_view_matrix() const override;
    [[nodiscard]] glm::mat4 get_projection_matrix() const override;
};

// 1ピクセルがfloat値1のスケールの正射影カメラ
class OrthoCamera : public Camera {
    const Viewport &viewport_;

  public:
    OrthoCamera(World &world, const Viewport &viewport)
        : Camera(world)
        , viewport_(viewport) {}

    [[nodiscard]] glm::mat4 get_view_matrix() const override;
    [[nodiscard]] glm::mat4 get_projection_matrix() const override;
};
