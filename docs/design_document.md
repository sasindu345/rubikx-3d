# RubikX 3D: Technical Design & Academic Project Proposal

**Course Component:** CS-401 Computer Graphics & Interactive Systems Project  
**Development Stack:** C++17, OpenGL 2.1 (Fixed-Function Pipeline), GLUT / FreeGLUT  
**Target Platform:** macOS, Linux, Windows  

---

## 1. Project Overview & Objectives

**RubikX 3D** is an interactive 3D learning, visualization, and solving platform designed to demonstrate foundational and advanced concepts in Computer Graphics. Unlike algorithmic puzzles that prioritize complex solving AI (such as Kociemba's two-phase algorithm), RubikX 3D focuses on the graphical representation, geometric transformations, perspective projections, surface visibility, and smooth rendering animations of a modular 3D puzzle.

### System Objectives
- **Interactive Visualization:** Provide a smooth, real-time 3D rendering of Rubik's Cubes of varying sizes ($2\times2$, $3\times3$, $4\times4$).
- **Demonstrate CG Principles:** Map core coursework concepts (3D Transformations, Viewing, Clipping, Back Face Culling, Animation) directly to active, debuggable code components.
- **Provide Visual Solve Learning:** Implement a history-based playback solver that records user scrambles/turns and plays back the exact inverse steps, visually showing users the reverse path to solve the cube.
- **Rich 2D Interface Overlay:** Render a custom heads-up display (HUD) detailing real-time statistics, active playback sequences, and control hints using 2D orthographic projection over the 3D scene.

---

## 2. Functional & Non-Functional Requirements

### Functional Requirements (FR)
- **FR-1 (Multi-Size Cube):** The system shall render $2\times2\times2$, $3\times3\times3$, and $4\times4\times4$ modular cubes dynamically.
- **FR-2 (Face Rotation):** The system shall rotate any face slice clockwise (CW) or counter-clockwise (CCW) via keyboard commands (`R/r`, `L/l`, `U/u`, `D/d`, `F/f`, `B/b`).
- **FR-3 (Interactive Orbit Camera):** The user shall be able to orbit the camera around the cube using click-and-drag mouse actions and zoom using the scroll wheel.
- **FR-4 (History Logging):** The system shall maintain an in-memory chronological stack of all manual and scramble moves.
- **FR-5 (Reverse Solver):** The system shall compute the solver sequence by reversing the history stack and inverting each individual move.
- **FR-6 (Playback Control):** The user shall be able to play, pause, step forward, step backward, and adjust playback speed for the solver.
- **FR-7 (Aesthetics & Scoring):** The system shall maintain scoring metrics based on elapsed time, move counts, and display a visual scoreboard HUD.

### Non-Functional Requirements (NFR)
- **NFR-1 (Frame Rate):** The rendering engine must maintain a consistent update rate of $\ge 60$ frames per second (FPS) for smooth animations.
- **NFR-2 (Platform Portability):** The system codebase must compile cleanly under C++17 on macOS (Clang), Linux (GCC), and Windows (MSVC) using standard CMake build scripts.
- **NFR-3 (Low Latency Input):** Interactive inputs (clicks, drags, keystrokes) must have response times $<16.7\text{ ms}$ (within one frame update).

---

## 3. User Stories & Use Cases

### User Stories
- *As a student learning graphics,* I want to see how individual cubies undergo translation and rotation, so that I can visually trace homogeneous coordinates in practice.
- *As a Rubik's Cube novice,* I want to scramble a cube and watch it solved step-by-step, so that I can visualize and memorize face rotation directions.
- *As an application tester,* I want to manually rotate a face during solver playback, so that I can see the active solver sequence cancel safely and allow new manual moves.

### Use Case Diagram (Textual Representation)
```
  +--------------+
  |  USER        |
  +-------+------+
          |
          +-----> [ UC-1: Scramble Cube ]
          |
          +-----> [ UC-2: Rotate Face Manually ]
          |
          +-----> [ UC-3: Zoom/Orbit Camera ]
          |
          +-----> [ UC-4: Trigger Solver Playback (Z) ]
          |
          +-----> [ UC-5: Play/Pause/Step Solver ]
```

---

## 4. Graphics Concepts Mapping

```mermaid
graph TD
    classDef graphics fill:#f9f,stroke:#333,stroke-width:2px;
    classDef logic fill:#bbf,stroke:#333,stroke-width:1px;

    cubeModel[3D Cubie Mesh] :::graphics --> trans[3D Transformations: Translate/Rotate] :::graphics
    trans --> proj[3D Viewing: gluPerspective / View Matrix] :::graphics
    proj --> culling[Visible Surface Detection: glCullFace / Depth Test] :::graphics
    culling --> clip[Clipping & View Volume: Near/Far planes] :::graphics
    
    subgraph Logic Engine
        history[History Manager] :::logic
        solver[Reverse Solver] :::logic
    end
    
    solver --> animation[Animation Engine: Timer 60FPS] :::graphics
    animation --> trans
```

### Direct Mapping Table
| Graphics Concept | Project Module | OpenGL APIs / Functions | Mathematical Basis |
|---|---|---|---|
| **3D Transformations** | `Renderer` & `Animation` | `glMultMatrixf`, `glTranslatef`, `glRotatef` | Homogeneous transformation matrices, Local to World mapping |
| **3D Viewing** | `Camera` | `gluLookAt`, `glLoadIdentity` | Camera coordinates, View Matrix conversion |
| **Projection** | `Reshape Callback` | `gluPerspective` | Perspective Projection Matrix, field-of-view ($FOV$), aspect ratio |
| **Visible Surface Detection** | `Renderer` / Initialization | `glEnable(GL_CULL_FACE)`, `glEnable(GL_DEPTH_TEST)` | Back Face Culling (dot product $\vec{N} \cdot \vec{V} < 0$), Z-Buffer |
| **Clipping & View Volume** | `Reshape Callback` | Near/Far clipping limits inside `gluPerspective` | Viewing Frustum plane clipping equations |
| **Computer Animation** | `Timer Callback` | `glutTimerFunc`, interpolation updates | Frame-time delta updates, linear interpolation ($\theta(t)$) |

---

## 5. Mathematical & OpenGL Implementation Basis

### A. 3D Transformations
Every cubie in the Rubik's Cube is modeled as an independent mesh centered initially at local coordinate origin. To place a cubie at its correct position $(x, y, z)$ within the $N\times N\times N$ grid, we translate it. During face rotations, a group of cubies rotates around the rotation axis of the selected slice.

#### 1. Translation Matrix
To position a cubie at grid offset $(t_x, t_y, t_z)$:
$$
T(t_x, t_y, t_z) = \begin{bmatrix}
1 & 0 & 0 & t_x \\
0 & 1 & 0 & t_y \\
0 & 0 & 1 & t_z \\
0 & 0 & 0 & 1
\end{bmatrix}
$$

#### 2. Rotation Matrices
Rotation of a face slice is performed by rotating affected cubies around one of the three primary axes:
- **X-axis rotation (Right/Left faces) by angle $\theta$:**
$$
R_x(\theta) = \begin{bmatrix}
1 & 0 & 0 & 0 \\
0 & \cos\theta & -\sin\theta & 0 \\
0 & \sin\theta & \cos\theta & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}
$$
- **Y-axis rotation (Up/Down faces) by angle $\theta$:**
$$
R_y(\theta) = \begin{bmatrix}
\cos\theta & 0 & \sin\theta & 0 \\
0 & 1 & 0 & 0 \\
-\sin\theta & 0 & \cos\theta & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}
$$
- **Z-axis rotation (Front/Back faces) by angle $\theta$:**
$$
R_z(\theta) = \begin{bmatrix}
\cos\theta & -\sin\theta & 0 & 0 \\
\sin\theta & \cos\theta & 0 & 0 \\
0 & 0 & 1 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}
$$

