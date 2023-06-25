#include <assert_test.hpp>

int main() {
    int x = 334;
    std::string str = "hello";

    f1(x);
    // f1(str);

    Property p;

    p + 9;
    // p + str;

    p = 9.5;
    // p = str;
    // p += str;
}
