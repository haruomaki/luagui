#pragma once

#include "Component.hpp"
#include "WorldObject.hpp"

// ビュー行列と射影行列を与える機能を持つワールドオブジェクト
struct CameraInterface {
    virtual ~CameraInterface() = default;
    [[nodiscard]] virtual glm::mat4 get_view_matrix() const = 0;
    [[nodiscard]] virtual glm::mat4 get_projection_matrix() const = 0;
};

class NormalCamera : public CameraInterface, virtual public WorldObject {
  public:
    NormalCamera() = default;

    [[nodiscard]] glm::mat4 get_view_matrix() const override;
    [[nodiscard]] glm::mat4 get_projection_matrix() const override;
};

// float値1が物理ディスプレイ上での1mというスケールの正射影カメラ
class OrthoCamera : public CameraInterface, public Component {
  public:
    enum CameraMode {
        Center,      // 画面中心
        TopRight,    // 右上
        BottomRight, // 右下
        TopLeft,     // 左上
        BottomLeft,  // 左下
        // Custom       // カスタムモード（詳細は別途指定）
    };

    CameraMode mode = Center;

    OrthoCamera() = default;

    [[nodiscard]] glm::mat4 get_view_matrix() const override;
    [[nodiscard]] glm::mat4 get_projection_matrix() const override;
};
