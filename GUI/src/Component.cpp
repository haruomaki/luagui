#include <Component.hpp>

Component::Component()
    : owner_(WorldObject::get_parent_static()) {
    if (owner_ == nullptr) {
        throw std::runtime_error("Componentの所有者の設定に失敗");
    }
}

void Component::erase() { owner().components_.request_erase(this); }

WorldObject &Component::owner() const { return *owner_; }
World &Component::world() const { return owner_->get_world(); }
Window &Component::window() const { return owner_->get_world().window; }

UpdateComponent::UpdateComponent(std::function<void(UpdateComponent &)> &&f) {
    this->func_ = [f, this] { trace("lambda from UpdateComponent (id: ", this->id, ")"); f(*this); trace("lambda ended"); };
    world().updates.request_set(&this->func_);
}
UpdateComponent::~UpdateComponent() {
    world().updates.request_erase(&this->func_);
}
