# Native first-person scene probe: original-level resource tables + bounded host traversal

**Engineering stage:** independent replacement engine **renderer staging**, not a
faithful recreation of the DOS rasterizer. Owner's original data is never
committed or compiled into the host executable.

## Real source-derived work

Previously, \`NativeLevelPayload\` retained the original packed cells,
10-byte control records, three compact families, scripts, automap and
annotations, but discarded the fixed \`0x2A4\` level header. That prevented
original resource IDs from reaching the renderer.

This stage retains all **676 source header bytes for every one of 32
levels**, including five 64 × uint16 banks beginning at relative offsets:
\`0x00A\`, \`0x08A\`, \`0x10A\`, \`0x18A\`, \`0x20A\`. The fifth bank is
automap style data; the other four are renderer resource candidate banks.
The entire original header, including opaque and profile fields, is
preserved byte-for-byte, not normalized or re-authored. The established
\`rloft_native_payload_audit\` round-trip now enforces original header
byte equality for each map and emits the independent-comparable
\`header_sha256\` alongside the preexisting 256 section SHA-256s.

\`native_scene_resource_candidates\` queries the live cell and immutable
source tables and exposes raw table IDs plus at-most-one supported resource
variant:

- Non-kind-1 10-byte control state \`record[1]&3\` when the live cell has
  \`0x1000\`.
- For a frame cell \`0x0100\` with an **explicitly caller-supplied**
  cardinal adjacent neighbor carrying special-center \`0x0080\`, use the
  neighbor's six-bit cell code and an offset of one.
- When there is no proven state-selection context, the offset is **-1,
  unknown**, not a guessed zero. A conservative candidate bank lookup is
  still available, but \`selection_branch_verified=false\`.
- The original CODE.2 view chooses which adjacent direction/table with
  additional orientation logic; **this stage does not assert that choice
  is understood**. The four table candidate values are not proofs of
  actual displayed RES3 shape IDs or a completed rendering order.

## Host view probe — explicitly non-original raycaster

\`trace_native_probe_view\` implements a strictly bounded C++17
uniform-grid DDA view from a caller-selected camera position and angle.
It queries the existing **source-grounded normal collision gate**, recording
hit location, source cell code and an experimental perspective distance.
\`move_native_probe_camera\` uses subdivided host movement with basic
corner guards and the same collision gate. This is scaffolding for a
first-person native scene, **not** original 16.16 motion, player footprint,
CODE.2 ray traversal, entity occupancy, hidden-passage mode or event VM.

\`apply_native_probe_visibility\` uses the proved source flag operations
\`OR 0x0440\` after the *host* ray probe reaches cells. Which cells the
original DOS walker would have reached is **not** certified.

\`render_native_probe_rgb\` emits simple artificial 24-bit colors for
background/floor and depth-based debug wall strips. It **does not** use
the original game's palette, textures, RES3 compiled display lists, EH6
samples, SINTAB executable code or character sprites. A resulting image
must be labeled diagnostic and must not be shown as faithful in-game art.

The optional CLI \`rloft_native_scene_probe <original-install> [debug.ppm]\`
chooses a reproducible passable interior cell of original level 22, runs
320 bounded sample rays, optionally exports a generated P6 PPM and emits
metadata-only JSON \`rloft-native-scene-probe/v1\` with explicit
\`original_dos_raycast_verified=false\`, \`original_projection_verified=false\`,
\`original_palette_or_textures_verified=false\`,
\`original_event_vm_verified=false\`, and
\`native_gameplay_parity_verified=false\`.

## Exact next fidelity stages

1. Independently compare all 32 **new fixed-header SHA-256** hashes from
   the *actual* compiled native tool to the user-owned \`LEVELS.DAT\`.
2. Capture CODE.2 cell-traversal orientation, side selection, FOV and
   visibility branch traces as original execution evidence; use them to
   replace, not re-label, the host DDA probe.
3. Bind original candidate IDs and CODE.2 pass ordering to validated
   original RES3/EH7 display-list geometry, SINTAB scale selection,
   EH6 texture sampling and original palette context.
4. Integrate native persistent player motion/actor occupancy/interaction
   and event side effects only after independent capture/oracle tests.
5. Rebuild and field-test a current-source Windows/MSVC demo with owner
   approval; Windows release Actions remain manual-only.

No DemoV1 runtime or original compiled x86 code is linked into the
independent NativeEngine.
