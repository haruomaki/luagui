#include <Component.hpp>
#include <WorldObject.hpp>

Component::Component()
    : owner_(WorldObject::get_parent_static()) {
    if (owner_ == nullptr) {
        throw std::runtime_error("Componentの所有者の設定に失敗");
    }
}

UpdateComponent::UpdateComponent(std::function<void(UpdateComponent &)> &&f) {
    this->func_ = [f, this] { f(*this); };
    print("UpdateComponentの関数登録開始");
    get_owner()->get_world().updates.request_set(&this->func_);
    print("UpdateComponentの関数登録完了");
}
UpdateComponent::~UpdateComponent() {
    get_owner()->get_world().updates.request_erase(&this->func_);
}