#### 3. OpenGL Implementation
We apply transformations hierarchically using OpenGL's matrix stack:
```cpp
glPushMatrix();
// 1. If cubie is in the actively rotating slice, apply dynamic rotation
if (isRotatingSlice) {
    glRotatef(animationAngle, axisX, axisY, axisZ);
}
// 2. Translate cubie to its grid position
glTranslatef(posX, posY, posZ);
// 3. Render cubie mesh
drawCubie();
glPopMatrix();
```

---

### B. 3D Viewing & Projections
The system transforms coordinates from World Space into Camera Space (Viewing Transformation) and then into Normalized Device Coordinates (Projection Transformation).

```
   [Local Object Space]
            |
            | (Model Matrix: glTranslatef, glRotatef)
            v
     [World Space]
            |
            | (View Matrix: gluLookAt)
            v
     [Camera/Eye Space]
            |
            | (Projection Matrix: gluPerspective)
            v
  [Clip Space / Normalized Device Coordinates]
```

#### 1. View Matrix (gluLookAt)
Given eye position $\mathbf{E}$, look-at target $\mathbf{T}$, and up vector $\mathbf{U}$, we compute the viewing coordinate frame:
- Forward vector: $\mathbf{f} = \frac{\mathbf{T} - \mathbf{E}}{\|\mathbf{T} - \mathbf{E}\|}$
- Side vector: $\mathbf{s} = \frac{\mathbf{f} \times \mathbf{U}}{\|\mathbf{f} \times \mathbf{U}\|}$
- Real Up vector: $\mathbf{u} = \mathbf{s} \times \mathbf{f}$

