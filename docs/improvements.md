# RubikX 3D: Architecture Improvements & Dynamic Size Selection

This document details immediate improvements, future enhancements, and the architectural design for dynamic cube size selection ($2\times2$, $3\times3$, $4\times4$, $5\times5$, and arbitrary $N\times N$).

---

## 1. System Improvements Classification

The proposed system improvements are structured into four key categories:

### A. Core Engine & State Management (Immediate & Architectural)
- **Model-View-Controller (MVC) Separation:** Decouple core cube representation (`RubiksCube`) from the visual rendering pipeline (`Renderer`). Currently, state changes, input logic, and timers are combined inside `src/main.cpp`. Encapsulating application contexts inside an `AppEngine` coordinator class removes global variable dependencies.
- **Dynamic Memory Allocation:** Allow the logical grid representation of Cubies to be re-allocated in heap memory when a user switches sizes, preventing stack bloat.

### B. Rendering & Visual Fidelity (Graphics Modernization)
- **Shader-Based Pipeline (OpenGL 3.3+ Core Profile):** Replace outdated OpenGL 2.1 fixed-function calls (`glBegin`/`glEnd`, `glRotatef`, `glTranslatef`) with compiled custom GLSL shaders (Vertex/Fragment). Manage vertex data using Vertex Buffer Objects (VBOs) and Vertex Array Objects (VAOs) for hardware acceleration.
- **Micro-Chamfers & Sticker Texturing:** Add visual depth by generating rounded corners (bevels) on the cubies and applying sticker grain or carbon fiber textures using 2D texture coordinates (`glTexCoord`).
- **Specular Shader Materials:** Implement Blinn-Phong lighting inside a Fragment Shader to create metallic, plastic, or glossy sticker reflections based on view direction and custom light positions.

### C. Input & User Interaction (Interaction Design)
- **3D Mouse Raycasting (Sticker Picking):** Remove the constraint of using only keyboard shortcuts for face turns. By implementing inverse projection matrix mapping, the user can click and drag individual stickers directly on screen.
- **Orbit Camera Snapping:** Align camera transitions to snap to standard views (Front, Back, Top, Bottom) when double-clicking, assisting user perspective alignment.

### D. Algorithmic puzzle-solving (Puzzle Logic)
- **Optimal Path Solver (Kociemba Algorithm):** Integrate an algorithmic solver that computes optimal paths (often $\le 20$ moves) using phase-based heuristics, rather than relying exclusively on scramble-history reversal.

---

## 2. Dynamic Cube Size Selection Feature Design

To allow users to switch dynamically between different Rubik's Cube sizes ($2\times2$, $3\times3$, $4\times4$, $5\times5$, and arbitrary sizes) while preserving all application features (scrambling, solving playback, rendering, stats tracking, HUD overlays), we propose the following architecture:

### A. Size Selection Input Mapping
We map keys `'1'`, `'2'`, `'3'`, and `'4'` to trigger dynamic size switching:
- `1` $\rightarrow$ Switch to **2x2**
- `2` $\rightarrow$ Switch to **3x3**
- `3` $\rightarrow$ Switch to **4x4**
- `4` $\rightarrow$ Switch to **5x5**

To prevent coordinate corruption, size selection is guarded so it can only be triggered when no face rotations are actively animating.

### B. Dynamic Memory Re-allocation
When a size switch is triggered, the system performs a reset lifecycle:
1. Clear the active face animation queues and reset the animation angle.
2. Re-instantiate the logical cube using `CubeFactory::create(newSize)`.
3. Clear the history stack `moveHistory` and reset `solutionPlayer`.

### C. Automatic Viewport & Camera Scaling
If the camera radius remains fixed, switching from a 2x2 cube to a 5x5 cube would cause the 5x5 cube to clip outside the viewing frustum or appear way too large. We implement **Dynamic Camera Target Distance Scaling** based on cube size $N$:
$$
\text{radius} = \text{Base Radius} + \alpha \cdot (N - 3)
$$
- Where $\text{Base Radius} = 10.0\text{f}$ (calibrated for a $3\times3$ cube).
- $\alpha = 2.0\text{f}$ (scale coefficient per extra unit layer).
- For a $2\times2$ cube: $\text{radius} = 10.0 + 2.0 \cdot (2 - 3) = 8.0\text{f}$.
- For a $5\times5$ cube: $\text{radius} = 10.0 + 2.0 \cdot (5 - 3) = 14.0\text{f}$.

This guarantees that the cube is always perfectly framed in the viewport, and that zoom limits clamp properly.

### D. Layer-Specific Move Interpretation
In a 2x2 or 3x3 cube, face rotations only occur on outer layers. In a 4x4 or 5x5 cube, rotations can occur on inner slices. The `Move` structural definition must adapt:
- **Struct Move properties:**
  - `face` (RIGHT, LEFT, etc.)
  - `direction` (CW, CCW)
  - `layer` (index from outermost slice $0$ to $\lfloor N/2 \rfloor - 1$)
- **Scrambler scaling:** The scramble move generator scales steps dynamically based on size to ensure complete randomization:
  - $2\times2$: 10 steps.
  - $3\times3$: 20 steps.
  - $4\times4$: 25 steps.
  - $5\times5$: 30 steps.
  - Generates inner-slice layer indices ($0$ and $1$) dynamically for $N \ge 4$.
- **Culling and Rendering scaling:** `Renderer::renderCube` scales the translation matrix calculations using `float halfSize = (size - 1) / 2.0f;` and centers all cubies dynamically about the origin $(0,0,0)$ regardless of size.

---

## 3. Dynamic Size Selection Code Implementation Idea

Here is the C++ block mapping for the size-selection handler inside the keyboard callback:

```cpp
void switchCubeSize(int newSize) {
    if (animation.isAnimating() || !animation.moveQueue.empty()) {
        std::cout << "Cannot change size during face rotation animation!" << std::endl;
        return;
    }

    std::cout << "Switching cube size to " << newSize << "x" << newSize << "..." << std::endl;
    
    // 1. Reset logical states
    activeCube = CubeFactory::create(newSize);
    moveHistory.clear();
    solutionPlayer.setMoves({});
    
    // 2. Dynamically scale camera distance so the cube fits perfectly in view
    camera.radius = 10.0f + 2.0f * (newSize - 3);
    
    // 3. Adjust camera zoom bounds
    // For smaller cubes, we can zoom closer; for larger cubes, we allow zooming farther out
    camera.zoom(0.0f); // trigger clamp logic
    
    std::cout << "Switched successfully. Camera radius adjusted to " << camera.radius << std::endl;
}
```

This dynamic implementation allows a single C++ codebase to manage arbitrary sized cubes cleanly and seamlessly.
