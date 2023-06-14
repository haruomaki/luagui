#pragma once

#include <Draw.hpp>
#include <KeyCallback.hpp>
#include <Shader.hpp>
#include <Update.hpp>
#include <guibase.hpp>

#include <fstream>
#include <optional>

template <typename T>
struct Rect {
    T left_, right_, bottom_, top_;
};

class WorldObject {
    Point<float> pos_;
    Point<float> abspos_;
    WorldObject *parent_ = nullptr;
    set<WorldObject *> children_;

    void refreshAbsolutePosition() {
        if (parent_ != nullptr) {
            abspos_ = parent_->abspos_ + pos_;
        }
        for (auto *child : children_) {
            child->refreshAbsolutePosition();
        }
    }

  public:
    WorldObject()
        : pos_{0, 0}
        , abspos_({0, 0}) {
        cout << "コンストラクタ " << this << endl;
    }

    ~WorldObject() {
        cout << "フリー！ " << this << endl;
        if (parent_ != nullptr) {
            parent_->children_.erase(this);
        }
        for (auto *child : children_) {
            child->parent_ = nullptr;
        }
    }

    WorldObject(const WorldObject &) = delete;
    //     : pos_(wo.pos_)
    //     , abspos_(wo.abspos_) {
    //     cout << "コピーコンストラクタ " << this << endl;

    //     if (wo.parent_ != nullptr) {
    //         wo.parent_->append(this);
    //     }
    //     for (auto *child : children_) {
    //         child->parent_ = nullptr;
    //     }
    // }

    WorldObject &operator=(WorldObject &&other) noexcept {
        cout << "ムーブ代入演算子です " << &other << " -> " << this << endl;
        pos_ = other.pos_;
        abspos_ = other.abspos_;
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

        debug();
        debug(this);
        debug(&other, other.children_);

        return *this;
    }

    // WorldObject(WorldObject &&other) = default;
    WorldObject(WorldObject &&other) noexcept {
        cout << "ムーブコンストラクタです " << this << endl;
        *this = std::move(other);
    }

    void append(WorldObject *child) {
        child->parent_ = this;
        this->children_.insert(child);
        cout << "Append❢ " << this->children_ << endl;
        this->refreshAbsolutePosition();
    }

    void deleteRecursively() {
        for (auto *child : children_) {
            child->deleteRecursively();
        }
        delete this;
    }

    void showAbsolutePositionRecursively(int depth) const {
        // cout << "showです。" << endl;
        cout << std::string(depth, ' ') << this->abspos_ << endl;
        for (auto *child : children_) {
            child->showAbsolutePositionRecursively(depth + 1);
        }
    }

    [[nodiscard]] Point<float> getPosition() const {
        return pos_;
    }

    [[nodiscard]] Point<float> getAbsolutePosition() const {
        return abspos_;
    }

    void setPosition(Point<float> pos) {
        pos_ = pos;
        this->refreshAbsolutePosition();
    }

    WorldObject *getParent() {
        return parent_;
    }

    friend class Window;
};

class Window {
    GLFWwindow *gwin_;
    Rect<double> camera_;
    float zoom_ = 1;
    bool looping_ = false;

  public:
    WorldObject world_object_root_;
    int tick_ = 0;
    GLuint program_id_;

    Window(int width, int height);
    ~Window();
    void mainloop(const std::function<void()> &callback);
    pair<int, int> getWindowSize();
    pair<int, int> getFrameBufferSize();
    pair<float, float> getWindowContentScale();
    void setCamera(Point<float> pos, float zoom);
    void setCameraCorner(Point<float> pos, float zoom);
};

GLuint loadTexture(const string &filename);

#include <CorePolygon.hpp>
#include <Polygon.hpp>
