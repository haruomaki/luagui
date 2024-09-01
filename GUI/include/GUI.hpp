#pragma once

#include <graphical_base.hpp>

class Window;

class GUI {
    std::vector<std::unique_ptr<Window>> windows_ = {};
    bool looping_ = false;
    GLFWmonitor *monitor_ = nullptr; // 初期化時のプライマリモニターを保持

  public:
    int tick = 0;

    GUI();
    ~GUI();

    // コピーコンストラクタ、コピー代入演算子を削除
    GUI(const GUI &) = delete;
    GUI &operator=(const GUI &) const = delete;

    // ムーブコンストラクタ、ムーブ代入演算子を削除
    GUI(GUI &&) = delete;
    GUI &operator=(GUI &&) const = delete;

    // ウィンドウを新規作成する。
    Window &create_window(int width, int height, const std::string &title);

    // 毎フレーム呼び出してウィンドウの更新＆イベント処理を行う。
    // 直接触らず、`mainloop()`関数から呼び出すことを推奨。
    void refresh_windows();

    // メインループに制御を移す。
    template <typename F = void (*)()>
    void mainloop(F &&custom_routine = [] {}) {
        if (looping_) {
            throw std::runtime_error("すでにメインループが始まっています");
        }
        looping_ = true;

        // 描画のループ
        while (!this->windows_.empty()) {
            tick++;
            this->refresh_windows();
            custom_routine();
        }

        looping_ = false;
    }

    // 戻り値の参照は指定したモニターが切断されるか、ライブラリが終了するまで有効
    [[nodiscard]] const GLFWvidmode &video_mode() const {
        // NOTE: メインモニターの情報しか取得できないことに注意
        return *glfwGetVideoMode(monitor_);
    }
};
