#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <chrono>
#include <thread>
#include <functional>

template <typename Duration>
class Watchdog {
public:
    Watchdog(Duration timeout, std::function<void()> timeoutHandler)
        : timeout(timeout), timeoutHandler(timeoutHandler), thread(&Watchdog::watch, this) {}

    void reset() {
        lastReset = std::chrono::steady_clock::now();
    }

    void stop() {
        running = false;
    }

    void watch() {
        while (running) {
            auto now = std::chrono::steady_clock::now();
            if (now - lastReset > timeout) {
                timeoutHandler();
                lastReset = now;
            }
            std::this_thread::sleep_for(timeout / 10);
        }
    }

private:
    Duration timeout;
    std::function<void()> timeoutHandler;
    std::atomic<bool> running{true};
    std::chrono::time_point<std::chrono::steady_clock> lastReset = std::chrono::steady_clock::now();
};

#endif // WATCHDOG_H

