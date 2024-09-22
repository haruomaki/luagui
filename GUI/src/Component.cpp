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

RigidbodyComponent::RigidbodyComponent(b2::Body::Params body_params) {
    print("RigidbodyComponentのコンストラクタです");

    // Worldのrigidbodyリストに追加
    get_owner()->get_world().rigidbody_components.request_set(this);

    auto &b2world = get_owner()->get_world().b2world;
    b2::Body tmp_body = b2world.CreateBody(b2::OwningHandle, body_params);
    b2body = std::move(tmp_body);
}

RigidbodyComponent::~RigidbodyComponent() {
    print("RigidbodyComponentのデストラクタです");

    // 関連するShapeをすべて削除
    // TODO: 型違いのColliderComponentを列挙する方法が分からない
    // get_owner()->get_components<ColliderComponent<ShapeTypes ShapeType>>()

    // Box2D上で物体を削除
    this->b2body.Destroy();

    // Worldのrigidbodyリストから削除
    get_owner()->get_world().rigidbody_components.request_erase(this);
}

// ---------------------
// ColliderComponent
// ---------------------

template <ShapeTypes ShapeType>
static RigidbodyComponent &get_rigidbody(ColliderComponent<ShapeType> *self) {
    auto rbcs = self->get_owner()->template get_components<RigidbodyComponent>();
    if (rbcs.empty()) throw std::runtime_error("Rigidbodyが付いていません");
    if (rbcs.size() > 1) throw std::runtime_error("Rigidbodyが複数付いています");
    return *rbcs[0];
}

template <ShapeTypes ShapeType>
ColliderComponent<ShapeType>::ColliderComponent(ShapeType shape, b2::Shape::Params shape_params) {
    // // Worldのrigidbodyリストに追加
    // get_owner()->get_world().rigidbody_components.request_set(this);

    // auto &b2world = get_owner()->get_world().b2world;
    // b2::Body tmp_body = b2world.CreateBody(b2::OwningHandle, body_params);
    // b2body = std::move(tmp_body);

    auto &rbc = get_rigidbody(this);

    // rbc->b2body.CreateShape(
    //     b2::DestroyWithParent,
    //     shape_params,
    //     b2Circle{.center = b2Vec2{x, y}, .radius = radius});
    rbc.b2body.CreateShape(b2::DestroyWithParent, shape_params, shape);
}

template <ShapeTypes ShapeType>
ColliderComponent<ShapeType>::~ColliderComponent() {
    this->shape_.Destroy();
}

// 明示的に実体化
template class ColliderComponent<b2Circle>;
template class ColliderComponent<b2Segment>;
