# RubikX-3D — Implementation Plan & Phase-by-Phase Checklist

This document details the roadmap, technical specifications, and progressive checklists for developing the **Interactive 3D Rubik's Cube Learning, Visualization and Solving System**.

---

## 🛠️ Environment Verification

| Dependency | Status | Version / Path |
|---|---|---|
| **C++ Compiler** | ✅ Installed | Apple Clang 17.0.0 (`/usr/bin/clang++`) |
| **CMake** | ✅ Installed | 4.2.3 (`/opt/homebrew/bin/cmake`) |
| **Make** | ✅ Installed | `/usr/bin/make` |
| **OpenGL** | ✅ System Framework | macOS SDK `OpenGL.framework` |
| **FreeGLUT** | ✅ Homebrew | 3.8.0 (`/opt/homebrew/lib/libglut.dylib`) |
| **GLUT Headers** | ✅ Available | `/opt/homebrew/include/GL/glut.h` |
| **Build State** | ✅ Clean compile | 23/23 source files → `RubikX3D` binary |

---

## 📂 Project Directory Structure

```
rubikx-3d/
├── .gitignore                     ✅ Created
├── CMakeLists.txt                 ✅ Created
├── Makefile                       ✅ Created
├── README.md                      ✅ Updated with full documentation
│
├── docs/
│   └── implementation_plan.md     👈 This file
│
├── include/
│   ├── core/
│   │   ├── Cubie.h                ✅ Stub
│   │   ├── RubiksCube.h           ✅ Stub
│   │   ├── Move.h                 ✅ Stub
│   │   └── CubeFactory.h         ✅ Stub
│   ├── graphics/
│   │   ├── Renderer.h             ✅ Stub
│   │   ├── Camera.h               ✅ Stub
│   │   ├── Lighting.h             ✅ Stub
│   │   ├── Animation.h            ✅ Stub
│   │   └── Colors.h               ✅ Stub
│   ├── solver/
│   │   ├── Solver.h               ✅ Stub (abstract interface)
│   │   ├── Solver2x2.h            ✅ Stub
│   │   ├── Solver3x3.h            ✅ Stub
│   │   ├── Solver4x4.h            ✅ Stub
│   │   └── Scrambler.h            ✅ Stub
│   ├── ui/
│   │   ├── HUD.h                  ✅ Stub
│   │   ├── Menu.h                 ✅ Stub
│   │   ├── PatternEditor.h        ✅ Stub
│   │   └── SolutionPlayer.h       ✅ Stub
│   └── utils/
│       ├── MathUtils.h            ✅ Stub
│       ├── Timer.h                ✅ Stub
│       ├── History.h              ✅ Stub
│       └── ScoreManager.h         ✅ Stub
│
├── src/
│   ├── main.cpp                   ✅ GLUT window + callbacks + test triangle
│   ├── core/                      ✅ All stubs
│   ├── graphics/                  ✅ All stubs
│   ├── solver/                    ✅ All stubs
│   ├── ui/                        ✅ All stubs
│   └── utils/                     ✅ All stubs
│
└── data/                          (created on first history save)
```

---

## 📅 Step-by-Step Development Checklist

---

### Phase 1 — Environment Setup & Project Skeleton ✅ COMPLETE

- [x] Install required libraries (FreeGLUT, CMake)
- [x] Verify compiler: Apple Clang 17.0.0 (C++17 support)
- [x] Create `CMakeLists.txt` with OpenGL/GLUT framework linking
- [x] Create fallback `Makefile` for direct compilation
- [x] Create `.gitignore` (build/, *.o, executables, IDE files)
- [x] Write `src/main.cpp` — GLUT initialization, display/reshape/keyboard/mouse callbacks
- [x] Create directory structure under `include/` and `src/` for all components
- [x] Create stub header/source files for all 22 classes
- [x] Update `README.md` with project description, build instructions, controls, CG concepts
- [x] Verify compile: 23 source files compile and link successfully
- [x] Commit Phase 1 skeleton to Git

---

### Phase 2 — Math Utilities & Cube Data Model 🔲 NEXT

- [ ] Implement `MathUtils.h/cpp` — `Vec3` struct (add, subtract, normalize, cross, dot)
- [ ] Implement `MathUtils.h/cpp` — `Mat4` struct (identity, multiply, rotateX/Y/Z, translate, scale)
- [ ] Implement `Colors.h/cpp` — Define 6 face colors as `Vec3` constants (White, Yellow, Red, Orange, Blue, Green) + Black for internal
- [ ] Implement `Move.h/cpp` — `Face` enum (RIGHT, LEFT, UP, DOWN, FRONT, BACK), `Direction` enum (CW, CCW), `Move` struct, inverse function, toString
- [ ] Implement `Cubie.h/cpp` — Grid position (ix, iy, iz), 6 face colors array, `applyRotation(Face, Direction)` to update position & colors
- [ ] Implement `RubiksCube.h/cpp` — `int size`, `vector<Cubie> cubies`, `applyMove(Move)`, `isSolved()`, `reset()`, `getCubies()`
- [ ] Implement `CubeFactory.h/cpp` — `static RubiksCube create(int size)` generating solved 2×2, 3×3, or 4×4
- [ ] Verify: create cube, apply moves, check `isSolved()` returns false, reset, check returns true
- [ ] Verify clean build passes
- [ ] Commit Phase 2 changes to Git

---

### Phase 3 — 3D Rendering (Static Cube) 🔲

