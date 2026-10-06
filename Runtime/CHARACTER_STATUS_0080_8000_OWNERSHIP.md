# CODE.2 character-status `0x0080` / `0x8000` ownership

Status: **BINARY-PROVEN STRUCTURAL SEMANTICS; source-pool gameplay identity remains pending.**

This checkpoint continues the CINE10/CINE11 ending-predicate audit against the shipped `CODE.2` object-1 bytes.

## 1. `0x8000` is an empty/unoccupied active-party-slot state

Routine `0x14FC0` scans the four active-party records (`4 * 0x248 = 0x920`). For each record it extracts high-byte bit `0x80`, i.e. word mask `0x8000`:

```asm
14FC5  mov dx,[eax+218F0]
14FCC  xor dl,dl
14FCE  and dh,80h
14FD4  test edx,edx
14FD6  je 14FE3
14FD8  push ebx
14FD9  call 148F0
```

When the bit is set, the routine immediately calls `0x148F0` with that slot index. `0x148F0` replaces the slot with a complete `0x248`-byte character image and initializes it. Thus `0x8000` is not merely a generic high-order condition: in this party-management path it identifies a slot eligible to be populated/replaced.

This also explains why the finale predicate excludes all statuses `>= 0x4000`: an `0x8000` empty slot must never count as a qualifying party character.

## 2. `0x148F0` is a complete party-record population path

`0x148F0(slot)`:

1. obtains a source-record index through the table at `0x13020`;
2. computes `source_index * 0x248`;
3. copies exactly `0x248` bytes from the source pool rooted at `0xA808` into active-party record `slot * 0x248 + 0x218E0`;
4. clears/reinitializes the trailing portion of the destination record;
5. performs class/casting and inventory initialization;
6. finally sets low-byte status bit `0x80` at `record + 0x18`:

```asm
14A4D  imul esi,esi,248h
14A53  or byte ptr [esi+218F0],80h
14A5A  ...
14A5D  ret
```

Therefore low-byte `0x0080` is a **proven provenance/population marker associated with characters installed through this source-pool-to-active-party path**.

The exact gameplay name of the source pool (`NPC`, recruit, pregenerated roster, etc.) is deliberately not frozen here until the `0xA808` pool is tied directly to its file/resource loader.

## 3. Independent caller confirms slot filling

`0x14FC0` first searches for an `0x8000` slot and fills it via `0x148F0`. If all four slots are already occupied, it selects a slot through `0x14CA4`, performs companion state handling, and again calls `0x148F0`:

```asm
14FF5  call 14CA4
...
15005  push ebx
15006  call 148F0
1500E  push ebx
1500F  call 14E9C
```

This is direct structural evidence that `0x148F0` is character installation/replacement rather than ordinary per-frame character initialization.

## 4. Finale semantics are now narrower

The event-10 ending selector counts a party record only when:

```c
status < 0x4000 && !(status & 0x0080)
```

Combining the newly proven ownership:

- `0x8000` records are empty/unoccupied party slots in the population path;
- `0x0080` marks characters installed through `0x148F0`'s source-pool path;
- CINE10 is retained if at least one occupied/normal-range party character lacks that population marker;
- CINE11 is selected when no such character exists.

The safe current interpretation is therefore:

```text
CINE10: at least one qualifying active character NOT sourced through 0x148F0
CINE11: no qualifying active character outside the 0x148F0-sourced class
```

This is stronger than the previous generic status predicate, but it is still intentionally short of naming the two classes as player-created versus NPC/recruited characters.

## 5. Next certification target

Trace the source pool at `0xA808` back to its loader/resource and identify the table at `0x13020`. If the pool is proven to be the NPC/recruit roster, `0x0080` can be promoted to a stable semantic flag and the CINE10/CINE11 gameplay distinction can be named without inference.