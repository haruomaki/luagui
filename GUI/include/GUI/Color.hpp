#pragma once

#include "graphical_base.hpp"

struct RGB {
    unsigned char r, g, b;
};

struct RGBA {
    float r, g, b, a;

    operator glm::vec4() const {
        return {r, g, b, a};
    }

    // std::vector<RGBA>をSolに渡すために必要。
    bool operator==(RGBA other) const {
        return glm::vec4(*this) == glm::vec4(other);
    }
};
