# Level 22 original RES4 EH6 compressed-ground decoding — verified original media, 2026-10-10

**Workstream B:** native reimplementation of the original game. **Branch:** `demo-v1`. **Checkpoint:** source-level correction and synthetic regression committed; current HEAD **not yet accepted by Windows CI or original DOS indexed-pixel comparison**. No automatic build, no other branch modified, no original assets committed.

## Original game media independently verified

The user supplied the original Strahd `INIT` and `RES4`, in addition to their existing original `CODE.2` and `LEV022.DAT`. Direct local binary reads establish:

| Immutable original | Size (bytes) | SHA-256 |
| --- | ---: | --- |
| `CODE.2` | 558,689 | `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314` |
| `INIT` | 1,879 | `cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276` |
| `RES4` | 1,975,542 | `d8989c50fa58e317ff33465e7baec16e970f40586417d790fd8a1b9005c60f46` |
| `LEV022.DAT` | 6,028 | `f88b033ff4cdafdd89620e0fac51ff2fb81bce103a6d31a26b91464532c9f51e` |

`u16(INIT+0x0E)=600`, original signed Level22 header+7 is `+55`, so original `1F388..1F398` selects **RES4[655]**, not RES4[55]. RES4 index has **667 entries, every one on disk `EH 06`**; 600 declare **8,192** uncompressed bytes and 67 declare **4,096**. The exact selected entry is 2,515 bytes, beginning `45 48 06 00 00 00 10 FF 9A 9A A0 9E 3B`.

The independent source-aligned Python 37700 LZSS decoder returned exactly **4,096** palette-index bytes from the selected 2,508-byte compressed stream with **no unread stream bytes and no undefined dictionary reference**. The source-decoded image has SHA-256 `2266dc05fa40037116bb7e6914624ee1d1deb0d636d4d404f58e3da3be94a0a0`, FNV1a32 `c1e71881` (noncryptographic). No texture bytes or images are added to Git.

## Original CODE.2 binary resolves the EH6/type1 contradiction

Prior `OriginalGroundRes4Audit::verified_tile_source` wrongly invoked `ResArchive::decode_type1` directly on the original **on-disk** RES4 entry. That path inevitably throws `RES entry is not an EH type-1 raster` on genuine supplied Strahd media, causing the flat lower surface observed in all 12 renderer captures in Full #654.

Independent original-code disassembly and a fail-closed binary verifier confirm the actual load chain:

1. `1F388..1F398` forms `s16(level header+7) + DWORD(258F8)`, loads resource bank **4** through `2F29C`.
2. `2ECB4` ensures the requested resource is loaded; `2EDDA..2EDFB` switches on original disk EH kind `1..7`. The original jump table `2EC98 + 4*(6-1)` targets `2EEC5`.
3. Disk type **6** goes through `2EED2 → 37590`. Routine `37590` reads a **u16 original decoded length**, allocates `length+2`, preserves its original two-byte header, then passes the following compressed stream to `2FB04→37700`.
4. `37700` initializes a 4096-byte LZSS history ring, fills its first `0xFEE` bytes with `0xFE`, and decompresses original 8-bit control groups / 12-bit backreferences.
5. **`2EF38: C6 44 03 02 01` reclassifies the successful original file load as runtime cache type 1**, independently of the disk's EH6 kind. `2F29C` therefore legitimately accepts the runtime entry as **type 1** and returns the original decompressed pointer.
6. Lower textured path `1F3BE..1F3C3` assigns the returned pointer to `C040` and calls `34A74`; `34AAC` accesses `C040+2` (the exact original 4096 texels, skipping the original u16 length, **not invented padding**).

New independent `tools/verify_original_res4_eh6_dispatch.py` checks **nine** original instruction signatures, **seven** E8 rel32 targets, the EH6 dispatch-table DWORD, and corresponding native source/Test/CTest references. The exact byte/call/window audit passed locally against immutable `CODE.2`. See the same source original logical offsets relative to object-1 file base `0x3AF24`.

## Native source correction

- `ResArchive::decode_type6_block(index)` implements the original 37590 path from the original EH6 data: after original five-byte EH framing and any declared extension, read u16 decoded length, decompress via existing original-source-shaped `lzss_4k`, return **original two-byte length followed by original indexed bytes**. The original Type1 raster decoder is unchanged.
- `OriginalGroundRes4Audit::verified_tile_source` selects the EH6 loader for an EH6 original archive record; it still permits authentic EH1 records if a different source archive contains them. This retains the original runtime cache type-1 acceptance path without faking an EH1 disk header.
- `OriginalGroundRes4Audit::inspect` recognizes both actual EH6 candidate blocks and previously supported EH1 candidates. It preserves source EH type per candidate and distinct type6/error counts, only decoding EH6 entries declaring 4096 bytes. This yields 67 original-media candidate headers, and the **selected** ID is always resolved from original INIT+level arithmetic.
- The forensic probe retains archive/header/error metadata even on failure and now reports type6 counts and candidate EH type. The build-free Python oracle independently decodes the selected source texture and outputs its indexed SHA/FNV, without generating art or modifying game files.
- Extended `original_ground_res4_audit_tests` exercises synthetic source EH6 LZSS and optional extended header; valid EH1 remains supported. The original-CODE2 binary oracle is registered as `original_res4_eh6_runtime_binary_contract_tests`, raising the source CTest catalog to **214 with Python** (after the prior 213).

## Acceptance boundaries

Authorized [Windows Full #655](https://github.com/Hyperbog/RLOFT/actions/runs/38082247830) **passed 212/212 CTests**, but used checkout `80cd269bb60cc2bf73ba984cab5caacd2b461a61`, **before this EH6 correction**. Its existing native ground probe cannot prove new corrected output. No new Windows build is automatically started.

The original selector, 64×64 indexed tile and native decoder cause are now independently determined, **not** pending another INIT/RES4 upload. What remains: compile and run new 214-test HEAD under separately authorized Windows Full; verify real GOG native source `ground_original_res4_verified=1`, texture draw eligibility and exact indexed 34A74 output; then compare the actual DOS VGA framebuffer/palette, camera phases, sprite layering, input, RNG and tick state. The native runtime's guarded open-field 256-ray path may still fall back when dynamic 22220/FF50/C026 state differs. **Do not increase 49.8% subsystem readiness or claim complete Level22 parity solely from successful decoding.**

Original `INIT` and `RES4` binaries need no further reupload in this session; all committed source proof is small original instruction windows and SHA metadata only.
