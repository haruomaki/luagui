#pragma once

#include <GUI.hpp>
#include <SizeCallback.hpp>

struct Viewport {
    GLint x, y;
    GLsizei width, height;

    Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
        : x(x)
        , y(y)
        , width(width)
        , height(height) {}

    void set() const {
        glViewport(x, y, width, height);
    }

    [[nodiscard]] glm::vec<2, GLint> getSize() const {
        return {width, height};
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
        x = y = 0;
        width = width;
        height = height;
        set();
    }
};
