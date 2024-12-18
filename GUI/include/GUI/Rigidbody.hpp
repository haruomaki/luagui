#pragma once

#include "Component.hpp"
#include <box2cpp/box2cpp.h>
#include <optional>
#include <variant>

class RigidbodyComponent : public Component {
  public:
    b2::Body b2body;

    RigidbodyComponent(b2::Body::Params body_params = {});
    ~RigidbodyComponent() override;
};

// Box2DのShapeたちをまとめるタグ付きユニオン
using ShapeVariant = std::variant<b2Circle, b2Capsule, b2Segment, b2Polygon>;

class ColliderComponent : public Component {
  public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    b2::ShapeRef shape_ref_; // publicだが直接触るのは非推奨
    std::optional<std::function<void(ColliderComponent &self, ColliderComponent &other)>> on_collision_enter = std::nullopt;

    ColliderComponent(ShapeVariant shape, b2::Shape::Params shape_params);

    ~ColliderComponent() override;
};

class ChainColliderComponent : public Component {
  public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    b2::ChainRef chain_ref_; // publicだが直接触るのは非推奨
    std::optional<std::function<void(ChainColliderComponent &self, ColliderComponent &collider)>> on_collision_enter = std::nullopt;

    ChainColliderComponent(b2::Chain::Params chain_params);
    ~ChainColliderComponent() override;
};
