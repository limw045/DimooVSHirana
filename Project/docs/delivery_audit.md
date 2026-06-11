# TCG6223 Delivery Audit

This audit maps the current project state to `docs/Project Guidelines-2610.pdf`.
Use it as the working checklist before preparing the final PDF report, team declaration form, and demo video.

## Hard Deadline

- Submission deadline: 19 June 2026, 11:59pm.
- Late penalty: 5 marks per day.
- Demo requirement: demonstrate on own laptop after submission.

## Required Submission Items

| Item | Guideline Requirement | Current Evidence | Status |
| --- | --- | --- | --- |
| Written report PDF | Cover page, introduction, model documentation, user manual, screenshots | Design docs exist in `docs/设计说明/`; screenshots exist in `review_screenshots/`; final report PDF is not present yet | Incomplete |
| Program | Organized source code, documented, error-free, Code::Blocks/MinGW compatible under Windows 10 | Source code exists in `src/`; build verified with `g++` into `FightingGameGoalCheck.exe` on 2026-06-09 | Mostly ready |
| Team declaration form | Milestones by each member and development timeline | No form found in current workspace | Missing |
| Video clip | Model demonstration under 5 minutes | No final video found in current workspace | Missing |

## Build Verification

Verified command:

```powershell
g++ src\main.cpp src\Game.cpp src\CGImageLoader.cpp src\scene\Arena.cpp src\scene\Camera.cpp src\scene\Lighting.cpp -I src -I src\scene -I . C:\MinGW\lib\libopengl32.a C:\MinGW\lib\libglu32.a -L . -lglut32 -ljpeg -o FightingGameGoalCheck.exe
```

Result:

- Build succeeded.
- Linker emitted GLUT stdcall fixup warnings only.
- Important note: linking with plain `-lopengl32` from the project root can fail because the local `opengl32.dll` is picked up as a library file. Use the explicit MinGW libraries above for verification builds.

Short runtime check:

- `FightingGameGoalCheck.exe` was launched locally on 2026-06-09 and stayed alive for 5 seconds before being closed automatically.
- This proves the executable can start in the local environment, but the final demonstration still needs a visible walkthrough with fresh screenshots or video capture.

## Runtime Evidence

Existing runtime log:

- `runtime_logs/output.txt`

Observed evidence:

- Textures loaded successfully:
  - `images/Floor.bmp`
  - `images/LeftRight.bmp`
  - `images/Background.bmp`
  - `images/cardboard.jpg`
  - `images/wood.jpg`
  - `images/rose_label.jpg`
  - `images/starry_sky.jpg`
  - `images/hirono_face.jpg`
  - `images/dimoo_face.jpg`
- Entry animation can start.
- Game can enter battle state.

Current screenshot evidence:

- `review_screenshots/scene_review_loaded_screenshot.png`
- `review_screenshots/scene_review_battle_screenshot.png`
- `review_screenshots/scene_review_after_plastic.png`
- `review_screenshots/scene_review_after_plastic_lowhp.png`
- `review_screenshots/prop_polish_battle_synced.png`
- `review_screenshots/bubble_wrap_full_floor_review.png`

## Marksheet Mapping

### 1. Construction of Characters - 14%

Guideline focus:

- Graphics model.
- Vertices, polygon, object transformation.
- More marks for complex objects created manually.
- Avoid generated OpenGL model code.

Current project alignment:

- `src/Game.cpp` contains Hirono and Dimoo drawing logic.
- `src/scene/Arena.cpp` contains arena and prop drawing logic.
- Character design references exist under `docs/设计说明/Dimoo/` and `docs/设计说明/小野/`.

Report writing angle:

- Explain each character as hierarchical OpenGL modeling.
- Show how primitive shapes, transformations, local coordinate systems, textures, and alpha materials are combined.
- Explicitly state that no prohibited code generator is used.

### 2. Animation of Character Skills - 14%

Guideline focus:

- Each blind box character should be user controlled during battle.
- Each character should cast at least one skill.
- Rendering effects such as lighting, shading, texturing.
- Bonus potential for special effects beyond lab content.

Current project alignment:

- P1 Hirono controls:
  - `A/D`: move.
  - `W`: jump.
  - `J/K/L`: attack, skill, ultimate.
- P2 Dimoo controls:
  - Arrow left/right/up: move and jump.
  - `1/2/3`: attack, skill, ultimate.
- Combat keys reduce HP, spawn hit sparks, and trigger camera shake.
- Ultimate keys trigger stronger effects and lid shake.

