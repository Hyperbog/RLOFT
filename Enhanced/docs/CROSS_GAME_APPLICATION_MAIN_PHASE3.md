# Cross-game application entry reconstruction — phase 3

**Scope:** The first application-owned CODE.1 entry functions and initial CODE.2
startup environment blocks of two different DreamForge titles. This does **not**
claim full game source or a playable Menzo/Stone port.

## Source authenticity

The original six Menzo/Stone game executables were previously SHA-256 authenticated
against `DnD-Classics` inventories. In this phase the four CODE original binaries
were fetched again, and 54 pinned source-byte/call-destination/text checks passed
against those GitHub bytes: 10 Menzo CODE.1, 18 Menzo CODE.2, 8 Stone CODE.1,
and 18 Stone CODE.2. SHA-256 validation against the original inventories is
separate from the bounded source-span checks. Strahd CODE.1 and CODE.2 were
also checked locally with their SHA-256-confirmed originals.

All offsets below are **LE code-object-relative**, not raw file offsets.

| Title | CODE.1 CRT call/main | CODE.2 CRT call/main |
|---|---|---|
| Strahd | `0xFCD9 -> 0x508C` | `0x395CC -> 0x20608` |
| Menzo | `0xFA2C -> 0x127C` | `0x41E36 -> 0x23AEC` |
| Stone | `0x14D71 -> 0x6C58` | `0x53317 -> 0x2DD04` |

### Complete CODE.1 wrapper instructions translated

- Menzo main `0x127C`: calls `0x1044`, `0x0C24`, `0x4280`, `0x6E1C`,
  `0x3D54`, `0x1198` in order, then `push 10; jmp 0xCC8E`. The C++ boundary
  reports this tail exit without synthesizing the unconverted functions.
- Stone main `0x6C58`: calls `0x680C`, calls `0x6134`, pushes that call's
  `EAX` result, invokes `0x6BB0` with that argument, and returns its result.
  This is a distinct control flow, not a relabeling of Menzo's wrapper.

### CODE.2 environment block translated

- Menzo app main `0x23AEC`: environment lookup helper `0x3CA41`, decimal
  conversion helper `0x3CA96`, join `0x23B7D`.
- Stone app main `0x2DD04`: environment lookup helper `0x4A920`, decimal
  conversion helper `0x4A97B`, join `0x2DD95`.
- Both test the `ALSIU` environment lookup for null. When present they look
  up `ALSIU` a second time, then `KHSSDKJ`, `CANLW`, `SEPVON`, converting each
  to an integer. When absent, the bounded prefix retains the defaults.
- Menzo registers/local before the gate: `EBX=4, ESI=6, EDI=180, local=1`.
  Converted results are assigned `local, ESI, EBX, EDI` respectively.
- Stone registers/local before the gate: `EBX=27, ESI=232, EDI=41, local=45`.
  Converted results are assigned `EDI, ESI, EBX, local` respectively.
- The game-facing meaning of those names/values is *not yet proven*. Their
  original CRT getenv/atoi use and register assignments are verified.

## What the source performs

`original_application_main.cpp/.hpp` exposes typed callbacks so callers can
replay the verified order of application function calls without claiming those
callees have been decompiled. It also translates the original environment
branch using a host-supplied read-only environment service. Its optional
source validation checks original CRT `CALL rel32` targets, each application
call edge, branch signature, default constants, and the actual strings in the
CODE.2 data object. It rejects altered source-byte witnesses.

**This is a bounded application-function translation**, not an image index,
not complete CODE.1/2 decompilation and not a new separate game engine.
Strahd's already reconstructed source remains authoritative for Strahd.

## Tests / build

C++17 GCC and Clang compile with warnings as errors. Run a test using owned
originals: `original_application_main_tests menzo CODE.1 CODE.2` or
`original_application_main_tests stone-prophet CODE.1 CODE.2`.
Ordinary CTest runs require no retail original media. The native Windows
build and user-facing Menzo/Stone gameplay still require verification.

## Next bounded step

Trace the initial application subroutine calls: Menzo CODE.1 `0x1044`,
Stone CODE.1 `0x680C` / `0x6134` and the post-environment CODE.2 startup
continuations, then translate only independently verified behavior.
