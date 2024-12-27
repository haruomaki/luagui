#include "ResourceUpdate.hpp"
#include "GUI.hpp"

ResourceUpdate::ResourceUpdate() {
    this->func_ = [this] { this->update(); };
    this->gui().resource_updates.request_set(&this->func_);
}
ResourceUpdate::~ResourceUpdate() {
    this->gui().resource_updates.request_erase(&this->func_);
}