The view matrix $V$ is:
$$
V = \begin{bmatrix}
s_x & s_y & s_z & -\mathbf{s} \cdot \mathbf{E} \\
u_x & u_y & u_z & -\mathbf{u} \cdot \mathbf{E} \\
-f_x & -f_y & -f_z & \mathbf{f} \cdot \mathbf{E} \\
0 & 0 & 0 & 1
\end{bmatrix}
$$

#### 2. Perspective Projection Matrix
To simulate human sight, perspective projection scales coordinates inversely with distance $z$. The frustum is defined by field of view ($fov$), aspect ratio ($aspect$), near plane ($z_{near}$), and far plane ($z_{far}$):
$$
P = \begin{bmatrix}
\frac{\cot(fov/2)}{aspect} & 0 & 0 & 0 \\
0 & \cot(fov/2) & 0 & 0 \\
0 & 0 & -\frac{z_{far} + z_{near}}{z_{far} - z_{near}} & -\frac{2 z_{far} z_{near}}{z_{far} - z_{near}} \\
0 & 0 & -1 & 0
\end{bmatrix}
$$

#### 3. OpenGL Implementation
```cpp
// Reshape: Setup Projection
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(45.0, aspect, 0.1, 100.0);

// Render: Setup Camera View
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
gluLookAt(eyeX, eyeY, eyeZ, targetX, targetY, targetZ, upX, upY, upZ);
```

---

### C. Visible Surface Detection (VSD)
To prevent rendering interior or rear-facing polygons, we enable hardware-accelerated Z-buffering and Back-Face Culling.

#### 1. Back-Face Culling
Every polygon face has a surface normal vector $\vec{N}$ pointing outward, and a viewing vector $\vec{V}$ pointing from the eye to the polygon face.
- If $\vec{N} \cdot \vec{V} \ge 0$, the face points away from the camera and is culled.
- If $\vec{N} \cdot \vec{V} < 0$, the face is visible and is rendered.

```
       Visible Face (Rendered)              Culled Face (Muted)
       
              Normal \                              / Normal
                      \                            /
                       v                          v
             +------------------+        +------------------+
             |   Front Face     |        |    Back Face     |
             +------------------+        +------------------+
                      ^                          ^
                      |                          |
                   Eye (V)                    Eye (V)
                   
                N . V < 0                  N . V > 0
```

#### 2. OpenGL Implementation
```cpp
glEnable(GL_DEPTH_TEST); // Enable Z-buffer comparison
glDepthFunc(GL_LESS);    // Nearer objects overwrite farther ones

glEnable(GL_CULL_FACE);  // Enable face culling
glCullFace(GL_BACK);     // Cull polygons facing away from viewer
glFrontFace(GL_CCW);     // Define Counter-Clockwise vertices as front-facing
```

