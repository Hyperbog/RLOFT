# Ravenloft Runtime / Executable Reconstruction

Status: **AUDITED — core runtime protocol certified; optional installer/hardware-driver work deferred.**

This tree preserves executable-side reconstruction separately from `Code1`, `Code2`, `DatFiles`, and `Resources`.

## Evidence policy

Shipped Ravenloft bytes are controlling evidence. Platform/toolchain documentation explains mechanisms only after those mechanisms are observed in the binaries. Historical checkpoint documents record the investigation sequence; where a checkpoint conflicts with a later corrected/canonical document, the later corrected/canonical document controls.

## Canonical current documents

- `STATE_TRANSITION_PROTOCOL.md` — canonical R.EXE / CODE.1 / CODE.2 / CHARS state machine.
- `CODE_OBJECT_MAP.md` — corrected application object/file mappings.
- `CHARS_XREF_RECONSTRUCTION.md` — producer/consumer/existence-gate proof.
- `CODE1_STATUS10_PROPAGATION.md` — CODE.1 status-10 handoff proof.
- `CODE2_STATUS12_OWNERSHIP.md` — corrected live status-12 taxonomy.
- `STATUS12_PARTY_WIPE_PRODUCER.md` — party-wipe/no-active-party cause.
- `STATUS12_23DF0_REACHABILITY_AUDIT.md` and corrected `STATUS12_FINAL_PRODUCER_XREF.md` — unreferenced duplicate finale routine.
- `CINE10_CINE11_VARIANT_PREDICATE.md` — corrected unsigned predicate and record offsets.
- `CHARACTER_STATUS_4000_DEATH_STATE.md`, `CHARACTER_STATUS_0080_8000_OWNERSHIP.md`, `FINALE_CHARACTER_CLASSIFICATION.md` — certified status semantics.
- `AUDIO_INTERFACE_DEPENDENCY_AUDIT.md` — CODE-side 59-byte `SOUND.CFG` contract and driver boundary.
- `OPTIONAL_EXECUTABLE_DRIVER_TRIAGE.md` — installer/audio-driver deferral decision.

## Certified runtime architecture

`R.EXE` is a small 16-bit Borland DOS launcher/supervisor, not the protected-mode game engine and not an embedded DOS/4GW protected-mode payload.

Its shipped binary is:

- size `12,368` bytes (`0x3050`);
- SHA-256 `a6d5874dc91fb068a3ed0a07290547c310ee7f4f50be4d8176c4788f251631b0`;
- MZ image size exactly equals file size;
- contains the DOS/4GW 1.96 banner/environment strings used while launching the protected-mode CODE children.

Certified control flow:

```text
RLOFT.BAT
    -> R.EXE
         -> create WORK / AUTOMAPS
         -> launch CODE.2
         -> if CODE.2 returns 10: launch CODE.1
         -> if CODE.2 returns 12: delete CHARS, normalize to 10
         -> while result == 10: repeat
         -> final CHARS cleanup
```

Canonical protocol meanings:

```text
status 10 = CONTINUE_MODULE_CYCLE
status 12 = RESTART_CODE_CYCLE_WITHOUT_CHARS
```

## CHARS contract

`CHARS` is a transient cross-process party-state image:

```text
4 records * 0x248 bytes = 0x920 bytes
```

- CODE.1 produces/writes it before the normal status-10 handoff.
- CODE.2 tests for it, reads exactly `0x920` bytes into active-party storage at runtime `0x218E0`, then performs post-load processing.
- R.EXE deletes it on status 12 and final shutdown.

It is not an ordinary persistent save file.

## Status-12 causes

Two live gameplay causes are certified:

1. **Party wipe / no active party** -> terminal presentation -> status 12.
2. **Five-artifact quest completion** -> event 10 -> `CINE10.ANM` or `CINE11.ANM` -> status 12.

The standalone finale routine at CODE.2 `0x23DF0` is real executable code but has no certified incoming static application edge and is preserved as an **unreferenced duplicate finale helper**, not a third live trigger.

## Finale character predicate

Active-party base/stride:

```text
base          0x218E0
record stride 0x248
status word   +0x10 / runtime 0x218F0
current HP    +0x1C / runtime 0x218FC
```

Event 10 uses:

```c
status < 0x4000u && (status & 0x0080u) == 0
```

Certified meanings:

```text
0x0080  recruited/template-character provenance
0x4000  zero-HP death state; revival clears it
0x8000  empty/unoccupied party slot
```

Thus `CINE10` requires at least one living original/non-recruited party character; otherwise `CINE11` is selected.

## Runtime scripts/configuration

- `MAKSAV.BAT` creates `SAVE01`, copies `LEV022.DAT` and `SAVE.DAT`, then deletes the working copies.
- `README.BAT` displays `README.TXT` through `MORE`.
- `PATCHES.INI` is the UltraSound LOADPATS patch-selection table for AIL 32.
- `RLOFT.BAT` and `SOUND.BAT` test Gravis-related environment state and run `R.EXE` / `SINSTALL.EXE`.

Important audit note: although both batch files contain comments describing a LOADPATS failure/revision test, the supplied scripts **do not actually invoke `LOADPATS.EXE`** before testing `ERRORLEVEL`. The exact historical LOADPATS activation path remains optional archival work.

## Audio boundary

Both CODE modules are directly proven to open `sound.cfg` and transfer exactly `0x3B` bytes into their audio configuration blocks.

The supplied `SOUND.CFG` sample is 59 bytes and contains `a32mt32.dll` at `+0x16` and `a32sbpdg.dll` at `+0x24`. CODE then consumes two distinct numeric/driver configuration groups matching Music versus Digital Sound Effects.

The 18 A32/GUS DLLs are third-party hardware/middleware backends. No evidence currently places missing Ravenloft gameplay logic inside them.

## Current reverse-engineering priority

```text
DONE / CORE
    R.EXE supervisor structure
    CODE handoff protocol
    CHARS transfer contract
    status 10 / status 12 semantics
    live status-12 causes
    finale variant predicate

REQUIRED FOR FAITHFUL AUDIO, BOUNDED
    CODE-side audio API behavior
    remaining SOUND.CFG numeric-field semantics
    game-visible audio timing/status behavior if any

DEFERRED / END-OF-PROJECT
    full SINSTALL.EXE decompilation
    full INSTALL.EXE decompilation
    LOADPATS exact activation/patch-loading behavior
    A32/GUS driver internals
    exact original ISA/DMA/IRQ hardware implementation
```

The core runtime/executable reconstruction is no longer a Phase-1 inventory task; it is architecturally closed unless contradictory shipped-byte evidence appears.
