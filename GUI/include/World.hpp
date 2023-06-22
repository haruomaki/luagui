#pragma once

#include <Camera.hpp>
#include <WorldObject.hpp>

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
