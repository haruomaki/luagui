#include "Rigidbody.hpp"
#include "World.hpp"

// ---------------------
// RigidbodyComponent
// ---------------------

RigidbodyComponent::RigidbodyComponent(b2::Body::Params body_params) {
    trace("RigidbodyComponentのコンストラクタです。", this);

    if (owner().get_components<RigidbodyComponent>().size() != 0) throw std::runtime_error("１つの物体に２つ以上のRigidbodyを付けることは不正です。");

    // Worldのrigidbodyリストに追加
    world().rigidbody_components.request_set(this);

    auto &b2world = world().b2world;
    b2::Body tmp_body = b2world.CreateBody(b2::OwningHandle, body_params);
    b2body = std::move(tmp_body);

    // 既にColliderが付いていたら、Shapeを遅延的に作成。
    for (auto *cc : owner().get_components<ColliderComponent>()) {
        cc->append_shape(this);
    }

    // Chainについても同様。
    for (auto *ccc : owner().get_components<ChainColliderComponent>()) {
        ccc->append_chain(this);
    }
}

RigidbodyComponent::~RigidbodyComponent() {
    trace("RigidbodyComponentのデストラクタです。", this);

    // Shapeを外す
    auto ccs = owner().get_components<ColliderComponent>();
    for (auto *cc : ccs) {
        info("ShapeはあるけどRigidbodyを消すよ。handle: ", cc->shape_ref_.Handle().index1, ", ", cc);
        cc->drop_shape(this);
    }

    // Chainも外す
    auto cccs = owner().get_components<ChainColliderComponent>();
    for (auto *ccc : cccs) {
        info("ChainはあるけどRigidbodyを消すよ。handle: ", ccc->chain_ref_.Handle().index1, ", ", ccc);
        ccc->drop_chain(this);
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

void ColliderComponent::append_shape(RigidbodyComponent *rbc) {
    // どの型が代入されていたとしても、共通のインタフェースを呼び出す
    std::visit(
        [&](auto &s) {
            active_ = true;
            shape_params_.userData = static_cast<void *>(this);
            shape_ref_ = rbc->b2body.CreateShape(b2::DestroyWithParent, shape_params_, s);
        },
        shape_);
}

void ColliderComponent::drop_shape(RigidbodyComponent * /*rbc*/) {
    active_ = false;
    this->shape_ref_.Destroy();
}

ColliderComponent::ColliderComponent(ShapeVariant shape, b2::Shape::Params shape_params)
    : shape_(shape)
    , shape_params_(shape_params) {
    auto rbcs = owner().get_components<RigidbodyComponent>();
    if (rbcs.size() == 0) {
        info("RigidbodyComponentが無いため、Shapeを付けずに保留。");
    } else {
        this->append_shape(rbcs[0]);
    }
}

ColliderComponent::~ColliderComponent() {
    trace("start ColliderComponent dtor");
    auto rbcs = owner().get_components<RigidbodyComponent>();
    if (rbcs.size() == 0) {
        info("RigidbodyComponentが無いため、そのまま終了。");
    } else {
        this->drop_shape(rbcs[0]);
    }
    trace("finish ColliderComponent dtor");
}

// ---------------------
// ChainColliderComponent
// ---------------------

void ChainColliderComponent::append_chain(RigidbodyComponent *rbc) {
    chain_params_.userData = static_cast<void *>(this);
    chain_ref_ = rbc->b2body.CreateChain(b2::DestroyWithParent, chain_params_);
}

void ChainColliderComponent::drop_chain(RigidbodyComponent * /*rbc*/) {
    active_ = false;
    this->chain_ref_.Destroy();
}

ChainColliderComponent::ChainColliderComponent(b2::Chain::Params chain_params)
    : chain_params_(chain_params) {
    auto rbcs = owner().get_components<RigidbodyComponent>();
    if (rbcs.size() == 0) {
        info("RigidbodyComponentが無いため、Chainを付けずに保留。");
    } else {
        this->append_chain(rbcs[0]);
    }
}

ChainColliderComponent::~ChainColliderComponent() {
    auto rbcs = owner().get_components<RigidbodyComponent>();
    if (rbcs.size() == 0) {
        info("RigidbodyComponentが無いため、そのまま終了。");
    } else {
        this->drop_chain(rbcs[0]);
    }
}
