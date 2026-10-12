# Native first-person scene-view staging — actual compiled / original-byte verified

Date: 2026-10-11 (CI UTC: 2026-10-12). This is a **source-backed view/rendering foundation** milestone, not a verified DOS-equivalent 3D renderer, Windows demo or playable finished game.

## Certified exact source revisions

- NativeEngine compiled SHA: \`0850d312a76a52715eec459063a47b393396382d\`.
- [Portable native build #38190025508](https://github.com/Hyperbog/RLOFT/actions/runs/38190025508): **18/18 CTests PASS**, including \`native_scene_view_tests\`, \`native_world_payload_tests\`, the source-backed world runtime and complete independent bank parser regressions.
- Canonical research branch SHA: \`9e46d816ad96984ac488cb2fa9f15cacd74f79d9\`.
- [Source-only research CI #38190191412](https://github.com/Hyperbog/RLOFT/actions/runs/38190191412): **129/129 tests PASS** (39 ResearchInfra, 90 SharedDecomp).
- The earlier compiled original-world/runtime milestones remain valid at their exact revisions and are not relabeled retroactively.

## Actual original source-byte verification — upgraded 288/288

Compiled Linux NativeEngine \`rloft_native_payload_audit\` from that exact
source was independently run locally against the owner's original:

\`\`\`text
LEVELS.DAT 499878 bytes
SHA-256 50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91
\`\`\`

The original \`SINTAB\` was also retained and fingerprint-checked in the
separate source evidence input. To satisfy the existing
\`NativeMedia::open\` directory recognition only, the local audit directory
contained empty synthetic INIT/RES0/T placeholders alongside authentic
\`LEVELS.DAT\` and \`SINTAB\`. **This does not constitute a playable
original install; these three placeholders were not used as content.**

A separately implemented Python parser read and SHA-256-hashed all 32
fixed-size 0x2A4 original headers, including all five 64-word resource
lookup tables and opaque header tail. All **32/32 native fixed-header**
digests agree with the original byte slices.

All previously preserved cell/control/event/automap/annotation/compact
A/B/C record hashes also match the actual original. The full present
verification therefore covers **32 levels × 9 independent SHA256
fields = 288/288 matching original-to-compiled-native sections**.

The compiled C++ \`native_world_fingerprints.cpp\` enforces byte-exact
reconstruction before emitting its hashes. A real-source run revealed
one malformed JSON separator in the new header SHA field; native commit
\`0850d312\` fixed it and added a C++ regression guard. The JSON from
that *fixed* executable parsed correctly and passed the independent
288-digest comparison.

Canonical user-facing metadata report:
\`rloft_compiled_native_scene_288_original_section_verification.json\`.
Independent original-only header baseline:
\`rloft_original_fixed_headers_reference.json\`.
Neither report contains original game payload bytes.

## Original-first scene resource candidates

The new NativeLevelPayload now preserves all 676 fixed-header bytes from
every level. \`resource_lookup(bank,code)\` exposes the five 64-word
banks from offsets \`0x00A,0x08A,0x10A,0x18A,0x20A\` without silently
reconstructing undocumented original semantics.

\`native_scene_resource_candidates\` stages the raw candidate IDs for
the first-person renderer. It supports (a) source-proven record state
offset \`control[1]&3\` on 0x1000 cells, and (b) source-proven special
0x0100 frame / 0x0080 center substitution with offset one when a caller
supplies an explicit adjacent direction. It **does not** invent which
CODE.2 branch/table/direction is actually taken: that remains to be
recovered from original executable traversal, geometry and rendering
evidence. Selection parity is marked false.

## A running first-person host probe, deliberately non-faithful graphics

The bounded host-only DDA scene-view helper implements first-person
ray hits, experimental wall distance/projection, camera movement guarded
by the already-proved normal collision gate, and visibility flag updates.
Its debug RGB scene image uses synthetic colors, **not** RES/EH6 texture
samples, RES3/EH7 raster-lists, VGA palette/DAC or original SINTAB
microcode. Its geometry is derived from original level cells but its
ray order, camera, perspective and presentation are provisional.

Compiled \`rloft_native_scene_probe\` from native SHA \`0850d312\` was
run locally against the genuine original level 22. The debug probe
started at the deterministically selected passable cell (column 2,row
2), fired 320 rays and measured 320 wall hits and 7,013 total stepped
map cells. It optionally writes a simple 320×200 PPM for renderer
plumbing diagnosis. Neither the viewpoint nor the output should be
portrayed as an authentic Ravenloft screenshot.

The additional source-only regression validates synthetic source lookup
tables, state offsets, special-neighbor selection, closed/open dynamic
barrier ray hits, bounded ray steps, source-isolated 0x0440 visibility,
host-camera collision and rejection of malformed camera and lookup
inputs. **18/18 CTests verified** at the exact source; no original
code/payload was embedded into CI.

## Next architectural gate

The next substantial implementation is CODE.2-faithful camera/side
ray-geometry selection feeding original level mapping banks; then
validated RES3/EH7 display-list records and SINTAB scale selection,
EH6 source textures and palette handling in the native renderer.
Corrected 3D rendering must be validated by trace/frame comparisons
against an actual original DOS run, not merely by the current host
debug viewpoint.

Other unverified work: full 16.16 player motion and footprints, actor
occupancy, multi-pass render order, animation, event VM, gameplay rules,
original SAVE/LEVnnn writeback, current-source Windows/MSVC build and
owner field playtesting. Manual-only Windows release workflow remains
untouched. NativeEngine must remain independent of DemoV1 gameplay.
