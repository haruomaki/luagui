#pragma once

#include <span>

// #include <Draw.hpp>
#include <GUI.hpp>
#include <KeyCallback.hpp>
#include <Shader.hpp>
#include <SizeCallback.hpp>
#include <Timer.hpp>
#include <Update.hpp>
#include <Window.hpp>
#include <WorldObject.hpp>

template <typename T>
struct Rect {
    T left_, right_, bottom_, top_;
};

GLuint loadTexture(const string &filename);

// Windowを利用するヘッダ。Windowの宣言のあとにインクルード
#include <Camera.hpp>

class World : public WorldObject {
    set<function<void(const Camera &)> *> draws_;

    friend class Draw;

  public:
    void masterDraw(const Camera &camera) {
        for (auto *draw : draws_) {
            (*draw)(camera);
        }
    }
};

class Draw {
    World &world_;
    function<void(const Camera &camera)> draw_;

  public:
    virtual void draw(const Camera &camera) const = 0;
    Draw(World &world);
    ~Draw();
};

inline Draw::Draw(World &world)
    : world_(world) {
    draw_ = [this](const Camera &camera) { this->draw(camera); };
    world_.draws_.insert(&draw_);
}
inline Draw::~Draw() {
    world_.draws_.erase(&draw_);
}

#include <DynamicArray.hpp>
#include <Polygon.hpp>
#include <Text.hpp>
