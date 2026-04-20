# 🟡 PacmanIOI

A fully-featured Pac-Man clone built from scratch in **C++** using **SFML 3**. All graphics and audio are procedurally generated — no external assets required.

---

## ✨ Features

- 🎮 **Classic Pac-Man gameplay** — Navigate the maze, eat all the orbs, and avoid the ghosts
- 👻 **4 AI ghost enemies** — Red, Pink, Blue, and Orange ghosts with Chase / Scatter / Frightened behaviour
- 💊 **3 powerup types** — Health (bonus points), Power (frighten ghosts), and Shield (temporary invincibility)
- 🔊 **Procedural audio** — All sound effects (blop, powerup, shield, ghost-eat, death, win) are synthesised at runtime
- 🎨 **Procedural graphics** — Animated chomping Pac-Man, ghost bobbing, portal effects, shield aura — all drawn with SFML primitives
- 🌀 **Tunnel wrapping** — Pac-Man and ghosts wrap around the edges of the map
- 🏆 **Victory / Game Over screen** — End screen with "Play Again" or "Quit" options
- ❤️ **Lives system** — 3 lives per game; lose a life on ghost contact (unless shielded)

---

## 🗂️ Project Structure

```
PacmanIOI/
├── main.cpp                          # Entry point — game loop, input, state management
├── arena.cpp                         # Maze rendering (rounded walls) + portal animations
├── globals.h                         # Shared types, enums, constants, map data, utility functions
├── globals_new.h                     # Alternate globals (without ghostIndex field)
├── pacman_movement.h                 # Keyboard input handling + tile-based entity movement
├── pacman_draw.h                     # Animated Pac-Man drawing (chomping mouth)
├── ghost_system.h                    # Ghost AI (Chase/Scatter/Frightened), collision, drawing
├── ghostVisuals.h                    # Detailed ghost rendering (body, eyes, moods, horns)
├── ghost_direction_and_visual_p1.h   # Basic ghost body drawing + direction helpers
├── xp_orbs.h                        # Orb spawning, rendering, collection, and score HUD
├── powerups.h                        # Powerup rendering (Health/Power/Shield) + shield aura
├── sounds.h                          # Procedural sound synthesis (all SFX generated in code)
├── victory_screen.h                  # End screen UI (win/lose message + menu)
├── .gitignore                        # Ignores compiled executables
└── sfml-*-3.dll / sfml-*-d-3.dll     # SFML 3 runtime DLLs (release + debug)
```

---

## 🛠️ Prerequisites

| Dependency | Version |
|---|---|
| **C++ compiler** | C++17 or later (MSVC / MinGW / GCC / Clang) |
| **SFML** | 3.x (Graphics, Window, Audio, System modules) |
| **OS** | Windows (DLLs included; other platforms need SFML installed separately) |

> The project uses `sf::Font("C:/Windows/Fonts/arial.ttf")` — this path is Windows-specific. On other platforms, change it to a valid font path.

---

## 🔨 Building

### With g++ (MinGW)

```bash
g++ -std=c++17 main.cpp -o arena.exe -lsfml-graphics-3 -lsfml-window-3 -lsfml-audio-3 -lsfml-system-3
```

### With MSVC (Developer Command Prompt)

```bash
cl /std:c++17 /EHsc main.cpp /link sfml-graphics-3.lib sfml-window-3.lib sfml-audio-3.lib sfml-system-3.lib
```

> Make sure the SFML `include/` and `lib/` directories are on your compiler's search paths, and that the `.dll` files are next to the executable at runtime.

---

## ▶️ Running

```bash
./arena.exe
```

---

## 🎮 Controls

| Key | Action |
|---|---|
| `W` / `↑` | Move Up |
| `S` / `↓` | Move Down |
| `A` / `←` | Move Left |
| `D` / `→` | Move Right |
| `↑` / `↓` + `Enter` | Navigate end screen menu |

---

## 🕹️ Gameplay

### Objective
Collect **all orbs** on the map to win. Avoid ghosts — each hit costs a life (you start with **3 lives**).

### Powerups
Powerups spawn randomly on walkable tiles at the start of each round:

| Powerup | Icon | Effect |
|---|---|---|
| **Health** | ❤️ Pulsing heart | +50 bonus points |
| **Power** | ⭐ Orbiting stars | Frightens all ghosts for **8 seconds** — eat them for points! |
| **Shield** | 🛡️ Green hexagon | Protects from ghost hits for **10 seconds** |

### Ghost AI
The four ghosts cycle between three behaviour modes:

| Mode | Behaviour | Duration |
|---|---|---|
| **Scatter** | Each ghost targets its assigned corner | 7 seconds |
| **Chase** | Ghosts pursue Pac-Man directly | 20 seconds |
| **Frightened** | Ghosts move randomly and turn blue — vulnerable to being eaten | 8 seconds (triggered by Power powerup) |

Ghosts move at **85%** of Pac-Man's speed.

### End Screen
When all orbs are collected (**win**) or all lives are lost (**game over**), an end screen appears with:
- **"Hurray, you won!"** or **"Skill issue, you lost"**
- **Play Again** — resets everything for a new round
- **Quit** — closes the game

---

## 🧩 Architecture Overview

```
main.cpp
  │
  ├── globals.h ─────────────── Enums, structs, map, utility functions
  │
  ├── arena.cpp ─────────────── drawArena(), drawPortal()
  │     └── globals.h
  │
  ├── pacman_movement.h ─────── handlePacmanInput(), moveEntity()
  │     └── globals.h
  │
  ├── pacman_draw.h ─────────── drawPacman()
  │     └── globals.h
  │
  ├── xp_orbs.h ─────────────── spawnOrbs(), drawOrbs(), collectOrbs(), drawScore()
  │     └── globals.h
  │
  ├── powerups.h ────────────── drawHealthPowerup(), drawPowerPowerup(),
  │     └── globals.h            drawShieldPowerup(), drawShieldAura()
  │
  ├── sounds.h ──────────────── makeBlopSound(), makePowerupSound(), etc.
  │
  ├── ghost_system.h ────────── Ghost struct, AI, collision, drawing
  │     ├── globals.h
  │     └── pacman_movement.h    (reuses moveEntity for ghost movement)
  │
  └── victory_screen.h ─────── EndScreen class (win/lose UI)
        └── globals.h
```

---

## 📝 License

This project is for educational / IOI competition purposes.