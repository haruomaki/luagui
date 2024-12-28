#include "Camera.hpp"
#include "Window.hpp"
#include "World.hpp"
#include <cassert>

// カメラ優先度を初期設定するアルゴリズム。
static int calc_default_priority(Window &window) {
    GUI &gui = window.gui();
    if (gui.cameras.is_locked()) throw std::runtime_error("camerasのロック中にカメラを追加することはできません。");
    gui.cameras.flush();

    // priorityの最大値
    int max_priority = std::numeric_limits<int>::min();
    gui.cameras.foreach ([&](const CameraInterface *camera) {
        max_priority = std::max(max_priority, camera->priority);
    });

    // 新規ワールドの描画優先度は、max_priorityより少しだけ大きい10の倍数とする
    int one_level_higher = 0;
    if (max_priority >= 0) {
        one_level_higher = max_priority - (max_priority % 10) + 10;
    }
    return one_level_higher;
}

Camera::Camera(Window *window, ProjectionMode projection_mode)
    : CameraInterface(window)
    , projection_mode(projection_mode) {

    render = [this] {
        auto &world = this->world();
        world.master_draw(*this);
    };

    viewport_provider = [this] {
        if (!this->window) throw std::runtime_error("カメラとウィンドウが紐付いていません。");
        auto [w, h] = this->window->fbsize_cache;
        return GL::Viewport{.x = 0, .y = 0, .width = w, .height = h};
    };

    priority = calc_default_priority(*window);

    gui().cameras.request_set(this);
}

Camera::~Camera() {
    gui().cameras.request_erase(this);
}

glm::mat4 Camera::get_view_matrix() const {
    switch (projection_mode) {
    case Perspective:
        return SCALE({-1, 1, -1}) * glm::inverse(owner().get_absolute_transform());
    case Orthographic:
        return glm::inverse(owner().get_absolute_transform());
    }
    assert(false && "投影モードが未知の値です。");
}

glm::mat4 Camera::get_projection_matrix() const {
    auto vp = this->viewport_provider();
    auto width = float(vp.width);
    auto height = float(vp.height);

    if (projection_mode == Perspective) {
        const auto aspect_ratio = width / height;
        const auto r = owner().scale_prop;
        const auto projection_matrix = glm::perspective(glm::radians(60.0F), aspect_ratio, 0.01F * r, 1000.F * r);
        return projection_matrix;
    }

    auto ms = gui().master_scale();
    const auto w = width * ms.x;
    const auto h = height * ms.y;
    const auto r = owner().get_scale().z;
    const auto ne = -1000.0f * r; // "near"はWindowsだとダメ
    const auto fa = 1000.0f * r;  // "far"もWindowsだとダメ
    switch (mode) {
    case CameraMode::Center:
        return glm::ortho(-w / 2, w / 2, -h / 2, h / 2, ne, fa);
    case CameraMode::TopRight:
        return glm::ortho(-w, 0.0f, -h, 0.0f, ne, fa);
    case CameraMode::BottomRight:
        return glm::ortho(-w, 0.0f, 0.0f, h, ne, fa);
    case CameraMode::TopLeft:
        return glm::ortho(0.0f, w, -h, 0.0f, ne, fa);
    case CameraMode::BottomLeft:
        return glm::ortho(0.0f, w, 0.0f, h, ne, fa);
        // case CameraMode::Custom:
        //     // カスタム設定に基づく計算
        //     return glm::ortho(
        //         custom_origin.x - w / 2 * custom_scale,
        //         custom_origin.x + w / 2 * custom_scale,
        //         custom_origin.y - h / 2 * custom_scale,
        //         custom_origin.y + h / 2 * custom_scale,
        //         custom_origin.z - 1000.0f,
        //         custom_origin.z + 1000.0f);
    }
    throw std::runtime_error("Unsupported camera mode");
}
