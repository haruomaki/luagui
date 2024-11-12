#pragma once

#include "Resource.hpp"
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
    [[nodiscard]] glm::vec<2, GLint> get_size() const {
        return {width_, height_};
    }
};

// 常にウィンドウの描画領域全体のサイズとなるビューポート
// 手動でwindow.registerSizeCallbackするのではなく、create関数を使うと最初の即時設定ができる
class MaximumViewport : public Viewport, public Resource {
  public:
    MaximumViewport();
};
