#include "Rigidbody.hpp"
#include "World.hpp"

// ---------------------
// RigidbodyComponent
// ---------------------

Rigidbody2D::Rigidbody2D(b2::Body::Params body_params) {
    trace("RigidbodyComponentのコンストラクタです。", this);

    if (owner().get_components<Rigidbody2D>().size() != 0) throw std::runtime_error("１つの物体に２つ以上のRigidbodyを付けることは不正です。");

    // Worldのrigidbodyリストに追加
    world().rigidbody_components.request_set(this);

    auto &b2world = world().b2world;
    b2::Body tmp_body = b2world.CreateBody(b2::OwningHandle, body_params);
    b2body = std::move(tmp_body);

    // 既にColliderが付いていたら、Shapeを遅延的に作成。
    for (auto *cc : owner().get_components<Collider2D>()) {
        cc->append_shape(this);
    }

    // Chainについても同様。
    for (auto *ccc : owner().get_components<ChainCollider2D>()) {
        ccc->append_chain(this);
    }
}

Rigidbody2D::~Rigidbody2D() {
    trace("RigidbodyComponentのデストラクタです。", this);

    // Shapeを外す
    auto ccs = owner().get_components<Collider2D>();
    for (auto *cc : ccs) {
        info("ShapeはあるけどRigidbodyを消すよ。handle: ", cc->shape_ref_.Handle().index1, ", ", cc);
        cc->drop_shape(this);
    }

    // Chainも外す
    auto cccs = owner().get_components<ChainCollider2D>();
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

void Collider2D::append_shape(Rigidbody2D *rbc) {
    // どの型が代入されていたとしても、共通のインタフェースを呼び出す
    std::visit(
        [&](auto &s) {
            active_ = true;
            shape_params_.userData = static_cast<void *>(this);
            shape_ref_ = rbc->b2body.CreateShape(b2::DestroyWithParent, shape_params_, s);
        },
        shape_);
}

void Collider2D::drop_shape(Rigidbody2D * /*rbc*/) {
    active_ = false;
    this->shape_ref_.Destroy();
}

Collider2D::Collider2D(ShapeVariant shape, b2::Shape::Params shape_params)
    : shape_(shape)
    , shape_params_(shape_params) {
    auto rbcs = owner().get_components<Rigidbody2D>();
    if (rbcs.size() == 0) {
        info("RigidbodyComponentが無いため、Shapeを付けずに保留。");
    } else {
        this->append_shape(rbcs[0]);
    }
}

Collider2D::~Collider2D() {
    trace("start ColliderComponent dtor");
    auto rbcs = owner().get_components<Rigidbody2D>();
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

void ChainCollider2D::append_chain(Rigidbody2D *rbc) {
    chain_params_.userData = static_cast<void *>(this);
    chain_ref_ = rbc->b2body.CreateChain(b2::DestroyWithParent, chain_params_);
}

void ChainCollider2D::drop_chain(Rigidbody2D * /*rbc*/) {
    active_ = false;
    this->chain_ref_.Destroy();
}

ChainCollider2D::ChainCollider2D(b2::Chain::Params chain_params)
    : chain_params_(chain_params) {
    auto rbcs = owner().get_components<Rigidbody2D>();
    if (rbcs.size() == 0) {
        info("RigidbodyComponentが無いため、Chainを付けずに保留。");
    } else {
        this->append_chain(rbcs[0]);
    }
}

ChainCollider2D::~ChainCollider2D() {
    auto rbcs = owner().get_components<Rigidbody2D>();
    if (rbcs.size() == 0) {
        info("RigidbodyComponentが無いため、そのまま終了。");
    } else {
        this->drop_chain(rbcs[0]);
    }
}
