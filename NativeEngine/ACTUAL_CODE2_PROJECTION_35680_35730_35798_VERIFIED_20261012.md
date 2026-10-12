# Authentic CODE.2 35680 / 35730 / 35798 projection arithmetic — verified (2026-10-12)

**Scope:** Source-grounded fixed-point multiplication, original near-plane thresholds, unsigned wall-height and signed screen/point projection into the previously authenticated RES4 texture-column renderer. This is **not** the original camera traversal, player pose, ray/side selection, wall ordering or a DOS frame match.

## Exact authoritative original instruction bytes

User-owned `CODE.2` file SHA256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, physical object-1 offset `0x3AF24`, object length `0x3A852`. The following original object-relative code ranges were separately extracted, disassembled with GNU objdump -m i386 -M intel, and SHA-pinned:

| Source function | Half-open object span | Original bytes | SHA-256 |
|---|---|---:|---|
| `35680` partial-product fixed-point multiply | `35680..356D8` | 88 | `4a0de259caeeb0ea8a47809aefce47c363bf37e956578ad0bb402399cd353df6` |
| `35730` projected wall height | `35730..35797` | 103 | `9c37f50641e5c02afc0d68f0c070ca2b38874be1a600d3eace267c9c3eeb9e60` |
| `35798` projected screen point | `35798..35869` | 209 | `af99ef81b74f6d16cc3cc666717ea76fad4188393dc951ea68bfb01c523550b3` |

Original `35680` uses `MUL BX` twice on the low and high 16-bit halves of the absolute input value, combines via `ADD AX,DI / ADC DX,0`, and corrects the result's sign from the source DWORD sign and coefficient **high WORD sign**. It computes the low32 of `(|input| * (coefficient & 0xFFFF)) >> 16`, negated modulo32 when those signs differ. **Ordinary signed 32×32 multiplication would be incorrect**, as would an unshifted 32-bit product.

Original `35730`:
```text
dx = wrap32([255F0]-[255EC])
dy = wrap32([255E8]-[255F4])
depth = wrap32(MUL35680(dx,[255F8])-MUL35680(dy,[255FC]))
if signed(depth)<0x1680: depth=0x1680
divisor = depth >> 8         (logical SHR)
wall_height = LOW16(UNSIGNED32([255E0]) / divisor)   (DIV)
```

Original `35798` accepts *separate caller point* DWORD x/y, computes signed-capable 32-bit wrapping camera-relative deltas, projects their difference via two `35680` products, subtracts `0x2000` from depth and computes the **separate transverse sum** via crossed coefficient multiplications. It rejects depth `<0x5800` under a signed check by writing only a zero height WORD, **preserving the previous screen-coordinate WORD**. Otherwise:

```text
screen_word=LOW16([C6D0] + SIGNED_IDIV_64x32(
   signed32([C6CC]) * signed32(transverse), signed32(depth)))
projected_height=LOW16(SIGNED_IDIV_32x32(
   signed32([255E0]), signed32(depth)>>8))
```

There is no extra modern clamp or guessed camera normalization. Host arithmetic throws on the original x86 `IDIV` zero/overflow condition. The caller provides every original-DGROUP register value; these APIs must not be said to recreate the live DOS player's view or traversal.

## Verified native source and regression

