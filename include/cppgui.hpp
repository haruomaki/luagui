#pragma once

#include <Draw.hpp>
#include <KeyCallback.hpp>
#include <Update.hpp>
#include <guibase.hpp>

#include <fstream>
#include <optional>

class WorldObject;

template <typename T>
struct Rect {
    T left_, right_, bottom_, top_;
};

class WorldObject {
    Point<float> pos_;
    Point<float> abspos_;
    WorldObject *parent_;
    set<WorldObject *> childs_;

    void refreshAbsolutePosition() {
        if (parent_ != nullptr) {
            abspos_ = parent_->abspos_ + pos_;
        }
        for (auto *child : childs_) {
            child->refreshAbsolutePosition();
        }
    }

  public:
    WorldObject()
        : pos_{0, 0}
        , abspos_({0, 0})
        , parent_(nullptr) {}

    ~WorldObject() {
        if (parent_ != nullptr) {
            parent_->childs_.erase(this);
        }
        for (auto *child : childs_) {
            child->parent_ = nullptr;
        }
    }

    void append(WorldObject &child) {
        child.parent_ = this;
        this->childs_.insert(&child);
        this->refreshAbsolutePosition();
    }

    void deleteRecursively() {
        for (auto *child : childs_) {
            child->deleteRecursively();
        }
        delete this;
    }

    Point<float> getPosition() const {
        return pos_;
    }

    Point<float> getAbsolutePosition() const {
        return abspos_;
    }

    void setPosition(Point<float> pos) {
        pos_ = pos;
        this->refreshAbsolutePosition();
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
    void mainloop(std::function<void()> f);
    pair<int, int> getWindowSize();
    pair<int, int> getFrameBufferSize();
    pair<float, float> getWindowContentScale();
    void setCamera(Point<float> pos, float zoom);
    void setCameraCorner(Point<float> pos, float zoom);
};

GLuint createShader();
GLuint loadTexture(string filename);

#include <CorePolygon.hpp>
