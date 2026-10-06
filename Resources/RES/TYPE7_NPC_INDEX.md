# RES type-7 NPC index mapping

Status: **all 83 `NPCS.DAT` definitions now have a direct archive-3/type-7 base index mapping.**

## Executable proof

CODE.2 `2B738..2B776` indexes the selected `NPCS.DAT` record, reads word `+0x14`, and copies it into generated 0x16-byte scene-render record field `+0x04`. Animation/orientation paths at `2B77A..2BA4F` add frame offsets to that field using the NPC definition's `+0x18..+0x1A` animation parameters and live state.

The first-person renderer later calls `36CEC` from `2C4E1`, `2C524`, `2C5AB`, `2C5FF`, `2C632`, `2C665`, and `2C6B6`. Its second argument is the scene record's `+0x04` value (sometimes with a small renderer-specific adjustment), and `36E94..36EA5` passes that argument directly to resource accessor `2F29C` with archive number **3**.

Therefore `NPCS.DAT +0x14` is specifically the **base archive-3 type-7 compiled-shape index**. The existing project name `scene_sprite_base` remains visually intuitive, but the underlying on-disk resource is the compiled-shape/display-list format documented in `TYPE7_RENDERER.md`.

Every supplied base index below lies inside `RES3`'s 4,234-entry all-type-7 archive. The shape column reports the geometry header of the base resource only; animation/state code can select later IDs relative to that base.

