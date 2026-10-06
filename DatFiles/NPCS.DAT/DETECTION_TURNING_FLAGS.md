# NPCS.DAT detection and turning flags

Status: **two additional low `behavior_flags` bits are behaviorally decoded from certified CODE.2: Detect Undead exclusion and a four-point Turn Undead roll penalty. The turning modifier is executable-supported but not exercised by any supplied NPC that is also marked undead.**

The physical field is:

```text
NPCS.DAT +0x31..+0x34  uint32 behavior_flags
```

The immutable CODE.2 image remains controlling evidence. `OBJECTS.DAT` supplies the serialized spell-name cross-check for Detect Undead.

## `0x00000001` — Detect Undead effect exclusion

The complete chain is serialized and executable-backed.

### Serialized spell -> internal effect

`OBJECTS.DAT` object 170 is:

```text
MAGE SCROLL OF 'DETECT UNDEAD'
```

The category-14 scroll dispatcher uses base object 167. Object 170 is therefore table index 3; CODE.2 scroll jump table `0x25DFC[3]` points to `0x2618A`. That stub pushes internal effect ID `3` and calls the common spell-use routine.

The spell/effect jump table at `0xB740` maps:

```text
effect 3 -> 0xBC2A
```

Handler `0xBC2A` increments the word at DGROUP `0xF03C`. The matching timed-state cleanup at `0x88A8` decrements the same word, proving it is an active-effect counter rather than a one-shot scratch value.

### NPC display/marker filter

The first-person scene/render path at `0x2C6BE..0x2C7B0` checks the detection counters. When the separate detection source at `0xF03E` is zero and the Detect Undead counter `0xF03C` is active, it resolves the NPC definition and executes at `0x2C724`:

```text
test byte [npc_definition + 0x31], 0x01
jne  skip_detection_marker
```

Thus this bit directly excludes the NPC from the Detect Undead-specific display/marker path.

Safe behavioral alias:

```text
behavior_flags & 0x00000001
    detect_undead_effect_exclusion
```

Supplied definitions carrying it:

```text
DOOM GUARD
WEREBAT
PYRE ELEMENTAL
WORG
HELLHOUND
ASSASSIN
TREANT
GARGOYLE
BANSHEE
WEREWOLF
BRIGAND
BROKEN ONE
DARKLING
GOBLYN
WERERAT
LIVING WALL
ALBINO WEREWOLF
```

Count: **17**.

This is intentionally named for the proved effect path rather than being treated as a universal living/undead taxonomy bit. The separately proved `behavior_flags_3A & 0x20` remains the engine's explicit undead classification for turning/combat systems.

## `0x00000004` — Turn Undead roll penalty of four

CODE.2 routine `0xB518..0xB6F0` is the undead-turning resolution path. Its behavior independently identifies the operation:

1. it rejects NPCs not carrying the proved `undead_classification` bit (`npc[+0x3A] & 0x20`);
2. it reads NPC hit dice at `+0x1D` and combines them with the acting character's progression/class result;
3. it generates the turning roll/table state;
4. immediately before the decisive threshold comparison, it tests `behavior_flags & 0x00000004`.

The exact mutation is:

```text
B6B0  test byte [npc_definition + 0x31], 0x04
B6B5  je   B6BC
B6B7  sub  turn_roll, 4
B6BC  ...
B6C3  compare table threshold against turn_roll
```

Higher rolls are favorable in this comparison, so subtracting four makes the affected undead harder to turn.

Safe behavioral alias:

```text
behavior_flags & 0x00000004
    turn_undead_roll_penalty_4
```

The spell/effect dispatcher also reaches this routine through internal effect ID 88 (`0xB740[88] -> 0xC68F -> call 0xB518`), consistent with its special cleric/turning role.

### Supplied-corpus caveat

Eighteen supplied NPC definitions carry bit `0x00000004`, but **none** of the 17 definitions carrying the separate `undead_classification` bit also carry it.

Therefore this four-point turning modifier is a real executable capability but is dormant for the supplied NPC database. It must still be preserved and implemented faithfully; it should not be promoted to a content claim such as "these 18 creatures have turn resistance."

Supplied bit carriers are:

```text
DOOM GUARD
WEREBAT
PYRE ELEMENTAL
WORG
HELLHOUND
STRAHD
ASSASSIN
TREANT
GARGOYLE
BANSHEE
WEREWOLF
BRIGAND
BROKEN ONE
DARKLING
GOBLYN
WERERAT
LIVING WALL
ALBINO WEREWOLF
```

## Safe implementation boundary

A high-level model may expose:

```text
behavior_flags & 0x00000001  detect_undead_effect_exclusion
behavior_flags & 0x00000004  turn_undead_roll_penalty_4
```

while retaining the original raw dword.

For the turning modifier, reproduce the exact order: first require the separate undead-classification flag, then apply the four-point roll reduction before threshold comparison. Do not infer that every serialized carrier is turn-resistant, because the shipped data never combines the two required flags.