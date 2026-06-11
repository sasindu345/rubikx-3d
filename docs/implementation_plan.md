# Solver Fix & Interactive Help Window HUD Overlay

We are addressing two primary requirements requested by the user:
1. **Z key solver not working**: Fixing the double-turn parsing issue (like `F2`, `R2`, `B2`) where the solver algorithm interpreter failed to rotate faces 180 degrees, leading to scrambled states and infinite loops.
2. **User help/instruction window**: Creating a toggleable text HUD panel in 2D orthographic screen space that displays controls, shortcuts, camera navigation instructions, and an active solver playback dashboard.

## User Review Required

> [!NOTE]
> The HUD help menu will be displayed by default when the application starts, greeting the user with clear instructions on how to use the interactive Rubik's cube system. The user can press `H` or `h` at any time to hide or show it.

## Open Questions

None at this time. The requirements are clear: resolve the 'Z' key hang/malfunction and build the visual helper overlay.

---

## Proposed Changes

### Solver Component

#### [MODIFY] [Solver3x3.cpp](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/src/solver/Solver3x3.cpp)
- Update `doAlg` to inspect each token for double moves (ending with '2').
- If a token ends with '2', strip the '2', parse it as a standard move, and execute `doMove` twice consecutively. This decomposes 180° rotations into two 90° rotations on the cube state.

---

### UI & Graphics Components

#### [MODIFY] [HUD.h](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/include/ui/HUD.h)
- Declare `render(int width, int height, const SolutionPlayer& player, bool showHelp)` in the `HUD` class.
- Declare helper methods `drawString` and `drawPanel` for rendering shapes and characters.

#### [MODIFY] [HUD.cpp](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/src/ui/HUD.cpp)
- Implement `render` to set up a 2D orthographic projection using `gluOrtho2D`.
- Render a semi-transparent dark grey side panel on the left side of the screen when `showHelp` is active.
- Display a categorized layout of keys: face rotations (CW/CCW), camera view inputs (left click drag to orbit, right click/scroll to zoom), and system controls (scramble `S`, solve `Z`, play/pause `P`, steps `.`/`,`, speed `+`/`-`).
- Render a playback dashboard at the bottom center of the window if a solution is loaded. This dashboard displays:
  - Playback status: `PLAYING`, `PAUSED`, or `COMPLETED`.
  - Step counter: `Step: X / Y`.
  - Speed: `X.XX moves/sec`.
  - A horizontal tape displaying the sequence of moves, highlighting the current move with a contrasting color (e.g. orange) and bracketing it, and showing past moves in grey and future moves in white.

#### [MODIFY] [main.cpp](file:///Users/oneionei/MyProjects/rubbicx/rubikx-3d/src/main.cpp)
- Add global `HUD hud;` and `bool showHelp = true;`.
- Modify `display()` to call `hud.render(windowWidth, windowHeight, solutionPlayer, showHelp)` after rendering the 3D cube.
- Update `keyboard()` to map `H`/`h` to toggle `showHelp`.

---

## Verification Plan

### Automated Tests
- Compile and run our test runner `/Users/oneionei/MyProjects/rubbicx/rubikx-3d/scratch/test_solver` to verify that 50 random scrambles are all correctly and fully solved by `Solver3x3`.

### Manual Verification
- Compile and launch the app: `./build/RubikX3D`.
- Verify the Help Panel displays on launch.
- Press `S` to scramble the cube.
- Press `Z` to compute the solver path.
- Verify the Solution Playback Dashboard appears.
- Press `P` to autoplay and ensure the cube is solved.
- Test step navigation using `.` and `,` and speed controls `+` and `-`.
- Press `H` to toggle the Help overlay on and off.
