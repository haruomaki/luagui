#pragma once

#include "FunctionSet.hpp"
#include "WorldObject.hpp"

class Camera;

// TODO: Drawに改名
class Draw : virtual public WorldObject {
    FunctionId func_id_;
    virtual void draw(const Camera &camera) const = 0;

  public:
    Draw();
    ~Draw() override;
    Draw(const Draw &) = delete;
    Draw &operator=(const Draw &) const = delete;
    Draw(Draw &&) = delete;
    Draw &operator=(Draw &&) const = delete;
};
