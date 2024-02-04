#pragma once

#include <Window.hpp>

class GUI {
    std::vector<Window> windows_ = {};
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

    Window &createWindow(int width, int height, const std::string &title) {
        this->windows_.emplace_back(width, height, title.c_str());
        return this->windows_.back();
    }

    void mainloop(const std::function<void()> &callback) {
        if (looping_) {
            throw std::runtime_error("すでにメインループが始まっています");
        }
        looping_ = true;

        // 描画のループ
        while (!this->windows_.empty()) {
            tick++;

            // 閉じるべきウィンドウを見つけてvectorから削除
            auto remove_begin = std::remove_if(this->windows_.begin(), this->windows_.end(), [](const Window &window) {
                return glfwWindowShouldClose(window.gwin_) != 0;
            });
            this->windows_.erase(remove_begin, this->windows_.end());

            // 生きている各ウィンドウに対して更新および描画
            for (const Window &window : this->windows_) {
                // // WorldObjectの更新 TODO: 一フレームごとに更新 vs setPosition()ごとに更新（重いかも）
                // world_object_root_.refreshAbsolutePosition();

                // 更新処理
                for (auto *update : window.updates_) {
                    (*update)();
                }

                // OpenGLの描画関数のターゲットにするウィンドウを指定
                glfwMakeContextCurrent(window.gwin_);

                // 画面の初期化
                constexpr RGBA bg_color{0.2f, 0.2f, 0.2f, 1};
                glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // ユーザの描画関数
                // glDisable(GL_DEPTH_TEST);
                glClear(GL_DEPTH_BUFFER_BIT);
                callback();
                // glEnable(GL_DEPTH_TEST);

                // 上記描画した図形を表画面のバッファにスワップする
                glfwSwapBuffers(window.gwin_);
            }

            // 受け取ったイベント（キーボードやマウス入力）を処理する
            glfwPollEvents();
        }

        looping_ = false;
    }
};
