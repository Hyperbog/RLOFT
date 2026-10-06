# Finale character provenance and death-state classification

Status: **CORRECTED / BINARY-PROVEN CROSS-SUBSYSTEM SEMANTICS.**

This document supersedes the earlier revision that used the obsolete character-relative HP offset `+0x24` and still treated the zero-HP state as only provisionally named.

## 1. Certified character geometry

The active-party record base is runtime `0x218E0`, with stride `0x248`.

Therefore:

```text
status word   runtime 0x218F0 = record +0x10
current HP    runtime 0x218FC = record +0x1C
```

The older `+0x18` / `+0x24` labels were eight bytes too high and must not be used.

## 2. `0x0080` = party-template / recruited-character provenance

`0x148F0(slot)` installs a complete predefined `0x248` character image into an active-party slot and finishes with:

```asm
14A53  or byte ptr [slot*248h + 218F0h],80h
```

The certified RES9 wordcode reconstruction proves opcode `0x05` reaches this party-template installation flow from recruitable/NPC interaction scripts.

Safe semantic name:

```text
status bit 0x0080 = PARTY_TEMPLATE / RECRUITED-CHARACTER PROVENANCE
```

## 3. `0x4000` = zero-HP death state

The damage path subtracts from current HP at `record +0x1C` / runtime `0x218FC`:

```asm
CDFA  sub word [character+218FCh],si
CE08  cmp word [character+218FCh],0
CE10  jg  CE93
CE16  mov word [character+218FCh],0
CE1F  or  byte [character+218F1h],40h
CE27  and byte [character+218F1h],0E3h
```

Since `0x218F1` is the high byte of the status word at `0x218F0`, the OR sets word mask `0x4000` exactly when HP reaches zero or below.

The inverse path at `0x9F72` restores HP to 1 and clears that same bit:

```asm
9F73  mov word [character+218FCh],1
9F82  and byte [character+218F1h],0BFh
```

This directly certifies a reversible death/revival lifecycle.

Safe semantic name:

```text
status bit 0x4000 = DEAD / ZERO-HP DEATH STATE
```

## 4. `0x8000` remains distinct

Independent party-population code proves `0x8000` marks an empty/unoccupied active-party slot. It is not the death flag.

Thus the relevant distinctions are:

```text
0x0080  recruited/template provenance
0x4000  zero-HP death state
0x8000  empty/unoccupied party slot
```

## 5. Finale predicate

Event 10 uses the effective unsigned predicate:

```c
status < 0x4000u && (status & 0x0080u) == 0
```

Therefore:

```text
CINE10.ANM
    at least one living original/non-recruited party character remains

CINE11.ANM
    no living original/non-recruited party character remains
```

Empty slots and dead original characters cannot qualify. Living recruited/template characters are excluded by provenance.

## 6. Preservation consequence

A native reconstruction should preserve provenance, death state, and slot occupancy as separate typed properties while retaining the legacy status word for compatibility and verification.
