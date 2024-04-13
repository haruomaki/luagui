#pragma once

#include <graphical_base.hpp>

class Window;

class GUI {
    std::vector<std::unique_ptr<Window>> windows_ = {};
    bool looping_ = false;
    GLFWmonitor *monitor_ = nullptr; // 初期化時のプライマリモニターを保持

  public:
    int tick = 0;

    GUI() {
        // ライブラリglfw の初期化
        if (glfwInit() == 0) {
            throw std::runtime_error("GLFWの初期化に失敗しました");
        }

        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
        monitor_ = glfwGetPrimaryMonitor();
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

    // 戻り値の参照は指定したモニターが切断されるか、ライブラリが終了するまで有効
    [[nodiscard]] const GLFWvidmode &video_mode() const {
        // NOTE: メインモニターの情報しか取得できないことに注意
        return *glfwGetVideoMode(monitor_);
    }
};
