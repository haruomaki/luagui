#pragma once

#include <Camera.hpp>
#include <World.hpp>

class DrawableWorldObject : virtual public WorldObject {
    function<void(const Camera &camera)> draw_;
    virtual void draw(const Camera &camera) const = 0;

  public:
    DrawableWorldObject();
    ~DrawableWorldObject() override;
};
