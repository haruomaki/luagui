#include "WorldObject.hpp"
#include "Mesh.hpp"
#include "World.hpp"

namespace {
WorldObject *parent_static = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables) WorldObjectのコンストラクタを0引数にするため、グローバル変数経由でparentを渡す
} // namespace

WorldObject *WorldObject::get_parent_static() {
    return parent_static;
}

void WorldObject::set_parent_static(WorldObject *parent) {
    parent_static = parent;
}

void WorldObject::refresh_absolute_transform() {
    const auto &parent_abs_transform = (parent_ != nullptr ? parent_->abs_transform_ : glm::mat4(1));
    abs_transform_ = parent_abs_transform * TRANSLATE(pos_) * glm::mat4_cast(rotate_) * SCALE(scale_);

    // メッシュオブジェクトの場合は描画のための更新
    const auto *obj = dynamic_cast<MeshObject *>(this);
    if (obj != nullptr) {
        this->world_.mesh_draw_manager_.set_model_matrix(obj);
    }

    children_.foreach ([](WorldObject &child) {
        child.refresh_absolute_transform();
    });
}

WorldObject::WorldObject()
    : pos_(glm::vec3(0))
    , rotate_(glm::quat(1, 0, 0, 0))
    , scale_(glm::vec3(1))
    , abs_transform_(glm::mat4(1))
    , parent_([] {
        auto *p = get_parent_static();
        if (p == nullptr) {
            throw std::runtime_error("WorldObjectの親の設定に失敗");
        }
        return p;
    }())
    , world_(this->parent_->world_) {
    // オブジェクトを生成したばかりでも絶対位置が機能するように
    this->refresh_absolute_transform();
}

WorldObject::~WorldObject() {
    print("WorldObjectのデストラクタ開始");
    // オブジェクトの寿命はコンポーネントの寿命より長い（コンポーネントのデストラクタでget_owner()が無効にならないようにする）
    components_.foreach_flush([](auto, Component &comp) {
        comp.erase();
    });

    print("WorldObjectのデストラクタ終了");
}

// NOTE: 現状、オブジェクトのクローンは不可能
// WorldObject::WorldObject(const WorldObject &other)
//     : pos_(other.pos_)
//     , rotate_(other.rotate_)
//     , scale_(other.scale_)
//     , abs_transform_(other.abs_transform_)
//     , parent_([] {
//         auto *parent = get_parent_static();
//         if (parent == nullptr) {
//             throw std::runtime_error("WorldObjectの親の設定に失敗");
//         }
//         return parent;
//     }())
//     , world_(other.world_) {
//     print("コピーコンストラクタ");
//     for (const auto &child : other.children_) {
//         // const auto &r = *child;
//         // const auto *type_name = typeid(r).name();
//         // debug(type_name);
//         parent_->append_child<WorldObject>(*child); // FIXME: 派生クラスのコピーコンストラクタを呼ぶ必要がある
//     }
// }
