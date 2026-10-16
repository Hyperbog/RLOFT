# Multi-platform video game disassembly — first architecture checkpoint

**Development targets:** DOS (x86-16/32), NES (6502-derived), SMS/Game Gear
(Z80); extensible to SNES, Game Boy, Genesis and newer game hardware.
The product is ONE general-purpose game reverse-engineering workbench.
DOS is a regression platform, not the architecture. All console/CPU
backends beyond the current DOS/x86 implementation remain planned.

## External research reviewed in October 2026

| Repository | Useful model | License boundary |
| --- | --- | --- |
| [NESRecomp](https://github.com/mstan/nesrecomp) | 6502 cycle-oriented output, ROM-view dispatch, observed fallback targets | PolyForm Noncommercial 1.0.0 |
| [SMSGGRecomp](https://github.com/mstan/smsggrecomp) | Z80 ISA semantics, hardware separation, bank-aware dispatch and emulator conformance | No project-wide license declared in inspected README |
| [z80-recomp-core](https://github.com/mstan/z80-recomp-core) | Shared Z80 state/semantics and host interface | PolyForm Noncommercial 1.0.0 |
| [pcrecomp](https://github.com/sp00nznet/pcrecomp) | MIT-licensed DOS/4GW host, x86-16/32 lifting, code/data/function scoring | MIT |
| [M-HT/SR](https://github.com/M-HT/SR) | Static recompilation of DOS games | Verify specific files before reuse |
| [Spice86](https://github.com/OpenRakis/Spice86) | Executed-code traces, observed CFGs and C# code-generation workflow | Apache-2.0 |

**Use policy:** The owner confirms this is a noncommercial project. PolyForm
Noncommercial components can be evaluated and incorporated only in compliance
with their conditions, attribution and relevant third-party notices. A public
repository without a grant is not permission to copy its source. This checkpoint
adapts architecture patterns only; it copies no external source or game media.
If licensing scope changes, reconsider all imported components.

## Implemented now: GuestSourceMemory

`DosDis.Core/GuestSourceMemory.cs` maps original SHA-256-bound bytes into
named guest address spaces and explicit, immutable mapper/loader views.
Multiple views may expose different source bytes at identical CPU addresses.
Adjacent guest addresses may map discontiguous physical source intervals;
reads return original-source provenance for each fragment.

Mappings reject original-file overrun, guest-address overflow, overlapping
intervals inside one view, empty intervals and invalid IDs. Reads abstain
on unmapped gaps or missing views, and return separate copies of bytes.
Memory windows are sorted and searched via binary search. No ROM format,
architecture, game identity or executable header is hardcoded.

The memory contract is **not** currently an NES or SMS file parser, CPU
decoder, emulation core, dynamic bank-switch detector, RAM model or GUI view.
Volatile/self-modifying guest memory must be marked separately from immutable
file backing. A future mapper must select a view explicitly rather than guess.

Synthetic regression tests in `DosDis.Smoke` cover DOS segmented, NES
bank-0/bank-1 and SMS page windows; mixed-physical spans; differing ROM banks
at one guest address; immutable input; unmapped gaps; missing mappings;
cross-platform isolation; overlapping intervals and arithmetic failures.

## Architectural steps after this checkpoint

1. **Format/memory adapters:** iNES/NES 2.0 PRG banks and mapper models,
   SMS cartridge and Sega/Codemasters paging, and existing DOS MZ/LE views.
   Reject unknown mappers/ambiguous headers instead of guessing.
2. **CPU-independent decoded instruction contract:** typed operand semantics,
   effect/flow information, cycle/bus metadata and exact source locations.
   Wrap existing Iced x86; add independently tested 6502 and Z80 decoders.
3. **Code/data ownership:** shared graph with explicit confirmed, candidate,
   conflicting and unknown states; bank-sensitive instructions, targets
   and symbols; function entry recovery and indirect branch provenance.
4. **Reassembly/recompilation:** labeled assembly export, reversible import,
   semantic IR and backend-specific C/C++ generation, with bounded interpreter
   fallback for incomplete indirect flow. Mark outputs experimental until
   round-trip and runtime differential gates pass.
5. **Independent correctness:** original SHA-pinned external decoder/oracle
   comparisons for each ISA, instruction starts/lengths, mapper changes,
   unknown data, flags, IRQ/NMI/interrupts, timing and runtime state.
   Retain every discrepancy and unknown denominator.

**Verification status:** source authored; Windows compilation and synthetic
tests must be confirmed on the exact resulting commit. This document does
not imply working NES/SMS disassembly or completed game recompilation.
