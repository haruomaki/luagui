#pragma once

#include "Resource.hpp"
#include <SumiGL/Window.hpp>

// 常にウィンドウの描画領域全体のサイズとなるビューポート
// 手動でwindow.registerSizeCallbackするのではなく、create関数を使うと最初の即時設定ができる
class MaximumViewport : public GL::Viewport, public Resource {
  public:
    MaximumViewport();
};
