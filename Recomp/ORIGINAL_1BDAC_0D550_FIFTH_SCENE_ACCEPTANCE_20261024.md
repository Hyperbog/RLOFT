# Part 29 — Fifth original input-loop scene (2026-10-24)

Scope: original CODE.2 Level 22 in branch `Recomp`. No original game media belongs in commits.

## Authentic source dependency

Original CODE.2 at `1BE24` calls `0D550` with actor, NPC definition, and actor index. Original `1BF87` calls `30CD4` with destination, source, and length for overlapping record compaction. Live fifth-tick original-media arguments for `0D550` are actor `0x14D80`, definition `0x101A701`, index `0`.

For NPC type 9 the original `0D550` uses a signed absolute camera/actor distance test. Its early return at `DAC4` applies when either coordinate difference exceeds `0x15000`. The fifth-scene actor is beyond that range. Original special type-15 and type-8 branches do not apply. Other AI branches remain fail-closed.

The callback now executes the **compiled original** `code2::function_0d550(Machine&, Application19Boundary&, u32, u32, u32)` for the verified callback-free distant-actor case, rather than a host-simulated action. The original `30CD4` memory move also executes from compiled source, with guest bounds checks.

## Windows original-media acceptance

[Windows #40070934675](https://github.com/Hyperbog/RLOFT/actions/runs/40070934675) **passed** its mandatory `ORIGINAL_1FC14_FIVE_SCENE_SOURCE_GATE_PASS` assertion at [1d9e38279860](https://github.com/Hyperbog/RLOFT/commit/1d9e382798608ec52553375e6349a6605e68d624).

- Five original `1FC14` scene returns; five authentic `1A510` VGA/CRTC page handoffs.
- Fifth scene: original polygon quads `8`, polygon scanlines `212`, RES3 sprites `175`, indexed sprite writes `41210`, actor and compass return `1` each.
- Fifth original page `0`; heading `270`; virtual keyboard code `0` after private compositor handshake.
- Original distant actor: NPC type `9`, actor x/y `1145856/489472`, camera x/y `294912/294912`.
- [Linux #40070870433](https://github.com/Hyperbog/RLOFT/actions/runs/40070870433) and [canonical link #40070870407](https://github.com/Hyperbog/RLOFT/actions/runs/40070870407) passed at the compiled original `0D550` integration commit [f0ae9d0767a3](https://github.com/Hyperbog/RLOFT/commit/f0ae9d0767a39a6283f08725592a7600caa1addd).

## Acceptance boundaries

Five scenes are **not** five complete `1F8FC → 1A5B0` gameplay-frame presentations; the accepted complete one-shot `1F8FC` frame count is still **one**. Retail RGB6 DAC parity remains verified, while temporally synchronized retail DOS indexed framebuffer parity remains **open**. Post-scene HUD/audio acceptance, full nearby-actor AI, and physical-input playability are not established.

Next: certify original `0D550` near-field `Application19Boundary` callbacks, post-scene `1FC14` HUD/audio processing, and genuine matched DOS Level 22 indexed pixels.
