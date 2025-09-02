#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <string>

int main(int argc, char* argv[]) {
    using clock = std::chrono::steady_clock;

    // -----------------------------
    // 1. Default values
    // -----------------------------
    int hz = 20;          // default 20 Hz
    int duration = 10;    // default 10 seconds

    // -----------------------------
    // 2. Parse CLI arguments
    // -----------------------------
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--hz" && i + 1 < argc) {
            hz = std::stoi(argv[++i]);
        } else if (arg == "--duration" && i + 1 < argc) {
            duration = std::stoi(argv[++i]);
        }
    }

    if (hz <= 0 || duration <= 0) {
        std::cerr << "Invalid arguments: hz and duration must be > 0\n";
        return 1;
    }

    // -----------------------------
    // 3. Timing setup
    // -----------------------------
    auto period = std::chrono::duration<double>(1.0 / hz); // seconds per tick
    auto start = clock::now();
    auto prev = start;

    int tick = 0;

    // -----------------------------
    // 4. Main loop
    // -----------------------------
    while (true) {
        auto deadline = start + std::chrono::duration_cast<clock::duration>((tick + 1) * period);
        std::this_thread::sleep_until(deadline);

        auto now = clock::now();
        double elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - start).count();
        double dt_ms   = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(now - prev).count();
        prev = now;

        std::cout << "TICK "
                  << std::setw(4) << std::setfill('0') << (tick + 1)
                  << " | elapsed " << std::fixed << std::setprecision(2) << elapsed
                  << " | dt_ms " << std::fixed << std::setprecision(1) << dt_ms
                  << std::endl;

        tick++;

        if (elapsed >= duration) {
            break;
        }
    }

    // -----------------------------
    // 5. Summary
    // -----------------------------
    int expected = hz * duration;
    double error_pct = 100.0 * (tick - expected) / expected;

    std::cout << "\n--- Summary ---\n"
              << "Total ticks: " << tick << "\n"
              << "Expected:    " << expected << "\n"
              << "Error:       " << std::fixed << std::setprecision(2) << error_pct << "%\n";
}
