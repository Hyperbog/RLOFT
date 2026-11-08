# R04 — original DOS/4GW memory-oracle capture protocol

**Purpose:** obtain *independent observed* DOS/4GW memory and selector
receipts to compare with the C++ canonical CODE.1/CODE.2 host runtime.
**Status: capture NOT performed.** Nothing here counts as an original-DOS
observation or satisfies R04 until an owner-original executable has
actually run and the evidence has been independently reviewed.

**Scope:** memory and selector services only. Do not expand this capture
into R05 filesystem, R06 character creation, or R09 gameplay acceptance.

## Trusted target, isolation and version record

Use a **local private installation** of the owner's original game,
including its original executable/launcher and supporting files.
Preserve the originals and record the exact versions. The two reference
LE payloads used by the current canonical C++ reconstruction are:

| Original payload | Bytes | SHA-256 |
| --- | ---: | --- |
| CODE.1 | 277787 | `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed` |
| CODE.2 | 558689 | `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314` |

If the hashes differ, **stop** and classify a separate version; do not
relax the pinned host oracle or silently compare incompatible patches.
The C++ `original_media_identity.hpp` now enforces these identities
when the original binaries are supplied to the startup programs.

Run under a debugger-enabled **DOSBox-X** on a private workstation.
Record the exact DOSBox-X build and commit, guest DOS version/config,
loaded extender, CPU model, memory configuration, and game patch
version. The official debugger documentation is at
https://github.com/joncampbell123/dosbox-x/blob/master/README.debugger .

This is a *repeatable suggested capture procedure*, not a claim that
DOSBox-X has already been installed or that every extender path is
supported by every build. Confirm live commands with `HELP`. If a
protected-mode entry or interrupt cannot be inspected, log it as
**not observed**, never infer a successful return.

## Acquisition

Start the **original game launcher**, not `CODE.1`/`CODE.2` as
independent MZ applications. A debugger-enabled DOSBox-X build
supports `DEBUGBOX` for launch/break (or its live debugger key).
The official debugger lists `BPINT` (interrupt and optional AH/AL
match), `BPM` (protected-mode code offset), `BPLM` (linear
breakpoint), `SELINFO`, `LDT`, `DOS MCBS`, `EV` and
`LOG`. Check the exact command syntax for the installed build.

Suggested **observation breakpoints** (all hexadecimal, applied
individually; these are example commands, not an unattended script):

```text
BPINT 21 30
BPINT 21 48
BPINT 21 49
BPINT 21 4A
BPINT 21 FF
BPINT 31 00 06
BPINT 31 01 00
BPINT 31 01 01
BPINT 31 05 01
BPINT 31 05 02
BPINT 31 05 03
```

The DPMI AX numbers are shown as **AH AL pairs** (`05 01` for
AX=`0501h`). The `BPINT 21 FF` hit must be filtered to
AX=`FF00h` and its actual extender response. Capture both **call**
and **completed return**, including carry; a breakpoint at the
interrupt entry alone does not establish a return or allocation.
Protected-mode DPMI services might be reached via an extender
dispatcher rather than a trap visible to `BPINT`; step from the
authenticated source call site if the interrupt breakpoint misses.

For each relevant event collect:

1. Executable identity (CODE.1 or CODE.2), runtime mode
   (0/1/2/9), caller CS:EIP or mapped original object offset.
2. Incoming EAX/EBX/ECX/EDX/ESI/EDI, DS/ES, EFLAGS CF, and
   PSP data-break value where relevant.
3. Outgoing registers and CF, including **failure** receipts, and
   whether the original code actually retries or falls back.
4. Selector lifetime: numeric identity, current base and inclusive
   limit/accessible length, selector ownership and invalidation
   after release. `SELINFO`/`LDT` can provide supporting output.
5. Allocation lifetime: requested and committed bytes/pages/
   paragraphs; previous and returned break or linear base; DPMI
   opaque handle (SI:DI); DOS segment/selector (AX/DX). Record
   DPMI `0503h` moved addresses and invalidation of the old handle.
6. Original heap metadata checks for CODE.1 `10AA9/109C1/1083F/
   107CB` and CODE.2 `39D4D/39E1C/39A8A/39A2C`,
   using **numeric pointers and counts**, without disclosing
   program code or original resource contents.

In particular distinguish **DOS AH=4Ah paragraphs** from the Phar
Lap extended DS data-break **AH=4Ah 4096-byte page** convention;
these are different contracts despite sharing the interrupt number.
Likewise a DPMI `0501h` linear address is not a selector, and a
`0100h` DOS selector is not necessarily the real-mode segment.

## Capture file contract

Store a private, append-only CSV or JSONL with a unique event ID and
matching before/after entry IDs. Prefer numeric scalar fields:
`media_sha256`, `emulator_build`, `caller_object_offset`,
`interrupt`, `function`, `mode`, `phase`, `in_registers`,
`out_registers`, `carry`, `selector`, `base`, `limit`,
`handle`, `bytes_requested`, `bytes_committed`, `owner`,
`failure_code`, and `confidence`.

Use `confidence=observed_original` **only** for an actual debugger
capture; use `host_reconstruction` for C++ traces. Never backfill
missing original outcomes from the host provider or from source CTests.
Retain the raw local debugger log as supporting evidence privately,
but do not commit proprietary memory dumps, launcher binaries or
game data to the public repository.

## R04 comparison and acceptance

Compare one event at a time, separately for:

- CODE.1 mode 1 DPMI 0501/0502/0503 and DOS 48/49 fallback;
- CODE.1 mode 2 native extender data break;
- CODE.2 0501/0502/0503 DPMI heap and original DOS fallback;
- CODE.2 mode 2 `39E1C` break growth;
- shared DPMI `0006/0100/0101` selector lifetimes;
- OOM, double free, stale selector, relocation and resize failure.

Different host allocations/selector identities may be acceptable
**only if** source-visible semantics match and the mapping is
documented. Real discrepancies need C++ source-service corrections
and focused retesting, not a weaker assertion.

**At the time of this document, there is no captured original-DOS
oracle in this working environment. R04 remains unaccepted and the
whole-game milestone count remains 3/12 = 25.00%.**
