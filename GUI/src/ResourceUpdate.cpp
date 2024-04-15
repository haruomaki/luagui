#include "ResourceUpdate.hpp"
#include "Window.hpp"

ResourceUpdate::ResourceUpdate() {
    auto func = [this] { this->update(); };
    this->func_id_ = this->get_window().resource_updates.request_set_function(std::move(func));
}
ResourceUpdate::~ResourceUpdate() {
    this->get_window().resource_updates.request_erase_function(this->func_id_);
}
