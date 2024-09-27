#include <sol/sol.hpp>

class Vec2 {
  public:
    int x, y;
};

int main() {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::coroutine);

    // INFO: sol3.3.0だと、Clang16~18ではコンパイル不可 https://github.com/ThePhD/sol2/issues/1581
    // <https://github.com/ThePhD/sol2/issues/1581#issuecomment-2103463524>に従い、sol.hppを修正して対応。
    lua.new_usertype<Vec2>(
        "Vec2",
        "x",
        &Vec2::x);

    lua.script(R"(
    )");
}
