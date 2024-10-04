#pragma once

#include <typeindex>

#include <Component.hpp>
#include <Property.hpp>
#include <buffered_container.hpp>
#include <graphical_base.hpp>

class World;

// ユーザはこのクラスを必ずappend_child関数経由でインスタンス化する
class WorldObject {
    glm::vec3 pos_;
    glm::quat rotate_;
    glm::vec3 scale_;
    glm::mat4 abs_transform_;
    WorldObject *const parent_;
    UniqueBufferedSet<WorldObject> children_;
    BufferedMultimap<std::type_index, Component> components_;
    World &world_; // parent_より後にする

    // 子孫ノードの絶対位置を再帰的に計算する関数
    void refresh_absolute_transform();

    // フレーム終わりに、すべてのコンポーネントの追加/削除および子孫ノードの追加/削除要求を再帰的に適用する関数
    void flush_components_children();

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
        this->children_.request_insert(std::move(ptr));
        return *raw;
    }

    bool erase() {
        auto *ptr_to_erase = this;

        // 生ポインタを使用して要素を削除する
        auto &candidates = this->parent_->children_;
        bool success = false;
        candidates.foreach ([&](WorldObject &obj) {
            if (&obj == ptr_to_erase) {
                candidates.request_delete(&obj);
                success = true;
            }
        });
        return success;
    }

    std::vector<WorldObject *> children() {
        return children_.elements();
    }

    // void showAbsolutePositionRecursively(int depth) const {
    //     // cout << "showです。" << endl;
    //     cout << std::string(depth, ' ') << this->abspos_ << endl;
    //     for (auto *child : children_) {
    //         child->showAbsolutePositionRecursively(depth + 1);
    //     }
    // }

    [[nodiscard]] World &get_world() const {
        return this->world_;
    }

    [[nodiscard]] const glm::vec3 &get_position() const {
        return pos_;
    }

    [[nodiscard]] const glm::quat &get_rotate() const {
        return rotate_;
    }

    [[nodiscard]] const glm::vec3 &get_scale() const {
        return scale_;
    }
    [[nodiscard]] float get_scale_prop() const {
        return std::pow(scale_.x * scale_.y * scale_.z, 1.f / 3);
    }

    [[nodiscard]] const glm::mat4 &get_absolute_transform() const {
        return abs_transform_;
    }

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

    void set_position(const glm::vec3 &pos) {
        pos_ = pos;
        this->refresh_absolute_transform();
    }

    void set_rotate(const glm::quat &rotate) {
        rotate_ = rotate;
        this->refresh_absolute_transform();
    }

    void set_scale(const glm::vec3 &scale) {
        scale_ = scale;
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

    template <typename T, typename... Args>
        requires std::derived_from<T, Component>
    T *add_component(Args &&...args) {
        // コンポーネントの実体をヒープ上に生成
        WorldObject::set_parent_static(this); // componentのコンストラクタにthisを伝えるため
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        WorldObject::set_parent_static(nullptr);

        T *component_ptr = component.get();
        components_.request_insert(std::type_index(typeid(T)), std::move(component)); // unique_ptrのムーブコンストラクタ。
        return component_ptr;
    }

    template <typename T>
        requires std::derived_from<T, Component>
    T *get_component() {
        Component *comp = components_.at(std::type_index(typeid(T)));
        return static_cast<T *>(comp);
    }

    template <typename T>
    std::vector<T *> get_components() {
        std::vector<T *> result;

        components_.foreach_equal(std::type_index(typeid(T)), [&](Component &comp) {
            result.push_back(static_cast<T *>(&comp));
        });
        return result;
    }

    Component *get_component_by_id(std::string_view id) {
        Component *ret = nullptr;
        for (Component *comp : components_.elements()) {
            if (id == comp->id) {
                if (ret != nullptr) warn("同一IDの異なるコンポーネントがあります: ", id);
                print("発見しました:", typeid(*comp).name());
                ret = comp;
            }
        }
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
