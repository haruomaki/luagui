#pragma once

#include <array>
#include <chrono>
#include <fstream>
#include <functional> // 関数型 std::function
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <span>
#include <sstream>   // 文字列のストリーム
#include <stdexcept> // 例外はとりあえずこれ std::runime_error
#include <vector>

using std::ostream, std::stringstream, std::vector, std::pair, std::array, std::set, std::string, std::cout, std::cerr, std::endl, std::function;

namespace base {

template <typename T1, typename T2>
ostream &operator<<(ostream &os, const pair<T1, T2> &input) {
    os << "(" << input.first << ", " << input.second << ")";
    return os;
}

template <typename T, size_t n>
ostream &operator<<(ostream &os, const array<T, n> &input) {
    os << "[";
    for (auto it = input.cbegin(); it != input.cend();) {
        os << *it++ << (it == input.cend() ? "" : ", ");
    }
    os << "]";
    return os;
}

template <typename T>
ostream &operator<<(ostream &os, const vector<T> &input) {
    os << "[";
    for (auto it = input.cbegin(); it != input.cend();) {
        os << *it++ << (it == input.cend() ? "" : ", ");
    }
    os << "]";
    return os;
}

template <typename T, size_t extent>
ostream &operator<<(ostream &os, const std::span<T, extent> &input) {
    os << "[";
    for (auto it = input.begin(); it != input.end();) {
        os << *it++ << (it == input.end() ? "" : ", ");
    }
    os << "]";
    return os;
}

template <typename T>
ostream &operator<<(ostream &os, const set<T> &input) {
    os << "{";
    for (auto it = input.cbegin(); it != input.cend();) {
        os << *it++ << (it == input.cend() ? "" : ", ");
    }
    os << "}";
    return os;
}

template <typename T>
string toStr(const T &val) {
    stringstream ss;
    ss << val;
    return ss.str();
}

static std::string strip(const std::string &str, const std::string &chars = " \t\r\n") {
    std::string result = str;
    // 文字列の先頭から指定された文字を削除
    result.erase(0, result.find_first_not_of(chars));
    // 文字列の末尾から指定された文字を削除
    result.erase(result.find_last_not_of(chars) + 1);
    return result;
}

inline std::vector<std::string> split(const std::string &str, const std::string &delimiter = ",", const std::string &strip_chars = " \t\r\n") {
    std::vector<std::string> list = {};
    size_t head = 0, tail = 0;
    do { // TODO: while文に
        tail = str.find(delimiter, head);
        // cout << head << "~" << tail << endl;
        // cout << "progress: " << list << " + " << str.substr(head, tail - head) << endl;
        auto sub = str.substr(head, tail - head);
        list.push_back(strip(sub, strip_chars));
        head = tail + delimiter.length();
    } while (tail != std::string::npos);
    return list;
}

// -----------------------------------
// デバッグ系
// -DDEBUGを指定したときだけ有効になる
// -----------------------------------

inline void printHeadline(const char *icon, const char *file, int line) {
    std::cerr << icon << "(" << file << ":" << line << ")";
}

// 引数を任意の区切り文字で標準エラーに出力
// 注意：改行しない、0引数は受け付けない
template <typename T, typename... Args>
inline void printImpl(const char *sep, const T &arg, const Args &...args) {
    std::cerr << arg;
    ((std::cerr << sep << args), ...);
}

template <typename... Args>
inline void printPre(const char *file, int line, const Args &...args) {
    printHeadline("🐝", file, line);
    if constexpr (sizeof...(args) > 0) {
        std::cerr << " ";
        printImpl("", args...);
    }
    std::cerr << std::endl;
}

template <class... Args> // NOTE: 未初期化変数の警告に対応するためconst T&を受け取る
inline void debugPre(const char *file, int line, const char *argnames, const Args &...args) {
    printHeadline("📦", file, line);
    // argsの要素数 0 or 1 or それ以上
    constexpr size_t len = sizeof...(args);
    if constexpr (len >= 2) {
        std::cerr << " [" << argnames << "] = [";
        printImpl(", ", args...);
        std::cerr << "]";
    } else if constexpr (len == 1) {
        cerr << " " << argnames << " = ";
        printImpl(", " /*doesn't matter*/, args...);
    }
    std::cerr << std::endl;
}

// ラムダ式を受け取り、実行時間を返す
template <typename Func>
inline std::chrono::duration<double> timeImpl(Func &&func) {
    // 時間計測しつつ実行
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();

    return end - start;
}

// ***Pre関数は複文マクロを避ける意味もある
template <typename Func>
inline void timePre(const char *file, int line, Func &&func) {
    auto duration = timeImpl(func);
    printHeadline("⏱️", file, line);
    std::cerr << " " << duration << std::endl;
}

#ifdef DEBUG
#define print(...) printPre(__FILE__, __LINE__ __VA_OPT__(, __VA_ARGS__))               // NOLINT(cppcoreguidelines-macro-usage)
#define debug(...) debugPre(__FILE__, __LINE__, #__VA_ARGS__ __VA_OPT__(, __VA_ARGS__)) // NOLINT(cppcoreguidelines-macro-usage)
#define time(...) timePre(__FILE__, __LINE__, [&] { __VA_ARGS__; })                     // NOLINT(cppcoreguidelines-macro-usage)
#else
#define debug(...)
#define time(...) __VA_ARGS__
#endif

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define DEFINE_RUNTIME_ERROR(name)           \
    class name : public std::runtime_error { \
        using runtime_error::runtime_error;  \
    }

// ファイルから文字列を読み込む
inline string loadString(const string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        cerr << "Failed to open shader file: " << path << '\n';
        return "";
    }
    stringstream ss;
    ss << file.rdbuf();
    file.close();
    return ss.str();
}

