#include "vec.hpp"

#include <utility>

inline float get_or(const vector<float> &v, size_t index, float default_value) {
    return (index < v.size() ? v[index] : default_value);
}

VI::VI(std::vector<int> tbl)
    : std::vector<int>(std::move(tbl)) {}

V2::V2(Points tbl)
    : std::vector<glm::vec2>(tbl.size()) {
    for (size_t i = 0; i < tbl.size(); i++) {
        (*this)[i].x = get_or(tbl[i], 0, 0);
        (*this)[i].y = get_or(tbl[i], 1, 0);
    }
}

V3::V3(Points tbl)
    : std::vector<glm::vec3>(tbl.size()) {
    for (size_t i = 0; i < tbl.size(); i++) {
        (*this)[i].x = get_or(tbl[i], 0, 0);
        (*this)[i].y = get_or(tbl[i], 1, 0);
        (*this)[i].z = get_or(tbl[i], 2, 0);
    }
}

CV::CV(Points tbl)
    : std::vector<RGBA>(tbl.size()) {
    for (size_t i = 0; i < tbl.size(); i++) {
        (*this)[i].r = get_or(tbl[i], 0, 0);
        (*this)[i].g = get_or(tbl[i], 1, 0);
        (*this)[i].b = get_or(tbl[i], 2, 0);
        (*this)[i].a = get_or(tbl[i], 3, 0);
    }
}

void register_vec(sol::state &lua) {
    lua.new_usertype<VI>(
        "VI",
        sol::constructors<VI(std::vector<int>)>());

    lua["VI"][sol::metatable_key]["__call"] = [](const sol::table & /*self*/, std::vector<int> tbl) { return VI(std::move(tbl)); };

    lua.new_usertype<V2>(
        "V2",
        sol::constructors<V2(Points)>());

    // `V2 {...}`のように、クラス名自体に対して関数呼び出しを可能に。
    lua["V2"][sol::metatable_key]["__call"] = [](const sol::table & /*self*/, Points tbl) { return V2(std::move(tbl)); };

    lua.new_usertype<V3>(
        "V3",
        sol::constructors<V3(Points)>());

    lua["V3"][sol::metatable_key]["__call"] = [](const sol::table & /*self*/, Points tbl) { return V3(std::move(tbl)); };

    lua.new_usertype<CV>(
        "CV",
        sol::constructors<CV(Points)>());

    lua["CV"][sol::metatable_key]["__call"] = [](const sol::table & /*self*/, Points tbl) { return CV(std::move(tbl)); };
}
