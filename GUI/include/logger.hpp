#pragma once

#include <array>
#include <chrono>
#include <iostream>
// #include <print>
#include <extra_formatter.hpp>
#include <source_location>

enum class LogLevel {
    Off,
    Warn,
    Print,
    Info,
    Trace,
};

#ifdef _WIN32
constexpr std::array<const char *, 5> icons = {"", "<warn>", "<print>", "<info>", "<trace>"};
#else
constexpr std::array<const char *, 5> icons = {"", "🐝", "✅", "ℹ️", "🏞️"};
#endif // _WIN32

constexpr LogLevel default_log_level = LogLevel::Print;

// -----------------------------------
// デバッグ系
// -DDEBUGを指定したときだけ有効になる
// -----------------------------------

inline void print_headline(const char *icon, const char *file, unsigned int line) {
    std::cerr << icon << " " << file << ":" << line << "]";
}
inline void print_headline(const char *icon, std::source_location loc) {
    print_headline(icon, loc.file_name(), loc.line());
}

// 引数を任意の区切り文字で標準エラーに出力
// 注意：改行しない、0引数は受け付けない
template <typename T, typename... Args>
inline void print_impl(const char *sep, const T &arg, const Args &...args) {
    // FIXME: プリコンパイル済みヘッダを作ろうとするとエラー
    std::cerr << arg;
    ((std::cerr << sep << args), ...);
    // std::print("{}", arg);
    // ((std::print("{}{}", sep, args)), ...);
}

template <LogLevel level, typename... Ts>
inline void log(std::source_location loc, Ts... args) {
#ifdef DEBUG
    if constexpr (level > default_log_level) return;

    print_headline(icons[int(level)], loc);
    if constexpr (sizeof...(args) > 0) {
        std::cerr << " ";
        print_impl("", args...);
    }
    std::cerr << std::endl;
#endif
}

#define warn(...) log<LogLevel::Warn>(std::source_location::current() __VA_OPT__(, __VA_ARGS__))   // NOLINT(cppcoreguidelines-macro-usage)
#define print(...) log<LogLevel::Print>(std::source_location::current() __VA_OPT__(, __VA_ARGS__)) // NOLINT(cppcoreguidelines-macro-usage)
#define info(...) log<LogLevel::Info>(std::source_location::current() __VA_OPT__(, __VA_ARGS__))   // NOLINT(cppcoreguidelines-macro-usage)
#define trace(...) log<LogLevel::Trace>(std::source_location::current() __VA_OPT__(, __VA_ARGS__)) // NOLINT(cppcoreguidelines-macro-usage)


template <class... Args> // NOTE: 未初期化変数の警告に対応するためconst T&を受け取る
inline void debug_pre(const char *file, int line, const char *argnames, const Args &...args) {
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
    //print_headline("⏱️", file, line);
    print_headline("wa", file, line);
    std::cerr << " " << duration << std::endl;
}

#ifdef DEBUG
#define debug(...) debug_pre(__FILE__, __LINE__, #__VA_ARGS__ __VA_OPT__(, __VA_ARGS__)) // NOLINT(cppcoreguidelines-macro-usage)
#define time(...) time_pre(__FILE__, __LINE__, [&] { __VA_ARGS__; })                     // NOLINT(cppcoreguidelines-macro-usage)
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
