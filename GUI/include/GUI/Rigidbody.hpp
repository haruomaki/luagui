#pragma once

#include "Component.hpp"
#include <box2cpp/box2cpp.h>
#include <optional>
#include <variant>

/// 一つの剛体を表すコンポーネント。
/// オブジェクトに対してRigidbodyComponentは一つだけ付与できる。
/// ColliderComponentだけ事前に追加しておくことも可能。その場合はRigidbodyComponentが追加されると同時にShapeも適用される。
class RigidbodyComponent : public Component {
  public:
    b2::Body b2body;

    RigidbodyComponent(b2::Body::Params body_params = {});
    ~RigidbodyComponent() override;
};

// Box2DのShapeたちをまとめるタグ付きユニオン
using ShapeVariant = std::variant<b2Circle, b2Capsule, b2Segment, b2Polygon>;

/// 剛体に付随する当たり判定を表すコンポーネント。
/// 一つの剛体に対して複数のコライダーを追加できる。
/// RigidbodyComponentが既に存在するときは即座にShapeが追加されるが、そうでないときはRigidbodyComponentが追加され次第Shapeも速やかに反映される。
class ColliderComponent : public Component {
    bool active_ = false; // Shapeが実際にBodyに付けられているか

    friend class RigidbodyComponent;
    ShapeVariant shape_;
    b2::Shape::Params shape_params_;

    // RigidBodyがあるときだけ呼び出す。その直下にShapeを追加する。
    void append_shape(RigidbodyComponent *rbc);
    // RigidBodyがあるときだけ呼び出す。その直下からShapeを削除する。
    void drop_shape(RigidbodyComponent *rbc);

  public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    b2::ShapeRef shape_ref_; // publicだが直接触るのは非推奨
    std::optional<std::function<void(ColliderComponent &self, ColliderComponent &other)>> on_collision_enter = std::nullopt;

    ColliderComponent(ShapeVariant shape, b2::Shape::Params shape_params);

    ~ColliderComponent() override;
};

/// チェーン状の特別な当たり判定を表すコンポーネント。
/// ColliderComponentと全く同様に、剛体とは独立に付けたり、複数付けることが可能。
class ChainColliderComponent : public Component {
    bool active_ = false; // Chainが実際にBodyに付けられているか

    friend class RigidbodyComponent;
    b2::Chain::Params chain_params_;

    // RigidBodyがあるときだけ呼び出す。その直下にChainを追加する。
    void append_chain(RigidbodyComponent *rbc);
    // RigidBodyがあるときだけ呼び出す。その直下からChainを削除する。
    void drop_chain(RigidbodyComponent *rbc);

  public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    b2::ChainRef chain_ref_; // publicだが直接触るのは非推奨
    std::optional<std::function<void(ChainColliderComponent &self, ColliderComponent &collider)>> on_collision_enter = std::nullopt;

    ChainColliderComponent(b2::Chain::Params chain_params);
    ~ChainColliderComponent() override;
};
