# SAVE.DAT spell-ID resource lookup

Status: **executable-certified lookup mechanism; spell IDs from both persistent preparation matrices are now traced through the common indexed resource reader. Exact family-B display names remain dependent on decoding the resource corpus rather than guessing from AD&D rules.**

This note continues the dual-family reconstruction in `SPELL_FAMILIES.md` by tracing the generated spell IDs beyond the matrices.

## Common spell/action record expansion

CODE.2 routine physical `0x5E0E4..0x5E2C8` expands ready spell copies from both persistent character matrices into runtime records.

For one selected spell-level bank it constructs two parallel ID bases:

```text
matrix A ID base = 0x42 + bank * 10
matrix B ID base = 0x92 + bank * 10
```

It then walks ten entries. For each cell whose low nibble is not `0xF` and whose high nibble is nonzero, it emits one runtime record per ready copy.

The emitted record stores:

```text
+0x1E  spell-level/bank argument
+0x1F  entry index
+0x20  character index
+0x21  family discriminator (0 = matrix A, 1 = matrix B)
```

Before those bytes are written, the routine calls physical `0x5728C` with the generated spell ID and the destination runtime record.

Therefore the `0x42` and `0x92` ranges are not merely UI numbering. They are direct keys into the executable's common spell/resource decoding path.

## Indexed resource reader at `0x5728C`

Routine physical `0x5728C..0x5732C` takes a destination buffer and numeric resource ID. Its access sequence is structurally clear:

1. Seek on handle stored at DGROUP `0x22224` to `resource_id * 4 + 4`.
2. Read a four-byte value from that table entry.
3. Seek the same handle to that retrieved value.
4. Read a two-byte payload length.
5. Read that many payload bytes into the destination buffer.
6. Append a NUL terminator.
7. XOR every payload byte with `0xDC` in place.

Equivalent reconstruction logic is:

```text
table_offset = 4 + resource_id * 4
payload_offset = read_u32(table_offset)
payload_length = read_u16(payload_offset)
payload = read_bytes(payload_offset + 2, payload_length)
for each byte in payload:
    byte ^= 0xDC
payload[payload_length] = 0
```

The exact file/compound-resource identity behind handle `0x22224` is still being audited. Do not treat an apparent DGROUP offset as a raw flat-file pointer without applying the project's certified LE object mapping and relocations.

## Consequence for spell-family identification

The exact display/action identity of a spell is obtained through this indexed resource mechanism. Thus family B should not be named from rules knowledge alone: the canonical path is to recover the resource attached to `0x22224`, decode IDs beginning at `0x92`, and compare those decoded entries with the `0x42` family.

This also explains why searching CODE.2's ordinary plaintext string corpus is insufficient to enumerate the spell lists. Generic spell UI strings are embedded in CODE.2, but the per-ID records consumed by `0x5728C` are length-prefixed and XOR-decoded at runtime.

## ID geometry

For each family the persistent matrix geometry and resource-ID geometry agree exactly:

```text
family A: id = 0x42 + bank*10 + entry
family B: id = 0x92 + bank*10 + entry
bank      = 0..7
entry     = 0..9
```

Each family therefore reserves 80 consecutive IDs, matching its `8 x 10` SAVE.DAT matrix exactly:

```text
matrix A reserved ID interval: 0x42..0x91
matrix B reserved ID interval: 0x92..0xE1
```

The intervals are contiguous: matrix B begins immediately after matrix A's complete 80-ID reservation. This is additional structural evidence that the two 80-byte SAVE matrices are serialized mirrors of two adjacent engine spell-resource families.

Storage capacity still must not be confused with shipped content. Per-bank entry-count tables determine which cells/IDs are meaningful.

## Native-port requirement

A faithful port should keep legacy spell IDs stable at the import boundary. Do not renumber matrix entries before the original resource mapping is fully recovered. The pair `(family, bank, entry)` can always be converted to the original ID using the formulas above.

The resource reader's XOR step is part of the original data format and should be documented/implemented explicitly rather than represented as unexplained transformed text.

## Next target

Identify the resource opened into DGROUP `0x22224`, reconstruct its index header, and decode representative IDs from `0x42` and `0x92`. Those decoded records should provide the independent evidence needed to assign matrix B's canonical spell-family label and enumerate actual spell entries by level.
