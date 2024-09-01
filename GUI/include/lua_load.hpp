// Lua入門: https://www.amusement-creators.info/articles/advent_calendar/2019/06_0/

#include <base.hpp>
#include <lua.hpp>

#include <master.hpp>

inline void mikan() {
    cout << "みかん🍊" << endl;
}

// Luaから呼び出せるようにラップした関数
int lua_create_window(lua_State *L) {
    // Luaスタックから引数を取得
    const char *title = luaL_checkstring(L, 1);
    int width = luaL_checkinteger(L, 2);
    int height = luaL_checkinteger(L, 3);

    // C++の関数を呼び出す
    mikan();
    GUI gui;
    Window &window = gui.create_window(width, height, "ウィンドウタイトル");
    gui.mainloop();

    // 結果をLuaに返す（ここではポインタをlightuserdata形式で返している）
    // lua_pushlightuserdata(L, window);
    return 0; // 戻り値の数（0個）
}

// Lua側に関数を登録する
void register_lua_functions(lua_State *L) {
}

inline int run_lua(const char *filename) {
    // Luaステートの作成
    lua_State *state = luaL_newstate();

    // 明示的に標準ライブラリの読み込み
    luaL_openlibs(state);

    // GUI関連の関数の登録
    lua_register(state, "create_window", lua_create_window);
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
