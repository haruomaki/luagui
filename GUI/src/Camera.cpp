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
    // ビュー行列を「カメラのモデル行列の逆行列」と愚直に定義してしまうと、カメラが後ろに付いているような視界になってしまう。
    // {-1, 1, -1}を掛けることでカメラ物体の前方を撮影するようになる。
    return SCALE({-1, 1, -1}) * glm::inverse(owner().get_absolute_transform());
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
    const auto left = -w * (1 + centering.x) / 2;
    const auto right = w * (1 - centering.x) / 2;
    const auto bottom = -h * (1 + centering.y) / 2;
    const auto top = h * (1 - centering.y) / 2;
    return glm::ortho(left, right, bottom, top, ne, fa);
}

void Camera::toggle_mode() { projection_mode = (projection_mode == Camera::Perspective ? Camera::Orthographic : Camera::Perspective); }
