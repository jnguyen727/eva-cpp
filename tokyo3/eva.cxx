#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <string>
#include <conio.h>

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

    bool running = true;
    bool paused = false;
    int step_pending = 0;

    // -----------------------------
    // 4. Main loop
    // -----------------------------
    while (running) {
        auto deadline = start + std::chrono::duration_cast<clock::duration>((tick + 1) * period);

        if (_kbhit()) {
          char c = _getch();
          if (c == 'q') {
            running = false;
          }
          else if (c == 'p') {
            paused = !paused;
            std::cout << (paused ? "[PAUSED]\n" : "[RESUMED]\n");
          }
          else if (c == 'n' && paused) {
            step_pending++;
          }
        }

        /* If we are paused and we have no pending steps, sleep ten milliseconds. */

        if (paused && step_pending == 0) {
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
          continue;
        }

        /* Else, if we are paused and have more steps pending, then decrement to fall through. */
        /* Equivalent to doing one tick. */

        if (paused && step_pending > 0) {
          step_pending--;
        }

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

        if (elapsed >= duration || !running) {
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
