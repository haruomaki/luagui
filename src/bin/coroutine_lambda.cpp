#include <chrono>
#include <coroutine>
#include <iostream>
#include <thread>

// コルーチンの定義
struct UpdateCoroutine {
    struct promise_type {
        // コルーチンの初期化
        UpdateCoroutine get_return_object() {
            return UpdateCoroutine{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        std::suspend_always yield_value(int value) {
            currentValue = value;
            return {};
        }

        int currentValue;
    };

    std::coroutine_handle<promise_type> coroutine;

    explicit UpdateCoroutine(std::coroutine_handle<promise_type> coroutine)
        : coroutine(coroutine) {}
    ~UpdateCoroutine() { coroutine.destroy(); }

    int getValue() {
        coroutine.resume();
        return coroutine.promise().currentValue;
    }
};

// Updateコルーチンを定義したラムダ式
auto Update = []() -> UpdateCoroutine {
    for (int i = 0; i < 5; ++i) {
        co_yield i;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
};

int main() {
    auto updateCoroutine = Update();

    while (true) {
        if (!updateCoroutine.getValue()) {
            break;
        }
    }

    return 0;
}
