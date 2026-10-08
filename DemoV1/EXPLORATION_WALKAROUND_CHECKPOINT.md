# Level 22 environmental walkaround checkpoint

## Why this exists

A deliberately small, **opt-in native preview** to let testers enter the real Level 22 world and walk around while original-frame wall fidelity, combat, inventory input, and relic transitions remain unfinished. It is NOT a separate reimagined level, a claim that the original dungeon is rendered accurately, or a replacement for faithful boot and character creation.

## How to launch (Windows x64)

1. Obtain the newest Windows build artifact `Ravenloft-Demo-v1-Windows-x64`; it now contains both `ravenloft_demo_v1.exe` and `Explore-Level22.cmd` together.
2. Keep both files in the same folder and double-click `Explore-Level22.cmd`.
3. The demo locates a legally installed Ravenloft copy using the normal GOG/Steam/installation discovery, or requests a directory if not found. You may also pass a quoted installation directory to the launcher.
4. **Arrow keys / numeric keypad:** move, turn and strafe using the existing reconstructed CODE.2 key-input model. **Esc:** leave the preview. The top-edge original automap and quit menu can still be used.

Equivalent manual invocation:

```text
ravenloft_demo_v1.exe --explore-level22
ravenloft_demo_v1.exe --explore-level22 "C:\\GOG Games\\Ravenloft Strahds Possession"
```

`--explore` is an alias. The original optional positional installation-folder argument remains accepted. Normal launch *without* either flag still follows the original boot, cinematic, menu and CHARS/character-creation route.

## What gets loaded and displayed

- Original CODE.2 Level 22 initial position `(4.5,4.5)`, heading `270`, time `14:00`.
- Original mutable `LEV022.DAT` takes precedence over the archive `LEVELS.DAT`, following the existing verified CODE.2 level loader. The supplied direct `LEV022.DAT` is a **30-column by 15-row level**, with environmental profile **1**, vertical offset `96`. Rendering still uses the original installed RES0/RES4/RES8/SINTAB/COLORS files.
- Normal world collision, input axes, movement, scene camera and automap code are preserved. HUD/environment upper/lower presentation and reconstructed surface rendering are reused. A native-only **window caption** reports level, current cell and facing; the original 320×400 indexed world framebuffer remains unchanged by that caption.
- When an exact original 0x920-byte writable `CHARS` image exists, use its bytes unchanged. Otherwise a temporary two-member preview party named EXPLORER/GUIDE is manufactured **in memory only**, with 12 HP each and no equipment. Those actors are not reconstructions of original game characters.

## Safety / isolation

- Preview does **not create, overwrite, or save** `CHARS`, `LEV022.DAT`, `SAVE.DAT` or any other game data.
- Preview disables native loose-item pickup while allowing movement and nonmutating exploration panels. It does not invent combat hit rolls, offer gameplay progression, trigger the unfinished Level 23 transition, or create rewards.
- No original copyrighted game resources are redistributed; users must supply their own copy.
- The optional launch flag is separate from the default faithful game flow, which is intentionally unchanged.

## Implementation

- `src/exploration_preview.cpp`, `include/rloft/exploration_preview.hpp`: isolated fallback party and existing-party selection.
- `src/win32_main.cpp`: opt-in flag parsing, direct Level 22 entry, title-bar coordinates and safety guard on loose-item pickup.
- `Windows/Explore-Level22.cmd`: simple double-click entry.
- `tests/exploration_preview_tests.cpp`: original party size and actor status, equipment sentinels, 12-HP safety state, valid existing CHARS immutability, invalid/missing CHARS fallback.
- `CMakeLists.txt` includes the implementation and CTest target; Windows Actions artifact stages the launcher beside the executable.

## Acceptance and limitations

**Windows build and automated tests are now verified.** GitHub Actions run [#602](https://github.com/Hyperbog/RLOFT/actions/runs/37716513471) built Windows x64 successfully, completed the CODE.2 syntax gate, and passed **101/101 CTest cases** including `exploration_preview_tests` and `surface_sintab_prepass_executor_tests`. The produced EXE was checked as an x64 PE image containing the UTF-16 `--explore-level22` option, and the Windows archive includes `Explore-Level22.cmd`. A real interactive walkaround with the original installed game files is still awaiting user acceptance. The main `FCD0=0` SINTAB wall pass and per-pixel DOS comparison remain outstanding. Accordingly, outdoors can show original sky/ground/environment primitives while **some walls, foliage, or scene elements can be missing or incorrectly drawn**. Do not claim full environmental or game fidelity.

### Walkaround-specific gates

1. [x] Direct opt-in Level 22 route, independent of saved CHARS.
2. [x] Read-only preview party / no game-file writes / no pickup in preview.
3. [x] Launcher staged alongside exe in Windows artifact.
4. [x] Fresh Windows build and regression suite confirmed passing (run #602, 101/101).
5. [ ] Real playable smoke test against installed original game resources.

**Walkaround checkpoint: 80% accepted (4/5 gates)**, pending a real game-file smoke test. **Overall demo end-to-end acceptance: 20% (1/5 major release gates)**, unchanged by this opt-in exploration slice.
