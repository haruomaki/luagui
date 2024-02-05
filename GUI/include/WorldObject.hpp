#pragma once

#include <Property.hpp>
#include <graphical_base.hpp>

class World;

// ユーザはこのクラスを必ずappend_child関数経由でインスタンス化する
class WorldObject {
    World *const world_;
    glm::vec3 pos_;
    glm::quat rotate_;
    glm::vec3 scale_;
    glm::mat4 abs_transform_;
    WorldObject *parent_;
    std::set<std::unique_ptr<WorldObject>> children_;

    void refresh_absolute_transform() {
        const auto &parent_abs_transform = (parent_ != nullptr ? parent_->abs_transform_ : glm::mat4(1));
        abs_transform_ = parent_abs_transform * TRANSLATE(pos_) * glm::mat4_cast(rotate_) * SCALE(scale_);
        for (auto const &child : children_) {
            child->refresh_absolute_transform();
        }
    }

    static WorldObject *get_parent_static();
    static void set_parent_static(WorldObject *parent);

    // 根ノードの生成のみに使うコンストラクタ
    friend class World; // Worldの生成に使ってもらう
    WorldObject(World &world, bool /*オーバーロードのため。値に意味はない*/)
        : world_(&world)
        , pos_(glm::vec3(0))
        , rotate_(glm::quat(1, 0, 0, 0))
        , scale_(glm::vec3(1))
        , abs_transform_(glm::mat4(1))
        , parent_(nullptr) {}

  public:
    // 子孫ノードを生成するコンストラクタ
    // このコンストラクタを呼ぶ直前には必ずset_parent_staticを実行しておく
    WorldObject(World &world)
        : world_(&world)
        , pos_(glm::vec3(0))
        , rotate_(glm::quat(1, 0, 0, 0))
        , scale_(glm::vec3(1))
        , abs_transform_(glm::mat4(1))
        , parent_(get_parent_static()) {
        if (this->parent_ == nullptr) {
            throw std::runtime_error("WorldObjectの親の設定に失敗");
        }
    }

    // ノード削除は必ずerase関数経由で行う
    virtual ~WorldObject() = default;
    // {
    // cout << "フリー！ " << this << '\n';
    // if (parent_ != nullptr) {
    //     parent_->children_.erase(this);
    // }
    // for (auto *child : children_) {
    //     child->parent_ = nullptr;
    // }
    // }

    // コピー禁止
    WorldObject(const WorldObject &) = delete;

    // ムーブも禁止
    WorldObject(WorldObject &&) = delete;
    // WorldObject(WorldObject &&other) noexcept
    //     : world_(other.world_)
    //     , pos_(other.pos_)
    //     , rotate_(other.rotate_)
    //     , scale_(other.scale_)
    //     , abs_transform_(other.abs_transform_)
    //     , parent_(other.parent_)
    //     , children_(other.children_) {

    //     // 親の子情報を更新
    //     if (parent_ != nullptr) {
    //         parent_->children_.erase(&other);
    //         parent_->children_.insert(this);
    //     }

    //     // 子たちの親情報を更新
    //     for (auto *child : children_) {
    //         child->parent_ = this;
    //     }
    // }

    // void append_child(WorldObject &child) {
    //     child.parent_ = this;
    //     this->children_.insert(&child);
    //     this->refresh_absolute_transform();
    // }

    template <typename T, typename... Args>
        requires std::is_constructible_v<T, Args...> &&    // ArgsはTのコンストラクタの引数
                 std::is_convertible_v<T *, WorldObject *> // TはWorldObjectの派生クラス
    T &append_child(Args &&...args) {
        // WorldObjectのコンストラクタを呼ぶ直前には必ずset_parent_staticを呼び、直後nullptrにリセット
        WorldObject::set_parent_static(this);
        // argsを引数として使って、ヒープ上にT型のオブジェクトを作成
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        WorldObject::set_parent_static(nullptr);

        auto [it, inserted] = this->children_.insert(std::move(ptr));
        if (!inserted) {
            throw std::runtime_error("append_childに失敗");
        }
        auto raw = static_cast<T *>(it->get());
        return *raw;
    }

    bool erase() {
        auto *ptr_to_erase = this;

        // 生ポインタを使用して要素を削除する
        auto &candidates = this->parent_->children_;
        auto it = candidates.begin();
        while (it != candidates.end()) {
            if ((*it).get() == ptr_to_erase) {
                it = candidates.erase(it);
                return true;
            }
            ++it;
        }
        return false;
    }

    // void showAbsolutePositionRecursively(int depth) const {
    //     // cout << "showです。" << endl;
    //     cout << std::string(depth, ' ') << this->abspos_ << endl;
    //     for (auto *child : children_) {
    //         child->showAbsolutePositionRecursively(depth + 1);
    //     }
    // }

    [[nodiscard]] World *get_world() const {
        return world_;
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

    [[nodiscard]] const glm::mat4 &get_absolute_transform() const {
        return abs_transform_;
    }

    [[nodiscard]] glm::vec3 get_front() const { return abs_transform_ * glm::vec4{0, 0, 1, 0}; }
    [[nodiscard]] glm::vec3 get_back() const { return abs_transform_ * glm::vec4{0, 0, -1, 0}; }
    [[nodiscard]] glm::vec3 get_right() const { return abs_transform_ * glm::vec4{-1, 0, 0, 0}; }
    [[nodiscard]] glm::vec3 get_left() const { return abs_transform_ * glm::vec4{1, 0, 0, 0}; }
    [[nodiscard]] glm::vec3 get_up() const { return abs_transform_ * glm::vec4{0, 1, 0, 0}; }
    [[nodiscard]] glm::vec3 get_down() const { return abs_transform_ * glm::vec4{0, -1, 0, 0}; }

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

    WorldObject *get_parent() {
        return parent_;
    }

    // struct PositionProperty {
    //     operator glm::vec3() const { return getPosition() }
    // }

    // プロパティ
    PropertyGetSet<&WorldObject::get_position, &WorldObject::set_position> position{this};
    PropertyGetSet<&WorldObject::get_scale, &WorldObject::set_scale, &WorldObject::set_scale_one> scale{this};
    PropertyGetSet<&WorldObject::get_rotate, &WorldObject::set_rotate> rotate{this};
};
