# LEVELS.DAT fixed-header tail controls

Status: **the active fields at header `+0x290..+0x29C` are behaviorally decoded from immutable CODE.2 and checked against all 32 supplied archive records. Higher gameplay/editor names remain conservative where the executable proves only renderer/resource behavior.**

The fixed level header is loaded verbatim to DGROUP `0x25618`:

```text
header +0x290 -> DGROUP 0x258A8
header +0x292 -> DGROUP 0x258AA
header +0x294 -> DGROUP 0x258AC
header +0x296 -> DGROUP 0x258AE
header +0x298 -> DGROUP 0x258B0
header +0x299 -> DGROUP 0x258B1
header +0x29A -> DGROUP 0x258B2
header +0x29C -> DGROUP 0x258B4
```

The immutable CODE.2 object-1 SHA-256 was reverified as `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3` while closing these fields.

## Proven layout

```text
+0x290  uint8  environment_profile_id_or_FF
+0x291  uint8  unresolved_zero_in_supplied_archive
+0x292  int16  special_npc_type[0]
+0x294  int16  special_npc_type[1]
+0x296  int16  special_npc_type[2]
+0x298  uint8  renderer_fill_base_A
+0x299  uint8  renderer_fill_base_B
+0x29A  int16  render_threshold_floor
+0x29C  uint8  resource_set_id
+0x29D..+0x2A3 unresolved; all zero in supplied archive
```

These labels are behavioral. `environment_profile_id_or_FF`, for example, is deliberately not renamed to a specific outdoor/indoor/sky concept without stronger proof.

## `+0x290`: environment/render profile byte with `0xFF` sentinel

CODE.2 reads this field strictly as a byte. The supplied archive contains only:

```text
0x00 : 10 levels
0x01 :  1 level
0xFF : 21 levels
```

Independent consumers establish that it selects a renderer/environment profile when it is not `0xFF`, while `0xFF` selects an alternate/default path:

- `01558` uses the `0xFF` distinction when selecting automap floor/party class `0x2E` versus `0x2F`.
- `04A52`, `0D6BB`, `0E621`, and multiple later interaction/render paths branch on `== 0xFF`.
- `1F2B1` uses a non-`0xFF` value as an index for renderer/profile setup through `314B0` and tables at `0x2B750/0x2B764`.
- during level load, if the field is `0xFF`, CODE.2 obtains two fallback values from a level-indexed table and passes them through `31EDC` and `31F38`; otherwise that fallback setup is skipped.

The safe interpretation is an **environment/render profile selector with `0xFF` meaning use the alternate/default level path**.

## `+0x292/+0x294/+0x296`: three NPC type filters

CODE.2 repeatedly treats these as three signed 16-bit entries:

```text
for i in 0..2:
    filter = int16(header[0x292 + i*2])
```

The values are compared directly against family-A runtime record type IDs (`record +0x0E`). Family A independently indexes `NPCS.DAT`, so these fields are safely identified as **NPC/creature type filters**.

Independent consumers include:

- `04228`, which scans family-A world records against a selected filter;
- `0E50F`, which compares the current family-A NPC type against all three entries before later record-state handling;
- `2D542`, which again compares a family-A type ID against the three-entry table.

The dominant sentinel is signed `-1` (`0xFFFF`), which cannot match any supplied NPC type ID `0..82`.

Archive value sets:

```text
special_npc_type[0] : -1, 0, 1, 4, 5, 6, 7, 12, 32
special_npc_type[1] : -1, 0, 1, 17, 21
special_npc_type[2] : -1, 0, 1
```

## `+0x298/+0x299`: two independent renderer fill-base bytes

Although word-aligned as a pair, CODE.2 consumes these as **two independent bytes**, not one semantic 16-bit value.

Routine `1F2B1` passes:

- byte `0x258B0` to `33E28`;
- byte `0x258B1` to `33DE0`.

Those certified graphics routines perform analogous VGA-memory fill operations. Each subtracts a per-row byte from table `FE94` and fills 64-byte spans in distinct framebuffer regions (`+0x148` and `+0x50F8`). The exact visual/editor-facing identities of the two planes remain unresolved, so `renderer_fill_base_A/B` is intentionally behavioral.

The low byte also has a special `== 0x2C` branch in `1F2B1`; unlike the earlier provisional corpus note, `0x2C` **does occur** in the supplied archive and is the most common low-byte value (9/32 levels).

## `+0x29A`: normalized render-threshold floor

This field explains the only byte difference between archive level 22's fixed header and supplied `LEV022.DAT`.

Immediately after loading a level, CODE.2 executes:

```text
if (int16(0x258B2) == 0)
    0x258B2 = 1;
```

The standalone writer later serializes the live `0x2A4` header verbatim. Therefore:

```text
archive level 22 +0x29A = 0
runtime                 = 1
LEV022.DAT      +0x29A = 1
```

This is a **proved loader normalization**, not unexplained save-state drift.

The value feeds renderer threshold byte `0xC02D` through the encoded calculation used after level load and by the corresponding options/control routine:

```text
threshold = 128 - encoded_div128(user_value * (128 - header_floor))
```

where `encoded_div128` preserves the binary's signed bias/shift behavior. Later rendering code compares per-render-record values against `0xC02D`. The safe name is therefore `render_threshold_floor`; a stronger lighting/fog/visibility label is not assigned yet.

Archive distribution:

```text
0  : 14 levels
1  : 12 levels
50 :  6 levels
```

Every archived zero becomes one in live state after load.

## `+0x29C`: resource-set identifier

CODE.2 consumes byte `0x258B4` as a resource-set identifier:

- `1831A` compares it against the current resource identifier (`29050`) immediately after the header load and reloads resources when current state does not match;
- `18C13..18C4C` repeats the same comparison and invokes the resource-loading sequence with this byte;
- `0E4B9` and options/control path `06681` pass it to the same resource-management family.

The supplied archive uses values `0..45` with substantial reuse. `resource_set_id` is a safe behavioral name; mapping those IDs to named RES content belongs in later resource-file reconstruction.

## Remaining seven bytes

No direct absolute CODE.2 consumer was found for header byte `+0x291` or bytes `+0x29D..+0x2A3`, and all 32 supplied archive records store zero in those positions. They must still be preserved verbatim; archive-wide zero is an observation, not a format rule.

## Safe implementation boundary

A reader/repacker can expose these fields while preserving the full raw header. In particular:

- do not combine `+0x298/+0x299` into one semantic word;
- preserve the `0xFF` environment-profile sentinel exactly;
- preserve all three signed NPC-filter words, including `-1`;
- distinguish serialized archive state from loader-normalized live state for `+0x29A`;
- do not zero currently unused/reserved bytes when round-tripping arbitrary inputs.
