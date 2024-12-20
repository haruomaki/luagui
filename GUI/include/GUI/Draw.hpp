#pragma once

#include "WorldObject.hpp"

class CameraInterface;

class Draw : virtual public WorldObject {
    std::function<void()> func_;
    virtual void draw() const = 0;

  public:
    Draw();
    ~Draw() override;
    Draw(const Draw &) = delete;
    Draw &operator=(const Draw &) const = delete;
    Draw(Draw &&) = delete;
    Draw &operator=(Draw &&) const = delete;
};
