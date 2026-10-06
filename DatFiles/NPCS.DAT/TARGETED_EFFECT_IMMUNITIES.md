# NPCS.DAT targeted spell/effect immunity bits

Status: **two additional bits in the aliased `behavior_flags` dword are now decoded from certified CODE.2 through complete serialized-scroll -> internal-effect -> handler -> NPC-mask chains: Lightning Bolt exclusion and Death Spell exclusion.**

The physical field is:

```text
NPCS.DAT +0x31..+0x34  uint32 behavior_flags
```

The immutable CODE.2 image is controlling evidence. `OBJECTS.DAT` provides an independent serialized spell-name/order cross-check.

## `0x00000200` — Lightning Bolt effect immunity/exclusion

The proof chain is fully serialized and executable-backed.

### 1. Serialized spell identity

`OBJECTS.DAT` object 188 is:

```text
MAGE SCROLL OF 'LIGHTNING BOLT'
```

The category-14 scroll dispatcher subtracts object base `0xA7` and indexes its jump table at logical `0x25DFC`.

For object 188:

```text
index = 188 - 167 = 21
jump_table[21] = 0x2634B
```

Stub `0x2634B` pushes internal effect ID `0x1C` / 28 and calls `0x25ACC`.

### 2. Internal effect 28 creates moving-effect type 1

The spell/effect jump table at logical `0xB740` maps:

```text
effect 28 -> 0xC370
```

The main effect dispatcher initializes `ESI = 1` before dispatch. Handler `0xC370` passes that `ESI` as the first argument to moving-effect constructor `0x8A80`.

Constructor `0x8A80` stores its first argument into the transient 22-byte record's word at `+0x0E`:

```text
mov eax,[esp+0x34]
mov word [record+0x0E],ax
```

Therefore Lightning Bolt is moving-effect type **1**.

### 3. Type-1 NPC collision mask

The moving-effect collision routine dispatches record type 1 to logical `0x8E77`.

That path resolves the target NPC definition and tests:

```text
test byte [npc_definition + 0x32], 0x03
```

Because `behavior_flags` begins at record `+0x31`, byte `+0x32` contains dword bits `0x00000100..0x00008000`. Mask `0x03` there is therefore:

```text
0x00000300 = 0x00000100 | 0x00000200
```

`0x00000100` is already independently proved as the common spell-effect immunity gate. If either bit is present, the collision path skips the NPC damage/effect call.

Thus the additional bit is safely:

```text
behavior_flags & 0x00000200
    lightning_bolt_effect_immunity
```

The name remains behavioral: it denotes exclusion from this encoded Lightning Bolt effect path, not a generalized creature taxonomy flag.

Supplied NPC definitions carrying it:

```text
STRAHD
VAMPIRE
BANSHEE
SHADOW FIEND
LIVING WALL
```

Count: **5**.

## `0x00000080` — Death Spell effect immunity/exclusion

This chain is similarly direct.

### 1. Serialized spell identity

`OBJECTS.DAT` object 197 is:

```text
MAGE SCROLL OF 'DEATH SPELL'
```

The category-14 scroll table entry is:

```text
index = 197 - 167 = 30
jump_table[30] = 0x2642C
```

Stub `0x2642C` pushes internal effect ID `0x33` / 51 and calls `0x25ACC`.

### 2. Internal effect 51 handler

The spell/effect jump table maps:

```text
effect 51 -> 0xC5EA
```

Handler `0xC5EA` calls logical `0xAED8`.

`0xAED8` performs the Death Spell target-area processing. For each eligible NPC it resolves the corresponding `NPCS.DAT` definition and executes at logical `0xAFCE`:

```text
test byte [npc_definition + 0x31], 0x80
jne  skip_target
```

If the bit is clear, the routine proceeds to the common NPC effect/damage handler; if set, that target is skipped.

Thus the bit is safely:

```text
behavior_flags & 0x00000080
    death_spell_effect_immunity
```

Supplied NPC definitions carrying it:

```text
ZOMBIE
DOOM GUARD
SKELETAL BAT
STRAHD
SHADOW
ZOMBIE LORD
BANSHEE
INQUISITOR
WRAITH
BONE GOLEM
ZOMBIE GOLEM
```

Count: **11**.

## Relationship to the existing common gate

Lightning Bolt uses the compound target mask:

```text
0x0300 = common_spell_effect_immunity | lightning_bolt_effect_immunity
```

Death Spell does **not** use the common `0x0100` bit in its target test; it has its own dedicated `0x00000080` exclusion.

This distinction must be retained in a faithful port.

## Safe implementation boundary

A high-level model may expose:

```text
behavior_flags & 0x00000080  death_spell_effect_immunity
behavior_flags & 0x00000200  lightning_bolt_effect_immunity
```

while retaining the complete original `uint32 behavior_flags` value.

Do not merge these with the common spell-effect bit or infer a broader AD&D resistance category without additional executable evidence.
