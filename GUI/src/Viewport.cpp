#include <Viewport.hpp>

void MaximumViewport::sizeCallback(int width, int height) {
    x = y = 0;
    this->width = width;
    this->height = height;
    set();
}
