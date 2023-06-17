#pragma once

#include <Draw.hpp>
#include <GUI.hpp>
#include <KeyCallback.hpp>
#include <Shader.hpp>
#include <Timer.hpp>
#include <Update.hpp>

template <typename T>
struct Rect {
    T left_, right_, bottom_, top_;
};

class WorldObject {
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
    WorldObject()
        : pos_(glm::vec3(0))
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

    WorldObject &operator=(WorldObject &&other) noexcept {
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

        return *this;
    }

    WorldObject(WorldObject &&other) noexcept {
        *this = std::move(other);
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

    [[nodiscard]] glm::vec3 getPosition() const {
        return pos_;
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
        // FIXME: z軸を拡大すると、視錐台から外れてしまう。射影行列を設定すべき
        setScale({scale, scale, 1});
    }

    WorldObject *getParent() {
        return parent_;
    }

    friend class Window;
};

class Window {
    GLFWwindow *gwin_;
    glm::mat4 view_matrix_ = glm::mat4(1);
    glm::vec3 camera_pos_;
    float camera_zoom_ = 1;
    static constexpr float default_camera_zoom = 1;
    bool looping_ = false;

  public:
    WorldObject world_object_root_;
    int tick_ = 0;
    ProgramObject shader_;

    Window(int width, int height);
    ~Window();
    void mainloop(const std::function<void()> &callback);
    pair<int, int> getWindowSize();
    pair<int, int> getFrameBufferSize();
    pair<float, float> getWindowContentScale();

    [[nodiscard]] const glm::mat4 &getViewMatrix() const;
    void setCamera(Point<float> pos, float zoom);
};

GLuint loadTexture(const string &filename);

#include <Polygon.hpp>