---

### D. Clipping & Viewing Volume
Objects outside the frustum must be discarded (clipped) to prevent incorrect projection arithmetic.
- The six planes of the viewing frustum (Near, Far, Left, Right, Top, Bottom) define the limits.
- A cubie whose coordinates fall completely outside these boundaries is clipped out of the rendering pipeline.
- *Near Plane:* $z = -0.1$ prevents geometry extremely close to the camera from blocking the viewport.
- *Far Plane:* $z = -100.0$ bounds the visibility envelope, discarding objects too far to be visible.

---

### E. Animation updates
Face rotation animations represent a sequence of keyframes updated linearly at regular intervals.
- The angle $\theta$ interpolates from $0^\circ$ to $90^\circ$ at speed $S$ (degrees per second):
$$
\theta(t) = \theta_0 + S \cdot \Delta t
$$
- Once $\theta(t) \ge 90^\circ$, the animation is marked complete, the logical model matrix is permanently updated with the completed move, and the animation state resets.

---

## 6. Project Directory Structure

```
RubikX-3D/
├── CMakeLists.txt              # Build system rules
├── Makefile                    # Root fallback compiler scripts
├── README.md                   # Installation and usage guides
│
├── include/                    # Header Directory
│   ├── core/                   # Cube Logical State Representational Headers
│   │   ├── Cubie.h
│   │   ├── RubiksCube.h
│   │   └── CubeFactory.h
│   ├── graphics/               # Renderer, Lighting, Camera, Animation Engine Headers
│   │   ├── Renderer.h
│   │   ├── Camera.h
│   │   ├── Lighting.h
│   │   └── Animation.h
│   ├── solver/                 # Reverse Scramble / Undoing Logic Headers
│   │   └── Scrambler.h
│   ├── ui/                     # 2D HUD Scoreboards & Tape Control overlays
│   │   ├── HUD.h
│   │   └── SolutionPlayer.h
│   └── utils/                  # Helper math, timing, score tracking libraries
│       └── Timer.h
│
├── src/                        # Implementation Files
│   ├── main.cpp                # Application entrypoint & GLUT callbacks
│   ├── core/
│   │   ├── Cubie.cpp
│   │   ├── RubiksCube.cpp
│   │   └── CubeFactory.cpp
│   ├── graphics/
│   │   ├── Renderer.cpp
│   │   ├── Camera.cpp
│   │   ├── Lighting.cpp
│   │   └── Animation.cpp
│   ├── solver/
│   │   └── Scrambler.cpp
│   ├── ui/
│   │   ├── HUD.cpp
│   │   └── SolutionPlayer.cpp
│   └── utils/
│       └── Timer.cpp
└── docs/                       # Project Technical Documentation
    └── design_document.md
```

---

## 7. Team Responsibility & Workload Matrix

To establish clear individual accountability, the project is structured so each of the 5 team members owns specific directories and files.

### Workload Assignment Matrix

| Member | Core Focus | Files Owned | Key Deliverables |
|---|---|---|---|
| **Member 1** | 3D Cubie Modeling & 3D Transformations | [Cubie.cpp](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/src/core/Cubie.cpp)<br>[RubiksCube.cpp](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/src/core/RubiksCube.cpp)<br>[CubeFactory.cpp](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/src/core/CubeFactory.cpp) | 3D mesh representation of cubies, coloring external faces, homogeneous coordinate transforms for face slicing. |
| **Member 2** | Camera System & Projection View | [Camera.cpp](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/src/graphics/Camera.cpp) | Orbit camera coordinate frame calculations, translation/projection setup via `gluPerspective` and `gluLookAt`. |
| **Member 3** | Rendering Engine & Visible Surfaces | [Renderer.cpp](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/src/graphics/Renderer.cpp)<br>[Lighting.cpp](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/src/graphics/Lighting.cpp) | Solid color face drawings, double buffering setup, face normals, back-face culling activations, two-point lighting rigs. |
| **Member 4** | Animation Engine & Timing loops | [Animation.cpp](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/src/graphics/Animation.cpp)<br>[Timer.cpp](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/src/utils/Timer.cpp) | Interpolating rotation angles, queueing anim frames at 60 FPS, trigger solver playback transitions. |
| **Member 5** | Puzzle Solver, Scoring, and HUD | [HUD.cpp](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/src/ui/HUD.cpp)<br>[SolutionPlayer.cpp](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/src/ui/SolutionPlayer.cpp)<br>[Scrambler.cpp](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/src/solver/Scrambler.cpp) | In-memory move history logs, scramble/solve generators, orthographic overlay HUD rendering, scores and timers. |

