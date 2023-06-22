#pragma once

#include <Camera.hpp>
#include <World.hpp>

class Draw {
    World &world_;
    function<void(const Camera &camera)> draw_;

  public:
    virtual void draw(const Camera &camera) const = 0;
    Draw(World &world);
    ~Draw();
};
