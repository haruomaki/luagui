#pragma once

#include "Resource.hpp"
#include <SumiGL/Window.hpp>

class GUI;
struct CameraInterface;

// 一つのウィンドウを表すクラス
class Window : public GL::Window {
    GUI &gui_;
    using KeyArray = std::array<bool, 512>;
    KeyArray key_down_{}, key_up_{};
    std::pair<double, double> last_cursor_ = cursor_pos();

  public:
    void update_routine();
    void post_process();

    // ウィンドウ内描画領域の大きさを表す変数。framebuffer_size()は直接取得する（故に重い）のに対し、ここには毎フレーム自動で取得されたものがキャッシュされている。
    std::pair<int, int> fbsize_cache = {0, 0};

    Window(GUI &gui, int width, int height, const std::string &title);
    ~Window();

    // コピーもムーブも禁止する
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    [[nodiscard]] GUI &gui() const { return gui_; }

    void close();

    // 前フレームからのカーソルの移動差分を得る。
    [[nodiscard]] std::pair<double, double> cursor_diff() const {
        auto [x, y] = cursor_pos();
        return {x - last_cursor_.first, y - last_cursor_.second};
    }

    // 現在フレームにおける、キー変更イベント（押した）の有無を表す
    [[nodiscard]] const KeyArray &key_down() const { return key_down_; }

    // 現在フレームにおける、キー変更イベント（離した）の有無を表す
    [[nodiscard]] const KeyArray &key_up() const { return key_up_; }
};
