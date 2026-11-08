# Part 24 — authenticated retail CODE.2 palette-write chain (static gate)

**Date:** 2026-10-19 / follow-on checkpoint. **Owner:** `Recomp`.  
**Scope:** the entire original `COLORS`-to-VGA-DAC output subsystem's *direct static* calls in original CODE.2 LE object 1. It is **not** DOS runtime reachability, a traced call stack, or full DOS-vs-native parity.

## Original evidence and methodology

The read-only source of truth is the owner-supplied retail `CODE.2`,
SHA256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Original LE object 1 begins at file offset `0x3AF24`. The bounded evidence verifier is
[`Recomp/tools/trace_original_palette_write_chain.py`](tools/trace_original_palette_write_chain.py);
it refuses a changed original SHA and emits addresses/counts, never original instruction bytes.
Static `E8 rel32` targets were independently enumerated, their candidate caller windows
checked against the actual executable, and DAC-related original instructions inspected
with a 32-bit x86 disassembler. No game binaries, grayscale values, or decompressed
resources are committed.

## Direct static graph and hardware output

| Original CODE.2 object-1 entry | Matching direct E8 candidates | Identified action |
| --- | ---: | --- |
| `0x325B0` | 23 | File-backed full `COLORS` bank read to staging |
| `0x32150` | 1 | Source palette brightness transform, called from the loader |
| `0x32220` | 14 | Palette forward effect/transition |
| `0x322EC` | 14 | Palette reverse effect/transition |
| `0x323BC` | 0 | Copy RGB-triplet color range |
| `0x32404` | 2 | Fill RGB-triplet color range |
| `0x32468` | 4 | Blend RGB-triplet color range |
| `0x324F8` | 0 | Rotate RGB-triplet color run |
| `0x29234` | 21 | Whole 768-byte DAC submission |
| `0x29248` | 0 | Partial DAC-submission wrapper |
| `0x29CA0` | 2 | VGA indexed DAC byte streaming (the two wrappers) |

The 23 `0x325B0` direct callers all contain an authenticated immediate `push 256`
count marker and write into guest palette staging buffer `0xC054`. This
does **not** establish exhaustive runtime calls via indirect branches, jumps,
other LE objects or code/data disambiguation beyond the identified windows.

Every one of the 21 direct `0x29234` callers has a uniquely identified nearby
staging-pointer push, grouped as follows:

| Staging buffer (guest offset) | Direct callers | Role |
| --- | ---: | --- |
| `0xC054` | 16 | Game/menu palette |
| `0xC355` | 2 | ANM/cinematic palette |
| `0xF9D0` | 3 | Temporary palette fade/effect output |

At `0x29234`, the original instruction sequence submits `0x300` bytes
beginning at DAC color index 0 to `0x29CA0`. The original `0x29CA0`
selects VGA index port `0x3C8`, streams RGB6 data to port `0x3C9`, and
limits each retrace-controlled batch to `0xC0` = 192 bytes.
The only byte-matched literal `mov dx,0x3C8` and `mov dx,0x3C9`
sequences in this original object are in that output function.
Dynamically calculated port selections and other LE objects are not excluded.

## Eight grayscale entries: what has and has not been resolved

The owner's two independent DOSBox-X `1.sav` captures had the same 768-byte
original DAC SHA:
`0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812`.
The 8 entries at palette indices **246..253** were independently matched to
authentic retail `COLORS` bank 3; all other entries match bank 0.
The existing *opt-in*, **provisional** saved-world source palette path achieves
that original RGB6 DAC SHA and is CI-gated separately.

This new source audit **does not** find a directly called 8-entry `COLORS`
loader or a directly called partial `0x29248` update. It **does not prove**
those routines are unused. The precise retail call sequence constructing or
installing the eight bank-3 grayscale entries remains **NOT_VERIFIED**.
Do not remove the provisional label, and do not claim exact DOS indexed pixels
or actor-frame equivalence from DAC equality.

## Reproduce and test

```sh
python3 Recomp/tools/trace_original_palette_write_chain.py --self-test
python3 Recomp/tools/trace_original_palette_write_chain.py \
  --code2 /private/retail/CODE.2 \
  --output /private/palette-chain-evidence.json
```

The original-media saved-world workflow calls both commands with exact
SHA-authenticated retail CODE.2 and deletes the temporary report before
the runner exits. This is a bounded source/hardware evidence gate only;
it does not export original bytes.

**Verification:** local self-test and local authenticated-original trace
both passed; the saved-world workflow [run #39347407452](https://github.com/Hyperbog/RLOFT/actions/runs/39347407452)
passed with the new original-media audit, while Linux regression and
CODE.1/CODE.2 source-link checks on the preceding source commit also passed.

**Next evidence required:** establish the executable caller/branch chain that
copies, blends, rotates, or otherwise stages the final 246..253 triples and
records the *real* runtime DAC write sequence. Use an independent protected-mode
DOS execution trace/port-write log or authenticated DPMI call trace, then
compare exact source-buffer state and call order at the matching saved camera.
A static absence of near E8 calls cannot close indirect-jump possibilities.
No owner action is necessary to preserve or run the current static gate.
