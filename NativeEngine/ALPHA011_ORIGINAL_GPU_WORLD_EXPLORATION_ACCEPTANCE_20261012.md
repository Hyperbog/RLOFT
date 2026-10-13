# GPU original-world viewport — Alpha 0.11 (2026-10-12 Eastern)

## Controlling status

**Separate, geometrically correct PROVISIONAL accelerated 3D presentation** compiled as a new independent Win32/OpenGL viewport in native Windows asset engine. User's Alpha0.10 still looked wrong because it drew source textures as perspective-scaled VGA columns using an original-unverified host projection. Further scale tweaks were inappropriate; we added a proper depth-tested original-file-backed level mesh and made it the primary exploration action. The old CODE.2/VGA first-person renderer remains accessible separately as a preservation diagnostic.

- **Source checkpoint:** `native-engine:c8fd692d7db948df03d375756067b6aadf4ac822`. A later documentation-only commit may advance HEAD.
- **Exact compiled Windows source:** `native-gpu-mesh-acceptance-20261012:7994b45de282a3a981e3b10185af37e8b4e61128`, based on the same native renderer source and an isolated one-shot workflow configuration. Normal branch Windows workflow remains manual-only.
- **Windows source/real original game acceptance:** [#38336947761](https://github.com/Hyperbog/RLOFT/actions/runs/38336947761), **34/34 Windows CTests PASS** plus authentic 32-level original-world mesh smoke and original-file SHA preservation.
- **Portable Linux source-only:** [#38336719071](https://github.com/Hyperbog/RLOFT/actions/runs/38336719071) 33/33 tests passed for core original-mesh source (and additional renderer-only Win32 changes independently compiled in Windows).
- [Exact Windows CI artifact #11785098355](https://github.com/Hyperbog/RLOFT/actions/runs/38336947761/artifacts/11785098355), original game files excluded.
- Windows GPU `rloft_native.exe` SHA256 `6acef5b86ae684be66146b417672c7a83a1c771a70b5cd4156d865a66ea7b551`.
- Official inner CI six-file ZIP SHA256 `fa0b0d652346ae60a1202de0bdebe750a4bbd4d4ac7602a1410a4b99dc398ad6`.
- Downloaded CI outer artifact ZIP SHA256 `58d3316d05cc24e5b5aef4417a3239c169462038a48907db36e593f10a6de1e4`.
- User-friendly extra README seven-entry ZIP SHA256 `19206f1c5454a2286b07cfcb0278a89e28035e4c39872a9116710b742ab2be73`. This is **not** the identical official source ZIP, but the executable bytes match exactly.

## Source-backed architecture

`NativeEngine/include/rloft_native/native_gpu_world_mesh.hpp` and `src/native_gpu_world_mesh.cpp` build a deterministic depth-testable vertex mesh from the **immutable typed original LEVELS.DAT level**, using mutable original-category collision state and the typed original 64-word RESOURCE lookup tables:

- Each **X-facing** wall takes lower RES4 bank **0** and optional upper bank **1**. Each **Y-facing** wall uses lower bank **2** and optional upper bank **3**, with original cell bit `0x0200` and original header +9 upper source rows. Faces hidden by adjacent solid neighboring cell heights are clipped, not drawn.
- Each open-floor tile uses the **original signed level selector +0x07** and the original narrow 32×128 RES4 base `600`. The upper plane uses signed level selector **+0x05** when nonnegative. A ceiling selector of **-1** is preserved as **NO ROOF**, not an invented flat surface.
- Original no-draw texture **147** remains collidable but **invisible**, as evidenced by the old v0.7 viewer and the original RES4 table. **Original level 22 uses wall ID 147 on all 172 exposed ordinary blocking faces**; it must NOT be made a fake colored wall in order to satisfy a generalized room geometry assertion.
- Mesh batches preserve original numeric RES4 IDs, decoded by the independent EH6 32/64×128 LZSS decoder. COLOR palette is selected via original CODE.2 32-level world bank. No source pixels are modified or proprietary game resources redistributed.
- Missing outdoor backdrop uses CODE.2 source `33E28/33DE0` original per-level RGB6 shaded-index row fills behind 3D world geometry, with no fabricated outdoor texture asset.
- All placed wall coordinates are world x/y/z vertices; near-wall perspective is handled by a **genuine GPU depth-buffered frustum**, not `projected_height = focal/d` columns. This is a presentation/geometry improvement and NOT proof of original DOS camera/depth byte equivalence.

`NativeEngine/src/win32_native_gpu_exploration.cpp` creates a genuine Windows OpenGL 1.1-compatible WGL context and uses native GLU look-at/perspective and indexed-color texture upload. It is part of the existing compiled `rloft_native.exe`, launched with **Explore Original World (3D GPU)**. The old **CODE.2 VGA reference view** is retained as a separate button. No extra runtime plugin, Python virtualenv, asset extraction or launch script is required.

### Input, presentation and map

- W/S walk, A/D strafe, Q/E turn. Original keyboard mode remains default.
- M or right mouse button toggles relative yaw/pitch, optional and focus-released on exit.
- Held keys/mouse deltas sampled once at a bounded ~16-ms timer tick; player uses swept-radius source-world collision, time-based movement and wall slide. Windows WM_MOUSEMOVE does not recursively repaint/recenter.
- F2 cycles window render resolution: 1260×840, 1440×900, 1600×900, 1920×1080; the window can also resize freely. The GPU renders actual additional geometric pixels without inventing source texture pixels.
- N toggles source-texel nearest versus GL_LINEAR presentation filtering.
- Tab shows in-memory read-only exploration automap, +/- zoom. Ray visibility sampling is movement/turn-throttled to prevent per-frame O(map × collision-state) stutters. No game saves written.
- PageUp/Down change original level, R respawns in a passable inspection cell, Esc closes.

**The 320×400 indexed four-plane VGA reference path is preserved; it is not silently replaced by the modern renderer or misdescribed as original.**

## Authentic original-data acceptance measurements

[Windows source run](https://github.com/Hyperbog/RLOFT/actions/runs/38336947761) loaded the original owner-controlled `DnD-Classics` loose-game files `INIT`, `RES0`, `T`, `LEVELS.DAT`, `RES4`, `COLORS`, `CODE.2` without changes. It ran the 34-case Win64 source testing suite and real original source 32-map audit. Windows PE AMD64 binary SHA and inner distribution SHA were independently checked after downloading the artifact.

| Metric | Actual result |
| --- | ---: |
| Loaded original maps | **32 / 32** |
| Source-geometric lower wall faces | **38,419** |
| Source-geometric upper wall faces | **8,936** |
| Original-source floor faces | **87,983** |
| Original-source ceiling faces | **76,761** |
| Material batches summed across 32 source maps | **870** |
| Original-code/software reference wall columns | **8,072** |
| EH6 original source texel reads in reference renderer | **609,056** |
| Source-verified upper VGA wall strips | **2,029** |
| Source-authored floor/ceiling preview texels | **987,256 / 842,872** |
| Windows source CTests | **34 / 34 pass** |
| Original game files altered | **0** |

Original 32-map source software VGA preview BGRA SHA `f172051b6fdfc05e9cd580a5e90ca35b4ca49216f958cf7f7790aed719460d3a`. The **actual WGL GPU desktop image and mouse movement have NOT been physically field-tested**; this is a compile, mesh and source-media acceptance gate, not a pixel-screenshot fidelity claim.

## Next game-facing integration work

The new GPU viewport can be spatially coherent without claiming gameplay completeness. The following originals must still be joined to actual scene geometry/interaction:

1. Original INIT + RES3 EH7 **static placed scenery**, especially outdoor level22, and original **split tree upper/lower halves** with authored 88-row overlap and eight orientation views. The existing typed `NativeAuthoredSceneDirectory`, EH7 decoder, source SINTAB sampler and original RES3 bank mapping should be the source, not invented sprites. The old Python v0.7 `PAIRED_SCENERY` and billboarding approach are helpful implementation references only. Missing these sprites means e.g. level22 will show a real no-wall map but currently looks underpopulated.
2. SOURCE-verified original **10-byte door records**, aperture panels, material variants and collisions from CODE.2, not static cube walls. Original `0x0100` special-frame and state `0x1000` must be integrated. Follow the existing original CODE.2 reconstruction and v0.7 `DOOR_RESEARCH.md` with independent source-byte confirmation.
3. Sprite geometry, actor pose/frame selection, occlusion and original placement data; native player start, party/UI, events, combat, saves and audio.
4. Original DOS frame-captured comparison at identical map coordinates and heading. Source-color and geometry identity alone **do not** imply DOS renderer frame parity. A dual renderer mode will remain necessary.

Do not ship original media in the Windows artifact; do not close the first-playable milestone because the technical GPU viewport now compiles. The current work is a source-backed OpenGL exploration foundation **Alpha 0.11**.
