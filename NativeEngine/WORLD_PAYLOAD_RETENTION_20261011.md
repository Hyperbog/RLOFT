# Original-world payload retention — independent NativeEngine

New source: \`NativeEngine/include/rloft_native/native_world_payload.hpp\`
and \`NativeEngine/src/native_world_payload.cpp\`. This is **native
replacement-engine code**, never a link to \`demo-v1\` or original DOS
gameplay. It complements, not replaces, the previously certified
\`decode_native_levels\` structural metadata audit.

## What is now preserved in memory

After **all 32 original map records pass** \`decode_native_levels\`,
\`decode_native_world_payload\` retains:

- Every raw packed 16-bit cell word, with the low six bits available through
  \`tile_code(x,y)\` (no invented semantics for unknown high bits).
- Every original 10-byte control record with its owning map column.
- Every original 11-byte A/B/C actor/world family record.
- All fifty 26-byte event slots **per level**, including unused command
  bytes rather than silently rewriting data.
- Each level's original automap byte grid and original annotation payload.

The resulting \`NativeWorldPayload\` is **source data**: nothing in these
objects interprets collision, executes scripts or decodes rasterizer x86
machine code. The separate \`NativeMutableWorld\` clones only mutable
cell/automap arrays for later simulation; modifications cannot change the
immutable decoded source. The user-owned original bytes are read-only.

The old \`NativeLevelSystem\` and \`NativeSintabSystem\` certified
structural APIs stay available and unchanged. This layer does not bundle
original data or change existing resource lookup, graphics, palette logic,
Windows UI or exported audit JSON.

## Local compile and test

\`\`\`sh
cmake -S NativeEngine -B build/native -DCMAKE_BUILD_TYPE=Debug
cmake --build build/native --parallel 2
ctest --test-dir build/native --output-on-failure
\`\`\`

New \`native_world_payload_tests\` (bringing expected CTest count to **17**)
covers all 32 synthetic levels, all raw cell values and tile mask,
per-column controls, all A/B/C families, active and unused event slots,
automap/annotation retention, bounds, source-isolated mutable clones and
original-source structural corruption.

**Verification status on source commit:** newly committed C++ and test
registration; **not a current-SHA Windows MSVC build**, nor a locally
executed C++ test. The previous real Windows 16/16 certification remains
at its exact earlier source revision. No Windows Actions workflow has been
triggered or changed. An owner-authorized Windows build and real original
source-backed test are still required for this layer.

## Follow-on parity work

The canonical \`code1-decompilation/SharedDecomp/original_world_oracle.py\`
now indexes SHA-256 fingerprints for actual shipped record/cell/control/
event/automap/annotation sections and the SINTAB scale blocks. A later
native audit adapter should expose **derived content hashes only** from
the payload objects so the independent oracle can check exact byte
preservation across both implementations. No original payload bytes should
be exported from the native audit.
