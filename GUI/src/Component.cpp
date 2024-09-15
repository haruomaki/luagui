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

// ---------------------
// RigidbodyComponent
// ---------------------

RigidbodyComponent::RigidbodyComponent() {
    print("RigidbodyComponentのコンストラクタです");

    // Worldのrigidbodyリストに追加
    get_owner()->get_world().rigidbody_components.request_set(this);

    // 球その１
    b2::Body::Params bp1;
    bp1.type = b2_dynamicBody;
    bp1.position = {0, 0};
    bp1.linearVelocity = {-0.01, 0.02};
    bp1.sleepThreshold = 0.0005f; // スリープ状態を防ぐ

    auto &b2world = get_owner()->get_world().b2world;
    b2::Body ball1 = b2world.CreateBody(b2::OwningHandle, bp1);
    b2body = std::move(ball1);

    b2::Shape::Params shape_params;
    shape_params.friction = 100.f;

    b2body.CreateShape(
        b2::DestroyWithParent,
        shape_params,
        b2Circle{.center = b2Vec2(), .radius = 10});
}

RigidbodyComponent::~RigidbodyComponent() {
    print("RigidbodyComponentのデストラクタです");

    // Worldのrigidbodyリストから削除
    get_owner()->get_world().rigidbody_components.request_erase(this);
}
