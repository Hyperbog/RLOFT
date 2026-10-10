# Original F790 authentic DATA and T resource closure — 2026-10-10

**Authority:** original shipped CODE.2 and OBJECTS.DAT bytes, plus original T file in the repository's DnD-Classics branch. **Status:** source-verified standalone resources, NOT yet wired to the Windows game.

## LE page-mapping discovery

Original CODE.2 (558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`) contains an LE header at file offset `0x27BDC`. Object 1 has 59 physical 4-KiB pages starting at `0x3AF24`. Its logical object length is only `0x3A852`, but the last disk page is padded. Consequently original object 2 begins at `0x3AF24+59*0x1000 = 0x75F24`, **not** at the simple concatenation offset `0x75776`. Object 2's initialized disk data spans `0x1273D` bytes; its larger declared virtual size includes zero-initialized RAM. The C++ static-data reader checks this object page-map and the original loaded literals, while the optional offline verifier checks the complete CODE.2 SHA-256.

## Exact CODE.2 data-segment literals

| Source | Exact text |
| --- | --- |
| DS `0x610` | `" +"` |
| DS `0x614` | `"; "` |
| DS `0x618` | `" "` |
| DS `0x61C` | `"-"` |
| DS `0x620` | `"/"` |
| DS `0x624` | `".5"` |
| DS pointer at `0x6D98` → `0x598` | `"; CHARGES: "` |

The original `0x6D98` operand is a DWORD **pointer to a string**; the static initial pointer is `0x598`, rather than a string at `0x6D98`.

## Original T file and text decryption

The unchanged original `MASTER/Ravenloft - Strahd's Possession/RLOFT/T` file (DnD-Classics branch) is 133,584 bytes and has Git blob SHA-1 `68cde745e55de1def1b6ce828f1455d38c6f483b`. Original CODE.2 routine `1C368` reads the DWORD record offset at `4+index*4`, the two-byte record length, then XOR-decodes all record bytes with `0xDC`. The source has **1,073 indexed records**. Independently decoding original repository bytes yielded:

| T index | Original message fragment |
| --- | --- |
| `0x3D` | `WEIGHT:` |
| `0x3E` | `DAMAGE:` |
| `0x3F` | `D` |
| `0x40` | `A.C. BONUS:` |
| `0x41` | `CAPACITY:` |

`OriginalTTextArchive` is a standalone implementation of the encoded T reader. Its CTest uses synthetic XOR-encoded records, not a bundled copyright file. The optional `--t` verifier compares the Git blob identity and authentic decoded labels when T is present locally.

## Mutable container descriptors, F5F8 and 336 original objects

The original data at `0x7330` is an array of **ten-byte container descriptors**: count at +0, original F790 suffix byte at +2, and an unaligned DWORD item-list pointer at +5. Source `F5F8` visits each signed-WORD item slot, skips `-1`, sums record byte +`0x30`, and recursively visits category-1 items through signed subtype byte +`0x31`. **Subtype `0x14` (20) is an explicit special case** returning object ID 80's weight byte, not ordinary recursion. The original data also supplies suffix at `0x7332+10*subtype` (including `0x7F` sentinel) and byte lookup at `0x67EC+subtype`.

The immutable original OBJECTS.DAT has **336 records**, SHA-256 `f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0`. Source baseline nested half-unit totals independently verified for subtypes `0:0, 1:5, 2:8, 9:4, 20:0`. These are **initial data-image values only**. They must NOT override mutable inventory/container contents while gameplay is running. Invalid/cyclic synthetic data is rejected by the offline model; the original's behavior for corrupted cyclic runtime state is not claimed.

## Standalone resolved-description composition

`OriginalF790ResolvedDescription` joins the existing F790 category/field-arithmetic model with **authentic parsed CODE.2 string references** and an **authentic decoded T-archive interface**, while accepting a callback for the current mutable nested weight. Its synthetic-resource regression passed strict C++17 compilation and execution for sample armor bonuses, weapon ranges, weight halves, known-item charges and capacity, with source-style strings such as `POUCH; WEIGHT:1; CAPACITY: 8`. These are synthetic-fixture test outcomes, **not DOS screenshots or native runtime validation**.

## New user-provided disassembler reports

`Code2.json`: 66,321 candidate instructions, 833 function candidates, 31 jump tables. `Code1.json`: 17,923 candidate instructions, 360 function candidates, five jump tables. Both source hashes match the immutable CODE.2/CODE.1, but both explicitly declare `analysisStatus=candidate-unverified` and `instructionBytesIncluded=false`. They were used only for navigation and source-address cross-checks. Binary bytes remain authoritative; unresolved indirect flows are not silently treated as complete.

## Verification and remaining live gates

This checkpoint adds four registered tests: C++ static CODE.2 DS/container parsing; C++ original T decoder; source/binary/JSON provenance Python guard; C++ combined authentic-resource description resolver. **198 total registered CTests with Python**, previously 194. Strict local C++17 tests passed on synthetic fixtures; real original CODE.2/OBJECTS.DAT static parsing and weight totals passed; candidate JSON provenance and CODE.2 binary hashes passed. Original T records were decoded via the connected GitHub original-file branch; the optional full local-T verifier awaits that binary being locally accessible.

**Last full Windows build remains #651 (182/182 passed on older source).** These source additions have NOT been Windows built or visually compared to original DOS. Remaining blockers: map all 17 original F790 invocation sites onto actual native inventory interactions; preserve live mutable container weights; route one existing ExplorationRuntime RNG through exact original event cadence and post-draw IRQ reseeds; and verify native-vs-DOS pixels. Do not modify live gameplay, invent modern artwork, advance Level23 independently, or increase parity/readiness scores without proof. Manual builds require user authorization.
