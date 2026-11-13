# Cross-game CODE.2 global-state and shared RES initialization — Phase 5

## Boundaries and original evidence

All offsets are logical offsets into the authenticated original CODE.2 LE
object 1. Menzo original SHA-256: `4a4fa725dddea00cb62ff44f44c48999cf6ed4c899b16c21826e8141ec7fda96`.
Stone original SHA-256: `00e225ec8f141a8fdb72ff0547fe93b2ea3aabdb4ec3b59d9261a5268de9203e`.

| Operation | Menzoberranzan CODE.2 | Stone Prophet CODE.2 |
|---|---|---|
| Original environment gate joins | `23B7D` | `2DD95` |
| Clears | 23 x `mov dword ptr [absolute],0` | 23 x `mov dword ptr [absolute],0` |
| Signed 16-bit sentinels | `8FC4=8000`, `8FC0=8000` | `16ECC=8000`, `16EC8=8000` |
| Mode byte | `F57C=1` | `1DC40=1` |
| Invalid handle | `8FBC=FFFFFFFF` | `16EC4=FFFFFFFF` |
| Extra global | `15468=1` | `24E3C=77` |
| Parameter fields | `ESI -> 8FC6` (word); `EBX -> 8FC2` (word); `EDI -> 8FC8` (dword) | `ESI -> 16ECE` (word); `EBX -> 16ECA` (word); local -> `16ED0` (dword) |
| Resource call | `23934(local)` | `2D45C(EDI)` |
| Derived fields after call | `19454=[3A660]`; `19450=[3A660]+[3A658]` | `281BC=[4812C]`; `281B8=[4812C]+[4815C]` |
| Next game setup | `23500()` | `2D010()` |
| Termination helper call | `23A94(-1)` | `2D70C(-1)` |
| First address after wrapper return | `23CD8` | `2DEF0` |

## Reconstructed C++ behavior

`original_code2_global_state.hpp/.cpp` translates this entire **bounded
post-environment wrapper** into original-width memory writes and ordered calls.
Original callees are still supplied by `OriginalResourceStartupPort`, since
resource setup, gameplay initialization, and process termination remain
separate, large machine-code functions. No C++ callback is represented as a
completed translation of the called function. No source semantics are inferred
for the numeric global-field addresses.

`verify_code2_global_state_witness` authenticates every original zeroing
store, sentinel, constant and resource/setup/termination call edge against the
original CODE.2 code object. The caller must additionally authenticate the
whole retail file by SHA-256. Independent direct audits of the original
GitHub binary bytes confirmed 33/33 Menzo and 32/32 Stone opcode witnesses.

## Shared RES loading

`load_shared_original_res` is a game-neutral read-only file access boundary.
It names the requested archive `RES0`, `RES1`, ... `RES12`, returns the original
bytes **unchanged**, and validates the index/`EH` record prefixes of `RES0`
using the existing phase-4 preflight. The pinned original `RES0` entry counts
are Strahd 141, Menzo 175 and Stone 174. Subsequent indexed extraction and
texture/audio decoding use the existing `ResArchive` implementation rather
than a second competing decoder. The real Strahd player obtains `RES0` and
`RES1` through this same function, retaining its title-specific graphics and
world-data interpretation. Menzo and Stone can use an independently supplied
read-only media callback without being misrouted through Strahd's
`GameSource::open` (which still deliberately rejects those titles).

This is NOT media-compatibility certification for every RES entry subtype, a
complete Menzo/Stone executable decompilation, or a playable campaign.

## Verification

- GCC C++17 and Clang C++17 with `-Wall -Wextra -Werror -pedantic`: PASS.
- State field widths, overflow, call sequence, title-specific values: PASS.
- Synthetic EH-index and wrong-title counts, out-of-range archives: PASS.
- Real Strahd `RES0`: 2,186,991 bytes / 141 EH-prefixed entries: PASS.
- Menzo/Stone original CODE.2 source-byte witnesses: 65/65 PASS.
- Windows complete native player compile and live Menzo/Stone runs: NOT CHECKED.

## Next bounded step

Reconstruct the actual resource calls at Menzo `23934` / Stone `2D45C`
through their INIT/RES and CHARS-handling branch edges, validate resource
subtype compatibility, then carry title-neutral services into the one native
player while keeping game-specific rules separate.
