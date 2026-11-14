# A BLAST FROM THE PAST

Vertical scrolling shooter mini-game made in C++, using SDL3, for the first challenge at CodeRun.

## Team

Team: Coace Doamne Prunele
Division: 3
Members: Stoian Andrea, Trif Alexandra, Salagean Lorena

## Gameplay

You control the green square at the bottom of the screen. Three orange enemies
spawn in the upper half of the window at random positions. Destroy all of them
without being hit.

### Controls

| Input | Action |
| --- | --- |
| Move the mouse | Move your ship (it stays inside the window) |
| Hold left mouse button | Fire upward, one shot every 0.3 s |
| `R` | Restart, once the round is over |
| `Esc` | Quit |

### Rules

- Your shots are the red circles, travelling straight up.
- Enemies start shooting one second into the round, then every three seconds.
  Their magenta shots are aimed at wherever your ship was when they fired, and
  travel at a fifth of your shots' speed.
- **You win** when every enemy is destroyed — your ship turns blue.
- **You lose** if an enemy shot hits you, or if you touch an enemy — your ship
  turns red.

The window is resizable, and the playfield adapts to the new size.

## Building

Requires CMake 3.22+, a C++20 compiler, SDL3, and glm.

On Ubuntu 24.04 there is no `libsdl3-dev` package yet, so SDL3 has to be built
from source and installed into `~/.local`. `CMakeLists.txt` already looks there
via `CMAKE_PREFIX_PATH`. glm comes from `apt install libglm-dev`.

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/SDL3Training
```

In VS Code, `Ctrl+Shift+B` builds and `F5` builds then runs under the debugger.

## Project Structure

| File | Contents |
| --- | --- |
| `main.cpp` | Game loop, input, collisions, win/lose state |
| `rectangle.h` / `rectangle.cpp` | `Rectangle` shape — the player and enemies |
| `circle.h` / `circle.cpp` | `Circle` shape — the projectiles |
| `CMakeLists.txt` | Build configuration |
| `pdf/` | Challenge brief and task description |

Shapes are drawn with the `FillBordered*` helpers, which fill the shape in its
own colour and then outline it in black. Every new `.cpp` file has to be added
to `add_executable` in `CMakeLists.txt` or it will not be compiled or linked.
