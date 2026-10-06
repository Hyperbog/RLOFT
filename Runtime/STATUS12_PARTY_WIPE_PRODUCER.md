# CODE.2 status-12 party-wipe producer

Status: **BINARY-PROVEN CONTROL FLOW; presentation content remains a secondary naming target.**

This checkpoint resolves the ownership of the direct status-12 producer at `0xCCE0`.

## 1. `0xCBC8` is reached only after all four party records fail an activity mask

The damage/state path immediately following the lethal-HP handling checks each of the four active-party status words at:

```text
0x218F0
0x21B38
0x21D80
0x21FC8
```

These addresses are separated by exactly `0x248`, the certified character-record stride.

For every record it applies the same mask:

```c
status & 0xD040
```

Only if **all four** masked results are nonzero does execution call `0xCBC8`:

```asm
CE93  mov ax,[218F0]
CE99  and eax,0xD040
CEA3  je  CEE0
...
CEC9  mov ax,[21FC8]
CECF  and eax,0xD040
CED9  je  CEE0
CEDB  call CBC8
```

An independent gameplay path around `0xDA28..0xDA6D` repeats the same four-record `0xD040` test and also calls `0xCBC8` only when all four records fail the mask test.

## 2. The mask includes the certified death and empty-slot states

Prior reconstruction established:

- `0x4000` is the reversible zero-HP/death state: lethal damage sets HP to zero and sets `0x4000`; revival restores HP and clears it.
- `0x8000` identifies an unoccupied/replaceable active-party slot in the party-population path.

Both bits are members of `0xD040` (`0x8000 | 0x4000 | 0x1000 | 0x0040`). Therefore dead characters and empty slots both count as non-active for this all-party test. The remaining `0x1000` and `0x0040` members need not be named to prove the aggregate condition: `0xCBC8` is entered only when **no one of the four active-party records is outside all of these disabling states**.

## 3. `0xCBC8` is the terminal party-failure presentation

`0xCBC8` performs a deliberate nine-step presentation sequence, including display/resource calls and special handling for entries 0 and 6. After completing all nine entries it waits for a two-phase input transition:

```asm
CCCE  call 1A1F4
CCD3  test eax,eax
CCD5  jne  CCCE       ; wait for release/inactive
CCD7  call 1A1F4
CCDC  test eax,eax
CCDE  je   CCD7       ; wait for press/active
CCE0  push 0x0C
CCE2  call 205B4
```

The status-12 request is therefore the intentional terminal action of the presentation, not an exception/error return.

Combining caller ownership with the already-certified supervisor behavior gives:

```text
all four party slots disabled by status & 0xD040
    -> terminal party-failure presentation
    -> wait for user acknowledgement
    -> CODE.2 status 12
    -> R.EXE deletes CHARS
    -> restart module cycle without transferred party state
```

## 4. Semantic promotion

The `0xCCE0` producer can now be named safely at gameplay level as:

**`PARTY_WIPE_OR_NO_ACTIVE_PARTY -> RESTART_CODE_CYCLE_WITHOUT_CHARS`**

`PARTY_WIPE` is exact for the common lethal-damage route because `0x4000` is proven zero-HP/death state. `NO_ACTIVE_PARTY` preserves the broader binary condition because the same aggregate mask also accepts empty slots and two additional disabling status bits whose individual labels are not yet certified.

This result is important for the global status-12 enum: status 12 is not exclusively an ending/finale result. It is a shared supervisor restart primitive used by both successful finale transitions and terminal party-failure handling.

## 5. Preservation consequence

A native port should model status 12 as a cause-independent transition such as:

```text
RestartWithoutTransferredPartyState
```

and keep the cause separately typed, for example:

```text
PartyWipe
QuestFinale
<remaining producer cause>
```

Do not collapse status 12 itself into `GAME_OVER`, `ENDING`, or `NEW_GAME`; the shipped executable demonstrably reuses the same process-level transition for distinct gameplay causes.

## Next target

Resolve the remaining direct producer at `0x23E10`. It shares the event-10/finale transition tail with the five-artifact quest-completion producer, but its owning entry condition still needs binary-level classification.