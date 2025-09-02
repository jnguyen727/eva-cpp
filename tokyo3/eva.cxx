#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

int main() {
    using clock = std::chrono::steady_clock;

    auto start = clock::now();                        // time_point
    std::chrono::duration<double> period(0.05);       // 20 Hz = 50 ms
    auto prev = start;                                // time_point

    for (int tick = 0; tick < 50; tick++) {
        auto deadline = start + std::chrono::duration_cast<clock::duration>((tick + 1) * period);
        std::this_thread::sleep_until(deadline);

        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(deadline - start).count();
        auto dt_ms   = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(deadline - prev).count();
        prev = deadline;

        std::cout << "TICK "
                  << std::setw(4) << std::setfill('0') << tick + 1
                  << " | elapsed "
                  << std::fixed << std::setprecision(2) << elapsed
                  << " | dt_ms "
                  << std::fixed << std::setprecision(1) << dt_ms
                  << std::endl;
    }
}