- Exact compiled source `native-engine:52ebe94988d74181bf469d6b1ad9bb400ed84420`.
- [Portable source-only Linux Actions #38208398620](https://github.com/Hyperbog/RLOFT/actions/runs/38208398620): **27/27 CTests PASS**, including `native_code2_projection_tests` and the extended original `native_code2_wall_emission_tests`.
- `NativeEngine/include/rloft_native/native_code2_projection.hpp`, `src/native_code2_projection.cpp`: typed pure original-DGROUP inputs, exact modulo32 partial multiplication, original signed/unsigned arithmetic and protected near-plane semantics.
- `draw_native_code2_wall_record_projected_35730` combines the source-proven `35730` wall-height result with the previously verified `32F8C/33124` two-wall resource emitter, the original 64-column RES4 material source and CODE.2 `338C8` fixed-point four-plane VGA sampler. Raster height outside the already-certified 1..256 span fails closed rather than silently faking DOS clipping.
- Native tests additionally exhaust **65,536 original coefficient low-word cases × six source values × both coefficient sign states = 786,432 original 35680 arithmetic checks**, plus original near-plane 35730/35798 boundaries, signed projection overflow, accepted and rejected point behavior, selected textured projected wall draws and rejection without partial VGA writes.
- New read-only `rloft_native_code2_projection_audit` loads owner's actual CODE.2, verifies the above three exact original instruction-window hashes, generates deterministic arithmetic sample cases and prints SHA256 metadata only. It never executes DOS x86 or embeds original executable bytes in Git/CI.

## Original-byte C++ versus independent Python evidence

The binary from exact successful [Actions #38208398620](https://github.com/Hyperbog/RLOFT/actions/runs/38208398620) was run privately against the authentic CODE.2 and compared against a separately written Python implementation of the actual two 16-bit multiplication partial products, original DWORD wrapping, near gates and signed x86 `IDIV`. The two independently calculated digests match **exactly**:

```text
Domain: RLOFT-CODE2-projection-35680-35730-35798-v1
7defd11aded87d648697c9448325f96ae3fb8cad28415fc199fcb172852f53fd
```

| Actual compiled C++ and independent Python audit | Verified |
|---|---:|
| Original instruction ranges SHA-confirmed | **3/3** |
| Deterministic projection cases | **4,096/4,096** |
| Original-result encoded bytes compared | **114,688 / 114,688** |
| Accepted projection points | **1,339** |
| Points rejected at original near plane | **2,757** |
| Wall heights clamped by original `0x1680` floor | **2,177** |
| First-eight structured projection samples identical | **8/8** |
| Digest / result discrepancies | **0** |
| Original physical binary bytes committed to Git | **0** |

Source-only independent canonical comparator `SharedDecomp/verify_native_code2_projection.py` in `code1-decompilation` now SHA-gates the actual original three spans, replicates all 4,096 original arithmetic cases, compares their digest and first eight typed samples against the C++ audit JSON, refuses unsupported camera/gameplay parity claims and rejects duplicate JSON keys. Its 11 new synthetic original-free regressions cover arithmetic, clipping, signed division, corrupted span bytes, missing/mutated hashes and CLI validation. [Research Actions #38208618987](https://github.com/Hyperbog/RLOFT/actions/runs/38208618987): **200/200 tests PASS** (39 ResearchInfra + 161 SharedDecomp), exact research implementation `38a24e582e8adf6f5f371710660a8bceb1feab71`.

Local original-free metadata reports:
- `/mnt/data/rloft_native_code2_projection_report.json` — actual GitHub-built native executable run against private original source.
- `/mnt/data/rloft_code2_projection_original_reference.json` — independent original-algorithm Python reference.
- `/mnt/data/rloft_code2_projection_independent.py` — independent source-only oracle.
- `/mnt/data/RLOFT-native-CODE2-projection-52ebe949.zip` — validated Linux source-only auditors; no original game files.
- The owner-owned original media stays private and outside Git/CI.

## Precise completion boundary / next priorities

**VERIFIED:** Original CODE.2 projection math helpers `35680`, `35730`, `35798` by source-byte identity and independent bit-width semantics; integrated generated original wall height into source-backed original LEVELS→RES4→EH6 textured four-plane output with caller-provided projection register context.

**NOT VERIFIED:** Original live camera registers / heading, original ray stepping and selection of near/far wall paths, signed sentinel/neighbor state, actual DOS 3D screen-destination clipping/size policy, sprite occlusion/depth ordering, texture shade assignment in the real world, VGA COLORS final presentation and complete reference DOS frame parity, gameplay events/AI/saves and current Windows graphical build. Next target is CODE.2 upstream camera-state construction/viewport-ray call chain and DOS reference frame traces. Do not confuse an input-driven projection primitive with completed original camera traversal. This independent NativeEngine is separate from the canonical faithful decomp/recomp project; do not link DemoV1 gameplay. Windows release Actions remain manual-owner-controlled.
