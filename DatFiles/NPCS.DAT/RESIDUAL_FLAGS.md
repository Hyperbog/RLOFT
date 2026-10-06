# NPCS.DAT residual behavior-flag audit

Status: **all populated serialized behavior bits are now partitioned into proved semantic aliases, exact-but-intent-unknown executable branches, and preserved residual capacity. No remaining populated bit is silently assigned from creature-name inference.**

The physical flag fields are:

```text
+0x31..+0x34  uint32 behavior_flags
+0x39         uint8  behavior_flags_39
+0x3A         uint8  behavior_flags_3A
```

This audit uses the immutable supplied `NPCS.DAT` plus complete CODE.2 object-1 disassembly. The purpose is not to force names onto every stored bit; it is to establish exactly what remains after the proved aliases in `BEHAVIOR_FLAGS.md`.

## Serialized bit coverage

The supplied 83 definitions collectively populate every low bit `0..19` of the main dword:

```text
populated behavior_flags mask = 0x000FFFFF
```

Twelve of those twenty bits are independently named today:

```text
0x00000001  detect_undead_effect_exclusion
0x00000002  hold_undead_effect_immunity
0x00000004  turn_undead_roll_penalty_4
0x00000008  hold_person_effect_immunity
0x00000010  hold_monster_effect_immunity
0x00000080  death_spell_effect_immunity
0x00000100  common_spell_effect_immunity
0x00000200  lightning_bolt_effect_immunity
0x00000800  cold_effect_immunity
0x00002000  fire_effect_immunity
0x00004000  weapon_category10_damage_floor_one
0x00008000  weapon_category9_damage_floor_one
```

The remaining populated mask is therefore exactly:

```text
0x000F1460
```

or these eight bits:

```text
0x00000020
0x00000040
0x00000400
0x00001000
0x00010000
0x00020000
0x00040000
0x00080000
```

## Complete direct-consumer boundary

The CODE.2 audit found two mechanisms that can consume the complete four-byte `behavior_flags` value rather than an already-decoded immediate bit/mask.

### Generic area-effect intersection at `B2FB`

`B2FB` loads the full dword and intersects it with a caller-supplied effect mask.

There are five calls to this area-effect routine in the certified object. Their supplied masks are exactly:

```text
0x2100
0x2100
0x0900
0x0900
0x2100
```

Those contain only the already-decoded common/fire/cold bits:

```text
0x0100  common_spell_effect_immunity
0x0800  cold_effect_immunity
0x2000  fire_effect_immunity
```

None of the residual bits enters the generic area-effect intersection through any supplied CODE.2 caller.

### Physical weapon gate at `C919..C99E`

This routine loads the full dword once and then consumes:

```text
0x00004000  category-10 one-damage gate       // decoded
0x00008000  category-9 one-damage gate        // decoded
0x00020000  category-11 anomalous comparison  // behavior preserved, intent unknown
0x00040000  category-11 anomalous comparison  // behavior preserved, intent unknown
```

The last two bits therefore are **not unused**. Their exact machine behavior is documented in `WEAPON_DAMAGE_FLAGS.md`; only their intended source-level semantics remain unresolved.

No `0x00080000` test occurs in this routine.

## Residual main-dword classes

### Stored but no proved semantic consumer

The complete direct-access audit did not establish a semantic branch for:

```text
0x00000020  22 definitions
0x00000040  18 definitions
0x00000400  16 definitions
0x00001000   4 definitions
0x00010000  15 definitions
0x00080000   1 definition  // BANSHEE only
```

These values are real serialized state and must be retained. The project does **not** call them padding or unused format bits: they may represent dormant features, legacy semantics, or behavior implemented outside the recovered direct tests. Their current correct status is **raw/preserved, no proved semantic consumer**.

### Executable-consumed but source intent unresolved

```text
0x00020000  3 definitions  // PYRE ELEMENTAL, STRAHD, BANSHEE
0x00040000  1 definition   // BANSHEE
```

Both participate in category-11 weapon handling. Because CODE.2 zero-extends the enchantment byte and compares it against sign-extended `-2`, the observed binary behavior is unusual. Preserve that behavior exactly; do not invent names such as `requires_+2_weapon` unless independent source evidence is recovered.

## `behavior_flags_39` residuals

All eight bits occur somewhere in the supplied byte:

```text
populated mask = 0xFF
```

Four are decoded:

```text
0x01 poison_attack
0x20 paralysis_attack
0x40 dynamic_barrier_activation
0x80 obstacle_passthrough
```

The remaining serialized mask is:

```text
0x1E = 0x02 | 0x04 | 0x08 | 0x10
```

with supplied counts:

```text
0x02  7 definitions
0x04  8 definitions
0x08  1 definition   // BRIGAND
0x10  2 definitions  // SKELETON, ZOMBIE GOLEM
```

Complete offset-reference review found direct NPC-definition tests only for `0x01`, `0x20`, `0x40`, and `0x80`. The four residual bits therefore remain raw. Similar byte-offset accesses elsewhere in CODE.2 were checked against their base tables and belong to OBJECTS/character/other structures rather than the NPC definition table.

## `behavior_flags_3A` is semantically closed for supplied data

The supplied populated mask is exactly:

```text
0x36 = 0x02 | 0x04 | 0x10 | 0x20
```

All four populated bits are decoded:

```text
0x02 level_drain_attack
0x04 strength_drain_attack
0x10 disease_attack
0x20 undead_classification
```

Thus there are **no populated unresolved bits** in `+0x3A` for this corpus.

## Zero serialized bytes `+0x3B/+0x3C`

Both bytes remain zero in all 83 supplied records and no NPC-definition consumer has been proved. They remain exact raw bytes. This audit does not elevate them to guaranteed padding because one sample corpus cannot establish that contract for hypothetical alternate databases.

## Preservation boundary

A faithful implementation should keep the raw flag storage authoritative and expose semantic aliases as views:

```text
uint32 behavior_flags;
uint8  behavior_flags_39;
uint8  behavior_flags_3A;
```

Do not rebuild these fields from named booleans alone. In particular, doing so would destroy:

- six populated main-dword bits with no proved semantic consumer;
- the two anomalous category-11 weapon bits;
- four populated residual bits in `behavior_flags_39`.

`analyze_residual_flags.py` reproduces the populated/residual masks and, when supplied the certified CODE.2 image, verifies the two full-dword consumer sites and all five caller masks of the generic area-effect path.
