#include <GUI/Color.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <ostream>
#include <sol/sol.hpp>

using Points = std::vector<std::vector<float>>;

struct VI : std::vector<int> {
    VI(std::vector<int> tbl);
};

inline std::ostream &operator<<(std::ostream &os, const VI &v) {
    os << "VI[";
    for (size_t i = 0; const auto &e : v) {
        os << (i++ > 0 ? ", " : "") << e;
    }
    return os << "]";
}

struct V2 : std::vector<glm::vec2> {
    V2() = default;
    V2(Points tbl);
    V2(std::vector<glm::vec2> &&v)
        : std::vector<glm::vec2>(std::move(v)) {}
    V2 operator*(size_t n) {
        V2 ret;
        for (size_t i = 0; i < n; i++) {
            for (auto e : *this) {
                ret.push_back(e);
            }
        }
        return {ret};
    }
};

inline std::ostream &operator<<(std::ostream &os, const V2 &v) {
    os << "V2";
    for (const auto &e : v) {
        os << "[" << e.x << ", " << e.y << "]";
    }
    return os;
}

struct V3 : std::vector<glm::vec3> {
    V3(Points tbl);
    V3(std::vector<glm::vec3> &&v)
        : std::vector<glm::vec3>(std::move(v)) {}
};

inline std::ostream &operator<<(std::ostream &os, const V3 &v) {
    os << "V3";
    for (const auto &e : v) {
        os << "[" << e.x << ", " << e.y << ", " << e.z << "]";
    }
    return os;
}

struct CV : std::vector<RGBA> {
    CV(Points tbl);
    CV(size_t n, RGBA c)
        : std::vector<RGBA>(n, c) {}
    CV(std::vector<RGBA> &&v)
        : std::vector<RGBA>(std::move(v)) {}
};

void register_vec(sol::state &lua);
