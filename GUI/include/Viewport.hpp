#pragma once

#include <SizeCallback.hpp>
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

class MaximumViewport : public Viewport, SizeCallback {
  public:
    MaximumViewport(Window &window)
        : Viewport(0, 0, 0, 0)
        , SizeCallback(window) {
        const auto fbsize = window.getFrameBufferSize();
        sizeCallback(fbsize.first, fbsize.second);
    }

    void sizeCallback(int width, int height) override;
};
