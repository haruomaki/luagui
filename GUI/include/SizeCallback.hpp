#pragma once

#include <GUI.hpp>

class SizeCallback {
    GUI &window_;
    std::function<void(int, int)> size_callback_;

  public:
    virtual void sizeCallback(int width, int height) = 0;
    SizeCallback(GUI &window);
    virtual ~SizeCallback();
};
