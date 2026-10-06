# SAVE.DAT spell-ID resource lookup

Status: **executable-certified lookup mechanism and backing resource identity. Spell IDs from both persistent preparation matrices index the game-wide `T` text archive directly.**

This note continues the dual-family reconstruction in `SPELL_FAMILIES.md` by tracing the generated spell IDs beyond the matrices.

## Common spell/action record expansion

CODE.2 routine physical `0x5E0E4..0x5E2C8` expands ready spell copies from both persistent character matrices into runtime records.

For one selected spell-level bank it constructs two parallel ID bases:

```text
matrix A ID base = 0x42 + bank * 10
matrix B ID base = 0x92 + bank * 10
```

It walks ten entries. For each available cell with a nonzero ready count, it emits one runtime record per ready copy. The emitted record retains spell level/bank, entry index, character index, and family discriminator (`0` = matrix A, `1` = matrix B). Before those bytes are written, the routine calls physical `0x5728C` with the generated ID and destination buffer.

## Backing resource: `T`

The handle at DGROUP `0x22224` is now identified directly from the CODE.2 startup path.

At physical `0x5B3D7..0x5B3F9`, CODE.2 passes the data-object string at DGROUP offset `0x1148`, the one-character filename `T`/`t`, to the file-open wrapper at `0x6C1C4`, then stores the returned handle in DGROUP `0x22224`.

The project's independently reconstructed `Resources/MISC/T/FORMAT.md` identifies the supplied `T` file as a 1,073-record indexed text archive and proves the same reader semantics and the same runtime handle `0x22224`.

Therefore the spell IDs are **direct `T` record indexes**. There is no unidentified intermediate spell-text archive.

## Indexed reader at `0x5728C`

Routine physical `0x5728C..0x5732C` is the same indexed `T` reader documented by the resource reconstruction. Given destination and numeric index, it performs:

```text
seek(T, 4 + index * 4)
record_offset = read_u32(T)
seek(T, record_offset)
payload_length = read_u16(T)
read payload_length bytes
append runtime NUL
for each payload byte: byte ^= 0xDC
```

The supplied `T` corpus independently validates this physical format across all 1,073 active records:

```text
+0x0000  uint32 stored_count = 1073
+0x0004  uint32 offsets[1073]
...
record:
    uint16 encoded_length
    uint8  encoded_payload[encoded_length]
```

This cross-links SAVE spell state to the already-certified resource subsystem rather than duplicating a speculative format description.

## Spell ID geometry inside `T`

The matrix geometry maps directly onto `T` indexes:

```text
family A: T_index = 0x42 + bank*10 + entry
family B: T_index = 0x92 + bank*10 + entry
bank      = 0..7
entry     = 0..9
```

Each family reserves 80 consecutive `T` indexes, exactly matching one `8 x 10` SAVE.DAT matrix:

```text
matrix A reserved T interval: 0x42..0x91   (66..145)
matrix B reserved T interval: 0x92..0xE1   (146..225)
```

The intervals are contiguous. Storage capacity still must not be confused with shipped content: executable per-bank entry-count tables determine which cells/indexes are meaningful.

## Cross-corpus family evidence

Matrix A is independently certified as the scribeable spellbook family. The supplied OBJECTS.DAT corpus contains explicit `MAGE SCROLL OF ...` and `CLERIC SCROLL OF ...` item families, while CODE.2 keeps matrix A and matrix B distinct through runtime expansion. This strongly supports the expected mage/cleric split, but the serialized contract continues to call matrix B `family_b` until decoded `T` records in `0x92..0xE1` or an equivalent executable caller explicitly closes the label.

## Native-port requirement

A faithful port should preserve original `T` indexes at the import boundary. Do not renumber matrix entries before the original spell-list mapping is fully recovered. The pair `(family, bank, entry)` deterministically converts to the legacy `T` index with the formulas above.

The XOR step belongs to the `T` resource format, not SAVE.DAT itself.

## Next target

Decode and catalogue `T` records `0x42..0xE1` from the supplied `T` corpus, correlate meaningful indexes with executable entry-count tables and OBJECTS.DAT mage/cleric scroll names, then promote matrix B's exact family label and recover the shipped spell lists by level.
