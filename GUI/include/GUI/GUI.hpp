#pragma once

#include "ID.hpp"
#include "Resource.hpp"
#include "sound.hpp"
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
    AudioManager audio;
    BufferedSet<std::function<void()> *> resource_updates;
    IDGeneragor idg;
    std::function<bool()> resume_condition;

    GUI();
    ~GUI();
    GUI(GUI &) = delete;
    GUI &operator=(GUI &) = delete;
    GUI(GUI &&) = delete;
    GUI &operator=(GUI &&) = delete;

    // ワールドとリソースを完全に削除する。
    void cleanup();

    // メインループに制御を移す。
    template <typename F = void (*)()>
    void mainloop(F &&custom_routine = [] {}) {
        if (looping_) {
            throw std::runtime_error("すでにメインループが始まっています");
        }
        looping_ = true;

        // 描画のループ
        while (resume_condition()) {
            trace("[mainloop] メインループ：", epoch_);

            trace("[mainloop] カスタムルーチン開始：", epoch_);
            std::forward<F>(custom_routine)(); // 削除されたウィンドウへのアクセスを避けるため、ウィンドウ処理よりも前に置く
            trace("[mainloop] ウィンドウ更新開始：", epoch_);

            default_routine1();

            epoch_++;
        }

        looping_ = false;
    }

    // 経過フレーム数を取得する
    [[nodiscard]] long epoch() const { return epoch_; }

    World &create_world();
    void default_routine1();
};
