# OBJECTS.DAT container-state indirection

Status: **category-1 `+0x31` container-state indirection and the runtime 10-byte descriptor layout are instruction-proven from certified CODE.2.** SAVE.DAT persistence of descriptors `0..61` is now cross-linked, including the shipped selector-62 mismatch.

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

## SAVE.DAT persistence

The SAVE serializer loops exactly over descriptor indexes `0..61`. For each it writes:

```text
2 * descriptor.slot_count bytes from descriptor.contents_pointer
1 byte from descriptor +9
```

The supplied executable descriptor table has 438 total slots across indexes `0..61`, so SAVE persists exactly:

```text
438 * 2 + 62 = 938 bytes = 0x3AA
```

See `DatFiles/SAVE.DAT/GLOBAL_STATE.md` for the complete per-index width/offset catalogue and quick-start contents.

## Shipped state-61 / selector-62 mismatch

The final shipped container definition exposes a real data/runtime mismatch:

```text
OBJECTS ID 329  QUIVER  +0x31 = 62
CODE.2 descriptor 61    valid 8-slot subtype-6 QUIVER descriptor
CODE.2 descriptor 62    10 zero bytes
SAVE persistence        indexes 0..61 only
quick-start state 61    eight arrow object IDs (57)
```

A follow-up direct-path audit found **no executable remap from selector 62 to descriptor 61**. Generic open-container code at physical CODE.2 `0x5C5B7` sign-extends OBJECTS `+0x31`, multiplies it by ten, and immediately reads descriptor `+4`; insertion/search/render paths use the same literal selector geometry. Thus object 329's stored selector reaches descriptor 62 as-is.

The one-step mismatch is consistent with a shipped static-data/off-by-one error, but that interpretation is not a license to rewrite the original data. Faithful compatibility should preserve selector 62, the zero descriptor 62 behavior, and serialized state 61 unless an intentional compatibility-fix layer is separately documented and enabled.

## Safe implementation boundary

For category-1 objects, expose:

```text
OBJECTS.DAT +0x31 -> container_state_index
```

and model the descriptor/content store as separate runtime/save state. Do not embed container contents into the static `0x41`-byte OBJECTS record, do not replace duplicated container definitions solely because their display names/graphics match, and do not silently normalize the shipped state-61/selector-62 mismatch.