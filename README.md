# RubikX-3D: Interactive 3D Rubik's Cube Learning & Solving System

RubikX-3D is a feature-rich, high-performance interactive 3D Rubik's Cube learning, visualization, and solving application written in **C++17**, utilizing **OpenGL** and **GLUT/FreeGLUT**. The system is designed to provide a rich visual workspace for learning Rubik's Cube solving techniques while applying fundamental computer graphics concepts like 3D transformations, viewing projection, visible surface detection, and animation.

---

## 🌟 Key Features

1. **Multiple Cube Types:** Switch dynamically between **2x2**, **3x3**, and **4x4** Rubik's Cubes.
2. **Interactive 3D Viewport:** Free orbit-camera navigation, panning, zooming, and projection mapping.
3. **Smooth Face Rotation Animations:** Realistic rotation speeds and ease-in/out transitions.
4. **Random Solvable Scrambling:** Auto-scrambling with optimal move counts to generate valid, solvable puzzles.
5. **Step-by-Step Interactive Solver:** Automatic solvers with play, pause, slow down, speed up, next-step, and previous-step controls.
6. **2D Flat Pattern Editor:** A layout editor to customize your own color schemes and designs.
7. **Performance Scoring & HUD:** Real-time statistics overlay (move counter, active stopwatch, score multiplier, state history tracker).
8. **Save Solving History:** Persistent scoreboards saved locally to compare speed and moves across attempts.

---

## 🛠️ Tech Stack

* **Language:** C++17
* **Graphics API:** OpenGL 2.1 (fixed-function pipeline)
* **Windowing & Inputs:** GLUT / FreeGLUT
* **Build System:** CMake (fallback support for standard Makefiles)

---

## 📂 Project Directory Structure

```
rubikx-3d/
├── CMakeLists.txt                 # CMake build configuration
├── Makefile                       # Fallback Makefile
├── README.md                      # Project documentation (this file)
│
├── include/                       # Header Files
│   ├── core/                      # Core Cube Logic (Cubie, RubiksCube, Move)
│   ├── graphics/                  # Rendering, Camera, Lighting, Animation
│   ├── solver/                    # Solver algorithms (2x2, 3x3, 4x4)
│   ├── ui/                        # HUD, Menus, Pattern Editor
│   └── utils/                     # Timer, Math, History, Scoring
│
├── src/                           # Source Files
│   ├── main.cpp                   # Application entrypoint & GLUT loop
│   ├── core/                      # Cubie, RubiksCube, Move logic
│   ├── graphics/                  # Renderer, Camera, Lighting, Animation
│   ├── solver/                    # Ortega (2x2), Beginner (3x3), Reduction (4x4)
│   ├── ui/                        # HUD overlays & editor nets
│   └── utils/                     # Timer, History, Scoring, Math helpers
│
└── data/                          # Persistent storage (logs, scores)
```

---

## 🚀 How to Build and Run (macOS)

### Option A: Using CMake (Recommended)

1. Make sure you have CMake installed:
   ```bash
   brew install cmake freeglut
   ```
2. Configure and build:
   ```bash
   mkdir -p build && cd build
   cmake ..
   make
   ```
3. Run the executable:
   ```bash
   ./RubikX3D
   ```

### Option B: Using standard `make` (Fallback)

1. Simply run:
   ```bash
   make
   ```
2. Run the executable:
   ```bash
   ./RubikX3D
   ```

---

## 🎮 Keyboard & Mouse Controls

### Keyboard Controls

| Key | Action |
|:---:|---|
| `R` / `r` | Rotate **Right** face (Clockwise / Counter-Clockwise) |
| `L` / `l` | Rotate **Left** face (Clockwise / Counter-Clockwise) |
| `U` / `u` | Rotate **Up/Top** face (Clockwise / Counter-Clockwise) |
| `D` / `d` | Rotate **Down/Bottom** face (Clockwise / Counter-Clockwise) |
| `F` / `f` | Rotate **Front** face (Clockwise / Counter-Clockwise) |
| `B` / `b` | Rotate **Back** face (Clockwise / Counter-Clockwise) |
| `S` / `s` | Scramble the cube randomly |
| `Z` / `z` | Auto-solve the current cube state |
| `P` / `p` | Play/Pause automatic solution steps |
| `>` / `<` | Advance / Go back one step in solver mode |
| `+` / `-` | Speed up / Slow down rotation animations |
| `1` / `2` / `3` | Switch Cube Size (2x2, 3x3, 4x4) |
| `E` / `e` | Enter / Exit Custom Flat Pattern Editor mode |
| `H` / `h` | Toggle history overlay scoreboard panel |
| `Esc` | Reset current mode / Exit application |

### Mouse Interaction

* **Orbit Camera:** Click and drag using the **Left Mouse Button**.
* **Zoom View:** Scroll using the **Mouse Wheel** to zoom in or out.
* **Context Options:** Click the **Right Mouse Button** to open a quick actions menu.

---

## 📐 Computer Graphics Concepts Demonstrated

This system implements key graphics concepts directly using low-level OpenGL/GLUT techniques:
* **3D Transformations:** Local and global matrices are computed dynamically to rotate subsets of Cubies around specific axes (`glMultMatrixf`, `glRotatef`, `glTranslatef`).
* **3D Viewing & Projections:** Features a perspective camera (`gluLookAt` and `gluPerspective`) combined with orthographic overlays (`glOrtho`) for rendering the HUD.
* **Visible Surface Detection:** Implements back-face culling (`glEnable(GL_CULL_FACE)`) and Z-buffering depth tests (`glEnable(GL_DEPTH_TEST)`) to maximize performance.
* **Clipping:** Uses near and far clipping planes defined in the viewing volume to clip objects outside the viewport boundaries.
* **Shading & Lighting:** Implements two-point specular and diffuse lighting models for realistic face reflections.
