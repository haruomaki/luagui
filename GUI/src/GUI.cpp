#include "GUI.hpp"
#include "Window.hpp"

GUI::GUI() {
    // ライブラリglfw の初期化
    if (glfwInit() == 0) {
        throw std::runtime_error("GLFWの初期化に失敗しました");
    }

    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    monitor_ = glfwGetPrimaryMonitor();
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

    // 生きている各ウィンドウに対して更新および描画
    for (const auto &window : this->windows_) {
        trace("mainloop p1");
        window->update_routine();
        trace("mainloop p2");
        window->physics_routine();
        trace("mainloop p3");
        window->draw_routine();
        trace("mainloop p4");
    }

    // 受け取ったイベント（キーボードやマウス入力）を処理する
    glfwPollEvents();
}
