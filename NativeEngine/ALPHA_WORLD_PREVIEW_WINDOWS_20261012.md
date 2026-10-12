# Native Alpha 0.5: interactive original-world preview — 2026-10-12

**Branch:** `native-engine`. **Product:** the independent Windows `rloft_native.exe`, not `demo-v1` and not the original-DOS recompilation. **Status:** Win64/MSVC package and 32/32 CTests verified at explicit SHA; user interactive field test pending.

## Windows acceptance — BUILT and package-verified (2026-10-12)

**Real Windows/MSVC x64 runner PASS**:
[GitHub Actions #38323574522](https://github.com/Hyperbog/RLOFT/actions/runs/38323574522)
compiled the independent `rloft_native.exe` including
`win32_native_exploration.cpp`, ran **32/32 Windows CTests**
(30 portable and two additional tests, including the Win32 executable
target), and generated a verified original-media-free downloadable package.

To preserve the normal manual-only Windows release workflow, this
owner-authorized build used a **one-time push trigger** on isolated branch
`native-alpha-preview-build-20261012`. Build HEAD
`753c231cad1b086c62ce16546ddbac22654a0179`
is based on native-engine gameplay/source HEAD
`45f815955990aedd25c0f8e827211043054a9e64`; only the
workflow trigger differs. No Windows workflow trigger was changed on
`native-engine`.

[Download raw Actions artifact #11779368831](https://github.com/Hyperbog/RLOFT/actions/runs/38323574522/artifacts/11779368831).
The official build artifact contains a source-identified inner ZIP
`RLOFT-NativeEngine-Windows-x64-ASSET-PREVIEW.zip`.

| Exact CI-generated item | SHA-256 |
| --- | --- |
| Windows GUI `rloft_native.exe` | `894978cd72a88bbc16d63fee2e9a081bef51632dfc6c10e5e49026ef5fa7d9bc` |
| Inner original-free distribution ZIP | `115bf33af7ff8e185d0f067014c896b9fbe8cbf0fb2096a8d833170020a6657d` |
| Outer GitHub Actions ZIP | `7909039249d81c7d8f01d960f1f7f82987c7b8b8133e21c1857dac7d36632c43` |
| Native bank-audit Windows EXE | `09d0bcc4123fded7fed291b143af1fd7b3a5ae01f925d118db7224e040b2774b` |
| Native world-audit Windows EXE | `4e378a675102c09d281141be60e0a9097da4ddeb6049f86cc80bb16a45907d06` |

The exact CI built ZIP was downloaded and checked independently: its
inner ZIP SHA matches the workflow log and accompanying checksum, its
six payload entries contain only compiled EXEs, readmes, hashes and
provenance, and all three PE executables have AMD64 machine ID
`0x8664`. Binary digests match the original build logs.

**Still required:** direct user Windows field-test of the new
interactive first-person window against original owned GOG/DOS media;
the CI acceptance builds the GUI target but cannot prove interactive
window behavior, collision, persistence, DOS projection parity or
complete-game playability. This is a real distributable original-file-backed
*technical preview*, not the completed Alpha 1 fidelity milestone.

## What changed

The native asset browser now has an **Explore LEVELS.DAT (preview)** button next to the original-install selector. When a GOG `game.gog` image or loose original installation has already been opened, it reads the original `LEVELS.DAT` through the existing read-only `NativeMedia` source and the strict `decode_native_world_payload` implementation. It opens a **separate**, resizable Windows first-person preview that does not replace the existing original-resources/palettes browser.

The window provides:

- Original map/level selection **0 through 31** with PageUp/PageDown.
- A clearly identified, deterministic **host preview spawn** chosen from passable original cells, preferring more neighboring passable cells and central proximity. It is not a claimed original player/SAVE.DAT start.
- Keyboard stepping **W/S** or **Up/Down**, strafing **A/D**, turning **Left/Right**, level respawn **R**, and close **Esc**. Movement uses the existing bounded `move_native_probe_camera` traversal and original `normal_collision` cell classification.
- A resizable 320×200 CPU-generated image presented by standard GDI as BGRA with aspect-preserving scaling, live level/cell/hit statistics, and a permanent on-screen warning that this is **PROVISIONAL HOST RAYCAST / SYNTHETIC COLORS — NOT DOS GRAPHICS**.
- No reads or writes to user `CHARS`, `SAVE.DAT`, or other mutable original files. The preview owns its decoded world copy. Reopening/replacing the installation in the asset browser does not rebind the open preview's data.

**This milestone does not replace the earlier source-backed original EH6/EH7 RGB6 compositor.** The provisional host DDA renderer intentionally uses fake colors because original automatic camera, wall selection, viewport clipping, and draw-call scheduling remain unverified. The new UI path exercises original-world loading, basic input, and collision as a user-accessible integration slice; it is **not Alpha 1 fidelity or normal original-game progression**.

## Authentic source-data smoke check

User-provided original `LEVELS.DAT` (499,878 bytes; SHA-256 `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`) was read directly and its original 32-record level table checked. All **32 of 32** original map records have at least one ordinarily passable (`cell.code==0`, not special-center) cell, so all original maps contain potential preview spawn locations; this *does not* certify renderer screenshots or movement against DOS.

No new reconstruction-only test suite was added for this UI change. The existing native scene-view, world-runtime and media tests remain in CMake. An exact-source Windows/MSVC build and full 32/32 Windows CI are now complete (see evidence above). The real GOG user launch of this interactive window is still pending.

## Field-test procedure / acceptance

1. **Done:** source-identified Win64 `rloft_native.exe` built under MSVC and passed all 32 native CTests on the one-time isolated branch.
2. Open your original installed GOG/DOS directory using the existing `Open original installation...` button or `--install <path>`.
3. Choose **Explore LEVELS.DAT (preview)**; check it opens without errors at level 0 and that W/A/S/D/arrow navigation moves or blocks on original map walls. Verify PageUp/PageDown cover all 32 levels and Esc only closes the preview.
4. Check original `game.gog`, `CHARS`, `SAVE.DAT` remain byte-identical before/after. Diagnose the first reproducible failure with level/cell/input and screenshot.
5. **Do not** report this as original first-person parity, original startup, combat, save progress, or a playable alpha. Exact DOS camera and texture/sprite presentation, original input ticks, actor occupancy, events, animation, and real start position are all still open.

## Next product milestone

Replace the host DDA image generator behind the view interface with the original-CODE.2 automatic ray/scene command generator and proven four-plane RGB6 wall/sprite compositor. After verified original DOS frame and input comparisons, connect original party, gameplay, events and protected save behavior. This UI bridge is valuable only as a run-and-inspect shell for that verified engine work.
