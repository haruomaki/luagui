#include "Component.hpp"
#include "World.hpp"

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
