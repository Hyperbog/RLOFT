# Exact original-file ↔ typed NativeEngine source preservation

The owner-supplied retail original is the reference. The independent native
engine now has an asset-free fingerprint **source implementation** that will
serialize its retained native world payloads back into the original section
layouts, byte-compare every section, and emit **288 section SHA-256 hashes** at native compiled source `0850d312` (256 prior content-section hashes plus 32 complete original 0x2A4 fixed-header hashes).

The canonical tool \`SharedDecomp/verify_native_world_fingerprints.py\`
independently parses the exact locally supplied \`LEVELS.DAT\` and \`SINTAB\`
using \`original_world_oracle.audit\`; validates the full input source SHA,
section boundaries, dimensions and controls; and checks the native report's
32 × 9 hashes for entire 676-byte fixed headers, cells, ten-byte controls, 50 event slots, automap,
annotations and each of the three eleven-byte compact record families.
It fails on the first mismatch and saves full bounded diagnostic metadata.

\`\`\`sh
# First produce REAL NativeEngine output from the independent checkout:
rloft_native_payload_audit /private/original/install \
  > /private/rloft-native-payload.json

# Then run the canonical independent comparison on the SAME owned originals:
python3 SharedDecomp/verify_native_world_fingerprints.py \
  --levels /private/LEVELS.DAT --sintab /private/SINTAB \
  --native-report /private/rloft-native-payload.json \
  --out build/research/original-native-payload-diff.json

# Synthetic original-free self-tests:
python3 -m unittest discover -s SharedDecomp -p 'test_*.py' -v
\`\`\`

The original-file gate defaults to **the two exact user-supplied SHA256
identities**. \`--allow-other-version\` is opt-in for structurally compatible
originals, *not* permission to silently relabel a different version.

A passing report is \`ALL_32_ORIGINAL_SECTIONS_MATCH\`. It establishes
equality of the independently parsed section hashes and the submitted
native source report, **not** original x86 execution, a checked native
MSVC/Windows binary, or any gameplay/raster parity. The tool sets
\`compiled_native_binary_verified=false\`,
\`dos_original_execution_verified=false\`, and
\`native_gameplay_parity_verified=false\`.

The original-media oracle and the native result are kept separate:
The Python code never uses the native report to derive expected hashes,
and refuses missing levels, duplicate JSON keys, nonfinite numbers,
wrong version SHA, missing or invalid SHA strings, invalid native
provenance flags and unequal decoded byte sections.

**Current evidence:** source-only cross-project CI exercises synthetic
native-shaped reports and intentional corruption. A full actual native C++
build with original user media remains a separate acceptance gate. The
user-supplied original level fingerprints were independently rechecked
locally without modifying retail game files.

## Authentic retail header gate — added 2026-10-11

The source-only native Linux build at `0850d312a76a52715eec459063a47b393396382d`
passed 18/18 CTests. An actual locally executed GitHub-built payload auditor
retained all 32 original fixed 0x2A4 headers **byte-for-byte**, including
five 64-word mapping tables and unresolved original fields. Independent
Python SHA256 over the real user-owned `LEVELS.DAT` matched **32/32 native
header digests**, in addition to the previously certified 256/256 cell,
control, event, A/B/C family, automap and annotation digests. Total now
**288/288 original-source section hashes**, with the original whole-file
SHA separately pinned. The built scene-view raycaster remains a marked
provisional host demonstration: neither CODE.2 visual projection nor
original palette/textures are certified by hash preservation.

The verifier now **requires** `header_sha256` on every native level and
rejects historical pre-header native reports until they are regenerated
with the newer native auditor. Three new source-only tests exercise a
header-only original mutation, missing digest and digest mismatch.
