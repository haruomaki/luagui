#include "Component.hpp"
#include "GUI.hpp"
#include "World.hpp"

Component::Component()
    : owner_(WorldObject::get_parent_static()) {
    if (owner_ == nullptr) {
        throw std::runtime_error("Componentの所有者の設定に失敗");
    }
    id = gui().idg.generate();
}

void Component::erase() { owner().components_.request_erase(this); }

WorldObject &Component::owner() const { return *owner_; }
World &Component::world() const { return owner_->get_world(); }
GUI &Component::gui() const { return owner_->get_world().gui; }
