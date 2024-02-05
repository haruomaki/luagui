#include "WorldObject.hpp"

namespace {
WorldObject *parent_static = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables) WorldObjectのコンストラクタを0引数にするため、グローバル変数経由でparentを渡す
} // namespace

WorldObject *WorldObject::get_parent_static() {
    return parent_static;
}

void WorldObject::set_parent_static(WorldObject *parent) {
    parent_static = parent;
}
