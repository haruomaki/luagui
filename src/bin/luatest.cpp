// cmake --build build --target luatest && ./build/Debug/luatest
// Lua入門: https://www.amusement-creators.info/articles/advent_calendar/2019/06_0/

#include <base.hpp>
#include <lua.hpp>

int main() {
    // Luaステートの作成
    lua_State *state = luaL_newstate();

    // 明示的に標準ライブラリの読み込み
    luaL_openlibs(state);

    // Luaスクリプトを読み込む
    if (luaL_loadfile(state, "assets/scripts/luatest.lua") || lua_pcall(state, 0, 0, 0)) {
        cerr << "Error: " << lua_tostring(state, -1) << endl;
        return 1;
    }

    // Luaステートの破棄
    lua_close(state);
}
