#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <functional> // 関数型 std::function
#include <iostream>
#include <set>
#include <sstream>   // 文字列のストリーム
#include <stdexcept> // 例外はとりあえずこれ std::runime_error
#include <vector>

using namespace std;

namespace base {

template <typename T1, typename T2>
ostream &operator<<(ostream &os, const pair<T1, T2> &input) {
    os << "(" << input.first << ", " << input.second << ")";
    return os;
}

template <typename T>
ostream &operator<<(ostream &os, const vector<T> &input) {
    os << "[";
    auto it = input.cbegin();
    while (it != input.cend()) {
        os << *it++ << (it == input.cend() ? "" : ", ");
    }
    os << "]";
    return os;
}

template <typename T>
ostream &operator<<(ostream &os, const set<T> &input) {
    os << "{";
    auto it = input.cbegin();
    while (it != input.cend()) {
        os << *it++ << (it == input.cend() ? "" : ", ");
    }
    os << "}";
    return os;
}

template <typename T>
string to_str(const T &val) {
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

static std::vector<std::string> split(const std::string &str, const std::string &delimiter = ",", const std::string &strip_chars = " \t\r\n") {
    std::vector<std::string> list = {};
    size_t head = 0, tail;
    do {
        tail = str.find(delimiter, head);
        // cout << head << "~" << tail << endl;
        // cout << "progress: " << list << " + " << str.substr(head, tail - head) << endl;
        auto sub = str.substr(head, tail - head);
        list.push_back(strip(sub, strip_chars));
        head = tail + delimiter.length();
    } while (tail != std::string::npos);
    return list;
}

// 再帰の終端。引数が0個の場合を担当。改行を出力。
static void debugImpl(bool brace) {}

// 可変長引数。引数が1つ以上存在する場合を担当。
// 最初の引数をHead、残りをTailとして切り離すことを再帰的に行う。
template <class Head, class... Tail>
void debugImpl(bool brace, Head &&head, Tail &&...tail) {
    cerr << head;
    if (sizeof...(Tail) == 0) {
        cerr << (brace ? "]" : "");
    } else {
        cerr << ", ";
    }
    debugImpl(brace, std::move(tail)...);
}

template <class... T>
void debugPre(const char *file, int line, const char *argnames, T &&...args) {
    cerr << "🐝(" << file << ":" << line << ")";
    // argsの要素数 0 or 1 or それ以上
    constexpr size_t n = sizeof...(args);
    if (n >= 2) {
        cout << " [" << argnames << "] = [";
        debugImpl(true, args...);
    } else if (n == 1) {
        cerr << " " << argnames << " = ";
        debugImpl(false, args...);
    }
    cerr << endl;
}

#define debug(...) debugPre(__FILE__, __LINE__, #__VA_ARGS__ __VA_OPT__(, __VA_ARGS__))

template <typename T>
struct Point {
    T x_, y_;

    // Point(T x, T y)
    //     : x_(x)
    //     , y_(y) {}

    Point operator+(Point p) const {
        return Point{x_ + p.x_, y_ + p.y_};
    }

    operator pair<T, T>() const {
        return pair<T, T>(x_, y_);
    }

    friend ostream &operator<<(ostream &os, const Point &p) {
        os << static_cast<pair<T, T>>(p);
        return os;
    }
};

struct RGB {
    unsigned char r_, g_, b_;
};

struct RGBA {
    float r_, g_, b_, a_;
};

template <class T>
class MemoryView {
    T *data_;
    size_t size_;

  public:
    MemoryView(T *data, size_t size)
        : data_(data)
        , size_(size) {}

    T *data() const { return data_; }
    size_t size() const { return size_; }

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

} // namespace base

using namespace base;
