#include <iostream>
#include <source_location>

template <int n, typename... Ts>
void log(std::source_location loc, Ts... args) {
    std::cout << loc.file_name() << ":" << loc.line() << ":" << loc.column() << " ";
    for (size_t i = 0; i < n; i++) {
        (std::cout << ... << args) << std::endl;
    }
}

int main() {
    log<5>(std::source_location::current(), "Hello", " ", "World!"); // 非型引数Nを10として指定し、型引数は推論される
    return 0;
}
