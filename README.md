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
10. **Complete Move Set Support:** Full keyboard coverage of all Rubik's Cube notation — outer face moves, wide moves, inner layer moves (via the layer-arm system), slice moves, and whole-cube rotations — enabling manual execution of any solver-generated solution across all cube sizes from 2x2 to 7x7.

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

### macOS

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

### Linux (Ubuntu/Debian)

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

### Windows

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

---

#### 1. Cube Moves

RubikX-3D supports the **complete standard Rubik's Cube notation** across all cube sizes (2x2 through 7x7). Every move type that the auto-solver generates can also be executed manually via keyboard.

---

##### 1a. Outer Face Moves (CW / CCW)

Rotate the single outermost layer of a face. **Uppercase** = Clockwise, **Lowercase** = Counter-Clockwise.

| Move | Key (CW) | Key (CCW) | Face |
|:----:|:--------:|:---------:|------|
| R / R' | `R` | `r` | Right face |
| L / L' | `L` | `l` | Left face |
| U / U' | `U` | `u` | Up face |
| D / D' | `D` | `d` | Down face |
| F / F' | `F` | `f` | Front face |
| B / B' | `B` | `b` | Back face |

> These 6 moves and their inverses are mathematically sufficient to solve any NxN cube. Every scrambled state reachable on a 2x2 through 7x7 can be solved using only combinations of R, L, U, D, F, and B.

---

##### 1b. Double Moves (180° rotation)

A double move rotates a face 180° — equivalent to pressing the face key twice in a row.

| Move | Keys |
|:----:|------|
| R2 | `R` `R` |
| U2 | `U` `U` |
| F2 | `F` `F` |
| *(any face)* 2 | Press that face key twice consecutively |

---

##### 1c. Wide Moves — Outer + Inner Layer Together (3x3 and above)

Wide moves rotate the outermost layer **and the layer immediately inside it** simultaneously. In standard notation, wide moves are written as lowercase letters (`r`, `u`, `f` etc.) or with a `w` suffix (`Rw`, `Uw`).

Hold `Ctrl` with the face key for CW. Add `Shift` for CCW.

| Move | Key (CW) | Key (CCW) | Equivalent notation |
|:----:|:--------:|:---------:|---------------------|
| Rw / Rw' | `Ctrl+R` | `Ctrl+Shift+R` | r / r' |
| Lw / Lw' | `Ctrl+L` | `Ctrl+Shift+L` | l / l' |
| Uw / Uw' | `Ctrl+U` | `Ctrl+Shift+U` | u / u' |
| Dw / Dw' | `Ctrl+D` | `Ctrl+Shift+D` | d / d' |
| Fw / Fw' | `Ctrl+F` | `Ctrl+Shift+F` | f / f' |
| Bw / Bw' | `Ctrl+B` | `Ctrl+Shift+B` | b / b' |

> Wide moves are skipped automatically on 2x2 cubes since there is no inner layer to include.

---

##### 1d. Inner Layer Moves — Targeted Deep Slices (4x4 and above)

For large cubes (4x4 through 7x7), specific inner layers can be targeted using the **two-step layer arm system**:

**Step 1 — Arm the layer:** Hold `Alt` and press a digit (`2` through `7`) matching the layer number counting from the outside in. The HUD will display an orange banner confirming the armed layer.

**Step 2 — Fire the move:** Press any face key to rotate that specific layer. Uppercase = CW, Lowercase = CCW. The armed layer resets automatically after the move.

| Layer notation | Key sequence (CW) | Key sequence (CCW) | Works on |
|:--------------:|:-----------------:|:------------------:|----------|
| 2R / 2R' | `Alt+2` → `R` | `Alt+2` → `r` | 3x3–7x7 |
| 3L / 3L' | `Alt+3` → `L` | `Alt+3` → `l` | 4x4–7x7 |
| 4D / 4D' | `Alt+4` → `D` | `Alt+4` → `d` | 5x5–7x7 |
| 5U / 5U' | `Alt+5` → `U` | `Alt+5` → `u` | 6x6–7x7 |
| 6F / 6F' | `Alt+6` → `F` | `Alt+6` → `f` | 7x7 only |
| *(any N + face)* | `Alt+N` → face key | `Alt+N` → lowercase | N ≤ cube size |

> If the armed layer number is greater than or equal to the current cube size, the move is cancelled and a warning is printed to the console.

> Pressing any non-face key (e.g. `Esc`, `Space`, digit) while a layer is armed will cancel the arm without executing a move.

---

##### 1e. Slice Moves — True Middle Layer (odd-sized cubes: 3x3, 5x5, 7x7)

Slice moves rotate the exact centre layer of the cube. They only have a meaningful effect on odd-sized cubes where a true middle layer exists.

| Move | Key (CW) | Key (CCW) | Axis |
|:----:|:--------:|:---------:|------|
| M / M' | `J` | `j` | Between L and R faces |
| E / E' | `G` | `g` | Between U and D faces |
| S / S' | `N` | `n` | Between F and B faces |

> `S`, `M`, and `E` keys are unavailable (already bound to Scramble, and reserved system keys), so `J`/`G`/`N` are used as conflict-free alternatives.

---

##### 1f. Whole-Cube Rotations

These rotate the **entire cube** in space without changing its scramble state. Useful for viewing the cube from a different angle or executing algorithms from a different orientation.

| Rotation | Key (CW) | Key (CCW) | Axis |
|:--------:|:--------:|:---------:|------|
| x / x' | `[` | `]` | R-axis (follows Right face) |
| y / y' | `;` | `'` | U-axis (follows Up face) |
| z / z' | `\` | `/` | F-axis (follows Front face) |

---

##### Move notation quick-reference summary

| Category | Key pattern | Example |
|----------|-------------|---------|
| Outer CW | Uppercase letter | `R` |
| Outer CCW | Lowercase letter | `r` |
| Double (180°) | Same key twice | `R` `R` |
| Wide CW | `Ctrl` + uppercase | `Ctrl+U` |
| Wide CCW | `Ctrl+Shift` + uppercase | `Ctrl+Shift+U` |
| Inner layer CW | `Alt+digit` → uppercase | `Alt+3` → `L` |
| Inner layer CCW | `Alt+digit` → lowercase | `Alt+3` → `l` |
| Slice CW | `J` / `G` / `N` | `J` (M move) |
| Slice CCW | `j` / `g` / `n` | `j` (M' move) |
| Cube rotation CW | `[` / `;` / `\` | `[` (x rotation) |
| Cube rotation CCW | `]` / `'` / `/` | `]` (x' rotation) |

---

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
