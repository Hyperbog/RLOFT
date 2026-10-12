# Independent original-to-NativeEngine world runtime verification

\`SharedDecomp/verify_native_world_runtime.py\` is the canonical
source-only, original-file-backed comparator for the native persistent
world session. It uses the original 32-map directory and reconstructs
the independently hashed retained-source layout (not a fingerprint
copied from the native C++ executable).

\`\`\`sh
# Generate REAL native report from source-compiled auditor.
rloft_native_world_runtime_audit /private/original/install > /private/runtime.json

# Independently read the same locally owned original LEVELS.DAT.
python3 SharedDecomp/verify_native_world_runtime.py \
    --levels /private/LEVELS.DAT --native-report /private/runtime.json \
    --output build/research/world-runtime-original-diff.json
\`\`\`

The default \`LEVELS.DAT\` SHA256 is pinned to the user-supplied original
\`50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91\`.
\`--allow-other-version\` is explicit opt-in and must not be confused
with the owner's original version.

The canonical comparator independently checks:
- all 32 original level boundaries, 16-bit packed cell words and count totals;
- each 10-byte control's original column and row, as well as safe kind-1
  animation position/delta field ranges;
- all 32 levels of A/B/C compact eleven-byte families and all original
  50×26-byte event tables;
- original automap and annotations preserved in an independently computed
  cross-language retained-world **SHA256 source fingerprint**;
- CODE.2's *normal* movement collision decision only: special 0x0080
  center with kind-1 record at position zero passes, otherwise blocks;
  ordinary nonzero six-bit cell codes block;
- full counts for source cells, currently visible/explored cells, special
  centers, original authoring/legacy 0x2000 flags, dynamic control records,
  active animation deltas and three collision outcome categories.

It compares **12 typed evidence fields** in the native JSON (source
fingerprint, original level count, final audit level ID plus ten
source-derived counters) and rejects invalid schemas, false parity claims,
missing zero-valued fields, duplicates and nonfinite JSON.

## Exact actual retail-source milestone

The source-only Linux C++ build for \`native-engine\` commit
\`20543d8fd3a6c61aad7c886d6d1699f1fbf07005\`
passed [GitHub Actions run #38186427291](https://github.com/Hyperbog/RLOFT/actions/runs/38186427291)
with **17/17 CTests**. The asset-free compiled Linux executable was
downloaded and executed **locally on user-supplied original LEVELS.DAT**,
after satisfying \`NativeMedia\`'s loader signature using empty temporary
INIT/RES0/T placeholders. These do not comprise an original game install
or enter the native data being audited.

The **independent Python** original-file walk agreed with actual native C++
for all the following corpus facts:

\`\`\`text
levels                     32
cell words             115584
10-byte control records  1659
kind-1 dynamic records    324
special-center cells      745
ordinary blocked cells  26857
special blocked cells     744
normal passable cells   87983
preexplored cells           3
opaque 0x2000 markers       3
\`\`\`

It also independently reproduced the complete retained-source SHA-256
fingerprint \`a8232782ff8654f9b6c6651300b3fbb2235e58d2fadf61fe2221e11dbbc8bdcd\`
reported by the compiled C++ runtime. This comparison is fully grounded
in real original **input data and the actual native binary output**,
rather than synthetic JSON. The independent metadata-only local report
is \`rloft_native_runtime_original_32level_collision_comparison.json\`.

**Not certified:** original DOS code execution, full collision including
creatures or secret-mode bypass, first-person ray traversal/occlusion,
renderer parity, interactive scripts/NPC AI, audio, save-file compatibility,
current Windows/MSVC build, or playable native gameplay. Normal-collision
classification is source-grounded but is **not a native playthrough**.

CI uses only synthetic, original-free fixtures. Original files and decoded
copyrighted source payloads must never be committed or uploaded to CI.
