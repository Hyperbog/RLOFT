# LEVELS.DAT 10-byte cell records

Status: **record-group split, coordinates, several control bits, dynamic-door fields, and kind-specific handlers are instruction-proven from certified CODE.2; unresolved bytes remain raw.**

The archive stores one width-indexed collection of 10-byte records. The loader partitions that collection into two runtime groups:

```text
DGROUP 0xCB3C  group 1: archive byte +0 != 1
DGROUP 0xCD3C  group 2: archive byte +0 == 1
```

Standalone `LEVnnn.DAT` files serialize the two groups separately, with byte `0xCF50` between them.

## Common layout proved so far

```text
+0x00  uint8 kind/control discriminator
+0x01  uint8 control/state flags
+0x02  int8  event/script selector on proved kind-2 and kind-4 paths
+0x03  uint8 unresolved
+0x04  uint8 row
+0x05  uint8 column
+0x06  uint8 kind-specific state/resource field
+0x07  int8/uint8 kind-specific state; signed animation delta for group 2
+0x08  uint8 unresolved
+0x09  uint8 kind-specific field; resource/mode argument on kind 10
```

Bytes `+4/+5` are independently validated across every supplied archive record: all 1,659 records have `+5` equal to their owning column and `+4` inside map height.

## Byte +1 control field

Multiple independent consumers prove that byte `+1` is a bitfield rather than an undifferentiated value.

### Low two bits `0x03`

`1E9A0` returns `record[1] & 0x03` for group-1 records and first-person rendering uses that value as a small state selector.

For group-2 records, `20C0C` uses the same low-two-bit field as an interaction mode. Mode 1 toggles the dynamic position between endpoints 0 and 8; mode 2 drives the record toward endpoint 0 only.

### Bit `0x04`: interaction eligibility

The kind-1, kind-2, and kind-4 interaction handlers all reject records when this bit is clear. Other CODE.2 paths temporarily save this bit, set it for a call, then restore the previous state. The safest proved description is therefore **interaction-enable / eligibility gate**; it is not globally named `unlocked` or similar.

### Bit `0x08`: kind-3 inclusion modifier

Renderer/geometry scan `34F30` first requires bit `0x10`. If record kind is 3, it additionally requires bit `0x08` before admitting that record into the traced geometry path. No broader gameplay name is assigned.

### Bit `0x10`: active continuation/render gate

The group-1 serializer treats a record with kind zero as still valid when bit `0x10` is set:

```text
continue while record[0] != 0 OR (record[1] & 0x10) != 0
```

The geometry scan at `34F30` also requires this bit before processing a located record. This proves a persistent active/render/continuation role, while the exact editor-facing name remains unresolved.

### Bit `0x40`: interaction suppression

The kind-1 interaction path explicitly refuses records carrying bit `0x40`. Separate code sets this bit and another path clears it, proving a mutable suppression/inhibit state. Its higher gameplay reason remains unresolved.

Bits without an independently traced behavior remain unnamed.

## Kind dispatcher

The interaction dispatcher around `22B09` selects on `record[0] - 1` through a ten-entry jump table. The supplied archive uses kinds:

```text
kind   records
0        358
1        324
2        175
3        381
4         43
5          1
6         55
7          2
8        311
10         9
```

Kind 9 is accepted by the encoded dispatcher range but does not occur in the supplied archive data.

Proved handlers:

```text
kind 1   -> 20C0C dynamic barrier/door interaction
kind 2   -> 210B0 interactive two-state toggle
kind 4   -> 20E08 conditional interaction/event path
kind 10  -> passes byte +9 to 21DB4 as a resource/mode argument
```

Kinds 3, 5, 6, 7, 8, and 9 fall through the examined interaction dispatcher, but some participate in other render/geometry/update paths. Absence of an action in this dispatcher is not evidence that the kind has no gameplay function.

### Kind 2

`210B0` requires control bit `0x04`, derives a rendering/resource response from the cell's six-bit code, toggles the low two control bits with XOR 1, and calls the common event/script dispatcher `02A8` with signed byte `+2`.

Thus byte `+2` is an event/script selector on this path and low bits `+1 & 3` are persistent two-state state.

### Kind 4

`20E08` also requires control bit `0x04`. It performs location/resource eligibility tests, consumes byte `+6` through a dedicated accepted-ID predicate, and on successful paths calls `02A8` with signed byte `+2` before clearing bit `0x04`.

The higher gameplay identity of kind 4 and byte `+6` remains unresolved.

### Kind 10

The dispatcher passes record byte `+9` to `21DB4`, which changes an engine mode and loads the requested resource. The byte is therefore a proved kind-10 resource/mode selector, but that interpretation must not be generalized to other kinds.

## Group-2 dynamic fields

For kind 1 / runtime group 2, CODE.2 proves:

```text
+0x06  uint8 position, persistent endpoints 0 and 8
+0x07  int8  animation delta, normally -1, 0, +1
```

Traversal behavior proves endpoint 0 is open/passable and endpoint 8 is closed/blocking; intermediate positions 1..7 still block on the traced path. See `DYNAMIC_CELL_RECORDS.md` for the complete animation proof.

## Archive-wide control-byte distribution

Across all 1,659 supplied archive records:

```text
0x00 : 353
0x01 : 224
0x04 : 584
0x05 :  99
0x0C :  36
0x10 : 360
0x14 :   2
0x30 :   1
```

By kind:

```text
kind 0   0x10:357  0x30:1
kind 1   0x01:224  0x05:99  0x00:1
kind 2   0x04:174  0x00:1
kind 3   0x04:340  0x0C:36  0x14:2  0x00:3
kind 4   0x04:43
kind 5   0x10:1
kind 6   0x04:27   0x00:28
kind 7   0x10:2
kind 8   0x00:311
kind 10  0x00:9
```

These counts describe the supplied data set and are not format constraints.

## Database/type-family cross-check

The game loads `NPCS.DAT` through CODE.2 `16810` as `count * 0x45` bytes into runtime pointer `0x22230`. Family-A expansion at `186B9` indexes that exact pointer as `type_id * 0x45`, directly proving that family A's type namespace is `NPCS.DAT`. All 1,258 supplied family-A records use IDs `0..82`, within the 83-entry NPC database.

`OBJECTS.DAT` is independently loaded by `168C8` as `count * 0x41` bytes into `0x255D8`. Family-B and family-C type IDs both lie inside the 336-entry object database range in the supplied corpus; family B uses IDs `0..174` and family C `0..335`. Several family-B consumers compare IDs whose `OBJECTS.DAT` names are item/key types (for example type `0x9B`, `BLOOD KEY`). This is strong object/item-namespace corroboration, but the precise distinction between family B and family C remains unresolved and is not collapsed into one record type.

Use `analyze_cell_records.py` to reproduce the archive-wide kind/control distributions and database-range checks.

## Safe implementation boundary

A reader/editor can expose:

- `kind` at byte `+0`;
- raw `control_flags` at `+1`, plus proved bit views;
- signed `event_selector` at `+2` only for kinds 2 and 4;
- `row` / `column` at `+4/+5`;
- dynamic `position` / `delta` at `+6/+7` for kind 1;
- kind-10 `resource_or_mode` at `+9`.

All other bytes and unproved flag bits must remain losslessly preserved.