#include "vec.hpp"

#include <glm/geometric.hpp>
#include <utility>

static inline float get_or(const std::vector<float> &v, size_t index, float default_value) {
    return (index < v.size() ? v[index] : default_value);
}

static inline glm::vec2 to_vec2(const std::vector<float> &v) {
    auto x = get_or(v, 0, 0);
    auto y = get_or(v, 1, 0);
    return {x, y};
}
static inline glm::vec3 to_vec3(const std::vector<float> &v) {
    auto x = get_or(v, 0, 0);
    auto y = get_or(v, 1, 0);
    auto z = get_or(v, 2, 0);
    return {x, y, z};
}
static inline RGBA to_rgba(const std::vector<float> &v) {
    auto r = get_or(v, 0, 0);
    auto g = get_or(v, 1, 0);
    auto b = get_or(v, 2, 0);
    auto a = get_or(v, 3, 1);
    return {r, g, b, a};
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
        (*this)[i] = to_vec3(tbl[i]);
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
    lua.new_usertype<glm::vec2>(
        "vec2",
        sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
        "x", &glm::vec2::x,
        "y", &glm::vec2::y,
        sol::meta_function::addition, [](glm::vec2 a, glm::vec2 b) -> glm::vec2 { return a + b; },
        sol::meta_function::subtraction, [](glm::vec2 a, glm::vec2 b) -> glm::vec2 { return a - b; },
        sol::meta_function::multiplication, [](glm::vec2 v, float x) -> glm::vec2 { return v * x; },
        sol::meta_function::division, [](glm::vec2 v, float x) -> glm::vec2 { return v / x; },
        sol::meta_function::to_string, [](const glm::vec2 &v) -> std::string { std::stringstream ss; ss << "[" << v.x << ", " << v.y << "]"; return ss.str(); });
    lua["vec2"][sol::metatable_key]["__call"] = [](const sol::table & /*self*/, std::vector<float> v) -> glm::vec2 { return to_vec2(v); }; // NOLINT(performance-unnecessary-value-param)

    lua.new_usertype<glm::vec3>(
        "vec3",
        sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z,
        "length", [](glm::vec3 v) -> float { return glm::length(v); },
        "normalize", [](glm::vec3 v) -> glm::vec3 { return glm::normalize(v); },
        "dot", [](glm::vec3 a, glm::vec3 b) -> float { return glm::dot(a, b); },
        "cross", [](glm::vec3 a, glm::vec3 b) -> glm::vec3 { return glm::cross(a, b); },
        sol::meta_function::addition, [](glm::vec3 a, glm::vec3 b) -> glm::vec3 { return a + b; },
        sol::meta_function::subtraction, [](glm::vec3 a, glm::vec3 b) -> glm::vec3 { return a - b; },
        sol::meta_function::multiplication, [](glm::vec3 v, float x) -> glm::vec3 { return v * x; },
        sol::meta_function::division, [](glm::vec3 v, float x) -> glm::vec3 { return v / x; },
        sol::meta_function::to_string, [](const glm::vec3 &v) -> std::string {
            // NOTE: <<のオーバーロードがあれば不要かと思ったが、Linux環境だと出力できなかったので__to_stringを明示。
            std::stringstream ss; ss << "[" << v.x << ", " << v.y << ", " << v.z << "]"; return ss.str(); });
    lua["vec3"][sol::metatable_key]["__call"] = [](const sol::table & /*self*/, std::vector<float> v) -> glm::vec3 { return to_vec3(v); }; // NOLINT(performance-unnecessary-value-param)

    lua.new_usertype<RGBA>(
        "RGBA",
        sol::constructors<RGBA(), RGBA(float, float, float, float)>(),
        "r", &RGBA::r,
        "g", &RGBA::g,
        "b", &RGBA::b,
        "a", &RGBA::a,
        sol::meta_function::multiplication, [](RGBA c, size_t n) -> CV { return {n, c}; });
    lua["RGBA"][sol::metatable_key]["__call"] = [](const sol::table & /*self*/, std::vector<float> v) -> RGBA { return to_rgba(v); }; // NOLINT(performance-unnecessary-value-param)

    lua.new_usertype<VI>("VI", sol::constructors<VI(std::vector<int>)>());

    lua["VI"][sol::metatable_key]["__call"] = [](const sol::table & /*self*/, std::vector<int> tbl) { return VI(std::move(tbl)); };

    lua.new_usertype<V2>(
        "V2",
        sol::constructors<V2(Points)>(),
        sol::meta_function::multiplication, &V2::operator*);

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
