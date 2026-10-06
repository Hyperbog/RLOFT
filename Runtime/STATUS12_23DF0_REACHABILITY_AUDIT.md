# CODE.2 `0x23DF0` reachability audit

Status: **BINARY-PROVEN UNREFERENCED IN SHIPPED STATIC APPLICATION CONTROL FLOW.**

This checkpoint follows the LE-fixup audit and asks the narrower question: does any shipped CODE.2 application control-flow mechanism actually target the standalone finale routine at object-1 logical `0x23DF0`?

## 1. Target routine

The exact executable entry is `0x23DF0`:

```asm
23DF0  mov byte [BF38],0
23DF7  call 1F1C8
23DFC  call 1EA78
23E01  call 28168
23E06  push 0Ah
23E08  call 13EF8
23E0D  add esp,4
23E10  push 0Ch
23E12  call 205B4
23E17  add esp,4
23E1A  ret
```

Its semantics remain `event 10 finale -> controlled status 12`.

## 2. Direct control-flow scan

The complete extracted CODE.2 object-1 image was scanned for relative control transfers whose decoded destination is exactly `0x23DF0`.

Results:

- `CALL rel32` (`E8`): **0 targets**
- `JMP rel32` (`E9`): **0 targets**
- short `JMP rel8` (`EB`): **0 targets**
- short conditional branches (`70..7F`): **0 targets**
- linear disassembly contains no decoded direct branch/call to `0x23DF0`

Thus there is no ordinary static direct predecessor.

## 3. Application jump-table audit

CODE.2 object 1 contains numerous compiler-generated CS-relative indirect jump tables, including tables used by switch/dispatch sites at logical offsets such as `0x2FE`, `0x2E51`, `0x65B1`, `0x8392`, `0x8E50`, `0x93AB`, `0xB988`, `0xD69A`, `0xDCFE`, `0xE79B`, `0xF8CC`, `0x1145E`, `0x11CD4`, `0x121ED`, `0x13625`, `0x13D06`, `0x14B71`, `0x1556D`, `0x17C50`, `0x194AD`, `0x1ADB3`, `0x1BE11`, `0x1EFAC`, `0x1F88B`, `0x21B83`, `0x22A2B`, `0x22B34`, and `0x260ED`.

A raw aligned-dword scan initially finds bytes `F0 3D 02 00` at `0x8BE8`, but disassembly proves these bytes are the **rel32 displacement field** of:

```asm
8BE7  call 2C9DC
```

They are not a stored function pointer. No actual aligned table/data dword in the application image resolves to `0x00023DF0`.

Therefore the known static application dispatch tables do not target the routine.

## 4. Indirect-call audit

Within the application/gameplay portion surrounding `0x23xxx`, there is no general register-indirect `call eax/ebx/...` or memory callback call capable of supplying an arbitrary gameplay routine address. The indirect-call sites found later in object 1 belong to runtime/library callback machinery rather than this gameplay subsystem.

Combined with the absence of a stored `0x23DF0` pointer and the previous LE-fixup result (no relocation targeting `0x23DF0`), there is no binary evidence for a loader-created or statically stored callback to this routine.

## 5. Reachability classification

The cumulative evidence is now sufficient to change the classification from `INDIRECT FINALE TRIGGER (owner pending)` to:

```text
UNREFERENCED DUPLICATE FINALE ROUTINE
    0x23DF0
    -> event 10
    -> CINE10/CINE11
    -> status 12
```

This does **not** prove that no conceivable self-modifying/runtime-generated mechanism could jump to the address. It establishes the preservation-relevant fact: the shipped static CODE.2 application contains no direct call/jump, no recognized jump-table entry, no raw stored pointer, and no LE relocation targeting this routine.

The five-artifact quest-completion path at `0x26BB9..0x26BDB` independently inlines the same semantic sequence and is the certified live finale producer.

The most conservative reconstruction is therefore that `0x23DF0` is a compiled but unreferenced duplicate/helper left in the shipped binary.

## 6. Revised live status-12 taxonomy

The certified **live** gameplay causes are now:

```text
PARTY WIPE / NO ACTIVE PARTY
    -> terminal failure presentation
    -> status 12

FIVE-ARTIFACT QUEST COMPLETION
    -> event 10
    -> CINE10/CINE11
    -> status 12

status 12 supervisor meaning:
    RESTART_CODE_CYCLE_WITHOUT_CHARS
```

`0x23DF0` remains documented because it is real executable code and confirms the finale transition design, but it should not be counted as a third live gameplay trigger unless future dynamic evidence contradicts this static reachability audit.

## 7. Porting consequence

A faithful native port should preserve the two certified live causes as explicit state-machine transitions. It does not need to manufacture a gameplay trigger for `0x23DF0` merely because the original binary contains the routine.

For archival fidelity, retain `0x23DF0` in the binary provenance map as an **unreferenced duplicate finale helper**.

## Next checkpoint

Consolidate `R.EXE`, status 10, status 12, CHARS serialization, party-wipe restart, and finale restart into the canonical `Runtime/STATE_TRANSITION_PROTOCOL.md`, with unresolved claims separated from certified live transitions.