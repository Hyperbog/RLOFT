# Native runtime world state — original-code-constrained session foundation

## New native source (2026-10-11)

\`native_world_runtime.hpp/.cpp\` adds a source-owned, independent NativeEngine
runtime on top of the **256/256 byte-preservation-certified** original-world
decoder. Original media and immutable \`NativeWorldPayload\` are never edited.

It owns distinct per-level live state:
- full 16-bit packed map words and generated automap byte grids;
- every 10-byte control record (in source directory/column order);
- currently selected original level, with all 32 levels still resident;
- source-bound in-memory snapshot/restore of those mutable fields;
- a bounded inventory of cells explicitly marked by the scene traversal.

The runtime exposes the exact CODE.2-proven **six-bit cell code**, transient
visibility \`0x0040\`, persistent explored marker \`0x0400\`, special center
\`0x0080\`, center frame \`0x0100\`, optional extra render prepass \`0x0200\`,
hidden topology \`0x0800\`, 10-byte-record resource offset gate \`0x1000\`,
opaque legacy marker \`0x2000\` (never reinterpreted), and two-bit
shade-decrement \`0xC000\`.

## Behavior implemented to original CODE.2 evidence

The *normal collision gate only* is:

\`\`\`text
if packed_cell & 0x0080:
    if kind-1 record for this cell exists with position [+6] == 0:
        passable
    else:
        blocked special
else:
    if packed_cell & 0x003F: ordinary blocked
    else: passable
\`\`\`

This implements the certified original \`0B164\`/ \`1E930\` traversal
decision. It does **not** approximate entity occupancy, party footprints,
secret-mode walk-through, movement interpolation or other collision callers.
Controls retain archive field byte \`+4\` = row, \`+5\` = column; all 1,659
shipped source records satisfy their owning column/height constraints.
Resource variant \`record[1]&3\` is exposed for linked non-kind-1
controls only when \`0x1000\` is present, as supported by original
\`1E9A0\`. It does not select actual RES assets by itself.

The *dynamic animation tick* applies only to kind-1 group-2 records:
\`position [+6] += signed delta [+7]\`; at endpoints 0/8 the delta
is cleared; when no animations remain, every non-8 position is normalized
to 0. These exact updates are instruction-proven around \`20B40..20C0B\`.
A deliberately named **debug-only** transition starter accepts an index at
stable endpoint 0/8; it does NOT simulate the full original interaction
eligibility, collision checks, scripts, audio or NPC consequences.

\`mark_reached_by_view()\` ORs \`0x0440\`, while
\`clear_transient_visibility()\` clears only \`0x0040\`, exactly preserving
persistent \`0x0400\`. This only operates on cells explicitly supplied by
a caller; the first-person ray traversal and occlusion algorithm are
not reconstructed here. \`marked_scene_cells()\` is a row-major list of
currently marked scene **inputs**, not a renderer.

Source-bound snapshots record SHA-256 over immutable per-level cell words,
controls, original A/B/C world families, event slots, automap and
annotation bytes. A snapshot from a different source, altered coordinates,
changed control record kind, wrong dimensions or invalid kind-1 state is
rejected without overwriting live runtime state. This is **not** the
original \`LEVnnn.DAT\` or \`SAVE.DAT\` persistence format.

## Build and validation

\`\`\`sh
cmake -S NativeEngine -B build/native -DCMAKE_BUILD_TYPE=Debug
cmake --build build/native --parallel 2
ctest --test-dir build/native --output-on-failure

# Output ONLY bounded source-derived counts and a source fingerprint.
# This tool loads owned LEVELS.DAT through the same native media adapter:
build/native/rloft_native_world_runtime_audit /private/original/install
\`\`\`

New \`native_world_runtime_tests\` covers 32-level residency, source/live
separation, bit-preserving cell-code writes, mark/clear 0x0040 and
preservation of 0x0400, normal collision and resource variant gates,
8-step dynamic open/close transitions, bounded scene inputs,
snapshot restore/foreign-source rejection, malformed control
coordinates and invalid dynamic states.

The metadata-only schema is \`rloft-native-world-runtime-evidence/v1\`.
It reports total cells and normal collision categories, source controls,
special centers, opaque authoring markers, dynamic records, currently
visible/explored cells and active animation count. It explicitly sets
\`scene_projection_verified=false\`,
\`event_vm_executed=false\`, \`original_dos_execution_verified=false\`
and \`gameplay_parity_verified=false\`.

## Evidence / nonclaims

Evidence source: the project's original-code research
\`DatFiles/LEVELS.DAT/CELL_WORD.md\`,
\`CELL_RECORDS.md\`,
\`DYNAMIC_CELL_RECORDS.md\`, \`CELL_FLAG_LINKS.md\` on
\`code1-decompilation\`, plus the independent original-file audits.
This is a **persistent native world state and renderer-input layer**,
not a complete game. No DemoV1 runtime linkage, imported game logic,
SINTAB original x86 execution, original-save writeback, graphics
rasterization, creature AI or scripted event VM is implemented.
