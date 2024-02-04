#pragma once

#include <SizeCallback.hpp>
#include <Window.hpp>
#include <graphical_base.hpp>

// 描画領域（ビューポート）を管理するクラス
class Viewport {
  protected:
    GLint x_, y_;
    GLsizei width_, height_;

    Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
        : x_(x)
        , y_(y)
        , width_(width)
        , height_(height) {}

    // ビューポートを実際にセットする
    void set() const {
        glViewport(x_, y_, width_, height_);
    }

  public:
    // ビューポートの大きさを取得する
    [[nodiscard]] glm::vec<2, GLint> getSize() const {
        return {width_, height_};
    }
};

// 常にウィンドウの描画領域全体のサイズとなるビューポート
// 手動でwindow.registerSizeCallbackするのではなく、create関数を使うと最初の即時設定ができる
class MaximumViewport : public Viewport, public SizeCallback {
  public:
    MaximumViewport()
        : Viewport(0, 0, 0, 0) {
        this->size_callback = [](auto *thi, int width, int height) {
            auto *th = static_cast<MaximumViewport *>(thi);
            th->x_ = th->y_ = 0;
            th->width_ = width;
            th->height_ = height;
            th->set();
        };
    }

    // ビューポートの大きさ即時設定＆Windowに登録を一度に行うヘルパー関数
    static MaximumViewport &create(Window &window) {
        auto &viewport = window.makeChild<MaximumViewport>();
        const auto fbsize = window.getFrameBufferSize();
        viewport.size_callback(&viewport, fbsize.first, fbsize.second);
        return viewport;
    }
};
