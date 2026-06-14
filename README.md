# RubikX-3D: Interactive 3D Rubik's Cube Learning & Solving System

RubikX-3D is a feature-rich, high-performance interactive 3D Rubik's Cube learning, visualization, and solving application written in **C++17**, utilizing **OpenGL** and **GLUT/FreeGLUT**. The system is designed to provide a rich visual workspace for learning Rubik's Cube solving techniques while applying fundamental computer graphics concepts like 3D transformations, viewing projection, visible surface detection, alpha compositing, and animation.

---

## 🌟 Key Features

1. **Interactive Setup Screen (Welcome Screen):** An elegant full-screen onboarding configuration UI shown on launch. Users can customize puzzle settings like cube dimension, rendering style, color scheme, and gameplay preferences (Glass Cube, Practice Mode, Auto-Scramble) using keyboard navigation or mouse clicks. This menu can be re-opened anytime with the `O` key.
2. **Multiple Cube Dimensions:** Switch dynamically between cube sizes ranging from **2x2** up to **7x7**. Camera zoom distances adapt automatically to accommodate the size of the puzzle.
3. **Reorganized HUD Layout:** A single, clean HUD panel on the left of the screen, categorized logically (Cube Moves, Game Actions, Settings & View) to minimize visual clutter. Prefabs and active modes (e.g., Glass Cube, Render Mode, and Solve Time/History stats) are highlighted using overlay badges and scoreboards with fixed overlap layouts.
4. **Advanced rendering Styles:** Solid, Wireframe, and Textured rendering modes toggled dynamically using the `W` key.
5. **Transparency & Alpha Blending (Glass Cube):** Semi-transparent rendering mode using OpenGL alpha blending (`GL_BLEND`) and depth sorting (Painter's algorithm) to reveal internal structure.
6. **Multiple Color Scheme Palettes:** Dynamic cycle through Classic, Pastel, and High Contrast color schemes for sticker faces using the `C` key.
7. **Random Solvable Scrambling:** Scramble generation with custom move counts scaled based on the cube size (from 10 moves for 2x2 up to 55 moves for 7x7) to ensure valid, solvable configurations.
8. **Step-by-Step Interactive Solver:** Play, pause, slow down, speed up, advance a step, or reverse a step through auto-solved solutions. Includes keyboard shortcuts for checkerboard, superflip, and cube-in-cube patterns on various cube sizes.
9. **Interactive 3D Viewport:** Free orbit-camera navigation with click-and-drag mouse controls, panning, and scroll-wheel zoom.

---

## 🛠️ Tech Stack

* **Language:** C++17
* **Graphics API:** OpenGL 2.1 (fixed-function pipeline)
* **Windowing & Inputs:** GLUT / FreeGLUT
* **Build System:** CMake (cross-platform support)

---

## 📂 Project Directory Structure

```
rubikx-3d/
├── CMakeLists.txt                 # CMake build configuration
├── README.md                      # Project documentation (this file)
│
├── src/                           # Source & Header Files
│   ├── main.cpp                   # Application entrypoint & GLUT callbacks (keyboard, mouse, display)
│   │
│   ├── Animation.cpp/.h           # Handles interpolating 3D rotations for smooth face turns
│   ├── Camera.cpp/.h              # Orbiting perspective camera matrix configuration
│   ├── Colors.cpp/.h              # Handles sticker color scheme palettes (Classic, Pastel, High Contrast)
│   ├── Lighting.cpp/.h            # Configures diffuse and specular lights for realistic face reflection
│   ├── Renderer.cpp/.h            # Renders 3D cubie shapes, textured face stickers, and wireframe outlines
│   │
│   ├── algorithms/                # Traditional Computer Graphics Algorithms
│   │   ├── Algorithms.h           # Unified algorithms header
│   │   ├── Animation.cpp/.h       # Smooth frame interpolation
│   │   ├── Clipping.cpp/.h        # Polygon and line clipping
│   │   ├── Filling.cpp/.h         # Polygon raster filling routines
│   │   ├── Rasterization.cpp/.h   # 2D raster fonts & line drawing helpers
│   │   ├── Transformations.cpp/.h # Local/world coordinate matrix transformations
│   │   ├── Viewing.cpp/.h         # Projections & viewport transforms
│   │   └── Visibility.cpp/.h      # Depth buffering & backface culling
│   │
│   ├── cube/                      # Rubik's Cube Internal Logic
│   │   ├── CubeFactory.cpp/.h     # Factory to instantiate RubiksCube objects dynamically by size
│   │   ├── Cubie.cpp/.h           # Represents individual sub-cube pieces, positions, and active colors
│   │   ├── Move.cpp/.h            # Represents face rotations (Face, Direction, Layer indices)
│   │   └── RubiksCube.cpp/.h      # Encapsulates Rubik's Cube state and sticker transformations
│   │
│   ├── solver/                    # Automatic Solvers & Scrambling
│   │   ├── PatternLibrary.cpp/.h  # Predefined configurations (Checkerboard, Superflip, etc.)
│   │   ├── ReverseSolver.cpp/.h   # Backwards-step solver that reconstructs movements to solve the puzzle
│   │   └── Scrambler.cpp/.h       # Generates random sequence of moves to scramble the cube
│   │
│   ├── ui/                        # User Interface Overlays
│   │   ├── HUD.cpp/.h             # Left menu text panel, scoreboard statistics, and status badges
│   │   ├── SolutionPlayer.cpp/.h  # Handles playing/stepping through solver animations with variable speeds
│   │   └── WelcomeScreen.cpp/.h   # Renders the centered full-screen modal setup & config form
│   │
│   └── utils/                     # General Utilities
│       ├── MathUtils.cpp/.h       # Matrices & quaternion rotations mathematical operations
│       └── ScoreManager.cpp/.h    # Solved state check, time tracking, scores, and session logs
│
└── docs/                          # Developer Guides and Guidelines
    ├── DEVELOPMENT_GUIDELINE.md   # Guidelines for building, writing code, and style conventions
    └── improvements.md            # Planned UI and feature improvements logs
```

---

## 🚀 How to Build and Run (Cross-Platform)

This project uses **CMake** as the unified cross-platform build system.

### 🍎 macOS

1. Install dependencies:
   ```bash
   brew install cmake freeglut
   ```
2. Configure and build:
   ```bash
   cmake -B build
   cmake --build build
   ```
3. Run the executable:
   ```bash
   ./build/RubikX3D
   ```

### 🐧 Linux (Ubuntu/Debian)

1. Install dependencies:
   ```bash
   sudo apt update
   sudo apt install build-essential cmake freeglut3-dev libglew-dev
   ```
2. Configure and build:
   ```bash
   cmake -B build
   cmake --build build
   ```
3. Run the executable:
   ```bash
   ./build/RubikX3D
   ```

### 🪟 Windows

1. Install [CMake](https://cmake.org/download/) and a C++ compiler (such as **Visual Studio** with the *C++ Desktop Development* workload).
2. Configure and build (run in Command Prompt or PowerShell):
   ```cmd
   cmake -B build
   cmake --build build --config Release
   ```
3. Run the executable:
   ```cmd
   .\build\Release\RubikX3D.exe
   ```

---

## 🎮 Keyboard & Mouse Controls

### Welcome / Setup Screen

| Control | Action |
|:---:|---|
| `UP` / `DOWN` / `LEFT` / `RIGHT` | Navigate options inside the Welcome screen grid |
| `SPACE` | Select / Toggle active option (Size, Style, Scheme, Preferences) |
| `ENTER` | Start game with selected configuration |
| `Mouse Click` | Directly click cards or buttons to select preferences or start |
| `Esc` | Close Welcome Screen (if active) |

### Interactive Game Workspace (Main Screen)

The main keyboard controls are organized by category and displayed in the left HUD overlay:

#### 1. Cube Moves (CW / CCW)

Rotate the outer layers of the active face. Use **Uppercase** letters for Clockwise rotation, and **Lowercase** letters for Counter-Clockwise rotation:

* **Right Face:** `R` (CW) / `r` (CCW)
* **Left Face:** `L` (CW) / `l` (CCW)
* **Up Face:** `U` (CW) / `u` (CCW)
* **Down Face:** `D` (CW) / `d` (CCW)
* **Front Face:** `F` (CW) / `f` (CCW)
* **Back Face:** `B` (CW) / `b` (CCW)

#### 2. Game Actions

* `S` / `s`: Scramble the cube randomly (move sequence scaled based on dimension)
* `Z` / `z`: Auto-solve the current cube state (populates solution steps)
* `X` / `x`: Reset cube instantly to solved state
* `Y` / `y`: Retry the last generated scramble sequence
* `!` / `@` / `#`: Apply standard patterns to the cube (Checkerboard on all sizes, Superflip on >= 3, Cube-in-Cube on >= 2)

#### 3. Settings & View

* `O` / `o` / `0`: Open Setup / Welcome Menu screen
* `2` – `7`: Switch Cube Size dynamically (from 2x2x2 up to 7x7x7)
* `C` / `c`: Cycle sticker color schemes (Classic, Pastel, High Contrast)
* `T` / `t`: Toggle Glass Mode (semi-transparent acrylic cubies)
* `W` / `w`: Toggle Render Style (Solid rendering, Wireframe rendering, Textured stickers)
* `M` / `m`: Toggle Practice Mode (timer continues, but score updates/history won't save)
* `Space`: Pause / Resume timer
* `I` / `i`: Toggle Session Stats Panel display (top-right overlay)
* `H` / `h`: Toggle HUD help overlay visibility (left panel)
* `Esc`: Exit application

#### 4. Solution Player Controls

When a solution is active (computed by pressing `Z`):

* `P` / `p`: Play / Pause auto-playback of solution moves
* `.` / `>`: Step forward one move manually
* `,` / `<`: Step backward one move manually
* `+` / `=`: Speed up solution playback speed
* `-` / `_`: Slow down solution playback speed

### Mouse Interaction

* **Orbit Camera:** Click and drag using the **Left Mouse Button**.
* **Zoom View:** Scroll using the **Mouse Wheel** to zoom in or out.
* **Select Options:** Click options on the welcome setup grid directly.

---

## 📐 Computer Graphics Concepts Demonstrated

This system implements key computer graphics concepts directly using OpenGL fixed-function pipeline techniques:

* **3D Transformations:** Local and global matrices are computed dynamically to rotate subsets of Cubies around specific axes (`glMultMatrixf`, `glRotatef`, `glTranslatef`).
* **3D Viewing & Projections:** Features a perspective camera (`gluLookAt` and `gluPerspective`) combined with orthographic overlays (`glOrtho`) for rendering the HUD and menus.
* **Visible Surface Detection:** Implements back-face culling (`glEnable(GL_CULL_FACE)`) and Z-buffering depth tests (`glEnable(GL_DEPTH_TEST)`) to maximize performance.
* **Alpha Compositing:** Implements Glass Cube mode using `glEnable(GL_BLEND)` with source/destination blending factors (`glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)`).
* **Clipping:** Uses near and far clipping planes defined in the viewing volume to clip objects outside the viewport boundaries.
* **Shading & Lighting:** Implements two-point specular and diffuse lighting models for realistic face reflections.
