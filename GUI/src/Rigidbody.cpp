#include "Rigidbody.hpp"

// ---------------------
// RigidbodyComponent
// ---------------------

RigidbodyComponent::RigidbodyComponent(b2::Body::Params body_params) {
    trace("RigidbodyComponentのコンストラクタです。", this);

    // Worldのrigidbodyリストに追加
    world().rigidbody_components.request_set(this);

    auto &b2world = world().b2world;
    b2::Body tmp_body = b2world.CreateBody(b2::OwningHandle, body_params);
    b2body = std::move(tmp_body);
}

RigidbodyComponent::~RigidbodyComponent() {
    trace("RigidbodyComponentのデストラクタです。", this);
    auto ccs = owner().get_components<ColliderComponent>();
    // debug(ccs);
    for (auto *cc : ccs) {
        print("けすよ", cc->shape_ref_.Handle().index1, ", ", cc);
        cc->erase();
    }

    // ChainColliderComponentも消す
    // debug(get_owner()->get_components<ChainColliderComponent>());
    for (auto *ccc : owner().get_components<ChainColliderComponent>()) {
        print("チェーンをけすよ", ccc->chain_ref_.Handle().index1);
        ccc->erase();
    }

    // Box2D上で物体を削除
    this->b2body.Destroy();

    // Worldのrigidbodyリストから削除
    world().rigidbody_components.request_erase(this);
    trace("RigidbodyComponentのデストラクタおわりです");
}

// ---------------------
// ColliderComponent
// ---------------------

template <std::derived_from<Component> Comp>
static RigidbodyComponent &get_rigidbody(Comp *self) {
    auto rbcs = self->owner().template get_components<RigidbodyComponent>();
    if (rbcs.empty()) throw std::runtime_error("Rigidbodyが付いていません");
    if (rbcs.size() > 1) throw std::runtime_error("Rigidbodyが複数付いています");
    return *rbcs[0];
}

ColliderComponent::ColliderComponent(ShapeVariant shape, b2::Shape::Params shape_params) {
    // どの型が代入されていたとしても、共通のインタフェースを呼び出す
    std::visit(
        [&](auto &s) {
            auto &rbc = get_rigidbody(this);
            shape_params.userData = static_cast<void *>(this);
            shape_ref_ = rbc.b2body.CreateShape(b2::DestroyWithParent, shape_params, s);
        },
        shape);
}

ColliderComponent::~ColliderComponent() {
    trace("start ColliderComponent dtor");
    this->shape_ref_.Destroy();
    trace("finish ColliderComponent dtor");
}

// ---------------------
// ChainColliderComponent
// ---------------------

ChainColliderComponent::ChainColliderComponent(b2::Chain::Params chain_params) {
    auto &rbc = get_rigidbody(this);
    chain_params.userData = static_cast<void *>(this);
    chain_ref_ = rbc.b2body.CreateChain(b2::DestroyWithParent, chain_params);
}

ChainColliderComponent::~ChainColliderComponent() {
    this->chain_ref_.Destroy();
}
