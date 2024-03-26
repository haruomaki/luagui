#pragma once

#include <Camera.hpp>
#include <World.hpp>

// TODO: Drawに改名
class DrawableWorldObject : virtual public WorldObject {
    FunctionId func_id_;
    virtual void draw(const Camera &camera) const = 0;

  public:
    DrawableWorldObject();
    ~DrawableWorldObject() override;
    DrawableWorldObject(const DrawableWorldObject &) = delete;
    DrawableWorldObject &operator=(const DrawableWorldObject &) const = delete;
    DrawableWorldObject(DrawableWorldObject &&) = delete;
    DrawableWorldObject &operator=(DrawableWorldObject &&) const = delete;
};
