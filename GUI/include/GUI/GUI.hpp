#pragma once

#include <SumiGL/Context.hpp>

struct GUI {
    GL::Context ctx;

    // メインループに制御を移す。ctx.mainloopと全く同じ。
    template <typename F = void (*)()>
    void mainloop(F &&custom_routine = [] {}) {
        ctx.mainloop(std::forward<F>(custom_routine));
    }
};
