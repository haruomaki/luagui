#include "GUI.hpp"
#include "Window.hpp"
#include "constants.hpp"

GUI::GUI() {
    // ライブラリglfw の初期化
    if (glfwInit() == 0) {
        throw std::runtime_error("GLFWの初期化に失敗しました");
    }

    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    monitor_ = glfwGetPrimaryMonitor();

    // DPIを計算
    auto [width_mm, height_mm] = this->monitor_physical_size();
    auto vm = this->video_mode();
    dpi_ = {float(vm.width) / float(width_mm) * 1000.f * inch_meter, float(vm.height) / float(height_mm) * 1000.f * inch_meter};

    // DPIに基づいて、実スケールでのピクセル寸法を計算する
    px_meter_ = {inch_meter / dpi_.first, inch_meter / dpi_.second};

    // vpixelも計算する
    auto [xscale, yscale] = this->monitor_content_scale();
    vpixel_ = {px_meter_.first / xscale, px_meter_.second / yscale};
}

GUI::~GUI() {
    glfwTerminate();
}

Window &GUI::create_window(int width, int height, const std::string &title) {
    auto window = std::make_unique<Window>(*this, width, height, title.c_str());
    this->windows_.push_back(std::move(window));
    return *this->windows_.back();
}

void GUI::refresh_windows() {
    // auto vvv = this->windows_[0];
    // this->windows_.clear();
    // this->windows_.push_back(vvv);

    // 閉じるべきウィンドウを見つけてvectorから削除
    trace("mainloop starts deleting windows");
    auto remove_begin = std::remove_if(this->windows_.begin(), this->windows_.end(), [](const auto &window) {
        return glfwWindowShouldClose(window->gwin_) != 0;
    });
    this->windows_.erase(remove_begin, this->windows_.end());
    trace("mainloop finished deleting windows");

    // 生きている各ウィンドウに対して更新および描画＆後処理
    for (const auto &window : this->windows_) {
        // 更新処理。physicsとupdateは順不同？
        trace("mainloop p1");
        window->physics_routine();
        trace("mainloop p2");
        window->update_routine();

        // 更新処理ののち描画。
        trace("mainloop p3");
        window->draw_routine();
        trace("mainloop p4");
        window->post_process();
        trace("mainloop p5");
    }
}
