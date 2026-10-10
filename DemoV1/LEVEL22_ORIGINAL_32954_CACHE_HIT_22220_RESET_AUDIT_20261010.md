# Part 2 original-ground comparison: 335C8 / 32954 cache-hit depth reset (2026-10-10)

**Owner:** demo-v1 / native source-to-original comparison.
**Authority:** user-supplied original Strahd `CODE.2`, 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; object-1 file offset `0x3AF24`.
**Proof:** [original-byte verifier](tools/verify_original_ground_cache_hit_22220.py).
**Class:** original executable instruction-byte control-flow proof, **not** observed original DOS frame/IRQ/VGA parity.

## Newly verified bounded result

The end of the original ray preparation caller contains the *unconditional* source instruction at logical `335C8`:

```asm
335C8  C7 05 20 22 02 00 00 08 00 00   mov dword [22220], 0800h
335D2  E8 7D F3 FF FF                  call 32954
335D7  5E 5F 5B C9 C3                  pop esi; pop edi; pop ebx; leave; ret
```

The called `32954` routine invalidates `FCE8` and `FCEC` to `FFFFFFFF` **before** checking cache eligibility. A cache-hit return requires all four original conditions:

1. `DWORD[CF50] == 0` (branch at `32973` otherwise enters recomputation);
2. `DWORD[C658] == DWORD[FD04]` (facing compare `3297A`);
3. `DWORD[FCFC] == DWORD[255D0]` (camera X compare `32987`);
4. `DWORD[FD00] == DWORD[255D4]` (camera Y compare `32994`).

All mismatches go to `3299C` to save registers and jump to the traversal entry `329A8`. Only satisfying every predicate branches from `3299A` to `329A6: LEAVE; 329A7: RET`. This **early return precedes source ray traversal/materialization**. Therefore, **on this caller/cache-hit path, `22220` remains the freshly reset `0x800` at the `335D7` caller return boundary**. Replaying previously cached 256 ray records to derive a new nearest-depth value does not reproduce this original control flow. Ray records themselves can be retained while this scalar was reset.

The isolated existing `OriginalGroundNearest22220SourceModel` is still a valid *materializer-event* model; it is **not** a complete frame-lifecycle or cache-hit execution model. No runtime game change is authorized by this proof.

## Verification completed

The new standalone Python oracle validates the immutable complete original `CODE.2` SHA/length, object-1 mapping, SHA-256 of both full bounded instruction windows, every predicate/push/return instruction, all five relative branch destinations and the exact caller `E8` target. Against the user-supplied original bytes, it returned:

```text
ORIGINAL_PASS CODE.2 335C8 reset + 32954 cache-hit early-return boundary
```

Command (local installed original only):

```sh
python DemoV1/tools/verify_original_ground_cache_hit_22220.py --original-code2 /path/to/CODE.2
```

Neither original binary nor derived gameplay pixels were committed. This checker is **not** yet registered in `run_original_ground_proof_batch.py` because its original-input-only contract would be mislabeled as a passing source-only test without additional source-structure gates. No C++ gameplay change, CTest addition, Windows Full CI dispatch, or DOS acceptance is claimed.

## Remaining execution dependencies

- Prove the path from live frame draw through this caller, including mutation/validity of `CF50`, `FD04`, `FCFC`, `FD00` and camera updates.
- Prove source record persistence and whether other code modifies `22220` after `335D7` and before the consumer `1F1C8/FF50`.
- Reproduce both early-return and recomputation paths in a separate lifecycle test before connecting `22220` projection to partial clipping. The current diagnostic model should fail closed where lifecycle provenance is not known.
- Match DOS indexed VGA buffers, palette, IRQ/ticks and original runtime page/ES selectors at identical camera state. This original static byte proof is **not** runtime parity.

**Build acceptance:** last independently accepted Windows Full #658, 214/214 on an earlier checkout. Current source unbuilt; Windows CI remains manual-only. Provisional engineering readiness remains 49.8%, with no complete-original-game percentage inferred.
