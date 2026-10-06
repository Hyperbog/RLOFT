# LEVELS.DAT fixed-header tail controls

Status: **the active fields at header `+0x290..+0x29C` are behaviorally decoded from immutable CODE.2 and checked across all 32 supplied archive records. Higher gameplay/editor names remain conservative where the executable only proves renderer/resource behavior.**

The fixed level header is loaded verbatim to DGROUP `0x25618`. Therefore the tail addresses are:

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

These labels are behavioral. In particular, `environment_profile_id_or_FF` is not being promoted to a stronger outdoor/indoor or sky-type name without direct proof.

## `+0x290`: environment/render profile byte with `0xFF` sentinel

CODE.2 reads this field strictly as a byte. The supplied archive contains only:

```text
0x00 : 17 levels
0x01 :  2 levels
0xFF : 13 levels
```

Independent consumers establish that it selects a renderer/environment profile when it is not `0xFF`, while `0xFF` selects an alternate/default path:

- `01558` uses only the `0xFF` distinction when selecting automap floor/party class `0x2E` versus `0x2F`.
- `04A52`, `0D6BB`, `0E621`, and multiple later interaction/render paths branch on `== 0xFF`.
- `1F2B1` uses a non-`0xFF` value as an index passed to the renderer/profile setup path through `314B0` and its tables at `0x2B750/0x2B764`.
- during level load, if the field is `0xFF`, CODE.2 obtains two fallback values from a level-indexed table and passes them through `31EDC` and `31F38`; otherwise that fallback setup is skipped.

Thus the safe current interpretation is an **environment/render profile selector with `0xFF` meaning use the alternate/default level path**.

## `+0x292/+0x294/+0x296`: three NPC type filters

CODE.2 repeatedly treats these as three signed 16-bit entries:

```text
for i in 0..2:
    filter = int16(header[0x292 + i*2])
```

The values are compared directly against family-A runtime record type IDs (`record +0x0E`). Family A is independently proved to index `NPCS.DAT`, so these three fields are safely identified as **NPC/creature type filters**.

Two independent consumers support this:

- `04228` scans family-A world records and compares each NPC type against one selected filter.
- `0E50F` compares the current family-A NPC type against all three filters before applying its subsequent record-state logic.
- `2D542` again compares a family-A type ID against the three-entry table.

The common value `0x00FF` therefore acts as an out-of-database/nonmatching value for the supplied 83-entry `NPCS.DAT`; no stronger sentinel semantics are required.

Archive value sets:

```text
special_npc_type[0] : 0, 4, 7, 14, 15, 16, 255
special_npc_type[1] : 1, 2, 14, 255
special_npc_type[2] : 2, 3, 13, 255
```

## `+0x298/+0x299`: two independent renderer fill-base bytes

Although naturally word-aligned, CODE.2 consumes these as **two separate bytes**, not one 16-bit value.

Routine `1F2B1` passes:

- byte `0x258B0` to `33E28`;
- byte `0x258B1` to `33DE0`.

Those two certified graphics routines perform analogous VGA-memory fill operations. Each takes its input byte, subtracts a per-row value from table `FE94`, and fills 64-byte spans in distinct framebuffer regions (`+0x148` and `+0x50F8`). The exact visual/editor-facing names of the two planes remain unresolved, so `renderer_fill_base_A/B` is intentionally behavioral.

The low byte also has a special `== 0x2C` branch in `1F2B1`; that branch is executable-supported even though none of the 32 supplied archive levels uses `0x2C` there.

## `+0x29A`: normalized render-threshold floor

This field explains the only byte difference previously observed between archive level 22's fixed header and supplied `LEV022.DAT`.

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

The value feeds the renderer threshold byte at `0xC02D` through the encoded calculation used both after level load and by the corresponding options/control routine:

```text
threshold = 128 - round_signed(user_value * (128 - header_floor) / 128)
```

(`CODE.2` implements the exact signed bias/shift sequence rather than a source-level `round` call.)

Later rendering code compares per-render-record values against `0xC02D`. The safe name is therefore `render_threshold_floor`; a stronger lighting/fog/visibility label is not assigned yet.

Archive distribution is `0` in 31 levels and `1` in one level. After the loader normalization, a zero archive value becomes one in live state.

## `+0x29C`: resource-set identifier

CODE.2 consumes the low byte at `0x258B4` as a resource-set identifier:

- immediately after the header load, `1831A` compares it against the currently active resource identifier (`29050`) and reloads resources when the current state does not match;
- `18C13..18C4C` repeats the same check and invokes the resource-loading sequence with this byte;
- `0E4B9` and the options/control path at `06681` pass the byte to the same resource-management family.

The supplied archive uses values `0..45` with substantial reuse across levels. `resource_set_id` is therefore a safe behavioral name; the exact mapping from ID to named RES content belongs in the later resource-file reconstruction.

## Remaining seven bytes

No direct absolute CODE.2 consumer was found for header byte `+0x291` or bytes `+0x29D..+0x2A3`, and every supplied archive record stores zero in those positions. They must still be preserved verbatim by lossless tooling; archive-wide zero is an observation, not a format rule.

## Safe implementation boundary

A reader/repacker can expose the fields above with their behavioral names while preserving all raw header bytes. In particular:

- do not combine `+0x298/+0x299` into one semantic word;
- preserve the `0xFF` environment-profile sentinel exactly;
- preserve all three NPC-filter words even when `0x00FF` falls outside the supplied NPC database;
- distinguish archive bytes from normalized live state for `+0x29A`;
- do not zero the currently unused/reserved tail bytes when round-tripping arbitrary inputs.
