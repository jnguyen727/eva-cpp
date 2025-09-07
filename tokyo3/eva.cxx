#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <string>
#include <conio.h>

void render(int tick, double elapsed, double dt_ms, int width, int height, int hq_x, int hq_y);

int main(int argc, char* argv[]) {
    using clock = std::chrono::steady_clock;

    // -----------------------------
    // 1. Default values
    // -----------------------------
    int hz = 20;           // default 20 Hz
    int duration = 10;     // default 10 seconds
    const int width = 20;  // width of ascii grid
    const int height = 10; // height of ascii grid

    int hq_x = width / 2;  // HQ centered horizontally
    int hq_y = height / 2; // HQ centered vertically

    // -----------------------------
    // 2. Parse CLI arguments
    // -----------------------------
    std::string report_path = "";
    std::ofstream report;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--hz" && i + 1 < argc) {
            hz = std::stoi(argv[++i]);
        } else if (arg == "--duration" && i + 1 < argc) {
            duration = std::stoi(argv[++i]);
        } else if (arg == "--report" && i + 1 < argc) {
            report_path = argv[++i];
        }
    }

    if (!report_path.empty()) {
        report.open(report_path);
        report << "tick,planned_ms,actual_ms,oversleep_ms\n";
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

    // Maintain explicit deadline
    auto next_deadline = start + std::chrono::duration_cast<clock::duration>(period);

    // -----------------------------
    // 4. Main loop
    // -----------------------------
    while (running) {
        // Handle input
        if (_kbhit()) {
            char c = _getch();
            if (c == 'q') {
                running = false;
            } else if (c == 'p') {
                paused = !paused;
                std::cout << (paused ? "[PAUSED]\n" : "[RESUMED]\n");

                if (!paused) {
                    // On resume, reset deadline so we don’t try to catch up
                    next_deadline = clock::now() + std::chrono::duration_cast<clock::duration>(period);
                }
            } else if (c == 'n' && paused) {
                step_pending++;
            }
        }

        // Pause logic
        if (paused && step_pending == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        if (paused && step_pending > 0) {
            step_pending--;
            // For single-step, also reset deadline
            next_deadline = clock::now() + std::chrono::duration_cast<clock::duration>(period);
        }

        std::this_thread::sleep_until(next_deadline);
        next_deadline += std::chrono::duration_cast<clock::duration>(period);

        tick++;

        auto now = clock::now();
        double elapsed = tick * (1.0 / hz); // deterministic sim time
        double dt_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(now - prev).count();
        prev = now;

        render(tick, elapsed, dt_ms, width, height, hq_x, hq_y);

        if (report.is_open()) {
            double planned_ms = tick * (1000.0 / hz);
            double actual_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(now - start).count();
            double oversleep = actual_ms - planned_ms;

            report << tick << ","
                   << planned_ms << ","
                   << actual_ms << ","
                   << oversleep << "\n";
        }

        if (elapsed >= duration || !running) {
            break;
        }
    }

    if (report.is_open()) {
        report.close();
        std::cout << "Report written to " << report_path << "\n";
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

/*
 * Renders the grid/map of the simulator.
 * HQ at (hq_x, hq_y) is drawn as 'H'; all other cells are '.'.
 */
void render(int tick, double elapsed, double dt_ms, int width, int height, int hq_x, int hq_y) {
    system("cls"); // clear screen each tick
    std::cout << "TICK "
              << std::setw(4) << std::setfill('0') << tick
              << " | elapsed " << std::fixed << std::setprecision(2) << elapsed
              << " | dt_ms " << std::fixed << std::setprecision(1) << dt_ms
              << '\n';

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == hq_x && y == hq_y) {
                std::cout << "H";
            } else {
                std::cout << ".";
            }
        }
        std::cout << '\n';
    }
    std::cout << '\n';
} /* render() */
