#include <GUI/Color.hpp>
#include <sol/sol.hpp>

using Points = std::vector<std::vector<float>>;

struct VI : std::vector<int> {
    VI(std::vector<int> tbl);
};

struct V2 : std::vector<glm::vec2> {
    V2(Points tbl);
};

struct V3 : std::vector<glm::vec3> {
    V3(Points tbl);
};

struct CV : std::vector<RGBA> {
    CV(Points tbl);
};

void register_vec(sol::state &lua);
