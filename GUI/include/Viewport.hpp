#pragma once

#include <GUI.hpp>
#include <SizeCallback.hpp>

struct Viewport {
    GLint x_, y_;
    GLsizei width_, height_;

    void set() const {
        glViewport(x_, y_, width_, height_);
    }
};

class MaximumViewport : public Viewport, SizeCallback {
    void sizeCallback(int width, int height) override {
        x_ = y_ = 0;
        width_ = width;
        height_ = height;
        set();
    }
};
