# SAVE.DAT spell-ID resource lookup

Status: **executable-certified lookup mechanism, backing `T` resource identity, and spell-family class identities.** Matrix A is the Mage/scribeable spellbook family; matrix B is the shared Cleric/Paladin priest-style preparation family.

This note continues `SPELL_FAMILIES.md` by tracing generated spell IDs into the common text/resource index space.

## Common spell/action record expansion

CODE.2 physical `0x5E0E4..0x5E2C8` expands ready spell copies from both persistent character matrices into runtime records.

For one selected spell-level bank it constructs:

```text
Mage / matrix A ID base   = 0x42 + bank * 10
Priest / matrix B ID base = 0x92 + bank * 10
```

It walks ten entries. For each available cell with a nonzero ready count, it emits one runtime record per ready copy. The emitted record retains spell level/bank, entry index, character index, and family discriminator (`0` = Mage/matrix A, `1` = priest/matrix B). Before those bytes are written, it calls physical `0x5728C` with the generated numeric ID and destination buffer.

## Backing resource: `T`

At physical `0x5B3D7..0x5B3F9`, CODE.2 opens the one-character filename `T`/`t` and stores its handle at DGROUP `0x22224`. `Resources/MISC/T/FORMAT.md` independently reconstructs the supplied file as a 1,073-record indexed archive and proves the same reader/handle.

Therefore these spell IDs are direct **`T` record indexes**; there is no unidentified intermediate spell-name archive.

## Indexed reader at `0x5728C`

The reader performs:

```text
seek(T, 4 + index * 4)
record_offset = read_u32(T)
seek(T, record_offset)
payload_length = read_u16(T)
read payload_length bytes
append runtime NUL
for each payload byte: byte ^= 0xDC
```

The XOR transform belongs to `T`, not SAVE.DAT.

## Spell ID geometry inside `T`

```text
Mage family:   T_index = 0x42 + bank*10 + entry
Priest family: T_index = 0x92 + bank*10 + entry
bank           = 0..7
entry          = 0..9
```

Each matrix reserves 80 consecutive indexes:

```text
Mage / matrix A:   0x42..0x91  (66..145)
Priest / matrix B: 0x92..0xE1  (146..225)
```

Storage capacity is not the same as shipped content; executable per-bank entry-count tables determine which cells are meaningful.

## Family identity closure

The earlier conservative `family_b` label is superseded by executable class reconstruction.

Certified class codes are:

```text
1 Paladin
3 Cleric
4 Mage
```

Character initialization then provides direct matrix ownership:

- any class-code-4 slot activates casting-capability bit `0x02` and the matrix-A spellbook setup path;
- class code 3 activates bit `0x01` and calls `0x3ED08`, which reads/writes matrix B at runtime `0x2199A`;
- class code 1 activates bit `0x04` and calls `0x3EDC8`, which also reads/writes matrix B at runtime `0x2199A` with a different progression table.

Thus matrix B is the shared **Cleric/Paladin priest-style spell family**, not an unidentified second page. This also agrees with the static OBJECTS.DAT distinction between Mage and Cleric scroll families.

## Native-port requirement

Preserve original `T` indexes at the import boundary. Do not renumber spells until the complete original lists are recovered. The tuple `(family, bank, entry)` deterministically maps to the legacy index formulas above.

Recommended compatibility naming:

```cpp
LegacySpellCell mage_spellbook[8][10];
LegacySpellCell priest_spells[8][10];
```

## Next target

Decode/catalogue active `T` records `0x42..0xE1`, correlate them with the executable per-bank entry-count tables, and recover the exact shipped Mage and priest spell lists by level. Preserve unused reserved cells/indexes even when no shipped spell occupies them.