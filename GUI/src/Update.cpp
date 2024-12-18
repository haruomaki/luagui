#include "Update.hpp"
#include "World.hpp"

Update::Update() {
    this->func_ = [this] { trace("Updateクラスによるアップデート開始");this->update(); };
    this->get_world().updates.request_set(&this->func_);
}
Update::~Update() {
    this->get_world().updates.request_erase(&this->func_);
}
Update::Update(const Update & /*other*/) {
    this->func_ = [this] { this->update(); };
    this->get_world().updates.request_set(&this->func_);
}

UpdateComponent::UpdateComponent(std::function<void(UpdateComponent &)> &&f) {
    this->func_ = [f, this] { trace("lambda from UpdateComponent (id: ", this->id, ")"); f(*this); trace("lambda ended"); };
    world().updates.request_set(&this->func_);
}
UpdateComponent::~UpdateComponent() {
    world().updates.request_erase(&this->func_);
}
