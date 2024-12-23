#pragma once

#include <SumiGL/Context.hpp>

struct GUI {
    GL::Context ctx;
    bool looping = false;
    long epoch = 0;

    // メインループに制御を移す。
    template <typename F = void (*)()>
    void mainloop(F &&custom_routine = [] {}) {
        if (looping) {
            throw std::runtime_error("すでにメインループが始まっています");
        }
        looping = true;

        // 描画のループ
        while (!ctx.windows.empty()) {
            epoch++;
            trace("[mainloop] メインループ：", epoch);

            // 受け取ったイベント（キーボードやマウス入力）を処理する
            // キー押下の瞬間などを捉えるために、ユーザ処理よりも前に置く
            glfwPollEvents();

            trace("[mainloop] カスタムルーチン開始：", epoch);
            custom_routine(); // 削除されたウィンドウへのアクセスを避けるため、ウィンドウ処理よりも前に置く
            trace("[mainloop] ウィンドウ更新開始：", epoch);
            // 登録されている各ウィンドウに対してルーティンを実行
            ctx.windows.foreach ([](const GL::Window *window) {
                window->routine();
            });
        }

        looping = false;
    }
};
