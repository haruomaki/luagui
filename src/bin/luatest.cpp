#include <sol/sol.hpp>

class Vec2 {
  public:
    int x, y;
};

int main() {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::coroutine);

    lua.new_usertype<Vec2>(
        "Vec2",
        "x",
        &Vec2::x);

    lua.script(R"(
    )");
}
