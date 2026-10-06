# OBJECTS.DAT container-state indirection

Status: **category-1 `+0x31` container-state indirection and the runtime 10-byte descriptor layout are instruction-proven from certified CODE.2.**

`OBJECTS.DAT` does not embed mutable container contents in its 65-byte object definitions. For `item_category == 1`, signed byte `+0x31` selects a mutable runtime descriptor in the table beginning at DGROUP `0x7330`.

This distinction is essential for a faithful port/editor: the object definition identifies which container instance/state slot is used; the live contents and per-slot state live elsewhere.

## Runtime descriptor layout

CODE.2 multiplies the signed `+0x31` value by ten throughout the container UI, burden, insertion, removal and recursive-search routines. The selected 10-byte descriptor is:

```text
+0x00  uint8  slot_count
+0x01  uint8  ui_columns
+0x02  uint8  capacity_pounds
+0x03  uint8  ui_x_offset
+0x04  uint8  container_subtype
+0x05  uint32 contents_pointer
+0x09  uint8  per_slot_state_bits
```

### `slot_count`

`0F390`, `0F40C`, `0F5F8`, `10AF6..10BBB`, `11404..11626` and related paths loop exactly this many 16-bit object IDs through `contents_pointer`. Empty slots use object ID `-1` (`0xFFFF`).

### `ui_columns`

The container renderer divides the slot index by byte `+1`; quotient selects the vertical row and remainder selects the horizontal column. It is therefore the inventory-grid column count.

### `capacity_pounds`

Insertion logic at `1157B..115E2` computes current/prospective recursive item weight in OBJECTS half-pound units, loads descriptor byte `+2`, doubles it, and rejects the insertion when the half-pound total exceeds that doubled value.

Thus descriptor `+2` is capacity in **whole pounds**.

Representative supplied values:

```text
POUCH            8 lb
SACK            30 lb
CHEST          100 lb
SLING POUCH    127 lb
SCROLL CASE    127 lb
KEY RING       127 lb
QUIVER         127 lb
BAG OF HOLDING 255 lb
```

The 127/255 values function as very high limits for specialized/magical containers; no stronger original-source sentinel name is assigned.

### `ui_x_offset`

The renderer adds descriptor byte `+3` to the per-character/container panel base before laying out slots. It is a UI placement offset. Its exact original editor-facing name is not known.

### `container_subtype`

Descriptor byte `+4` controls insertion compatibility. The supplied descriptors establish:

```text
0  POUCH
1  SACK / BAG-OF-HOLDING behavior family
2  CHEST
3  SLING POUCH
4  IVORY SCROLL CASE
5  KEY RING
6  QUIVER
```

The specialized gates are exact:

```text
subtype 3 -> item_category 13  stone/sling-ammunition family
subtype 4 -> item_category 14  spell-scroll family
subtype 5 -> item_category 12  key family
subtype 6 -> item_category 15  arrow family
```

Subtypes 0/1/2 follow broader generic-container rules. The subtype-0/1 path additionally rejects an inserted object when the object's `attack_profile & 0x03` is nonzero. The gameplay/editor terminology for that low-two-bit restriction remains intentionally unresolved.

### `contents_pointer`

Descriptor `+5` is a 32-bit pointer to the actual 16-bit object-ID slot array. Recursive search, burden and container UI code all dereference it directly.

### `per_slot_state_bits`

Descriptor byte `+9` is addressed as a bitset indexed by container slot. Helpers `0F274` and `0F290` read/write individual bits. Item-transfer paths preserve/update the bit associated with the moved slot. The exact higher-level state represented by this bit is not yet named.

## OBJECTS.DAT instance mapping

Every one of the 62 supplied category-1 object definitions has a distinct `+0x31` value. This proves that duplicated definitions such as repeated CHEST, POUCH, QUIVER and SCROLL CASE records represent separate mutable container-state identities.

Observed referenced state indexes are:

```text
0..60 except 61, plus 62
```

Index 20 belongs to `BAG OF HOLDING`.

The examined certified CODE.2 data image has a zero-filled descriptor at index 62 even though object ID 329 (`QUIVER`) references it. This is preserved as an evidence point, not normalized or silently repaired. Arbitrary tools must not infer descriptor validity solely from the numeric state-index range.

## Safe implementation boundary

For category-1 objects, expose:

```text
OBJECTS.DAT +0x31 -> container_state_index
```

and model the descriptor/content store as separate runtime/save state. Do not embed container contents into the static 0x41-byte OBJECTS record, and do not replace duplicated container definitions solely because their display names and graphics match.