#pragma once

#include <GUI.hpp>
#include <SizeCallback.hpp>

struct Viewport {
    GLint x_, y_;
    GLsizei width_, height_;

    Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
        : x_(x)
        , y_(y)
        , width_(width)
        , height_(height) {}

    void set() const {
        glViewport(x_, y_, width_, height_);
    }

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

    void sizeCallback(int width, int height) override {
        x_ = y_ = 0;
        width_ = width;
        height_ = height;
        set();
    }
};
