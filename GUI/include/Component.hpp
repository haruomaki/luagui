#pragma once

#include <variant>

#include <graphical_base.hpp>

#include <box2cpp/box2cpp.h>

class WorldObject;
class World;

// コンポーネントの基底クラス。
class Component {
    // NOTE: このクラス（とその派生クラス）は必ずWorldObject::add_componentによって実体化される
  protected:
    WorldObject *owner_ = nullptr;

  public:
    // すべてのコンポーネントにIDを振るのは面倒＆ID無しも許容したいため、IDでなく型をキーとするmultimapで管理。
    string id;

    Component();
    virtual ~Component() = default;

    [[nodiscard]] WorldObject &owner() const;
    [[nodiscard]] World &world() const;
    [[nodiscard]] Window &window() const;

    // コンポーネントを削除する。
    bool erase();
};

class UpdateComponent : public Component {
    std::function<void()> func_;

  public:
    UpdateComponent(std::function<void(UpdateComponent &)> &&f);
    ~UpdateComponent() override;
    UpdateComponent(const UpdateComponent &) = delete;
    UpdateComponent &operator=(const UpdateComponent &) const = delete;
    UpdateComponent(UpdateComponent &&) = delete;
    UpdateComponent &operator=(UpdateComponent &&) const = delete;
};

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
