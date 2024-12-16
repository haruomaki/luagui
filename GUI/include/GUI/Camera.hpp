#pragma once

#include "Viewport.hpp"
#include "WorldObject.hpp"

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
  public:
    NormalCamera() = default;

    [[nodiscard]] glm::mat4 get_view_matrix() const override;
    [[nodiscard]] glm::mat4 get_projection_matrix() const override;
};

// float値1が物理ディスプレイ上での1mというスケールの正射影カメラ
class OrthoCamera : public Camera {
  public:
    OrthoCamera() = default;

    [[nodiscard]] glm::mat4 get_view_matrix() const override;
    [[nodiscard]] glm::mat4 get_projection_matrix() const override;
};
