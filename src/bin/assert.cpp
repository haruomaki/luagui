#include <assert_test.hpp>

int main() {
    int x = 334;
    std::string str = "hello";

    Property p;

    p + 9;
    // p + str;

    (p = 9.5).show();
    // p = str;
    // p += str;

    Derived q;
    q.showPrettily();
    // (q += 2).showPrettily(); エラーになる
}
