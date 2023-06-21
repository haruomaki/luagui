#include <chrono>
#include <functional>
#include <thread>

// https://anohobby.com/cpp-self-made-timer/
static void setInterval(int interval_ms, const std::function<bool()> &proc) {
    auto interval = std::chrono::milliseconds(interval_ms);

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
