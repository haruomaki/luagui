#include <lua.hpp>

#include <World.hpp>
#include <utility2.hpp>

// void*（またはT*など）をT&に変換する。
template <typename T, typename Ptr>
inline T &dereference(Ptr *ptr) {
    if (!ptr) {
        throw std::runtime_error("Null pointer exception");
    }
    return *static_cast<T *>(ptr);
}

inline World *check_world(lua_State *state, int index) {
    void *userdata = luaL_checkudata(state, index, "World");
    luaL_argcheck(state, userdata != nullptr, index, "'World' expected");
    return *static_cast<World **>(userdata);
}

inline int world_draw_line(lua_State *state) {
    auto &world = dereference<World>(check_world(state, 1));
    auto &line_obj = new_line(world);
    line_obj.mesh.vertices.setCoords({{0, 0, 0}, {0.05, 0, 0}});

    return 0;
}

inline void register_world(lua_State *state) {
    std::array<luaL_Reg, 2> world_methods = {
        {{"draw_line", world_draw_line}}};

    luaL_newmetatable(state, "World");
    lua_pushvalue(state, -1);
    lua_setfield(state, -2, "__index");
    luaL_setfuncs(state, world_methods.data(), 0);
    lua_pop(state, 1);
}
