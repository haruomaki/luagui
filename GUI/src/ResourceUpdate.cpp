#include "ResourceUpdate.hpp"
#include "Window.hpp"

ResourceUpdate::ResourceUpdate() {
    this->func_ = [this] { this->update(); };
    this->get_window().resource_updates.request_set(&this->func_);
}
ResourceUpdate::~ResourceUpdate() {
    this->get_window().resource_updates.request_erase(&this->func_);
}