Report writing angle:

- Hirono skill identity: rose/star/B-612 visual language.
- Dimoo skill identity: dream/butterfly/ripple visual language.
- Mention alpha blending, particles, point lights, texture mapping, and camera shake as rendering/animation effects.

### 3. Battle Environment - 8%

Guideline focus:

- The environment changes during battle.

Current project alignment:

- Blind box stage exists as the main arena.
- Environment props and debug triggers exist:
  - `H`: kick desiccant bag.
  - `P`: throw folded pamphlet.
  - `E`: trigger left wall collision shake.
  - `F6`: force Hirono wall crash.
  - `F7`: damage Dimoo and create hit feedback.
- Low HP visual state is represented through HP percentage updates and lighting behavior.
- Screenshots include prop polish, bubble wrap floor, and low HP review images.

Report writing angle:

- Explain wall shake, lid shake, bubble wrap, pamphlet, desiccant packet, and low HP lighting as battle-environment responses.
- Include before/after screenshots from `review_screenshots/`.

### 4. Document Management - 4%

Guideline focus:

- Concise, neat report.
- Organized and documented source code.
- Good grammar and no chat-like spelling.

Current project alignment:

- Source folders are organized under `src/`, `src/scene/`, `images/`, `docs/`, and `runtime_logs/`.
- Final report, declaration form, and demo video still need to be produced.

## Final Report Skeleton

Recommended PDF report structure:

1. Cover Page
   - Team name.
   - Student IDs.
   - Names.
   - Lab section.
   - Project title: `HIRONO vs DIMOO: Blind Box Battle`.

2. Introduction
   - Project concept: two blind box characters battle inside an opened blind box package.
   - One screenshot of the full battle scene.
   - Short explanation of why Hirono and Dimoo have contrasting personalities and skills.

3. Documentation of Virtual Model
   - Hirono construction:
     - Head, hair, face, coat, scarf, cape, rose dome, B-612/base elements.
     - Transform hierarchy and animation.
   - Dimoo construction:
     - Head, transparent body, ears, seated body, vine ring, butterflies.
     - Floating movement and butterfly/vine animation.
   - Arena construction:
     - Floor, walls, lid, cardboard texture, printed labels, props.
   - Rendering:
     - Lighting.
     - Texture mapping.
     - Alpha blending.
     - Particles.
     - Camera shake.

4. User Manual / Instructions
   - Start battle: `Space` or `Enter`.
   - Restart: `R`.
   - P1 Hirono:
     - `A/D`: move.
     - `W`: jump.
     - `J`: attack.
     - `K`: skill.
     - `L`: ultimate.
   - P2 Dimoo:
     - Arrow left/right: move.
     - Arrow up: jump.
     - `1`: attack.
     - `2`: skill.
     - `3`: ultimate.
   - Demonstration/debug keys:
     - `F4`: toggle colliders.
     - `F5`: skip intro.
     - `F6`: wall crash.
     - `F7`: damage P2.
     - `F8`: dummy AI.
     - `F9`: free camera.
     - `H`: kick desiccant.
     - `P`: throw pamphlet.
     - `E`: wall shake.

5. Screenshots
   - Title/loaded scene.
   - Battle scene with both characters.
   - Hirono skill or ultimate.
   - Dimoo skill or ultimate.
   - Environment response or prop interaction.
   - Low HP lighting/environment state.

6. Conclusion
   - Summarize manual modeling, interactive battle mechanics, and rendering effects.

## Demo Video Plan Under 5 Minutes

Target length: 3.5 to 4.5 minutes.

1. 0:00-0:20 - Title and project concept.
2. 0:20-0:50 - Full blind box arena overview.
3. 0:50-1:40 - Hirono model close-up and controls.
4. 1:40-2:30 - Dimoo model close-up and controls.
5. 2:30-3:30 - Battle controls, attacks, skills, and HP changes.
6. 3:30-4:20 - Environment changes: wall shake, lid shake, props, low HP lighting.
7. 4:20-4:40 - Final shot and team credits.

## Remaining Work

1. Produce final written report PDF from the skeleton above.
2. Fill and add the team declaration form.
3. Record and export a demo video under 5 minutes.
4. Run one final Code::Blocks/MinGW build on the laptop used for demonstration.
5. Run the final executable and capture fresh screenshots for the report.
6. If time allows, add a runtime OpenGL error check log line after major render passes.
