#include <GUI/Color.hpp>
#include <sol/sol.hpp>

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
};

struct CV : std::vector<RGBA> {
    CV(Points tbl);
    CV(size_t n, RGBA c)
        : std::vector<RGBA>(n, c) {}
};

void register_vec(sol::state &lua);
