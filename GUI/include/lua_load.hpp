// Lua入門: https://www.amusement-creators.info/articles/advent_calendar/2019/06_0/

#include <base.hpp>
#include <lua.hpp>

inline int run_lua(const char *filename) {
    // Luaステートの作成
    lua_State *state = luaL_newstate();

    // 明示的に標準ライブラリの読み込み
    luaL_openlibs(state);

    lua_register(state, "twice", [](lua_State *pL) -> int {
        // Luaからの引数を取得する
        const lua_Number ret = lua_tonumber(pL, 1);
        lua_pop(pL, lua_gettop(pL));

        // Luaに渡す戻り値を詰める
        const int val = static_cast<int>(ret) * 2;
        lua_pushinteger(pL, val);

        const int returnLuaNum = 1; // Luaに渡す戻り値の数
        return returnLuaNum;
    });

    // Luaスクリプトを読み込む
    if (luaL_loadfile(state, filename) || lua_pcall(state, 0, 0, 0)) {
        cerr << "Error: " << lua_tostring(state, -1) << endl;
        return 1;
    }

    // Luaステートの破棄
    lua_close(state);

    return 0;
}
