# Original CODE.2 placed and loose scene staging — 2026-10-12

**Owning branch:** `native-engine` · **implementation:** `NativeEngine/src/native_code2_scene_staging.cpp` and matching `include/rloft_native` header. This is a source-faithful, bounded preparatory stage, **not** an automatic renderer, DOS scene generation, gameplay, camera traversal, or frame-equivalence certificate.

## Independently authenticated original x86 evidence

Uploaded original `CODE.2`: 558,689 bytes, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Original LE object 1: file offset `0x3AF24`, size `0x3A852`,
SHA-256 `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
The following logical object-1 byte windows were independently SHA-256 checked before disassembly using `objdump -D -b binary -m i386 -M intel`.

| Original CODE.2 byte interval (inclusive) | SHA-256 |
| --- | --- |
| `2BBF4..2BCBD` | `db1c682fd279be61734208ac351fd0ed485b8a1c2355f2b0be2a9861db5f3506` |
| `2BE48..2BF17` | `8f9d8651a46a6db87baf2a5e4143531829ecce31e65bb3ff387a59c045b2fa4c` |
| `2BBA0..2BDF0` (complete class-one producer) | `43dd12d6d907228f059e61ba31f1a1025196933416f68be8bfe041a6c5a4e66f` |
| `2BDF4..2BFE9` (complete class-two producer) | `9fc197df0d42222a2ce7a8f87babbbd8ce8638f8d26353e386b50143801a43be` |

Disassembly establishes **two distinct producer paths**:

- Original `2BBA0` consumes 22-byte placed-world records and 7-byte INIT class-one descriptors through pointer `0x255DC`. It computes source shape word `+0x04 = definition WORD[0] + definition BYTE[4] * view`, with modulo-16-bit writes.
- Original `2BDF4` consumes 22-byte loose-object records and 0x41-byte OBJECTS class-two descriptors through `0x255D8`. It computes `+0x04 = definition WORD[0x28] + definition BYTE[0x2A] * view`, with the same truncated write.
- In both paths `view` is produced by source `SHL EAX,7` followed by `SHR AX,13` after reading source world record WORD at `+0x14`. An independent equivalent is `(WORD[0x14] >> 6) & 7`; retaining the intermediate 16-bit truncation is important.
- Visibility begins with a test of current cell WORD flag `0x0040` at `0x198E0`, indexed from source WORD `+0x0C`. That flag is supplied explicitly rather than inferred.
- The initial scene record WORD `+0x0C` is cleared; original `35798` projects source DWORDs `+0/+4` into output screen-X WORD `+0` and projected-height WORD `+2`. Then output WORDs `+0x0E,+0x10,+0x12,+0x14` are zeroed.
- Original `2BCA2..2BCC2` / `2BEF7..2BF17` accept further processing only for nonzero projected height strictly greater than CODE.2 DGROUP BYTE `0xC02D`. The host receives that original threshold explicitly.

The new native function `stage_native_code2_non_npc_2bbf4` stops **before** `2B2F4`, `2B088`, the live frame/pose selection, final record-type stamps, record publication, and draw dispatch. It cannot supply original ordering inputs by itself. It also rejects wrong definition lengths and unknown family tags as host safety policy.

## Testing completed locally

1. Original-independent native synthetic regression: both definition layouts, view=7, 16-bit wrapped frame calculation, source `35798` projection producing screen X=9 and height=146, preserved screen X on original near-plane rejection, strict `C02D` threshold, visibility early exit, zeroed tail words, and invalid-source rejection.
2. Exhaustive original-word extraction regression: **65,536 packed input WORD values × both definition families = 131,072 cases**. Expected view indices and frame offsets are independently calculated using `(packed >> 6) & 7`; no mismatches.
3. Owner-original media: `OBJECTS.DAT`, 21,844 bytes, SHA-256 `f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0`. Its length is exactly `4 + 336 × 65`. All **336 definitions × 8 packed view cases = 2,688** original-object staging results agree with direct source-byte calculations, including the expected stage screen coordinate/height/zero fields. Observed original definition stride distribution: **26 zero, 15 one, 17 two, 278 eight**.
4. Standalone C++17 tests and original-object auditor were compiled locally with `-Wall -Wextra -Wpedantic -Werror -UNDEBUG -fsanitize=address,undefined` and passed. No original media was copied into the repository or CI.

The INIT class-one path is verified against x86 and independent synthetic descriptors, not an entire owner-original INIT media corpus in this checkpoint.

## Gate boundaries / follow-up

`NativeEngine/CMakeLists.txt` registers the new `native_code2_scene_staging_tests`. Two additional source CTests (scene order and staging) now follow the prior **29/29 independently documented Linux baseline**. **31/31 on current HEAD is not claimed**: the full Windows/Linux CMake suite still needs a passing exact-source build.

Continue the original `2B2F4` frame/callback path and `2B6D0` NPC-source producer; then join the real 400-record active scene buffer, original `2C47E..2C7B0` renderer call dispatch, and already source-backed EH7/SINTAB compositor. Do not add a gameplay camera, stage fabricated live NPCs, or claim original DOS image parity.
