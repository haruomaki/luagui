#include "ResourceUpdate.hpp"
#include "Window.hpp"

ResourceUpdate::ResourceUpdate() {
    auto func = [this] { this->update(); };
    this->func_id_ = this->get_window().resource_updates.set_function(std::move(func));
}
ResourceUpdate::~ResourceUpdate() {
    this->get_window().resource_updates.erase_function(this->func_id_);
}
