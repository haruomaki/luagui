#include "vec.hpp"

#include <GUI/graphical_base.hpp>
#include <utility>

inline float get_or(const vector<float> &v, size_t index, float default_value) {
    return (index < v.size() ? v[index] : default_value);
}

using Points = std::vector<std::vector<float>>;

struct V2 : std::vector<glm::vec2> {
    V2(Points tbl);
};

V2::V2(Points tbl)
    : std::vector<glm::vec2>(tbl.size()) {
    for (size_t i = 0; i < tbl.size(); i++) {
        (*this)[i].x = get_or(tbl[i], 0, 0);
        (*this)[i].y = get_or(tbl[i], 1, 0);
    }
}

void register_vec(sol::state &lua) {
    auto v2_type = lua.new_usertype<V2>(
        "V2",
        sol::constructors<V2(Points)>(),
        "hello", []() {});

    // `V2 {...}`のように、クラス名自体に対して関数呼び出しを可能に。
    lua["V2"][sol::metatable_key]["__call"] = [](const sol::table & /*self*/, Points tbl) { return V2(std::move(tbl)); };
}
