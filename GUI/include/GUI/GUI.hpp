#pragma once

#include "Resource.hpp"
#include <SumiGL/Context.hpp>
#include <SumiGL/buffered_container.hpp>

struct Material;
class World;
class Window;
struct CameraInterface;

class GUI : public GL::Context {
    bool looping_ = false;
    long epoch_ = 0;

    std::vector<std::unique_ptr<World>> worlds_;

  public:
    BufferedSet<CameraInterface *> cameras;
    BufferedSet<Window *> windows;
    ResourceManager resources;
    BufferedSet<std::function<void()> *> resource_updates;

    GUI();
    ~GUI();
    GUI(GUI &) = delete;
    GUI &operator=(GUI &) = delete;
    GUI(GUI &&) = delete;
    GUI &operator=(GUI &&) = delete;

    // メインループに制御を移す。
    template <typename F = void (*)()>
    void mainloop(F &&custom_routine = [] {}) {
        if (looping_) {
            throw std::runtime_error("すでにメインループが始まっています");
        }
        looping_ = true;

        // 描画のループ
        while (!windows.empty()) {
            epoch_++;
            trace("[mainloop] メインループ：", epoch_);

            // 受け取ったイベント（キーボードやマウス入力）を処理する
            // キー押下の瞬間などを捉えるために、ユーザ処理よりも前に置く
            glfwPollEvents();

            trace("[mainloop] カスタムルーチン開始：", epoch_);
            std::forward<F>(custom_routine)(); // 削除されたウィンドウへのアクセスを避けるため、ウィンドウ処理よりも前に置く
            trace("[mainloop] ウィンドウ更新開始：", epoch_);

            // リソースの更新処理
            trace("[update] 《resource》->world");
            this->resource_updates.foreach ([](const auto *update) {
                (*update)();
            });

            default_routine1();

            default_routine2();

            // フラッシュ TODO: 場所はここでいい？
            resource_updates.flush();
        }

        looping_ = false;
    }

    // 経過フレーム数を取得する
    [[nodiscard]] long epoch() const { return epoch_; }

    World &create_world();
    void default_routine1();
    void default_routine2();
};
