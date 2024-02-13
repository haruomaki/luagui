#pragma once

#include <Viewport.hpp>
#include <WorldObject.hpp>

// ビュー行列と射影行列を与える機能を持つワールドオブジェクト
class Camera : virtual public WorldObject {
  public:
    Camera() = default;
    [[nodiscard]] virtual glm::mat4 get_view_matrix() const = 0;
    [[nodiscard]] virtual glm::mat4 get_projection_matrix() const = 0;

    // このカメラをそれが属するワールドのアクティブカメラとする
    void set_active();
};

class NormalCamera : public Camera {
    const Viewport &viewport_;

  public:
    NormalCamera(const Viewport &viewport)
        : viewport_(viewport) {}
    // virtual ~NormalCamera() = default;

    [[nodiscard]] glm::mat4 get_view_matrix() const override;
    [[nodiscard]] glm::mat4 get_projection_matrix() const override;
};

constexpr float inch_meter = 0.0254f;       // 1インチは2.54cm
constexpr float px_meter = inch_meter / 72; // 72dpiのピクセル幅

// 1ピクセルがfloat値0.0003528のスケールの正射影カメラ
class OrthoCamera : public Camera {
    const Viewport &viewport_;

  public:
    OrthoCamera(const Viewport &viewport)
        : viewport_(viewport) {}
    // virtual ~OrthoCamera() = default;

    [[nodiscard]] glm::mat4 get_view_matrix() const override;
    [[nodiscard]] glm::mat4 get_projection_matrix() const override;
};
