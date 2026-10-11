# Native typed-world byte preservation and independent original oracle

## Implementation

The independent replacement engine now has a read-only
\`rloft_native_payload_audit\` executable (source only, not yet a verified
Windows package). It loads **user-owned LEVELS.DAT**, decodes all 32 maps into
the new \`NativeWorldPayload\`, reconstructs each typed source section in
memory, and requires **exact equality against the original bytes** before
outputting a section's SHA-256 digest. No original cell, script, graphics
or executable bytes are included in its JSON.

Schema: \`rloft-native-world-payload-fingerprints/v1\`.

For all 32 levels it emits:
- \`cells_sha256\`: all packed cell words restored as little-endian uint16.
- \`controls_sha256\`: per-column uint16 count and every ten-byte record.
- \`events_sha256\`: all 50 original 26-byte slots including unused fields.
- \`automap_sha256\` and \`annotations_sha256\`.
- \`family_a_sha256\`, \`family_b_sha256\`, \`family_c_sha256\`: complete
  original eleven-byte authoring records.
- Cell, control and event-command counts.

This is **32 × 8 = 256 independent-comparable section SHA-256 values**.
The computed original input SHA and byte length are also retained so a
separate oracle can reject reports from a different game version.

The source remains a new replacement engine: it never compiles DemoV1
gameplay or executes original x86 SINTAB microcode. The existing RES-bank,
world structural audit and original-window GUI remain unchanged. The new
binary is an internal native validation utility and is not yet included
in existing asset-free Windows release packaging.

## Independent Python comparison

On \`code1-decompilation\`, the companion script
\`SharedDecomp/verify_native_world_fingerprints.py\` consumes the **actual**
native audit report and independently parses locally owned \`LEVELS.DAT\`
plus \`SINTAB\`. It checks the full original SHA identity and all 256
reconstructed-source section SHA-256 hashes (including 11-byte families),
reporting first mismatching original map/field.

Commands when both checkouts/tools are available:

\`\`\`sh
cmake -S NativeEngine -B build/native -DCMAKE_BUILD_TYPE=Debug
cmake --build build/native --parallel 2
ctest --test-dir build/native --output-on-failure
build/native/rloft_native_payload_audit /private/original/install > /private/native_payload.json
python3 SharedDecomp/verify_native_world_fingerprints.py \
  --levels /private/LEVELS.DAT --sintab /private/SINTAB \
  --native-report /private/native_payload.json \
  --out build/research/native-payload-diff.json
\`\`\`

(The read-only CLI also accepts an original GOG \`game.gog\` ISO through
\`NativeMedia\`. The executable path above is platform-dependent.)

## Verification gates, not claims

- **Local isolated C++17 SHA-256 smoke tests:** empty message, "abc", and one
  million "a" bytes passed. This tests the standalone hashing algorithm only.
- **Independent original-data audit:** previously recorded original level
  fingerprints were independently rechecked against all 32 locally supplied
  original level records (192 source sections total); source SHA matches.
- **CMake tests:** existing \`native_world_payload_tests\` now checks three
  official SHA test vectors, all 32 synthetic levels and mutations of cell,
  control, event and automap bytes; same **17 tests registered**.
- **Native current-SHA C++ compilation:** not yet independently executed;
  Windows MSVC and owner field/gameplay acceptance remain pending. No new
  native GitHub Actions run has been scheduled.

A metadata hash match never proves the game is playable. \`native_gameplay_parity_verified\`
remains false, and the independent Python gate never asserts DOS runtime
execution. Original media and derived frame data stay out of Git.