auto relu(const auto &x) { return std::max(x, 0); }

// 分割数指定で等差数列を生成する
inline vector<float> linspace(float start, float stop, size_t num, bool endpoint = true) {
    vector<float> ret = {};
    float step = (stop - start) / float(endpoint ? num - 1 : num);
    for (size_t i = 0; i < num; i++) {
        ret.emplace_back(start + step * float(i));
    }
    return ret;
}

// 幅指定で等差数列を生成する
inline vector<float> arange(float start, float stop, float step) {
    vector<float> ret = {};
    float value = start;
    while (step > 0 ? value < stop : stop < value) {
        ret.emplace_back(value);
        value += step;
    }
    return ret;
}

template <typename T>
struct Point {
    T x, y;

    // Point(T x, T y)
    //     : x_(x)
    //     , y_(y) {}

    Point operator+(Point point) const {
        return Point{x + point.x, y + point.y};
    }

    operator pair<T, T>() const {
        return pair<T, T>(x, y);
    }

    friend ostream &operator<<(ostream &os, const Point &point) {
        os << static_cast<pair<T, T>>(point);
        return os;
    }
};

// TODO: C++20のviewsを使う
template <class T>
class MemoryView {
    const T *data_;
    const size_t size_;

  public:
    MemoryView(T *data, size_t size)
        : data_(data)
        , size_(size) {}

    T *data() const { return data_; }
    [[nodiscard]] size_t size() const { return size_; }

    // TODO: ポインタ演算を避ける？
    T &operator[](size_t index) { return data_[index]; }
    const T &operator[](size_t index) const { return data_[index]; }

    friend ostream &operator<<(ostream &os, const MemoryView &view) {
        os << "[";
        for (size_t i = 0; i < view.size();) {
            os << view[i++] << (i < view.size() ? ", " : "");
        }
        os << "]";
        return os;
    }
};

template <typename T, typename Fn>
decltype(auto) map(const vector<T> &vec, Fn &&f) {
    vector<std::invoke_result_t<Fn, T>> result;
    result.reserve(vec.size()); // 処理前に予めメモリを確保しておくと効率的です
    for (const T &element : vec) {
        result.emplace_back(std::forward<Fn>(f)(element)); // 関数を適用して結果を新たなベクターに追加
    }
    return result;
}

} // namespace base

using namespace base;