---

## 8. Class Diagram & Architecture Descriptions

### Key Class Interfaces

#### 1. Core Logic Group (Member 1)
```cpp
struct Cubie {
    int ix, iy, iz; // Grid index positions (0 to N-1)
    Colors::ColorName colors[6]; // RGB colors for Right, Left, Up, Down, Front, Back
    
    Cubie(int x, int y, int z, int size);
    void rotate(Face face, Direction dir, int size);
};

class RubiksCube {
private:
    int size;
    std::vector<Cubie> cubies;
public:
    RubiksCube(int size);
    void reset();
    void applyMove(const Move& move);
    bool isSolved() const;
};
```

#### 2. Graphics & Animation Group (Members 2, 3, & 4)
```cpp
class Camera {
private:
    float radius, theta, phi; // Spherical coordinates
public:
    void orbit(float dTheta, float dPhi);
    void zoom(float dRadius);
    void apply() const; // calls gluLookAt
};

class Animation {
public:
    Move currentMove;
    float currentAngle;
    bool active;
    std::queue<Move> moveQueue;
    
    void queueMove(const Move& move);
    bool update(float deltaTime); // returns true when rotation completes
};
```

#### 3. Controller & UI Group (Member 5)
```cpp
class SolutionPlayer {
private:
    std::vector<Move> moves;
    int currentIndex;
    bool playing;
public:
    void setMoves(const std::vector<Move>& newMoves);
    void stepForward(Animation& anim, RubiksCube& cube);
    void stepBackward(Animation& anim, RubiksCube& cube);
    void update(float deltaTime, Animation& anim, RubiksCube& cube);
};
```

---

## 9. System Sequence & Execution Flow

This diagram describes the loop cycle from input events to screen redraws:

```
[User Input Event] --(keyboard/mouse)--> [main.cpp: Keyboard Callbacks]
                                                |
                                                v
                                       [queueUserMove()]
                                                |
                                                +---> [Updates moveHistory]
                                                |
                                                v
                                       [Animation::queueMove()]
                                                |
                                                v
[Timer Callback] --------------------> [Animation::update()]
                                                |
                                                +---> (On completion: updates activeCube)
                                                |
                                                v
                                       [glutPostRedisplay()]
                                                |
                                                v
[Render Frame] ----------------------> [display()]
                                                |
                                                +---> [Camera::apply()]
                                                +---> [Renderer::renderCube()]
                                                +---> [HUD::render()] (Orthographic 2D)
```

---

## 10. Auto-Solver Design: Reversing Scramble Stack

To maintain simplicity and prevent complex algorithmic bottlenecks (e.g. implementing full Kociemba solver state parsing in C++), the system uses an academic-grade **History-Based Reversal Solver**.

```
                   --- SCRAMBLING & PLAY ---
   [Solved Cube] ---> (Apply M1, M2, M3) ---> [Scrambled State]
   History Stack: [M1, M2, M3]

                   --- HISTORY RESOLUTION (Z) ---
   History Stack: [M1, M2, M3] --(Reverse & Invert)--> [M3', M2', M1']
   Loaded Solver Moves: [M3', M2', M1']

                   --- SOLVING PLAYBACK ---
   [Scrambled State] ---> (Apply M3', M2', M1') ---> [Solved Cube]
```

### Logical Verification Flow
1. **Scramble Phase:** When `S` is pressed, the scramble generator produces $K$ random moves ($M_1, M_2, \ldots, M_K$).
   - The logical model is reset to the solved state first.
   - The history stack `moveHistory` is cleared.
   - The moves are pushed sequentially to `moveHistory` and queued for visual animation.
