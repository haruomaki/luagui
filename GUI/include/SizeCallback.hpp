#pragma once

#include <Window.hpp>

class SizeCallback {
    Window &window_;
    std::function<void(int, int)> size_callback_;

  public:
    virtual void sizeCallback(int width, int height) = 0;
    SizeCallback(Window &window);
    ~SizeCallback();
};
