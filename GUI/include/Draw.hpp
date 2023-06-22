#pragma once

#include <Camera.hpp>
#include <World.hpp>

class Draw : public WorldObject {
    function<void(const Camera &camera)> draw_;
    virtual void draw(const Camera &camera) const = 0;

  public:
    Draw(World &world);
    ~Draw();
};