2. **Manual Play Phase:** If the user performs manual turns, the moves are appended to `moveHistory`.
   - To keep history clean, consecutive opposite moves (such as `R` followed immediately by `r`) are automatically popped from the stack (canceling out).
3. **Solver Generation (Z-key):** When the solve command is triggered:
   - A solution vector is generated by iterating backward through `moveHistory` and applying `getInverse()` to each move.
   - The inverse moves list is loaded into `solutionPlayer`.
4. **Playback Phase:** The user autoplays or steps through the solution sequence.
   - If the user makes a manual rotation move during playback, the playback immediately cancels to prevent state divergence.
   - When the final move finishes, `activeCube.isSolved()` evaluates to `true`, clearing the remaining history stack and solver display.

---

## 11. 12-Week Development Roadmap & Milestones

```
Week  1-2: Setup Build System, window creation, CMake integration, & GLUT event loop loops.
Week  3-4: 3D modeling of Cubies, drawing color faces, & enabling lighting rigs. [M1, M3]
Week  5-6: Setup orbital camera coordinates, viewing projection matrix, & mouse interactions. [M2]
Week  7-8: Build face rotation calculations, local-world matrices, and timer loops. [M1, M4]
Week  9-10: Implement moveHistory trackers, undo vectors, and play/pause solution playback. [M5]
Week 11-12: Implement orthographic overlay text HUD, verify all builds, & draft final documents. [All]
```

* **Milestone 1 (Week 4):** Standard 3D cube rendered on screen with static camera perspective.
* **Milestone 2 (Week 8):** Smooth animated face rotations running via keyboard inputs.
* **Milestone 3 (Week 10):** History tracker saving moves and replaying them backwards to solve the cube.
* **Milestone 4 (Week 12):** Fully documented HUD overlay with stats, complete walkthrough, and test cases.

---

## 12. Appendix: Project README.md Draft

### RubikX-3D - Interactive C++/OpenGL Rubik's Cube Learning & Solving System

RubikX-3D is an educational computer graphics application demonstrating 3D transformations, viewing projection, visible surface detection, back-face culling, and frame-rate independent animations.

### Quick Setup

#### Prerequisites
Ensure you have CMake and FreeGLUT installed on your system:
- **macOS:** `brew install cmake freeglut`
- **Linux (Debian/Ubuntu):** `sudo apt-get install cmake freeglut3-dev`
- **Windows:** Download binary packages for CMake and FreeGLUT.

#### Compilation & Running
1. Compile using standard Makefile:
   ```bash
   make
   ```
2. Run the application:
   ```bash
   ./RubikX3D
   ```

### Key Keyboard Controls
- `S`/`s`: Scramble cube.
- `R`/`r`, `L`/`l`, `U`/`u`, `D`/`d`, `F`/`f`, `B`/`b`: Manual face rotation CW (uppercase) / CCW (lowercase).
- `Z`/`z`: Generate backward steps solver path.
- `P`/`p`: Toggle autoplay solver sequence.
- `,`/`<` and `.`/`>`: Step backward / forward through solving steps.
- `H`/`h`: Show/Hide instruction HUD panel.
- `Esc`: Close application.

---

## 13. System Improvements & Future Enhancements

For detailed plans on potential system improvements, graphics engine modernization, and the complete architecture for dynamic cube size selection, please refer to the dedicated [improvements.md](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/docs/improvements.md) document.

### Summary of Future Enhancements
1. **Graphics Pipeline Modernization:** Transitioning to compiled GLSL shaders and Vertex Array Objects (VAOs) under OpenGL 3.3+.
2. **Dynamic Cube Size Scaling:** Automating the camera radius scaling ($10.0\text{f} + 2.0\text{f} \cdot (\text{size} - 3)$) and generating dynamic scrambling ranges.
3. **Interactive 3D Picking:** Implementing mouse raycasting for dragging faces directly.
4. **Optimal Solvers:** Integrating phase-based shortest-path solving solvers.
