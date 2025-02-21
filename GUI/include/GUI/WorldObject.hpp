#pragma once

#include "Component.hpp"
#include "graphical_base.hpp"
#include "property.hpp"
#include <SumiGL/buffered_container.hpp>
#include <memory> // Ubuntuで必要

inline glm::mat4 triple_to_glm(glm::vec3 p, glm::quat r, glm::vec3 s) {
    return TRANSLATE(p) * glm::mat4_cast(r) * SCALE(s);
}

class World;

// ユーザはこのクラスを必ずappend_child関数経由でインスタンス化する
class WorldObject {
    glm::vec3 pos_;
    glm::quat rotate_;
    glm::vec3 scale_;
    glm::mat4 abs_transform_;
    WorldObject *const parent_;
    BufferedMap<WorldObject *, std::unique_ptr<WorldObject>> children_;
    BufferedMap<Component *, std::unique_ptr<Component>> components_;
    World &world_; // parent_より後にする

    // 子孫ノードの絶対位置を再帰的に計算する関数
    void refresh_absolute_transform();

    // フレーム終わりに、すべてのコンポーネントの追加/削除および子孫ノードの追加/削除要求を再帰的に適用する関数
    void flush_components_children();

    // 位置を再設定時、Bullet Physicsの剛体位置を変更する。
    void sync_bullet();

    static WorldObject *get_parent_static();
    static void set_parent_static(WorldObject *parent);

    // 根ノードの生成のみに使うコンストラクタ
    friend class World; // Worldの生成に使ってもらう
    WorldObject(World &world)
        : pos_(glm::vec3(0))
        , rotate_(glm::quat(1, 0, 0, 0))
        , scale_(glm::vec3(1))
        , abs_transform_(glm::mat4(1))
        , parent_(nullptr)
        , world_(world) {}

  public:
    std::string id;

    // 子孫ノードを生成するコンストラクタ
    // このコンストラクタを呼ぶ直前には必ずset_parent_staticを実行しておく
    WorldObject();

    // ノード削除は必ずerase関数経由で行う
    virtual ~WorldObject();
    // {
    // cout << "フリー！ " << this << '\n';
    // if (parent_ != nullptr) {
    //     parent_->children_.erase(this);
    // }
    // for (auto *child : children_) {
    //     child->parent_ = nullptr;
    // }
    // }

    // クローンは可能
    WorldObject(const WorldObject &);
    WorldObject &operator=(const WorldObject &) const = delete; // コピー代入は消す

    // ムーブは禁止
    WorldObject(WorldObject &&) = delete;
    WorldObject &operator=(WorldObject &&) const = delete;

    template <typename T, typename... Args>
        requires std::constructible_from<T, Args...> &&  // ArgsはTのコンストラクタの引数
                 std::convertible_to<T *, WorldObject *> // TはWorldObjectの派生クラス
    T &append_child(Args &&...args) {
        // WorldObjectのコンストラクタを呼ぶ直前には必ずset_parent_staticを呼び、直後nullptrにリセット
        WorldObject::set_parent_static(this);
        // argsを引数として使って、ヒープ上にT型のオブジェクトを作成
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        WorldObject::set_parent_static(nullptr);

        auto raw = dynamic_cast<T *>(ptr.get());
        this->children_.request_set(raw, std::move(ptr));
        if (this->children_.is_locked()) warn("ロック中にappend_childが呼ばれました💀💀💀");
        this->children_.flush(); // NOTE: 同一フレームで即座に反映させるためにとりあえずフラッシュしているが、問題が起きたら見直す。
        return *raw;
    }

    // 物体を初期化する。
    void clear();

    // 物体の削除要請を出す。
    void erase(bool flush = false) {
        if (this->parent_ == nullptr) {
            warn("根オブジェクトをeraseしています。代わりにclearを使ってください。");
            return;
        }
        this->parent_->children_.request_erase(this);
        if (flush) this->parent_->children_.flush();
    }

    // ただちに削除。
    void force_erase() {
        erase(true);
    }

    std::vector<WorldObject *> children() {
        return children_.keys();
    }

    // void showAbsolutePositionRecursively(int depth) const {
    //     // cout << "showです。" << endl;
    //     cout << std::string(depth, ' ') << this->abspos_ << endl;
    //     for (auto *child : children_) {
    //         child->showAbsolutePositionRecursively(depth + 1);
    //     }
    // }

    [[nodiscard]] unsigned long ptr() const { return reinterpret_cast<unsigned long>(this); }
    bool operator==(const WorldObject &other) const { return ptr() == other.ptr(); }
    [[nodiscard]] World &get_world() const { return this->world_; }

    [[nodiscard]] const glm::vec3 &get_position() const { return pos_; }
    [[nodiscard]] const glm::quat &get_rotate() const { return rotate_; }
    [[nodiscard]] const glm::vec3 &get_scale() const { return scale_; }
    [[nodiscard]] float get_scale_prop() const { return std::pow(scale_.x * scale_.y * scale_.z, 1.f / 3); }
    [[nodiscard]] const glm::mat4 &get_absolute_transform() const { return abs_transform_; }

