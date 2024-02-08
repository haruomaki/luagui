#pragma once

#include "Shader.hpp"
#include "graphical_base.hpp"

class Window;

class GUI {
    std::vector<std::unique_ptr<Window>> windows_ = {};
    bool looping_ = false;

  public:
    int tick = 0;
    std::optional<ProgramObject> default_shader;

    GUI() {
        // ライブラリglfw の初期化
        if (glfwInit() == 0) {
            throw std::runtime_error("GLFWの初期化に失敗しました");
        }

        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    }

    ~GUI() { glfwTerminate(); }

    // コピーコンストラクタ、コピー代入演算子を削除
    GUI(const GUI &) = delete;
    GUI &operator=(const GUI &) const = delete;

    // ムーブコンストラクタ、ムーブ代入演算子を削除
    GUI(GUI &&) = delete;
    GUI &operator=(GUI &&) const = delete;

    Window &create_window(int width, int height, const std::string &title);
    void mainloop();
};
