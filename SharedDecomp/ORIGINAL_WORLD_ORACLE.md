# Retail world-data oracle and native structural comparison

Owner: \`code1-decompilation:SharedDecomp/\`. This is an **independent**,
read-only, Python standard-library oracle for locally supplied
\`LEVELS.DAT\` and \`SINTAB\`, designed to compare to the existing native
C++ \`rloft_native_world_audit\` JSON output.

## Exact commands

\`\`\`sh
# Source-only original-byte audit with exact known original identities.
python3 SharedDecomp/original_world_oracle.py \
  --levels /private/LEVELS.DAT --sintab /private/SINTAB \
  --require-known --report build/research/original-world.json

# Use a REAL independently generated native audit report here.
python3 SharedDecomp/original_world_oracle.py \
  --levels /private/LEVELS.DAT --sintab /private/SINTAB \
  --require-known --native-report /private/native-world-audit.json \
  --report build/research/original-vs-native-world.json

python3 -m unittest discover -s SharedDecomp -p 'test_*.py' -v
\`\`\`

The \`--require-known\` gate matches the two user-supplied Strahd originals:
\`LEVELS.DAT\` 499878 bytes, SHA-256
\`50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91\`;
\`SINTAB\` 333757 bytes, SHA-256
\`19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87\`.

It independently validates the complete 32-level offset directory, section
boundaries, grid dimensions, 16-bit cell-word payload lengths, all 10-byte
column control lists, three 11-byte compact families, fifty 26-byte
event slots per level, opcode bounds, automap grids and annotation tails.
It also audits all 127 SINTAB generated x86 scale blocks, the 129-entry
descriptor tables and the 512-byte zero trailer without executing x86.
Its output includes section-level and block-level SHA-256 fingerprints, not
copyrighted bytes or private filesystem paths.

Independently examined supplied originals yield 115584 packed cells,
1659 controls, 394 active event slots, 555 commands and 324989 generated
scale bytes. The cell bit 0x2000 occurs exactly three times, at level
12/13/14 coordinates (44,34). This is a byte observation, not a claim of
known game behavior.

An equal original-versus-native report is **STRUCTURAL_MATCH only**.
NativeEngine's v1 world audit currently exports dimensions and counts,
not its decoded cell words or microcode bytes. This gate therefore always
reports \`native_cell_bytes_compared=false\`,
\`native_sintab_bytes_compared=false\`,
\`native_original_sha256_attested=false\`,
\`dos_execution_verified=false\` and \`native_gameplay_parity=false\`.
It does not establish visual fidelity or executable DOS gameplay parity.

GitHub source-only CI uses generated, original-free mock level/SINTAB
structures. The genuine retail comparison requires the user's local
files plus an actual separate NativeEngine executable output.
