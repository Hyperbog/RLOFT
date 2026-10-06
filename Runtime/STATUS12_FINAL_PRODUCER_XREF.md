# CODE.2 final direct status-12 producer: `0x23DEF..0x23E1A`

Status: **BINARY-PROVEN TERMINAL SEMANTICS; invocation ownership remains indirect/unresolved.**

This checkpoint audits the last direct CODE.2 `status 12` site without assigning an unsupported gameplay trigger.

## 1. Correct routine boundary

Raw object-1 bytes show the terminal routine begins at logical `0x23DEF` (the linear disassembler can lose alignment across the preceding padding):

```asm
23DEF  mov byte [BF38],0
23DF6  call 1F1C8
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

The routine immediately before it, `0x23D84`, returns at `0x23DE4`; bytes between are alignment/padding. The predicate beginning at `0x23E1C` is a separate routine.

## 2. Semantics of the body

Every application-visible action in this routine is now independently understood at the protocol level:

1. clear global presentation/state byte `BF38`;
2. perform gameplay/presentation teardown through the same cleanup family used by the five-artifact finale;
3. invoke numbered event `10` through `0x13EF8`;
4. event 10 selects `CINE10.ANM` or `CINE11.ANM` from party survival/provenance state;
5. request controlled process status `12` through `0x205B4`;
6. R.EXE responds to 12 by deleting `CHARS` and restarting the CODE cycle without transferred party state.

Therefore this routine itself is safely classified as a **shared finale terminal primitive**:

```text
RUN_EVENT10_FINALE_AND_RESTART_WITHOUT_CHARS
```

It is not an error exit and not a generic status-12 helper.

## 3. Xref result

A complete scan of the extracted CODE.2 object-1 bytes found **no direct relative `CALL` to `0x23DEF/0x23DF0`**. No raw absolute object-relative pointer to the entry was found either. Consequently its owner cannot be certified from ordinary direct-call xrefs.

This is a useful result rather than a reason to invent an owner: the routine is reached through an indirect/fixup-mediated dispatch path, or through an address representation reconstructed by the LE loader rather than present as a simple raw object-relative constant.

Nearby code supports the dispatcher/subsystem interpretation: `0x23D84` is a coordinate/render helper; `0x23E1C` is a party-state predicate; `0x23E58` classifies indexed entities. The terminal primitive sits among application dispatch helpers rather than in DOS/runtime termination code.

## 4. Relationship to the five-artifact finale

The five-artifact success path at `0x26BB9..0x26BDB` duplicates the same semantic tail inline:

```text
clear BF38
cleanup/teardown
13EF8(10)
205B4(12)
```

Thus `0x23DEF` and the five-artifact path are not two unrelated meanings of status 12. They are two code paths implementing the same **event-10 finale -> restart-without-CHARS** transition. What remains unresolved is the indirect trigger that invokes the standalone primitive.

## 5. Certified status-12 taxonomy

The direct producer taxonomy can now be expressed without conflating mechanism and cause:

```text
PARTY_WIPE
    -> terminal failure presentation
    -> status 12

FIVE-ARTIFACT QUEST FINALE
    -> event 10 (CINE10/CINE11)
    -> status 12

INDIRECT FINALE TRIGGER (owner pending)
    -> shared 0x23DEF finale primitive
    -> event 10 (CINE10/CINE11)
    -> status 12

status 12 protocol meaning in all cases:
    RESTART_CODE_CYCLE_WITHOUT_CHARS
```

The standalone producer's **terminal action is resolved**; only its upstream indirect dispatch owner remains to be named.

## 6. Preservation consequence

A native port should not preserve `0x23DEF` as a process-exit function. Model it as a gameplay transition action such as:

```cpp
Transition runFinaleAndRestart(FinaleTrigger trigger);
```

with event-10 cinematic selection remaining a separate deterministic operation. The unidentified upstream trigger should remain an explicit unresolved enum/value until LE fixup/dispatch ownership is certified.

## Next checkpoint

Parse CODE.2 LE fixup/relocation records around the `0x23DEF` entry and reconstruct indirect dispatch ownership. After that, consolidate all runtime results into `Runtime/STATE_TRANSITION_PROTOCOL.md`.