| NPC | Name | Base type-7 ID | Base shape | Frame stride | Cycle divisor | Span |
|---:|---|---:|---|---:|---:|---:|
| 0 | ZOMBIE | 0 | 64w; x 9..46; y 18..128 | 8 | 4 | 3 |
| 1 | DOOM GUARD | 40 | 64w; x 13..56; y 8..128 | 8 | 4 | 3 |
| 2 | WEREBAT | 80 | 64w; x 4..58; y 4..128 | 8 | 4 | 3 |
| 3 | PYRE ELEMENTAL | 120 | 64w; x 0..62; y 2..128 | 1 | 3 | 1 |
| 4 | SKELETON | 127 | 64w; x 6..46; y 19..128 | 8 | 4 | 3 |
| 5 | WORG | 167 | 96w; x 25..67; y 33..128 | 8 | 4 | 3 |
| 6 | SKELETAL BAT | 207 | 64w; x 1..62; y 0..128 | 8 | 3 | 3 |
| 7 | HELLHOUND | 239 | 64w; x 18..44; y 64..128 | 8 | 4 | 1 |
| 8 | STRAHD | 279 | 128w; x 15..119; y 0..128 | 1 | 4 | 6 |
| 9 | ASSASSIN | 315 | 64w; x 15..51; y 27..128 | 8 | 4 | 3 |
| 10 | SHADOW | 355 | 64w; x 12..50; y 17..128 | 8 | 4 | 3 |
| 11 | ZOMBIE LORD | 395 | 64w; x 12..49; y 17..128 | 8 | 4 | 3 |
| 12 | VAMPIRE | 435 | 64w; x 6..59; y 24..128 | 8 | 3 | 3 |
| 13 | TREANT | 467 | 64w; x 0..63; y 0..128 | 8 | 4 | 3 |
| 14 | GARGOYLE | 507 | 64w; x 10..54; y 11..128 | 8 | 4 | 3 |
| 15 | BANSHEE | 547 | 64w; x 4..57; y 24..128 | 8 | 4 | 3 |
| 16 | GHAST | 587 | 64w; x 5..49; y 37..128 | 8 | 4 | 3 |
| 17 | GHOUL | 627 | 64w; x 5..49; y 37..128 | 8 | 4 | 3 |
| 18 | GHOUL LORD | 667 | 64w; x 5..49; y 29..128 | 8 | 4 | 3 |
| 19 | INQUISITOR | 707 | 64w; x 15..50; y 15..128 | 8 | 4 | 3 |
| 20 | SPECTRE | 747 | 64w; x 1..60; y 24..128 | 8 | 4 | 3 |
| 21 | WEREWOLF | 787 | 64w; x 11..52; y 20..128 | 8 | 4 | 3 |
| 22 | WRAITH | 827 | 64w; x 17..44; y 25..128 | 8 | 4 | 3 |
| 23 | BONE GOLEM | 867 | 64w; x 10..56; y 7..128 | 8 | 4 | 3 |
| 24 | BRIGAND | 907 | 64w; x 15..51; y 27..128 | 8 | 4 | 3 |
| 25 | BROKEN ONE | 947 | 64w; x 3..62; y 40..128 | 8 | 2 | 3 |
| 26 | DARKLING | 971 | 64w; x 15..51; y 27..128 | 8 | 4 | 3 |
| 27 | GOBLYN | 1011 | 64w; x 8..53; y 33..128 | 8 | 4 | 3 |
| 28 | REVENANT | 1051 | 64w; x 9..45; y 37..128 | 8 | 4 | 3 |
| 29 | SHADOW FIEND | 1091 | 64w; x 9..49; y 2..128 | 8 | 4 | 3 |
| 30 | WIGHT | 1131 | 64w; x 9..50; y 37..128 | 8 | 4 | 3 |
| 31 | ZOMBIE GOLEM | 1171 | 64w; x 4..55; y 3..128 | 8 | 4 | 3 |
| 32 | WERERAT | 1211 | 64w; x 17..46; y 43..128 | 8 | 4 | 3 |
| 33 | LIVING WALL | 1251 | 64w; x 32..33; y 0..0 | 8 | 3 | 1 |
| 34 | MAN IN WOODS | 1277 | 64w; x 21..41; y 23..126 | 8 | 7 | 0 |
| 35 | BRIGAND | 1309 | 64w; x 16..49; y 9..128 | 8 | 7 | 0 |
| 36 | VISTANI | 1317 | 64w; x 8..45; y 21..127 | 8 | 4 | 0 |
| 37 | BRAY | 1341 | 64w; x 21..42; y 21..128 | 8 | 8 | 0 |
| 38 | TATYANA | 1381 | 64w; x 15..39; y 44..128 | 8 | 8 | 0 |
| 39 | PRISONER CHAINED | 1421 | 64w; x 6..48; y 44..125 | 8 | 7 | 0 |
| 40 | PRISONER UNCHAINED | 1453 | 64w; x 6..48; y 44..128 | 8 | 7 | 0 |
| 41 | PRIEST KNEELING | 1485 | 64w; x 19..43; y 40..128 | 8 | 1 | 0 |
| 42 | PRIEST STANDING | 1493 | 64w; x 18..44; y 18..128 | 8 | 7 | 0 |
| 43 | GHOST | 1493 | 64w; x 18..44; y 18..128 | 8 | 4 | 0 |
| 44 | CLERIC | 1517 | 64w; x 20..42; y 21..128 | 8 | 7 | 0 |
| 45 | ELVEN MAGE | 1541 | 64w; x 17..47; y 36..128 | 8 | 7 | 0 |
| 46 | GRAVEROBBER OUT | 1565 | 64w; x 14..46; y 29..128 | 8 | 8 | 0 |
| 47 | GRAVEROBBER TAVERN | 1589 | 64w; x 22..51; y 22..128 | 8 | 8 | 0 |
| 48 | ADVENTURER | 1613 | 64w; x 20..44; y 28..128 | 1 | 0 | 0 |
| 49 | STRAHDS PRISONER | 1637 | 64w; x 17..49; y 9..128 | 8 | 8 | 0 |
| 50 | BURGOMASTER | 1653 | 64w; x 12..54; y 17..128 | 8 | 8 | 0 |
| 51 | WERERAVEN | 1693 | 64w; x 15..46; y 2..128 | 1 | 4 | 0 |
| 52 | PYOOR | 1697 | 64w; x 14..46; y 17..128 | 8 | 8 | 0 |
| 53 | ELVEN WARRIOR | 1713 | 64w; x 20..42; y 20..128 | 8 | 8 | 0 |
| 54 | MEMBER OF KEEPERS | 1769 | 64w; x 16..49; y 9..128 | 1 | 0 | 0 |
| 55 | WEREWOLF | 1777 | 64w; x 10..53; y 26..127 | 8 | 8 | 0 |
| 56 | MAD OLD WOMAN | 1793 | 64w; x 24..43; y 34..128 | 1 | 8 | 0 |
| 57 | MAGIC MIRROR WHOLE | 1797 | 64w; x 0..63; y 0..128 | 8 | 1 | 0 |
| 58 | MAGIC MIRROR BROKE | 1805 | 64w; x 0..63; y 0..128 | 8 | 1 | 0 |
| 59 | STATUE OF MAIDEN 1 | 1813 | 64w; x 15..47; y 0..128 | 8 | 1 | 0 |
| 60 | STATUE OF MAIDEN 2 | 1821 | 64w; x 15..47; y 0..128 | 8 | 1 | 0 |
| 61 | STRAHD DISGUISED | 1829 | 64w; x 8..50; y 10..128 | 8 | 2 | 0 |
| 62 | FINAL STRAHD | 1829 | 64w; x 8..50; y 10..128 | 1 | 0 | 0 |
| 63 | DANDY WOMAN | 1845 | 64w; x 16..50; y 3..128 | 8 | 1 | 0 |
| 64 | MAGE | 1853 | 64w; x 16..47; y 26..128 | 8 | 3 | 0 |
| 65 | OLD PALADIN | 1877 | 64w; x 6..48; y 50..128 | 8 | 2 | 0 |
| 66 | VISTANI INSIDE | 1317 | 64w; x 8..45; y 21..127 | 8 | 1 | 0 |
| 67 | ELVEN MAGE IN BAR | 1541 | 64w; x 17..47; y 36..128 | 8 | 7 | 0 |
| 68 | STRAHDS PRIS OUT | 1637 | 64w; x 17..49; y 9..128 | 8 | 8 | 0 |
| 69 | DANDY WOMAN IN BAR | 1845 | 64w; x 16..50; y 3..128 | 8 | 1 | 0 |
| 70 | MAN IN WOOD IN BAR | 1277 | 64w; x 21..41; y 23..126 | 8 | 7 | 0 |
| 71 | GOON | 1893 | 64w; x 18..46; y 20..128 | 8 | 1 | 0 |
| 72 | MAN SITTING | 1901 | 64w; x 16..47; y 35..128 | 8 | 2 | 0 |
| 73 | OLD MAN SITTING | 1917 | 64w; x 16..47; y 37..128 | 8 | 2 | 0 |
| 74 | BOGUS WITH STAFF | 1933 | 64w; x 15..51; y 28..126 | 8 | 4 | 0 |
| 75 | ATMOSPHERE BAT | 1965 | 64w; x 21..42; y 15..128 | 8 | 8 | 0 |
| 76 | ATMOSPHERE RAT | 2029 | 64w; x 29..33; y 112..128 | 8 | 4 | 0 |
| 77 | GYPSY | 2061 | 64w; x 17..50; y 31..128 | 8 | 4 | 0 |
| 78 | DOOM GUARD STATUE | 40 | 64w; x 13..56; y 8..128 | 8 | 0 | 0 |
| 79 | OLD WOMAN SITTING | 2093 | 64w; x 16..47; y 35..128 | 8 | 2 | 0 |
| 80 | ALBINO WEREWOLF | 2109 | 64w; x 11..52; y 20..128 | 8 | 4 | 3 |
| 81 | ATMOSPHERE CROW | 2149 | 64w; x 29..33; y 104..128 | 1 | 8 | 0 |
| 82 | ADVENTURER | 1613 | 64w; x 20..44; y 28..128 | 1 | 0 | 0 |

## Notable sharing

Several definitions intentionally share the same type-7 base, proving that the base is a reusable visual-resource reference rather than a one-record/one-resource identity. Examples include:

- `DOOM GUARD STATUE` and `DOOM GUARD` -> base 40;
- `VISTANI INSIDE` and `VISTANI` -> base 1317;
- `ELVEN MAGE IN BAR` and `ELVEN MAGE` -> base 1541;
- `STRAHDS PRIS OUT` and `STRAHDS PRISONER` -> base 1637;
- `DANDY WOMAN IN BAR` and `DANDY WOMAN` -> base 1845;
- `MAN IN WOOD IN BAR` and `MAN IN WOODS` -> base 1277;
- `FINAL STRAHD` and `STRAHD DISGUISED` -> base 1829;
- both `ADVENTURER` definitions -> base 1613.

## Preservation consequence

A modern loader can resolve a live NPC's base scene shape without heuristic asset matching:

```text
npc_definition = NPCS.DAT[npc_type]
shape_id        = npc_definition.scene_sprite_base + executable-defined frame/state offset
shape           = RES3[shape_id]   // EH type 7
```

The frame/state offset logic remains part of CODE.2 and must be reproduced rather than assuming a fixed frame count from the distance between neighboring base IDs.
