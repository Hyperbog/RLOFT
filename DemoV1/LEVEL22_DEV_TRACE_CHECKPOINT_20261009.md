# Level 22 native diagnostic flight recorder — implementation checkpoint

**Scope:** Level22 playability and original DOS parity troubleshooting. Instrumentation is opt-in and HOST-ONLY; no original game action, fixed-point gameplay value, asset file or save byte is changed.

## User workflow (available after the next explicitly authorized Full Windows build)

1. Unpack the Windows-x64 build artifact, keeping `Diagnose-Level22.cmd` next to `ravenloft_demo_v1.exe`.
2. Double-click **Diagnose-Level22.cmd**. It supplies `--explore-level22 --dev-inspect --dev-interact --dev-trace`. An optional quoted game installation path is forwarded untouched; the existing remembered GOG/game folder continues working.
3. Reproduce the problem with inventory, party portraits, top menu, automap, quit confirmation, outdoor ground, NPCs or world items. Press **F12** at each visible defect; up to sixteen F12 markers are preserved independently of normal frame limits. F12 is diagnostic-only while trace mode is active.
4. Exit the game normally. The blocking launcher creates **one ZIP** with that session's machine-readable logs; copy the printed path and upload the ZIP in this conversation.

An existing `CHARS` is used if present. In `--explore-level22`, missing `CHARS` yields the existing temporary developer party; `--dev-trace` without `--explore-level22` may instead accompany normal CODE.1/CODE.2 startup, and records only after entering exploration.

## Files in a diagnostic ZIP

- `manifest.json`: explicit format v1, native-only provenance, exact compiled source revision, type of original media, fixed 320×400 indexed framebuffer/palette format and capture caps. No installation path.
- `events.jsonl`: monotonic event order, Win32 key virtual codes (never typed character data), left/right mouse click coordinates in original VGA space, screen entries, significant authoritative world/party/inventory/camera/quest state changes, top-menu activations, source-backed actual loose-item pickups, full-run terminal event and stage-tagged bounded errors.
- `frame_NNNN.idx`: **exact original-resolution 128,000-byte indexed native framebuffer** at key presentations (including automap, party/inventory, quit confirmation, top menu and viewport). Unlike a screenshot, these have original indices.
- `frame_NNNN.pal`: **768 bytes**, 256×3 raw 6-bit VGA palette entries.
- `frame_NNNN.json`: screen type, original input IRQ tick, fixed-point camera, facing, level/time, NPC/item/actor counts, source-session hashes of party/inventory/quest/world, and independent FNV64 pixel and palette checksums. No raw character names, passwords, writable saved-game contents or original installed file paths.
- `frame_NNNN.draws.tsv`: ordered native indexed-framebuffer API call trace. Each entry gives operation (clear, Type1 sprite frame, mirrored/mapped/scaled draw, fill or region restore), source frame index, X/Y screen coordinates, extent and variant parameters. This is the direct evidence needed to diagnose overlapping portraits and equipment. A final `omitted` count reports entries beyond the per-presentation bound.
- `marker_NNNN.{idx,pal,json}`: an F12 event's actual next presented native framebuffer plus state and IRQ metadata. Includes `marker_NNNN_recent.jsonl` containing **up to 256 prior event records**, including the marker request.
- `renderer/frame_NNNN.{idx,pal,tsv}`: the existing opt-in CODE.2 source-referenced Level22 environment/raycast/RES3/type7 trace and native indexed capture, now colocated under the same session rather than a separate temporary folder; at most 64 distinct level-cell/facing records, with independent failure.tsv diagnostics.

**The original drawn hardware/software cursor is composited by the native Win32 host after the indexed framebuffer.** F12 `.idx` captures record the pre-cursor indexed VGA framebuffer. Mouse click coordinates allow investigating input hit regions. The new `.draws.tsv` files record actual bitmap placement/restore order for the original modal drawing API; this is not a trace of every direct planar-memory pixel write. Full original RES3 source resource identities, ray/column render plans and original source offsets remain in the existing `renderer/*.tsv` environment trace. Type1 resource IDs are not directly encoded in the framebuffer API call and must be cross-referenced with the model/source.

## Bounded runtime behavior

- 16,384 persistent JSONL event records with a 256-event in-memory recent ring. Event writes flush to disk to survive unexpected native termination.
- 96 automatic indexed frame/palette/JSON capture triplets, of which **at most 64** are viewport/top-edge frames, reserving 32 slots for later modal-screen captures.
- Up to 1,024 source-ordered per-presentation indexed drawing operations recorded for each captured frame, with an explicit omitted-call count; opt-in only, including original party/inventory modal drawing calls. The same draw records are saved for F12 snapshots.
- 16 independent F12 indexed frame/palette/JSON triplets, unaffected by exhaustion of the 96 automatic snapshot budget.
- Dynamic world/time changes are decimated to one automatic frame per nine original-style IRQ ticks unless a screen change/input occurs. This avoids using the budget on 70-Hz paints while preserving meaningful user-triggered changes.
- All frame data uses source bytes, not encoded BMPs, modern UI overlays or surrogate thumbnails. Captured output writes to `%TEMP%\\RLOFT-Demo-Diagnostics\\<session-id>` only, never to the original game directory.
- `latest-session.txt` is written outside the individual ZIP solely for the CMD launcher. The launcher packages only that session after the game exits.
- Exception reasons containing local path separators are redacted. Keys are virtual codes, not entered letters/text. The bundle contains only hashed party/world values, not raw personal characters or saved files.
- Normal launches without `--dev-trace` allocate no flight-recorder files and continue using the existing untouched gameplay logic. `--capture-render` remains separately supported.

## Regression and build discipline

`dev_session_trace_tests` verifies original-frame sizes, palette bytes, screen changes, source revision, state fingerprint capture, F12 preservation and event snapshot, JSON escaping, independent marker limit, auto viewport cap, later quit-modal snapshot reservation and clean close, plus source-order Type1 sprite/fill/restore operation capture, extent/frame metadata and bounded draw-call overflow reporting. `CMakeLists.txt` now registers **179** distinct CTests (old Full #648: 178/178, successful at checkout `fb091234`). The workflow stages/archives the new launcher only during explicit user-authorized builds. **No new build dispatched**. Runtime file/zip behavior and eventual DOS fidelity remain unaccepted until the user tests the new executable.

**Development priority:** Fully playable source-faithful Level22 through the original mist sequence; absolutely no independent Level23 gameplay work.
