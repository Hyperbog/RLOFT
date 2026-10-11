# Exact original-file ↔ typed NativeEngine source preservation

The owner-supplied retail original is the reference. The independent native
engine now has an asset-free fingerprint **source implementation** that will
serialize its retained native world payloads back into the original section
layouts, byte-compare every section, and emit 256 section SHA-256 hashes.

The canonical tool \`SharedDecomp/verify_native_world_fingerprints.py\`
independently parses the exact locally supplied \`LEVELS.DAT\` and \`SINTAB\`
using \`original_world_oracle.audit\`; validates the full input source SHA,
section boundaries, dimensions and controls; and checks the native report's
32 × 8 hashes for cells, ten-byte controls, 50 event slots, automap,
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
