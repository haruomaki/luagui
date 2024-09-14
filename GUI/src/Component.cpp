#include <Component.hpp>

Component::Component()
    : owner_(WorldObject::get_parent_static()) {
    if (owner_ == nullptr) {
        throw std::runtime_error("Componentの所有者の設定に失敗");
    }
}

bool Component::erase() {
    auto *ptr_to_erase = this;

    // 生ポインタを使用して要素を削除する
    auto &candidates = get_owner()->components_;
    auto it = candidates.begin();
    while (it != candidates.end()) {
        if (it->second.get() == ptr_to_erase) {
            it = candidates.erase(it);
            return true;
        }
        ++it;
    }
    return false;
}

UpdateComponent::UpdateComponent(std::function<void(UpdateComponent &)> &&f) {
    this->func_ = [f, this] { f(*this); };
    get_owner()->get_world().updates.request_set(&this->func_);
}
UpdateComponent::~UpdateComponent() {
    get_owner()->get_world().updates.request_erase(&this->func_);
}
