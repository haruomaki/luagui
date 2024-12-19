#include "Update.hpp"
#include "World.hpp"

UpdateComponent::UpdateComponent(std::function<void(UpdateComponent &)> &&f) {
    this->func_ = [f, this] { trace("lambda from UpdateComponent (id: ", this->id, ")"); f(*this); trace("lambda ended"); };
    world().updates.request_set(&this->func_);
}
UpdateComponent::~UpdateComponent() {
    world().updates.request_erase(&this->func_);
}
