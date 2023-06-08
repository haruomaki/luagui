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
    auto it = input.cbegin();
    os << "[" << *it;
    for (it++; it != input.cend(); it++) {
        os << ", " << *it;
    }
    os << "]";
    return os;
}

template <typename T>
ostream &operator<<(ostream &os, const set<T> &input) {
    auto it = input.cbegin();
    os << "{" << *it;
    for (it++; it != input.cend(); it++) {
        os << ", " << *it;
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

} // namespace base

using namespace base;