- [ ] Implement `Renderer.h/cpp` — `renderCube(const RubiksCube&)`: draw each cubie as 6 colored quads with `glBegin(GL_QUADS)`
- [ ] Add visual gaps (0.05 units) between cubies for grid-line effect
- [ ] Center the Rubik's Cube at coordinate origin (offset by `-size/2`)
- [ ] Implement `Lighting.h/cpp` — Set up ambient, diffuse, and specular lighting (`GL_LIGHTING`, `GL_LIGHT0`)
- [ ] Implement `Camera.h/cpp` — Static camera using `gluLookAt` and perspective projection via `gluPerspective`
- [ ] Update `main.cpp` — Instantiate default 3x3 cube, call renderer in display loop
- [ ] Enable `GL_DEPTH_TEST` and `GL_CULL_FACE` (back-face culling visible surface detection)
- [ ] Verify: static, colored, lit 3x3 cube renders on window
- [ ] Verify clean build passes
- [ ] Commit Phase 3 changes to Git

---

### Phase 4 — Camera Interaction 🔲

- [ ] Extend `Camera` class to track orbit coordinates: `theta` (azimuth), `phi` (elevation), `radius` (distance)
- [ ] Implement `orbit(float dTheta, float dPhi)` with elevation bounds to prevent flipping
- [ ] Implement `zoom(float dRadius)` with safety bounds
- [ ] Apply camera views dynamically to `gluLookAt`
- [ ] Wire mouse click+drag in `main.cpp` to rotate camera view
- [ ] Wire scroll wheel for zooming
- [ ] Map arrow keys to orbit controls as keyboard shortcut
- [ ] Verify: mouse drag rotates view and scroll zooms in/out
- [ ] Verify clean build passes
- [ ] Commit Phase 4 changes to Git

---

### Phase 5 — Face Rotations with Animation 🔲

- [ ] Implement `Animation.h/cpp` — Track active face rotation animation, interpolation angle (0° to 90°) and animation speed
- [ ] Implement animation queue (buffers rapid keypresses and triggers them sequentially)
- [ ] Modify `Renderer.cpp` — rotate matching slice of cubies using `glRotatef` during animations
- [ ] Commit move changes to `RubiksCube` state model once animation concludes
- [ ] Map keys `R/r, L/l, U/u, D/d, F/f, B/b` to CW/CCW face movements
- [ ] Set up `glutTimerFunc` at ~16ms (60 FPS) to step animation values
- [ ] Verify: pressing keys smoothly rotates face layers by 90 degrees
- [ ] Verify clean build passes
- [ ] Commit Phase 5 changes to Git

---

### Phase 6 — Scrambler & Random Puzzle Generation 🔲

- [ ] Implement `Scrambler.h/cpp` — `generateScramble(size, steps)` (e.g. 20 random moves for 3x3)
- [ ] Check scrambler output: filter out redundant moves (e.g., U followed by U')
- [ ] Map `S` key to initiate fast-animation scramble sequence
- [ ] Verify: pressing `S` scrambles cube and counts moves
- [ ] Verify clean build passes
- [ ] Commit Phase 6 changes to Git

---

### Phase 7 — Automatic Solver & Solution Playback 🔲

- [ ] Implement `Solver3x3.h/cpp` — Layer-by-layer Beginner's Method algorithm
- [ ] Implement `Solver2x2.h/cpp` — Ortega solver
- [ ] Implement `Solver4x4.h/cpp` — Center reduction and edge pairing
- [ ] Implement `SolutionPlayer.h/cpp` — Handles play, pause, stepping forward/backward, and playback speed
- [ ] Map `Z` key to solve, `P` for play/pause, `>` for next step, `<` for previous step
- [ ] Verify: clicking `Z` computes solution; steps through the solver smoothly
- [ ] Verify clean build passes
- [ ] Commit Phase 7 changes to Git

---

### Phase 8 — Custom Pattern Editor 🔲

- [ ] Implement `PatternEditor.h/cpp` — Toggle 2D editor mode with `E` key
- [ ] Render flat 2D projection net of the 6 faces using orthographic mode (`glOrtho`)
- [ ] Handle mouse click picking to cycle facelet colors
- [ ] Validate custom state validity (e.g. check color counts)
- [ ] Apply pattern to the 3D cube state and switch back to 3D view
- [ ] Verify: custom net edits apply correctly on 3D cube model
- [ ] Verify clean build passes
- [ ] Commit Phase 8 changes to Git

---

### Phase 9 — Scoring, History & Timer 🔲

- [ ] Implement `Timer.h/cpp` — tracks active solving time using `glutGet(GLUT_ELAPSED_TIME)`
- [ ] Implement `ScoreManager.h/cpp` — calculates user score based on elapsed time, move counts, and hint/solve usage
- [ ] Implement `History.h/cpp` — save and load solve metrics (time, moves, score) to a local file in `data/history.txt`
- [ ] Verify: completing solver logs data to history file; check that file persists
- [ ] Verify clean build passes
- [ ] Commit Phase 9 changes to Git

---

### Phase 10 — HUD, Statistics Dashboard & Polish 🔲

- [ ] Implement `HUD.h/cpp` — draw text panels for active stats (moves, timer, current score) in 2D overlay
- [ ] Render semi-transparent UI window panels using alpha blending
- [ ] Implement `Menu.h/cpp` — right-click popup context menu for easy options switching
- [ ] Implement cube size controls (keys `1`, `2`, `3`)
- [ ] Apply polish: chamfered/beveled cubie margins, smooth lighting normals, color tuning
- [ ] Verify: all HUD metrics and controls respond correctly
- [ ] Verify clean build passes
- [ ] Commit Phase 10 changes to Git
