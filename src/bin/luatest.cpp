#include <sol/sol.hpp>
#include <vector>

class Vec2 {
  public:
    int x, y;
};

class Hoge {
  public:
    Vec2 position;
};

int main() {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::coroutine);

    // INFO: sol3.3.0だと、Clang16~18ではコンパイル不可 https://github.com/ThePhD/sol2/issues/1581
    // <https://github.com/ThePhD/sol2/issues/1581#issuecomment-2103463524>に従い、sol.hppを修正して対応。
    lua.new_usertype<Vec2>(
        "Vec2",
        "x", &Vec2::x,
        "y", &Vec2::y);

    lua.script(R"(
    )");

    lua.new_usertype<Hoge>(
        "Hoge",
        sol::constructors<Hoge()>(),
        "position", sol::property([](Hoge *hoge) { return hoge->position; }, [](Hoge *hoge, std::vector<int> p) { hoge->position = {p[0],
                                                                                                                                    p[1]}; }));
    lua.set_function("new_hoge", [] { return Hoge{}; });

    lua.script_file("assets/scripts/luatest.lua");
}
