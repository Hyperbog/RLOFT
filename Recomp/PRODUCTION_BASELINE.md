# Recomp production baseline — 2026-11-08

**Status: engineering baseline, NOT a production game release.**
Authoritative development branch: `Recomp`. The original media remain on
`DnD-Classics/MASTER`; historical reconstruction on `Decomp` and the original
reconstructed sources `Code1/Code1.cpp` and `Code2/Code2.cpp` are preservation
inputs, not obsolete generated files.

## Ownership and reproducibility

- The game source owns one original CODE.1/CODE.2 runtime, guest memory, stack,
  resource cache, VGA device, IRQ lifecycle, and save state.
- `Recomp/src/game/{core,audio,graphics,resources,filesystem,input,timing,ui,world,save}`
  contains host services and independently compiled C++17 system libraries.
  `Recomp/cmake/GameSubsystems.cmake` lists the static library dependencies.
  Original CODE.2 callbacks that depend on guest-local types remain compiled in
  the canonical owner; independently compiling another CODE.2 is prohibited.
- No original commercial game data belongs in the source build, test fixtures,
  or public engineering artifacts. A release uses player-provided media.

## Required code acceptance checks

Windows (x64 MSVC, from the repository root):

```pwsh
cmake -S Recomp -B build/recomp -A x64
cmake --build build/recomp --config Debug --parallel 2
ctest --test-dir build/recomp -C Debug --output-on-failure
```

Linux (portable C++17):

```bash
bash dev/linux.sh test
```

Repository inventory (both OS):

```bash
python3 -m unittest discover -s Recomp/tools -p test_audit_compatibility_headers.py -v
python3 Recomp/tools/audit_compatibility_headers.py --root .
python3 Recomp/tools/audit_recomp_source_layout.py --root .
```

The GitHub Linux portable build and Windows canonical CODE.1/CODE.2 source-link
workflows are PR checks for changes targeting `Recomp`. Both must pass on the
**exact merge candidate**. Host-unit, synthetic mouse, original reconstructed ISR,
and title menu fixture tests do not establish live retail DOS parity.

## Cleanup policy

1. Classify a file as source implementation, live compatibility forward, test,
   tooling, historical research, original program or original media.
2. Migrate every source include and every explicit CMake/workflow reference to its
   canonical owner before removing a compatibility forward.
3. Inspect `audit_compatibility_headers.py`'s report-only inventory. Remove only
   a forwarding header with **zero** remaining C++ includes and build/tool refs;
   review any documentation links before deletion.
4. Preserve the relevant original source STOP gates, same-guest runtime, and
   source ABI contracts; CI must pass after the deletion.
5. Never delete original media, reconstructed CODE.1/CODE.2, prior source
   evidence, or a research branch as a substitute for a real dependency audit.

## Engineering package versus production release

The `Recomp/tools/release_package_gate.py` validator enforces a strict source-only
package manifest and hashes. Its `production` mode also requires **all twelve**
accepted gates in `Recomp/ACCEPTANCE_GATES.json`. As of this baseline, only
**R01–R03** are accepted (3/12). Packaging success does not make a game playable.

Pending acceptance includes true DOS/4GW lifetime, original character-creation
handoff, normal original-media first boot, physical title click-through and new
game, native gameplay across the campaign, authenticated audio/VGA behavior,
ordinary save/load UI, and independent DOS comparison. These are not waived by
green CTests.

## Next functional milestone

From the native player with player-owned original game media:
source startup → complete intro/title → physical New Game selection →
original `7B94` handoff → character creation → first gameplay frame.
Capture exact SOURCE_COMMIT, input, source STOP reason (if any), frame hash,
and separately recorded DOS-original comparator. No fabricated successful
return values, per-level gameplay shortcuts, or fixture-only unlocks.
