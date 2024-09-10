#pragma once

#include <iostream>
#include <set>
#include <span>

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &input) {
    os << "(" << input.first << ", " << input.second << ")";
    return os;
}

template <typename T, size_t n>
std::ostream &operator<<(std::ostream &os, const std::array<T, n> &input) {
    os << "[";
    for (auto it = input.cbegin(); it != input.cend();) {
        os << *it++ << (it == input.cend() ? "" : ", ");
    }
    os << "]";
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &input) {
    os << "[";
    for (auto it = input.cbegin(); it != input.cend();) {
        os << *it++ << (it == input.cend() ? "" : ", ");
    }
    os << "]";
    return os;
}

template <typename T, size_t extent>
std::ostream &operator<<(std::ostream &os, const std::span<T, extent> &input) {
    os << "[";
    for (auto it = input.begin(); it != input.end();) {
        os << *it++ << (it == input.end() ? "" : ", ");
    }
    os << "]";
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::set<T> &input) {
    os << "{";
    for (auto it = input.cbegin(); it != input.cend();) {
        os << *it++ << (it == input.cend() ? "" : ", ");
    }
    os << "}";
    return os;
}
