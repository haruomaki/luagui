#include <Viewport.hpp>

void MaximumViewport::sizeCallback(int width, int height) {
    x_ = y_ = 0;
    this->width_ = width;
    this->height_ = height;
    set();
}
