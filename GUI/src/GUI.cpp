#include "GUI.hpp"
#include "World.hpp"

Window &GUI::create_window(int width, int height, const std::string &title) {
    auto window = std::make_unique<Window>(width, height, title.c_str());
    this->windows_.push_back(std::move(window));
    return *this->windows_.back();
}

void GUI::mainloop(const std::function<void()> &callback) {
    if (looping_) {
        throw std::runtime_error("すでにメインループが始まっています");
    }
    looping_ = true;

    // 描画のループ
    while (!this->windows_.empty()) {
        tick++;

        // auto vvv = this->windows_[0];
        // this->windows_.clear();
        // this->windows_.push_back(vvv);

        // 閉じるべきウィンドウを見つけてvectorから削除
        auto remove_begin = std::remove_if(this->windows_.begin(), this->windows_.end(), [](const auto &window) {
            return glfwWindowShouldClose(window->gwin_) != 0;
        });
        this->windows_.erase(remove_begin, this->windows_.end());

        // 生きている各ウィンドウに対して更新および描画
        for (const auto &window : this->windows_) {
            window->update_routine();
            window->draw_routine();
            callback();
        }

        // 受け取ったイベント（キーボードやマウス入力）を処理する
        glfwPollEvents();
    }

    looping_ = false;
}
