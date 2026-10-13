# Part 16 — Original C050 ground draw page vs EF4D selected page

**Branch:** demo-v1. **Scope:** initial original video-page
provenance in normal Level22 frame reports; live rendering unchanged.

## Original executable evidence

Owner original CODE.2 is 558689 bytes; SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

- Initial CODE.2 LE object-2 **DWORD[C050] = 1** (bytes
  `01 00 00 00`, physical `0x75F24+0xC050`).
- Initial **BYTE[EF4D] = 0** (physical `0x75F24+0xEF4D`).
- Original code `1F1D6` computes
  `DWORD[FDBC] = 0xA0000 + (DWORD[C050] << 15)`;
  initial image therefore yields `0xA8000`, not `0xA0000`.
- Original `291B0` selects pages through
  `29FEC→2AB6C` and stores **EF4D** independently.
  The game can mutate C050 later, so the binary's static
  value is **not a live original DOS draw-page observation**.

The original full SHA, image variables and byte window at
`0x3AF24+0x1F1D6` are checked by the new optional private binary
oracle. No proprietary data is committed to public CI.

## Change and acceptance boundary

`Type7VgaPlaneMaskModel` now owns separate immutable
initial C050 and EF4D fields, validates full C050 DWORD
is in [0,1], and projects original initial FDBC. Existing EF4D
frame selection remains unchanged; no source-unsupported
gameplay page toggle was introduced.

`ExplorationRuntimeFrame` now conveys initial C050 provenance
as an optional value separately from the native mutable EF4D page.
Captured native `frame_XXXX.tsv` declares
`scene_page_source=EF4D-native-selected-not-C050` and
`original_live_C050_ground_draw_page=unverified`.
Missing CODE.2 emits `unavailable` for initial C050.

The planar mask C++ regression has Release assertions enabled,
covers C050=1 and EF4D=0 together, computes FDBC=0xA8000,
checks the other bank and rejects malformed C050 DWORD.
The native frame-capture C++ regression checks the distinct
metadata. Source-only CTest
`original_ground_c050_ef4d_domain_contract_tests` checks
the endpoints and rejects a deliberate erroneous alias mutation.

**Still open for Alpha:** reproduce real original DOS C050
mutations/page selection at the first user-visible Level22
frame, verify ES/VGA aliasing, and compare 320x400 indexed
pixels+palette without inferring dynamic ground span behavior.
This commit is not a DOS acceptance claim.
