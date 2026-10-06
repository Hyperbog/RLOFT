# NPCS.DAT weapon-category damage gates

Status: **two additional `behavior_flags` bits are behaviorally decoded from certified CODE.2 as weapon-category damage gates. Both force the resulting hit to the engine's minimum of one damage; they are resistance/floor behaviors, not zero-damage immunity.**

The relevant serialized field is:

```text
NPCS.DAT +0x31..+0x34  uint32 behavior_flags
```

The immutable CODE.2 object is controlling evidence. `OBJECTS.DAT` category/enchantment fields provide the serialized item-class cross-check.

## Damage-gate routine

CODE.2 logical `0xC910..0xC99E` is called immediately before NPC damage application at `0xE947..0xE955`.

Its inputs are the current damage value, the target NPC definition pointer, and a weapon/object ID. It loads:

```text
npc_flags       = dword(npc + 0x31)
object_category = byte(object + 0x2F)
enchantment     = byte(object + 0x37)
```

The object fields are independently decoded in `OBJECTS.DAT` as `item_category` and `enchantment_bonus`.

The routine applies an important normalization before the flag checks:

```text
weapon_id == -1        -> category 9
otherwise:
    category = object.item_category
    if object.enchantment_bonus != 0:
        category = 11
```

Thus the category-9/category-10 tests below apply to unenchanted definitions; every object with nonzero enchantment is rerouted through the category-11 branch.

After all category-specific tests, `0xC993..0xC998` enforces:

```text
if damage < 1:
    damage = 1
```

Consequently a matched resistance flag never makes an ordinary hit do zero damage. It forces the post-gate result to **one damage**.

## `0x00008000` — weapon category 9 damage floor

For category 9, CODE.2 executes:

```text
C95F  test CH, 0x80    // ECX bit 15 == behavior_flags 0x00008000
C962  je   C993
C991  damage = 0
C993  clamp damage to >= 1
```

Safe behavioral alias:

```text
behavior_flags & 0x00008000
    weapon_category9_damage_floor_one
```

The supplied `OBJECTS.DAT` category-9 family is:

```text
MACE
QUARTERSTAFF
WARHAMMER
```

with separate +1/+2 definitions. Because any nonzero enchantment is rerouted to category 11 first, this flag affects the unenchanted category-9 path plus the executable's `weapon_id == -1` fallback.

Fourteen supplied NPC definitions carry the flag:

```text
WEREBAT
PYRE ELEMENTAL
STRAHD
SHADOW
VAMPIRE
GARGOYLE
BANSHEE
GHOUL LORD
SPECTRE
WEREWOLF
WRAITH
WIGHT
WERERAT
ALBINO WEREWOLF
```

The project retains the category-based name rather than asserting an original source enum such as `blunt_resistance`, even though the shipped category content is mace/staff/hammer oriented.

## `0x00004000` — weapon category 10 damage floor

For category 10, CODE.2 executes:

```text
C966  test CH, 0x40    // ECX bit 14 == behavior_flags 0x00004000
C969  je   C993
C991  damage = 0
C993  clamp damage to >= 1
```

Safe behavioral alias:

```text
behavior_flags & 0x00004000
    weapon_category10_damage_floor_one
```

The supplied category-10 family contains the normal axe/sword/dagger/pole/thrusting definitions, including:

```text
BATTLE AXE
SHORT SWORD
LONG SWORD
BROAD SWORD
DAGGER
TWO-HANDED SWORD
HALBERD
SPEAR
THROWING KNIFE
```

plus named and enchanted variants. Again, nonzero enchantment reroutes the object to category 11 before this check.

Sixteen supplied NPC definitions carry the flag:

```text
WEREBAT
PYRE ELEMENTAL
SKELETON
STRAHD
SHADOW
VAMPIRE
GARGOYLE
BANSHEE
GHOUL LORD
SPECTRE
WEREWOLF
WRAITH
BONE GOLEM
WIGHT
WERERAT
ALBINO WEREWOLF
```

Skeleton and Bone Golem are useful corpus cross-checks: both carry the category-10 gate while lacking the category-9 gate. The executable behavior, not that content pattern, remains the basis for the semantic name.

## Category-11 high-bit branch: exact behavior, names still unresolved

The same routine contains two additional high-bit tests after category 11 has been selected:

```text
behavior_flags & 0x00020000
behavior_flags & 0x00040000
```

These are deliberately **not** promoted to semantic aliases yet because the surrounding comparison is unusual and must be preserved exactly.

For `0x00020000`, CODE.2 loads `object[+0x37]` with `MOVZX` and compares the zero-extended value against sign-extended immediate `-2`:

```text
C96D  test ECX, 0x00020000
C975  movzx EBX, byte [object+0x37]
C979  cmp   EBX, -2
C97C  jl    C980
C97E  damage = 0
```

With the supplied unsigned byte domain `0..255`, the `< -2` branch cannot be taken, so a set `0x00020000` bit drives category-11 damage to the one-damage floor in this executable.

For `0x00040000`, the complementary comparison is:

```text
C980  test ECX, 0x00040000
C988  movzx EDX, byte [object+0x37]
C98C  cmp   EDX, -2
C98F  jge   C993
C991  damage = 0
```

With the same zero-extended byte domain, the `jge` path is always taken, so this bit does not independently reduce supplied-object damage in the observed binary behavior.

Supplied occurrences are:

```text
0x00020000  PYRE ELEMENTAL, STRAHD, BANSHEE
0x00040000  BANSHEE
0x00080000  BANSHEE   // no semantic consumer assigned here
```

These bits remain raw. A faithful port must reproduce the actual signed-immediate/zero-extended comparison behavior unless deliberately implementing a separately documented bug fix.

## Safe implementation boundary

A high-level model may expose:

```text
behavior_flags & 0x00004000  weapon_category10_damage_floor_one
behavior_flags & 0x00008000  weapon_category9_damage_floor_one
```

while retaining the complete raw `uint32 behavior_flags` value.

Implementation order matters:

1. derive the OBJECTS category;
2. if enchantment is nonzero, replace the category with 11;
3. apply the category-specific NPC flag gate;
4. clamp the returned damage to at least 1.

Do not implement these aliases as zero-damage immunity, and do not collapse category 9/10 into guessed AD&D damage-type enums.