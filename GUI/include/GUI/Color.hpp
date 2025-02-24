#pragma once

#include <glm/vec4.hpp>
#include <iostream>

// struct RGBA;

// struct Color {
//     virtual ~Color() = default;
//     virtual RGBA into_rgba() = 0;
// };

struct RGBA {
    float r, g, b, a;

    operator glm::vec4() const {
        return {r, g, b, a};
    }

    // RGBA into_rgba() override { return *this; }

    // std::vector<RGBA>をSolに渡すために必要。
    bool operator==(const RGBA &other) const {
        return glm::vec4(*this) == glm::vec4(other);
    }

    friend std::ostream &operator<<(std::ostream &os, const RGBA &c) {
        return os << "RGBA(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ")";
    }
};
