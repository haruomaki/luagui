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

void WorldObject::flush_components_children() {
    // trace("flush_childrenです:", this);
    components_.flush(); // コンポーネントとchildrenの順番はどっちでもいい？
    children_.flush();
    children_.foreach ([](std::unique_ptr<WorldObject> &child) {
        child->flush_components_children();
    });
    // trace("flush_childrenおわり:", this);
}

void WorldObject::refresh_absolute_transform() {
    const auto &parent_abs_transform = (parent_ != nullptr ? parent_->abs_transform_ : glm::mat4(1));
    abs_transform_ = parent_abs_transform * TRANSLATE(pos_) * glm::mat4_cast(rotate_) * SCALE(scale_);

    // メッシュコンポーネントの場合は描画のための更新
    for (auto *mc : this->get_components<MeshComponent>()) {
        this->world_.mesh_draw_manager_.set_model_matrix(mc);
    }

    children_.foreach ([](std::unique_ptr<WorldObject> &child) {
        child->refresh_absolute_transform();
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
    trace("WorldObjectのデストラクタ開始 components_: ", components_.keys());
    clear();
    // print("WorldObjectのデストラクタ終了");
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

void WorldObject::clear() {
    // オブジェクトの寿命はコンポーネントの寿命より長い（コンポーネントのデストラクタでget_owner()が無効にならないようにする）
    components_.flush(); // これが無いと、コンポーネントとその親の物体を同時に消すときなどに二重削除になる。
    components_.foreach ([](auto &comp) {
        trace("component iteration ", comp->id);
        comp->erase();
    });
    components_.flush();

    // 子オブジェクトの削除
    children_.flush(); // 二重解放防止
    children_.foreach ([&](std::unique_ptr<WorldObject> &obj) {
        obj->erase(); // drawsやupdatesが消える前にUpdate等のデストラクタを呼ぶ
    });
    children_.flush(); // 即時flushしないと子オブジェクトがメモリから消えない
}
