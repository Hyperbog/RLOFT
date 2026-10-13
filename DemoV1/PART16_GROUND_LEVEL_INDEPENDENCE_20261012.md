# Part 16 — CODE.2 original ground across all authored levels

**Owner:** `demo-v1` at verified project HEAD. Original full DOS game
remains the sole fidelity authority.

## The source defect

The native `original_full_ground_eligible` had a host-invented
`source_level_id != 22` rejection. Original CODE.2 does NOT restrict
the ground drawing to Level 22: original `1F388` uses a signed
selector read from `2561F` (the CURRENT level header), plus the
RES4 bank base established from `INIT+0x0E`. A Level 0 or Level 31
view is not categorically different from Level 22; only its actual
original world cells, ray depths and RES4 tile matter.

Original immutable CODE.2 length 558689, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Verified exact x86 windows at source logical `1F388`
(`0F BF 05 1F 56 02 00`), `1F2A5` (FF50=128),
`335C8` (22220=0x800), `34A74` (ENTER/PUSHAD).
LE code physical offset is `0x3AF24+logical`. Original
code is private and never included in the repository.

## Bounded correction

Remove **only** the Level22-ID conditional. Keep the strict original
full-128-row branch limited to authenticated RES4 type-1 64x64 terrain
(4098-byte source), initialized perspective, exactly 256 complete
original ray records, `resource_inner==0x93` for every main ray,
and `resource_outer==0x93` for any active prepass. Everything
else still falls back explicitly; no speculative FF50 or depth is
introduced.

Existing `exploration_presenter_tests.cpp` now activates its assert
checks in Release and tests this gate on original level IDs
0,21,22,31. Additional assertions reject a drawable ray and
missing source on level 21. Source-only Python CTest rejects a
reintroduced Level22 hardcode; optional `--original` validates
all original byte windows under pinned full-file identity.

**Unaccepted:** arbitrary wall-depth ground spans, player-visible
original-DOS framebuffer match, the other levels' authored route
transitions, full input/IRQ, and end-to-end original-game completion.
Windows CTest alone is not a DOS acceptance claim.
