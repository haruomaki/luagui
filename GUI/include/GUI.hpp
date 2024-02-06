#pragma once

#include "Window.hpp"

class GUI {
    std::vector<std::unique_ptr<Window>> windows_ = {};
    bool looping_ = false;

  public:
    int tick = 0;

    GUI() {
        // ライブラリglfw の初期化
        if (glfwInit() == 0) {
            throw std::runtime_error("GLFWの初期化に失敗しました");
        }

        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    }

    ~GUI() { glfwTerminate(); }

    // デフォルトのコピーコンストラクタ、コピー代入演算子
    GUI(const GUI &) = default;
    GUI &operator=(const GUI &) = default;

    // デフォルトのムーブコンストラクタ、ムーブ代入演算子
    GUI(GUI &&) = default;
    GUI &operator=(GUI &&) = default;

    Window &create_window(int width, int height, const std::string &title);
    void mainloop(const std::function<void()> &callback);
};
