#include "Viewport.hpp"
#include "Window.hpp"

MaximumViewport::MaximumViewport()
    : Viewport(0, 0, 0, 0) {

    auto size_callback = [this](int width, int height) {
        this->x_ = this->y_ = 0;
        this->width_ = width;
        this->height_ = height;
        this->set();
    };
    const auto fbsize = this->get_window().get_frame_buffer_size();
    size_callback(fbsize.first, fbsize.second);
    this->get_window().set_callback<CallbackKind::Size>(std::move(size_callback));
}
