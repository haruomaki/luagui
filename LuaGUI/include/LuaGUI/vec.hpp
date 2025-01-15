#include <GUI/graphical_base.hpp>
#include <sol/sol.hpp>

using Points = std::vector<std::vector<float>>;

struct V2 : std::vector<glm::vec2> {
    V2(Points tbl);
};

struct V3 : std::vector<glm::vec3> {
    V3(Points tbl);
};

void register_vec(sol::state &lua);
