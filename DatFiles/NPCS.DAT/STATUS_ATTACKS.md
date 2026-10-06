# NPCS.DAT poison/disease attack flags

Status: **the two previously unnamed creature attack-status bits are now decoded from certified CODE.2 by tracing their party-status bits into the cleric cure dispatcher.**

The relevant serialized fields are:

```text
NPCS.DAT +0x39  uint8 behavior_flags_39
NPCS.DAT +0x3A  uint8 behavior_flags_3A
```

The immutable executable is controlling evidence. `OBJECTS.DAT` spell-scroll names provide an independent serialized-name/order cross-check.

## Party status-byte relationship

The NPC melee/special-attack path writes conditions into the high byte of the party status word at runtime `actor + 0x218F0`:

```text
actor + 0x218F1 bit 0x04  disease condition
actor + 0x218F1 bit 0x08  poison condition
actor + 0x218F1 bit 0x10  paralysis condition
```

The paralysis identity was already independently proved through `RING OF FREE ACTION`. The cure dispatcher now identifies the two remaining bits directly.

## `behavior_flags_39 & 0x01` — poison attack

The main NPC attack routine tests NPC definition byte `+0x39` at logical `0xD947`:

```text
test byte [npc + 0x39], 0x01
```

After the common target resistance/save helper succeeds in allowing the effect, CODE.2 sets:

```text
or byte [actor + 0x218F1], 0x08
```

at logical `0xD960`.

The character-status dispatcher `0x9D90` later handles internal effect code `0x71` at `0xA175`. It verifies status bit `0x0800` in the 16-bit status word and clears exactly the same high-byte bit:

```text
and byte [actor + 0x218F1], 0xF7
```

The cleric spell dispatcher maps effect ID `113` to the handler at logical `0xBF87`. That handler passes internal code `0x71` to the common character-effect routine.

The serialized scroll definition providing an independent name check is:

```text
OBJECTS.DAT object 223 = CLERIC SCROLL OF 'NEUTRALIZE POISON'
```

Thus the condition removed by code `0x71` is poison, and the NPC capability bit that sets the corresponding status is safely:

```text
behavior_flags_39 & 0x01  poison_attack
```

Only one supplied NPC definition carries it:

```text
DARKLING
```

The name does not imply a particular AD&D poison strength/type; it records only the proved condition family.

## `behavior_flags_3A & 0x10` — disease attack

The same NPC attack routine tests definition byte `+0x3A` at logical `0xD967`:

```text
test byte [npc + 0x3A], 0x10
```

Before the common resistance/save helper, one character-class value (`actor + 0x218F5 == 1`) is explicitly exempted. When the effect applies, CODE.2 sets:

```text
or byte [actor + 0x218F1], 0x04
```

at logical `0xD98C`.

The status dispatcher handles internal code `0x64` at logical `0xA19D`. It verifies status bit `0x0400` and clears precisely that high-byte condition:

```text
and byte [actor + 0x218F1], 0xFB
```

The cleric spell dispatcher maps effect ID `100` to logical handler `0xBF6C`, which passes internal code `0x64` to the character-effect routine.

The serialized spell definition is:

```text
OBJECTS.DAT object 215 = CLERIC SCROLL OF 'CURE DISEASE'
```

Therefore the creature bit is safely:

```text
behavior_flags_3A & 0x10  disease_attack
```

The supplied definitions carrying it are:

```text
ZOMBIE LORD
GHOUL LORD
INQUISITOR
```

The encoded class exemption is retained as executable behavior but is not used here to infer a source-level class-specific rule name.

## Independent paralysis control

The same mechanism provides a useful control case:

```text
internal code 0x68
cleric effect ID 104
OBJECTS.DAT object 219 = CLERIC SCROLL OF 'REMOVE PARALYSIS'
status clear at 0xA1D1:
    and byte [actor + 0x218F1], 0xEF
```

This clears bit `0x10`, exactly matching the already-proved NPC `paralysis_attack` path. The three cure handlers therefore form a consistent serialized/runtime mapping:

```text
0x04  disease   <-> code 0x64  Cure Disease
0x08  poison    <-> code 0x71  Neutralize Poison
0x10  paralysis <-> code 0x68  Remove Paralysis
```

## Safe implementation boundary

A high-level NPC model may expose:

```text
behavior_flags_39 & 0x01  poison_attack
behavior_flags_39 & 0x20  paralysis_attack
behavior_flags_3A & 0x10  disease_attack
```

while preserving the raw bytes and every unresolved bit exactly.

`analyze_status_attacks.py` verifies the NPC bit sets, spell-scroll names, dispatcher jump-table targets, internal cure codes, attack-side status setters, and cure-side status clearers against the certified CODE.2 binary.