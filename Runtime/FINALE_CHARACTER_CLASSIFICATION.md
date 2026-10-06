# Finale character provenance and HP-depletion classification

Status: **BINARY-PROVEN CROSS-SUBSYSTEM SEMANTICS.**

This checkpoint closes the two unresolved terms in the CINE10/CINE11 event-10 predicate by joining fresh CODE.2 disassembly with the independently reconstructed RES9 wordcode VM and NPC interaction index.

## 1. `0x0080` is the party-template / recruited-character provenance marker

The prior audit proved that `0x148F0(slot)` copies a complete `0x248` character image from the predefined source pool into an active-party slot and ends with:

```asm
14A53  or byte ptr [slot*248h + 218F0h],80h
```

The caller is now independently identified. CODE.2 wordcode opcode `0x05` dispatches at `0x156C7`:

```asm
156C7  call 14FC0
```

The certified RES9 VM reconstruction names opcode `0x05` **apply/add party-character template**. Its one operand is a static template ID. The active corpus uses template IDs `0..9`.

The independently reconstructed NPC interaction index further proves that opcode `0x05` occurs in party-relevant/recruitable NPC interactions. Examples include TATYANA, CLERIC, ELVEN MAGE, STRAHD'S PRISONER, DANDY WOMAN, MAGE, and other mapped NPC interactions. The operation is therefore not generic character initialization: it installs a predefined party template originating from NPC interaction flow.

Safe semantic promotion:

```text
status bit 0x0080 = PARTY_TEMPLATE / RECRUITED-NPC PROVENANCE
```

`PARTY_TEMPLATE` is the strict engine-level name; `RECRUITED_NPC` is valid for the active RES9 interaction use.

## 2. `0x4000` is set when the character's HP-like combat value reaches zero

Fresh CODE.2 disassembly identifies direct `0x4000` writers through the high byte of the status word.

In the damage path around `0xCDF4`:

```asm
CDFA  sub word [character+218FCh],si
CE08  cmp word [character+218FCh],0
CE10  jg  CE93
CE16  mov word [character+218FCh],0
CE1F  or  byte [character+218F1h],40h
CE27  and byte [character+218F1h],0E3h
```

Because `218F1h` is the high byte of the status word at `218F0h`, OR `0x40` there sets word mask `0x4000`.

A second path at `0xD48D..0xD4B3` explicitly zeroes the same `+0x24` character value and then sets status `0x4000` while clearing the same condition-bit group.

Thus `0x4000` is a combat life-state transition produced when the character's HP-like value at record `+0x24` is exhausted/zeroed. Existing actor-selection helpers reject `status >= 0x4000`, so the flag removes that record from ordinary usable-character processing.

Safe semantic name:

```text
status bit 0x4000 = HP_DEPLETED / COMBAT_INACTIVE
```

The historical UI term `dead`, `unconscious`, or another AD&D-specific state is not frozen here without the remaining text/UI correlation.

## 3. Finale predicate now has gameplay-level meaning

Event 10 uses:

```c
status < 0x4000 && !(status & 0x0080)
```

The two terms can now be read as:

1. character is not in the HP-depleted/high-order inactive range; and
2. character is not a predefined party-template/recruited-NPC character.

Therefore:

```text
CINE10.ANM
  if at least one qualifying non-template/original party character remains active

CINE11.ANM
  if no qualifying non-template/original party character remains active
```

This is no longer merely a raw status-bit correlation. The ending variant explicitly distinguishes the continued presence of the original/non-recruited party-character class from a party containing only recruited/template characters, empty slots, or HP-depleted original characters.

## 4. Preservation consequence

A port should preserve provenance separately from life state:

```cpp
bool is_template_character = legacy_status & 0x0080;
bool hp_depleted_or_inactive = legacy_status >= 0x4000;

bool cine10 = any_party_member(
    !is_template_character && !hp_depleted_or_inactive
);
```

Do not collapse `0x0080` into an NPC AI flag: after recruitment/template application the record is a normal live party-character record. Likewise, do not collapse every status `>=0x4000` into a single modern `dead` Boolean until the remaining high-order encodings are enumerated.

## Next checkpoint

Correlate the `+0x24` HP-like field and `0x4000` transition with UI/text and healing/resurrection paths to determine the original gameplay term (`dead`, `unconscious`, etc.), then classify the remaining CODE.2 status-12 producers against the now-resolved finale semantics.