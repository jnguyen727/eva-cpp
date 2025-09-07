# NERV Defense (Terminal Simulation)

This is a small terminal-based defense game.  
Enemies ("angels") spawn on the grid and move toward your base.  
Turrets automatically fire projectiles to stop them.  

## How to Play
- Run the program in your terminal.
- Watch as turrets engage incoming enemies.
- The simulation ends when the set time is up or enemies reach the base.

## Controls
- **p** = Pause  
- **n** = Single-step (only works when paused)  
- **q** = Quit  

## Command Line Options
You can customize the simulation when you run it:
- `--hz <number>` → How many ticks (updates) per second (default: 20)  
- `--duration <seconds>` → How long the simulation runs (default: 10)  
- `--seed <number>` → Random seed for reproducible runs  
- `--spawn_ms <milliseconds>` → Time between enemy spawns  
- `--report <file.csv>` → Save tick data into a CSV file  

## Example
```bash
./tokyo3 --hz 10 --duration 15 --seed 42 --spawn_ms 1200 --report ticks.csv
