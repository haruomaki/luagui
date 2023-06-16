#pragma once

#include <GUI.hpp>

void masterDraw();

class Draw {
    std::function<void()> draw_;

  public:
    virtual void draw() const = 0;
    Draw();
    ~Draw();
};
