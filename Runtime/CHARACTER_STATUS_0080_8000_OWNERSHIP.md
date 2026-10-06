# CODE.2 character-status `0x0080` / `0x8000` ownership

Status: **CORRECTED / BINARY-PROVEN WITH LATER SEMANTIC PROMOTION.**

This document preserves the structural ownership proof while normalizing offsets to the certified active-party base `0x218E0` and incorporating the later RES9/NPC correlation.

## 1. Certified character geometry

Active-party record `n` begins at:

```text
0x218E0 + n * 0x248
```

The 16-bit status word is at runtime `0x218F0`, therefore serialized/record-relative offset:

```text
+0x10
```

The earlier `+0x18` label was based on an obsolete character-base revision and must not be used.

## 2. `0x8000` = empty/unoccupied active-party slot

Routine `0x14FC0` scans all four active-party records and extracts word mask `0x8000`:

```asm
14FC5  mov dx,[eax+218F0h]
14FCC  xor dl,dl
14FCE  and dh,80h
14FD4  test edx,edx
14FD6  je 14FE3
14FD8  push ebx
14FD9  call 148F0
```

When set, the slot is populated/replaced by `0x148F0`. The same routine has a replacement path when no empty slot exists. This establishes `0x8000` as the empty/unoccupied active-party-slot state.

## 3. `0x148F0` installs a complete predefined party record

`0x148F0(slot)`:

1. resolves a source template index;
2. copies a complete `0x248` character image from the predefined source pool into `0x218E0 + slot*0x248`;
3. performs class/casting/inventory initialization;
4. sets low-byte status bit `0x80` at status offset `+0x10`:

```asm
14A4D  imul esi,esi,248h
14A53  or byte ptr [esi+218F0h],80h
```

Later cross-subsystem reconstruction proves CODE.2 RES9 opcode `0x05` invokes this party-template installation flow from recruitable/NPC interaction scripts.

Certified semantic name:

```text
status & 0x0080 = PARTY_TEMPLATE / RECRUITED-CHARACTER PROVENANCE
```

`PARTY_TEMPLATE` is the strict engine-level description; recruited NPC is the active gameplay use established by the RES9/NPC corpus.

## 4. Finale consequence

The event-10 ending predicate is:

```c
status < 0x4000u && (status & 0x0080u) == 0
```

Combined with independently certified status meanings:

- `0x4000` = zero-HP/death state;
- `0x8000` = empty/unoccupied slot;
- `0x0080` = recruited/template provenance;

`CINE10.ANM` is selected when at least one living original/non-recruited party character remains. `CINE11.ANM` is selected when none does.

## 5. Preservation consequence

Keep slot occupancy, life/death state, and character provenance as distinct concepts. In particular, do not reinterpret `0x0080` as an NPC-AI flag: once installed, the template character is an ordinary active party record carrying provenance metadata.
