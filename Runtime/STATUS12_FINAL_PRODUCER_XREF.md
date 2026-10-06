# CODE.2 final direct status-12 producer: `0x23DF0..0x23E1A`

Status: **BINARY-PROVEN TERMINAL SEMANTICS; ordinary xrefs and LE fixups do not identify an invocation owner.**

This checkpoint audits the last direct CODE.2 `status 12` site without assigning an unsupported gameplay trigger.

## 1. Corrected routine boundary

A byte-level recheck corrects the previous boundary by one byte. Object-relative `0x23DEF` is a zero padding byte. The first instruction begins at `0x23DF0`:

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

The routine immediately before it, `0x23D84`, returns at `0x23DE4`; `0x23DE5..0x23DEF` are padding/alignment bytes. The predicate at `0x23E1C` is separate.

## 2. Semantics of the body

The body remains fully classified:

1. clear global presentation/state byte `BF38`;
2. perform the same cleanup/presentation teardown family used by the five-artifact finale;
3. invoke numbered event 10 through `0x13EF8`;
4. event 10 selects `CINE10.ANM` or `CINE11.ANM` from party survival/provenance state;
5. request controlled status 12 through `0x205B4`;
6. R.EXE deletes `CHARS` and restarts the CODE cycle without transferred party state.

Safe semantic name:

```text
RUN_EVENT10_FINALE_AND_RESTART_WITHOUT_CHARS
```

## 3. Direct-xref result

A complete scan of extracted CODE.2 object 1 finds no direct relative `CALL` or `JMP` to `0x23DF0`, and no simple raw object-relative function pointer to it. The apparent raw `F0 3D 02 00` occurrence at file `0x43B0C` is the displacement operand of an unrelated `call 0x2C9DC` at object-relative `0x8BE7`, not a pointer to this routine.

## 4. LE fixup audit

The CODE.2 embedded LE header is at file `0x27BDC`. Its fixup-page table is header-relative `0x235`; its fixup-record table is header-relative `0x371`. The page table begins with record offsets `0, 1234, 2303, 3620, ...`, confirming a populated page-indexed relocation stream.

Internal fixup records use the expected LE form visible directly in the bytes. For example the record stream begins with records such as:

```text
07 10 A7 0B 02 E0 98 01 00
07 00 82 0B 02 60 17
```

where source type `07` is a 32-bit offset relocation and flag `10h` selects a 32-bit target offset.

Searching the complete fixup-record extent finds **no target encoding for object-1 offset `0x23DF0`** (neither the required 32-bit target `F0 3D 02 00` nor a valid shorter target representation). Therefore the previous hypothesis that LE relocation records would directly reveal an owner is rejected.

This is negative but important evidence: `0x23DF0` is not exposed as an ordinary relocated callback pointer by the loader's fixup stream.

## 5. Relationship to the five-artifact finale

The five-artifact success path at `0x26BB9..0x26BDB` duplicates the same semantic tail inline:

```text
clear BF38
cleanup/teardown
13EF8(10)
205B4(12)
```

Thus the standalone primitive and five-artifact path implement the same event-10-finale -> restart-without-CHARS operation. The standalone routine's upstream reachability remains unresolved; dead/unreferenced code is now also a legitimate possibility and must be tested rather than assuming indirect dispatch.

## 6. Certified status-12 taxonomy

```text
PARTY_WIPE
    -> terminal failure presentation
    -> status 12

FIVE-ARTIFACT QUEST FINALE
    -> event 10 (CINE10/CINE11)
    -> status 12

STANDALONE FINALE PRIMITIVE 0x23DF0
    -> event 10 (CINE10/CINE11)
    -> status 12
    -> upstream reachability unresolved

status 12 protocol meaning:
    RESTART_CODE_CYCLE_WITHOUT_CHARS
```

## 7. Preservation consequence

Do not invent an indirect owner for `0x23DF0`. Preserve its body as a certified semantic routine, but mark its reachability as unresolved until dispatch-table construction, runtime writes, or dead-code evidence settles it.

## Next checkpoint

Audit runtime-built dispatch tables and all writes of executable object addresses around the `0x23xxx` subsystem. If no construction path can produce `0x23DF0`, classify the standalone primitive as unreferenced/dead duplicate code. Then consolidate the certified runtime state machine in `Runtime/STATE_TRANSITION_PROTOCOL.md`.