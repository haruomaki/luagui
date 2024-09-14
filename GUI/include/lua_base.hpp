#include <lua.hpp>

#include <stdexcept>

// void*（またはT*など）をT&に変換する。
template <typename T, typename Ptr>
inline T &dereference(Ptr *ptr) {
    if (!ptr) {
        throw std::runtime_error("Null pointer exception");
    }
    return *static_cast<T *>(ptr);
}
