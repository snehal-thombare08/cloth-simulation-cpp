# 🧵 Cloth Simulation in C++ with SFML

A real-time physics-based cloth simulation built from scratch using **C++17** and **SFML 3.0**, featuring Verlet integration, wind dynamics, interactive tearing, and strain-based color visualization.

![Cloth Simulation Preview](https://raw.githubusercontent.com/snehal-thombare08/cloth-simulation-cpp/main/Screenshot%202026-06-17%20105222.png)

---

## ✨ Features

- **Verlet Integration** — Stable, realistic cloth physics without springs
- **Interactive Grab** — Click and drag to pull the cloth in any direction
- **Tear / Cut Mode** — Right-click or use Scissors mode to rip cloth apart
- **Wind Simulation** — Dynamic gusting wind with smooth interpolation
- **Strain Visualization** — Cloth color shifts blue → green → red as it stretches
- **Pin System** — Top edge pinned with red pin indicators
- **Unpin All** — Drop the entire cloth and watch it fall with gravity
- **Reset** — Instantly regenerate a fresh cloth

---

## 🎮 Controls

| Key / Button | Action |
|---|---|
| `LMB` (Left Click + Drag) | Grab and move cloth |
| `RMB` (Right Click) | Tear cloth instantly |
| `W` | Toggle Wind ON/OFF |
| `S` | Toggle Scissors mode (move mouse to cut) |
| `G` | Unpin all — cloth falls freely |
| `R` | Reset cloth to original state |
| `Esc` | Quit |

---
## screenshot
![Cloth Simulation Preview](https://raw.githubusercontent.com/snehal-thombare08/cloth-simulation-cpp/main/Screenshot%202026-06-17%20105222.png)

## 🛠️ Tech Stack

| Tool | Version |
|---|---|
| Language | C++17 |
| Graphics | SFML 3.0.2 |
| Build System | CMake 3.16+ |
| Compiler | MinGW GCC 16.1 |
| Package Manager | vcpkg |

---

## ⚙️ Physics Details

- **Algorithm:** Verlet position integration (no velocity stored explicitly)
- **Constraint Solver:** 5 iterations per frame for stability
- **Tear Threshold:** Stick breaks when stretched beyond 2.8× rest length
- **Damping:** 0.99 per frame (slight energy loss for realism)
- **Grid Size:** 60 × 40 points = 2400 particles, ~4600 sticks

---

## 🚀 Build & Run (Windows)

### Prerequisites
- [MinGW-w64](https://www.mingw-w64.org/)
- [CMake](https://cmake.org/)
- [vcpkg](https://github.com/microsoft/vcpkg) with SFML 3

### Steps

```bash
git clone https://github.com/snehal-thombare08/cloth-simulation-cpp.git
cd cloth-simulation-cpp
mkdir build && cd build

cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake \
         -DSFML_DIR=C:/vcpkg/installed/x86-mingw-dynamic/share/sfml \
         -DCMAKE_CXX_COMPILER="C:/mingw64/mingw32/bin/g++.exe" \
         -G "MinGW Makefiles"

mingw32-make
```

Copy DLLs and run:
```bash
copy "C:\vcpkg\installed\x86-mingw-dynamic\debug\bin\*.dll" .
.\ClothSim.exe
```

---

## 📦 Download

👉 **[Download ClothSim-v1.0-Windows.zip](https://github.com/snehal-thombare08/cloth-simulation-cpp/releases/tag/v1.0)**

Extract all files and run `ClothSim.exe` — no install needed!

---

## 📁 Project Structure

```
cloth-simulation-cpp/
├── src/
│   └── main.cpp        # All physics + rendering code
├── CMakeLists.txt      # Build configuration
├── .gitignore
└── README.md
```

---

## 🎯 What I Learned

- Implementing **Verlet integration** for stable cloth physics
- **Constraint solving** with iterative relaxation
- Real-time rendering with **SFML VertexArray** for performance
- **Dynamic color mapping** based on physical strain
- Handling **DLL dependencies** for Windows C++ deployment

---

*Built with ❤️ using C++ and SFML 3.0*
