#pragma once

#include <WorldObject.hpp>
#include <graphical_base.hpp>

#include <box2cpp/box2cpp.h>

class WorldObject;

// コンポーネントの基底クラス。
class Component {
    // NOTE: このクラス（とその派生クラス）は必ずWorldObject::add_componentによって実体化される
  protected:
    WorldObject *owner_ = nullptr;

  public:
    Component();
    virtual ~Component() = default;

    [[nodiscard]] WorldObject *get_owner() const { return owner_; }

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

// Box2DのShapeの型であるかを判定するコンセプト
template <typename ShapeType>
concept ShapeTypes = is_any_of<ShapeType, b2Circle, b2Segment>;

template <ShapeTypes ShapeType>
class ColliderComponent : public Component {
  public:
    b2::Shape shape_; // NOLINT(readability-identifier-naming)

    // ColliderComponent(b2Circle circle, b2::Shape::Params shape_params = {});
    ColliderComponent(ShapeType shape, b2::Shape::Params shape_params);

    ~ColliderComponent() override;
};
