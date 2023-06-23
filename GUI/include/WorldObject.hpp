#pragma once

#include <GUI.hpp>

class World;

class WorldObject {
    World *const world_;
    glm::vec3 pos_;
    glm::vec3 scale_;
    glm::mat4 abs_transform_;
    WorldObject *parent_ = nullptr;
    set<WorldObject *> children_;

    void refreshAbsoluteTransform() {
        const auto &parent_abs_transform = (parent_ != nullptr ? parent_->abs_transform_ : glm::mat4(1));
        abs_transform_ = parent_abs_transform * TRANSLATE(pos_) * SCALE(scale_);
        for (auto *child : children_) {
            child->refreshAbsoluteTransform();
        }
    }

  public:
    WorldObject(World &world)
        : world_(&world)
        , pos_(glm::vec3(0))
        , scale_(glm::vec3(1))
        , abs_transform_(glm::mat4(1)) {}

    ~WorldObject() {
        cout << "フリー！ " << this << endl;
        if (parent_ != nullptr) {
            parent_->children_.erase(this);
        }
        for (auto *child : children_) {
            child->parent_ = nullptr;
        }
    }

    // コピー禁止
    WorldObject(const WorldObject &) = delete;

    // ムーブは可能
    WorldObject(WorldObject &&other) noexcept
        : world_(other.world_) {
        pos_ = other.pos_;
        scale_ = other.scale_;
        abs_transform_ = other.abs_transform_;
        parent_ = other.parent_;
        children_ = other.children_;

        // 親の子情報を更新
        if (parent_ != nullptr) {
            parent_->children_.erase(&other);
            parent_->children_.insert(this);
        }

        // 子たちの親情報を更新
        for (auto *child : children_) {
            child->parent_ = this;
        }
    }

    void append(WorldObject *child) {
        child->parent_ = this;
        this->children_.insert(child);
        this->refreshAbsoluteTransform();
    }

    void deleteRecursively() {
        for (auto *child : children_) {
            child->deleteRecursively();
        }
        delete this;
    }

    // void showAbsolutePositionRecursively(int depth) const {
    //     // cout << "showです。" << endl;
    //     cout << std::string(depth, ' ') << this->abspos_ << endl;
    //     for (auto *child : children_) {
    //         child->showAbsolutePositionRecursively(depth + 1);
    //     }
    // }

    [[nodiscard]] World *getWorld() const {
        return world_;
    }

    [[nodiscard]] glm::vec3 getPosition() const {
        return pos_;
    }

    [[nodiscard]] glm::vec3 getScale() const {
        return scale_;
    }

    [[nodiscard]] glm::mat4 getAbsoluteTransform() const {
        return abs_transform_;
    }

    void setPosition(glm::vec3 pos) {
        pos_ = pos;
        this->refreshAbsoluteTransform();
    }

    void setScale(glm::vec3 scale) {
        scale_ = scale;
        this->refreshAbsoluteTransform();
    }
    void setScale(float scale) {
        setScale({scale, scale, scale});
    }

    WorldObject *getParent() {
        return parent_;
    }

    // struct PositionProperty {
    //     operator glm::vec3() const { return getPosition() }
    // }

    friend class Window;
};
