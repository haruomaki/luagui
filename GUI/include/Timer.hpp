#include <chrono>
#include <functional>
#include <thread>

// https://anohobby.com/cpp-self-made-timer/
inline void setInterval(const float interval_sec, const std::function<bool()> &proc) {
    const auto interval = std::chrono::duration<float>(interval_sec);

    std::thread([proc, interval] {
        while (true) {
            auto start = std::chrono::system_clock::now();
            if (!proc()) {
                break;
            }
            auto waste = std::chrono::system_clock::now() - start;
            if (waste < interval) {
                std::this_thread::sleep_for(interval - waste);
            }
        }
    }).detach();
}
