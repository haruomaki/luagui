#pragma once

#include <iostream>
#include <source_location>

// enum class LogLevel {
//     Trace,
//     Warn,
// };

// constexpr LogLevel log_level = LogLevel::Trace;

// struct LogLevelWithLocation {
//     int value;
//     std::source_location loc;

//     LogLevelWithLocation(int log_level,
//                          const std::source_location &l = std::source_location::current())
//         : value(log_level)
//         , loc(l) {}
// };

// template <typename... Args>
// void debug(LogLevelWithLocation lv, Args &&...args) {
//     printf("%s:%d] ", lv.loc.file_name(), lv.loc.line());
//     ((std::cout << std::forward<Args>(args) << " "), ...);
//     std::cout << std::endl;
// }

// -----------------------------------
// デバッグ系
// -DDEBUGを指定したときだけ有効になる
// -----------------------------------

inline void print_headline(const char *icon, const char *file, int line) {
    std::cerr << icon << "(" << file << ":" << line << ")";
}

// 引数を任意の区切り文字で標準エラーに出力
// 注意：改行しない、0引数は受け付けない
template <typename T, typename... Args>
inline void print_impl(const char *sep, const T &arg, const Args &...args) {
    std::cerr << arg;
    ((std::cerr << sep << args), ...);
}

template <typename... Args>
inline void print_pre(const char *icon, const char *file, int line, const Args &...args) {
    print_headline(icon, file, line);
    if constexpr (sizeof...(args) > 0) {
        std::cerr << " ";
        print_impl("", args...);
    }
    std::cerr << std::endl;
}

template <class... Args> // NOTE: 未初期化変数の警告に対応するためconst T&を受け取る
inline void dump_pre(const char *file, int line, const char *argnames, const Args &...args) {
    print_headline("📦", file, line);
    // argsの要素数 0 or 1 or それ以上
    constexpr size_t len = sizeof...(args);
    if constexpr (len >= 2) {
        std::cerr << " [" << argnames << "] = [";
        print_impl(", ", args...);
        std::cerr << "]";
    } else if constexpr (len == 1) {
        std::cerr << " " << argnames << " = ";
        print_impl(", " /*doesn't matter*/, args...);
    }
    std::cerr << std::endl;
}

// ラムダ式を受け取り、実行時間を返す
template <typename Func>
inline std::chrono::duration<double> time_impl(Func &&func) {
    // 時間計測しつつ実行
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();

    return end - start;
}

// ***Pre関数は複文マクロを避ける意味もある
template <typename Func>
inline void time_pre(const char *file, int line, Func &&func) {
    auto duration = timeImpl(func);
    print_headline("⏱️", file, line);
    std::cerr << " " << duration << std::endl;
}

#ifdef DEBUG
#define print(...) print_pre("✅", __FILE__, __LINE__ __VA_OPT__(, __VA_ARGS__))       // NOLINT(cppcoreguidelines-macro-usage)
#define warn(...) print_pre("🐝", __FILE__, __LINE__ __VA_OPT__(, __VA_ARGS__))        // NOLINT(cppcoreguidelines-macro-usage)
#define dump(...) dump_pre(__FILE__, __LINE__, #__VA_ARGS__ __VA_OPT__(, __VA_ARGS__)) // NOLINT(cppcoreguidelines-macro-usage)
#define time(...) time_pre(__FILE__, __LINE__, [&] { __VA_ARGS__; })                   // NOLINT(cppcoreguidelines-macro-usage)
#else
#define debug(...)
#define time(...) __VA_ARGS__
#endif

// template <int log_level, typename... Ts>
// struct new_print {
//     new_print(Ts &&...ts, const std::source_location &loc = std::source_location::current()) {
//         if constexpr (log_level >= 3) {
//             std::cout << loc.function_name() << " line " << loc.line() << ": ";
//             ((std::cout << std::forward<Ts>(ts) << " "), ...);
//             std::cout << std::endl;
//         }
//     }
// };

// template <typename... Ts>
// new_print(Ts &&...) -> new_print<4, Ts...>;
