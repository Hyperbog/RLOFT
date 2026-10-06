# NPCS.DAT creature sound fields

Status: **all four trailing 16-bit sound fields are now behaviorally separated from certified CODE.2. In particular, the formerly unresolved `+0x41` word is the surviving damage/effect reaction sound, distinct from the attack sound and the death sound.**

The physical fields are:

```text
+0x3D  uint16 ambient_or_proximity_sound_id
+0x3F  uint16 attack_sound_id
+0x41  uint16 damage_reaction_sound_id
+0x43  uint16 death_sound_id
```

The immutable CODE.2 object is controlling evidence. All four fields must remain 16-bit even though the supplied high bytes are zero.

## `+0x3D` — ambient/proximity sound

The active NPC behavior path at logical `0x1B9A7` resolves the current NPC definition and loads the word at `+0x3D`. It adds the creature-sound base at DGROUP `0x258C0` and queues the result through `28110`.

This occurs inside proximity/active-behavior logic and is probability-gated before playback, supporting the existing behavioral name `ambient_or_proximity_sound_id`.

## `+0x3F` — attack sound

The NPC attack path reads `+0x3F` directly and queues it through the same `0x258C0` creature-sound base.

A representative site is:

```text
DA05  movzx eax, word [npc_definition + 0x3F]
DA0E  add   eax, [0x258C0]
DA15  call  28110
```

The same field is also used on a special attack branch at `D5B4`.

## `+0x41` — damage-reaction sound

The central NPC damage/effect routine is logical `0xE284`.

After applying its HP/state changes, it tests the live NPC HP word at `+0x12`:

```text
E31F  cmp word [live_npc + 0x12], 0
E324  jg  E553
```

Thus `E553` is reached only when the NPC remains alive after the damage/effect operation.

That branch performs:

```text
E55B  movzx eax, word [npc_definition + 0x41]
E564  add   eax, [0x258C0]
E56B  call  28110
```

The safe serialized name is therefore:

```text
damage_reaction_sound_id
```

This is intentionally broader than `pain_sound`: `E284` is the common damage/effect handler, and at least one caller can pre-adjust HP before invoking it with a zero direct-damage argument. The executable guarantee is that `+0x41` is the sound selected by this handler when the target survives.

## `+0x43` — death sound

If the HP test at `E31F` does **not** take the positive-HP jump, `E284` enters its death-resolution path. After story/special death handling converges, CODE.2 resolves the NPC definition and executes:

```text
E4D7  cmp word [npc_definition + 0x43], 0
E4E2  movzx eax, word [npc_definition + 0x43]
E4EB  add   eax, [0x258BC]
...
E503  call  28110
```

If the stored word is zero, the routine queues the base value at `0x258BC` itself as the fallback.

This independently confirms `death_sound_id` and also shows that the death sound uses the `0x258BC` base rather than the ordinary creature-sound base `0x258C0` used by `+0x3D/+0x3F/+0x41`.

## Supplied-corpus pattern

Across the 83 supplied definitions:

```text
+0x3D ambient/proximity  nonzero 33, unique values 31, range 0..94
+0x3F attack             nonzero 34, unique values 32, range 0..95
+0x41 damage reaction    nonzero 35, unique values 32, range 0..96
+0x43 death              nonzero  9, unique values  5, range 0..90
```

For **32 definitions**, the first three sound IDs form an exact consecutive triplet:

```text
ambient = N
attack  = N + 1
reaction= N + 2
```

Examples:

```text
ZOMBIE          6 /  7 /  8
SKELETON        9 / 10 / 11
WEREBAT        15 / 16 / 17
PYRE ELEMENTAL 33 / 34 / 35
WEREWOLF       58 / 59 / 60
BONE GOLEM     82 / 83 / 84
LIVING WALL    94 / 95 / 96
```

This corpus organization is consistent with the three independently proved playback contexts, but the executable behavior remains the semantic authority.

## Safe implementation boundary

A faithful model may expose all four names directly:

```text
ambient_or_proximity_sound_id
attack_sound_id
damage_reaction_sound_id
death_sound_id
```

Implementation must preserve the separate playback bases and fallback behavior:

- `+0x3D`, `+0x3F`, `+0x41` use the creature-sound base at `0x258C0`;
- `+0x43` uses the death-sound base at `0x258BC`;
- zero `+0x43` selects the base death sound rather than suppressing playback.
