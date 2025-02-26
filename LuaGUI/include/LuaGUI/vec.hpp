#include <GUI/Color.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <ostream>
#include <sol/sol.hpp>

// vec3などをlua側でprintするのに必要。
template <int length, typename T>
std::ostream &operator<<(std::ostream &os, const glm::vec<length, T> & /*v*/) {
    // os << "[" << v.x << ", " << v.y << ", " << v.z << "]"; 内容は不要。
    return os;
}

using Points = std::vector<std::vector<float>>;

struct VI : std::vector<int> {
    VI(std::vector<int> tbl);
};

struct V2 : std::vector<glm::vec2> {
    V2() = default;
    V2(Points tbl);
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

struct V3 : std::vector<glm::vec3> {
    V3(Points tbl);

    std::ostream &operator<<(std::ostream &os) const {
        os << "V3の出力だよ";
        return os;
    }
};

struct CV : std::vector<RGBA> {
    CV(Points tbl);
    CV(size_t n, RGBA c)
        : std::vector<RGBA>(n, c) {}
};

void register_vec(sol::state &lua);
