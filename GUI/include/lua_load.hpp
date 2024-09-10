// Lua入門: https://www.amusement-creators.info/articles/advent_calendar/2019/06_0/

#include <base.hpp>
#include <lua.hpp>

#include <master.hpp>

#include "lua/World.hpp"

// ウィンドウを一つ作成し、メインループを開始する
inline int lua_create_window(lua_State *state) {
    print("lua_create_window開始");

    // Luaスタックから引数を取得
    int width = int(luaL_checkinteger(state, 1));
    int height = int(luaL_checkinteger(state, 2));
    const char *title = luaL_checkstring(state, 3);
    luaL_checktype(state, 4, LUA_TFUNCTION); // 第4引数が関数であることを確認

    // C++側でウィンドウを作成し、Luaのグローバル変数に保持する
    print("ウィンドウ作成開始");
    GUI gui;
    Window &window = gui.create_window(width, height, title);
    print("ウィンドウ作成完了");
    lua_pushlightuserdata(state, &window);
    lua_setglobal(state, "window");

    debug(gui.dpi());

    // Luaの関数をコルーチンとして作成
    lua_State *co = lua_newthread(state);
    lua_pushvalue(state, 4);                      // 第4引数の関数をコピー
    int ref = luaL_ref(state, LUA_REGISTRYINDEX); // 関数への参照を保存

    bool coroutine_finished = false;

    // フレームごとにコルーチンを進める。コルーチンが最後まで到達したら速やかにウィンドウを閉じる。
    // ウィンドウ一つごとに一つのメインループ。
    gui.mainloop([state, co, ref, &coroutine_finished, &window]() {
        if (!coroutine_finished) {
            lua_rawgeti(state, LUA_REGISTRYINDEX, ref); // 保存した関数を取得
            lua_xmove(state, co, 1);                    // 関数をコルーチンのスタックに移動

            int nres;
            int status = lua_resume(co, state, 0, &nres);

            if (status == LUA_YIELD) {
                // コルーチンが yield した
            } else if (status == LUA_OK) {
                // コルーチンが終了した
                std::cout << "Game loop finished" << std::endl;
                coroutine_finished = true;
                window.close();
            } else {
                // エラーが発生した
                std::cerr << "Error in Lua coroutine: " << lua_tostring(co, -1) << std::endl;
                coroutine_finished = true;
                window.close();
            }
        }
    });

    luaL_unref(state, LUA_REGISTRYINDEX, ref); // 参照を解放

    // 結果をLuaに返す（ここではポインタをlightuserdata形式で返している）
    // lua_pushlightuserdata(L, window);
    return 0; // 戻り値の数（0個）
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
    lua_register(state, "create_world", [](lua_State *state) -> int {
        lua_getglobal(state, "window");
        auto &window = dereference<Window>(lua_touserdata(state, -1));

        auto &world = window.create_world();
        auto **world_ptr = static_cast<World **>(lua_newuserdata(state, sizeof(World *)));
        *world_ptr = &world;

        auto &camera = world.append_child<MobileOrthoCamera>();
        camera.set_active();

        luaL_getmetatable(state, "World");
        lua_setmetatable(state, -2);

        return 1;
    });
    register_world(state);

    // Luaスクリプトを読み込む
    if (luaL_loadfile(state, filename) || lua_pcall(state, 0, 0, 0)) {
        cerr << "Error: " << lua_tostring(state, -1) << endl;
        return 1;
    }

    // Luaステートの破棄
    lua_close(state);

    return 0;
}
