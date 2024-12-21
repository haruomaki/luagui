#pragma once

#include <string>

class WorldObject;
class World;
class Window;

// コンポーネントの基底クラス。
class Component {
    // NOTE: このクラス（とその派生クラス）は必ずWorldObject::add_componentによって実体化される
  protected:
    WorldObject *owner_ = nullptr;

  public:
    // すべてのコンポーネントにIDを振るのは面倒＆ID無しも許容したいため、IDでなく型をキーとするmultimapで管理。
    std::string id;

    Component();
    virtual ~Component() = default;

    [[nodiscard]] WorldObject &owner() const;
    [[nodiscard]] World &world() const;
    [[nodiscard]] Window &window() const;

    // コンポーネントを削除する。
    void erase();
};
