# INIT environment-profile panorama tables

Status: **CONFIRMED for the supplied Ravenloft build.** The final 530 bytes of `INIT` are a two-profile environment/backdrop placement table consumed by CODE.2 `16990` and rendered by `314B0`. The level-header byte at `LEVELS.DAT header +0x290` / DGROUP `0x258A8` selects profile `0` or `1`; `0xFF` bypasses this table and uses the alternate/default level path.

## Physical location

The earlier INIT structures end exactly at `0x0545`:

- `0x0000..0x0023`: 18-word fixed initialization header;
- `0x0024..0x04EE`: 175 source-class-1 scene descriptors;
- `0x04EF..0x0544`: 12 source-class-3 scene descriptors.

CODE.2 `16990` continues sequentially at `INIT +0x0545` and consumes the remainder of the file through EOF.

```text
+0x0545  uint16 profile_count = 2

repeat profile_count:
    uint16 res8_base
    uint16 stored_record_count
    placement[stored_record_count]

placement (8 bytes):
    int16 resource_selector
    int16 azimuth_degrees
    int16 draw_width_pixels
    int16 screen_y
```

The supplied file closes exactly at `0x0757` / 1,879 bytes. There is no tail slack.

## Exact supplied profiles

```text
profile 0
    RES8 base          8
    stored records    38
    active placements 37
    terminator index  37

profile 1
    RES8 base         45
    stored records    27
    active placements 26
    terminator index  26
```

Both lists end with an all-zero 8-byte placement record. No earlier record has a zero `resource_selector`.

Although `16990` needs `stored_record_count` to allocate/read the list, renderer `314B0` walks the list until `resource_selector == 0`. A faithful representation therefore preserves both the serialized count and the terminating zero record.

## Profile selector link to LEVELS.DAT

The fixed level header is loaded verbatim to DGROUP `0x25618`, so:

```text
LEVELS header +0x290 -> DGROUP 0x258A8
```

The supplied 32-level archive contains only:

```text
0x00 : 10 levels
0x01 :  1 level
0xFF : 21 levels
```

At CODE.2 `1F2B1`, a non-`0xFF` value is passed directly to `314B0(profile_id)`. `314B0` uses that value to index the two arrays populated by `16990`:

- `0x2B750[profile_id]` -> `res8_base`;
- `0x2B764[profile_id]` -> pointer to the profile's placement list.

Thus the two serialized groups are exactly the two environment profiles selected by shipped level headers.

## RES8 resource resolution

`314B0` ultimately calls `2986C`, which calls generic resource accessor `2F29C` with archive number **8**.

For each placement:

```text
if resource_selector >= 8:
    absolute_RES8_id = res8_base + resource_selector
else:
    absolute_RES8_id = resource_selector
```

The supplied active records all use selectors >= 24, so they all take the base-relative branch.

Resolved IDs are:

```text
profile 0 -> RES8 {32,33,35,36,37,38,39,40,41,42,43}
profile 1 -> RES8 {69,70,71,72}
```

`RES8` contains 73 entries, therefore every resolved placement ID is valid.

The same profile base is also used earlier in `314B0` to choose the profile's backdrop resources according to renderer phase/shading state. This establishes `res8_base` as the base of a profile-specific RES8 backdrop/environment graphics range, not merely an offset applied to the placement list.

## Placement fields

### `+0x00 resource_selector`

Passed through the base-resolution rule above and then to archive-8 graphics rendering. Zero terminates the runtime placement walk.

The executable supports selectors below 8 as direct RES8 IDs, but the supplied active placements do not exercise that form.

### `+0x02 azimuth_degrees`

Directly proven as an angular world/view coordinate.

`314B0` subtracts current view angle global `0xC658`, wraps by adding or subtracting 360 degrees, and maps the angular displacement onto horizontal screen position using the encoded `*128 / 36` projection arithmetic.

Supplied ranges:

```text
profile 0: 0..355 degrees
profile 1: 0..350 degrees
```

### `+0x04 draw_width_pixels`

The renderer halves this word when centering the graphic and uses the full value in horizontal visibility/clipping tests before the draw call. It is therefore safely a pixel width.

Supplied widths are 32..128 pixels.

### `+0x06 screen_y`

Passed directly as the vertical coordinate argument to the graphics draw helper `2986C` / `29564`.

Supplied ranges:

```text
profile 0: 4..94
profile 1: 4..105
```

## Runtime role

The safe architectural description is **environment-profile backdrop/panorama placements**:

```text
LEVELS.DAT environment_profile_id
              |
              v
INIT profile {0,1}
    |-- profile RES8 base / phase backdrop resources
    `-- azimuth-anchored RES8 placement list
              |
              v
314B0 projects world azimuth to screen X
              |
              v
archive-8 graphics renderer
```

This name is deliberately renderer-facing. It does not assume an original editor/source term such as "skyline", "horizon", or "landmark" without documentary evidence.

## Reproducible validation

`analyze_init.py` parses every byte of the supplied 1,879-byte `INIT`, reserializes it byte-identically, and optionally validates all resolved environment graphics IDs against `RES8`.

For the supplied binaries it confirms:

```text
round_trip_identical        true
class-1 descriptors          175
class-3 descriptors           12
environment profiles           2
profile 0 stored/active     38/37
profile 1 stored/active     27/26
terminal zero records        2/2
premature zero records       0
RES8 resolved-ID failures    0
```