    [[nodiscard]] glm::vec3 get_front() const { return abs_transform_ * glm::vec4{0, 0, 1, 0}; }
    [[nodiscard]] glm::vec3 get_back() const { return abs_transform_ * glm::vec4{0, 0, -1, 0}; }
    [[nodiscard]] glm::vec3 get_right() const { return abs_transform_ * glm::vec4{-1, 0, 0, 0}; }
    [[nodiscard]] glm::vec3 get_left() const { return abs_transform_ * glm::vec4{1, 0, 0, 0}; }
    [[nodiscard]] glm::vec3 get_up() const { return abs_transform_ * glm::vec4{0, 1, 0, 0}; }
    [[nodiscard]] glm::vec3 get_down() const { return abs_transform_ * glm::vec4{0, -1, 0, 0}; }
    [[nodiscard]] glm::vec3 get_absolute_position() const {
        return {abs_transform_[3]};
    }
    [[nodiscard]] float get_absolute_scale_prop() const {
        glm::mat3 scale_rotate = {abs_transform_};
        return std::cbrtf(glm::determinant(scale_rotate));
    }

    void set_transform(const glm::mat4 transform, bool raw = false) {
        decompose_transform(transform, pos_, rotate_, scale_);
        if (!raw) sync_bullet();
        refresh_absolute_transform();
    }

    void set_position(const glm::vec3 &pos) {
        pos_ = pos;
        sync_bullet();
        this->refresh_absolute_transform();
    }

    void set_rotate(const glm::quat &rotate) {
        rotate_ = rotate;
        sync_bullet();
        this->refresh_absolute_transform();
    }

    void set_scale(const glm::vec3 &scale) {
        scale_ = scale;
        sync_bullet();
        this->refresh_absolute_transform();
    }
    void set_scale_one(float scale) {
        set_scale({scale, scale, scale});
    }
    void set_scale_prop(float scale) {
        auto r = get_scale_prop(); // rで割ることで正規化する
        set_scale({scale_.x / r * scale, scale_.y / r * scale, scale_.z / r * scale});
    }

    WorldObject *get_parent() {
        return parent_;
    }

    // struct PositionProperty {
    //     operator glm::vec3() const { return getPosition() }
    // }

    // プロパティ
    PropertyGetSet<&WorldObject::get_position, &WorldObject::set_position> position{this};
    PropertyGetSet<&WorldObject::get_scale, &WorldObject::set_scale, &WorldObject::set_scale_one> scale{this};
    PropertyGetSet<&WorldObject::get_scale_prop, &WorldObject::set_scale_prop> scale_prop{this};
    PropertyGetSet<&WorldObject::get_rotate, &WorldObject::set_rotate> rotate{this};

    // ------------------
    // コンポーネント操作
    // ------------------

    friend class Component; // get_parent_static()のため

    template <std::derived_from<Component> T, typename... Args>
        requires std::constructible_from<T, Args...>
    T &add_component(Args &&...args) {
        // コンポーネントの実体をヒープ上に生成
        WorldObject::set_parent_static(this); // componentのコンストラクタにthisを伝えるため
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        WorldObject::set_parent_static(nullptr);

        T *component_ptr = component.get();
        components_.request_set(component_ptr, std::move(component));
        if (components_.is_locked()) warn("ロック中にadd_componentが呼ばれました💀💀💀");
        components_.flush(); // NOTE: children_同様ここで即時flushしてみるものの、問題があるかもしれない。
        return *component_ptr;
    }

    // append_child＆コンポーネント追加を一括で行うヘルパー関数。
    template <std::derived_from<Component> T, typename... Args>
    T &child_component(Args &&...args) {
        auto &child = this->append_child<WorldObject>();
        return child.add_component<T>(std::forward<Args>(args)...);
    }

    template <std::derived_from<Component> T>
    T *get_component(bool assert_null = true) {
        auto comps = get_components<T>();
        if (comps.empty()) {
            if (assert_null) warn(typeid(T).name(), "型のコンポーネントがありません。");
            return nullptr;
        }
        return comps[0];
    }

    template <typename T>
    std::vector<T *> get_components() {
        std::vector<T *> result;
        components_.foreach ([&](std::unique_ptr<Component> &comp) {
            auto *p = dynamic_cast<T *>(comp.get());
            if (p) result.emplace_back(p);
        });
        return result;
    }

    Component *get_component_by_name(std::string_view id) {
        Component *ret = nullptr;
        for (auto &[k, comp] : components_) {
            if (id == comp->name) {
                if (ret != nullptr) warn("同一IDの異なるコンポーネントがあります: ", id);
                ret = comp.get();
            }
        }
        if (ret == nullptr) warn("ID=\"", id, "\"のコンポーネントが見つかりませんでした。");
        return ret;
    }
};

// template <class T>
// T &clone(T& obj) {
// void clone(WorldObject& obj) {
//     auto parent = obj.get_parent();
//     assert(parent != nullptr);
//     parent->append_child<T>(Args &&args...)
// }
