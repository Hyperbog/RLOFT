# OBJECTS.DAT renderer auxiliary bytes

Status: **record byte `+0x2B` is behaviorally identified as a scene-cell influence propagation step used by the first-person scene builder; `+0x2C` has no OBJECTS-relative consumer in certified CODE.2 and is zero in all supplied records.**

This document extends `FORMAT.md` for the two bytes between `shape_stride` and the unaligned UI graphic word:

```text
+0x2B  uint8 scene_influence_step
+0x2C  uint8 unresolved_2C
```

The shipped OBJECTS.DAT stores zero in both fields for all 336 object definitions. Their semantics therefore come from executable behavior rather than corpus variation.

## `+0x2B`: scene-cell influence propagation step

Family-C loose-item scene emission in CODE.2 `2BDF4..2BFDC` does the following after selecting the object's normal world sprite/orientation data:

```text
if object_def[+0x2B] != 0:
    2B088(object_def[+0x2B], world_record.cell_index)
```

The second argument is the object's current map-cell index from runtime world record `+0x0C`.

Routine `2B088..2B186` walks the engine's 256-entry scene-cell table at DGROUP `0x15A2C`. Entries are grouped by a dword map-cell key. For every group it updates the accumulated dword at entry `+4` using the caller's `raw_step`:

```text
same cell as base_key:
    accumulator += raw_step

8 immediate-neighbor cells, when raw_step > 1:
    accumulator += raw_step - 1

16 outer-neighbor cells, when raw_step == 3:
    accumulator += 1
```

The same accumulated value is propagated to every contiguous scene-table entry sharing a map-cell key.

The immediate-neighbor offsets come from the eight signed words at `0x27108`; the outer ring uses the sixteen signed words at `0x27128`. Thus the byte is not an animation-frame selector or item state. It controls the magnitude/radius of a spatial scene-cell influence centered on the emitted world object.

The safe project name is **`scene_influence_step`**. A stronger label such as `occlusion_radius`, `height`, or `blocking_radius` is not assigned because the final visual use of the scene-table accumulator is not yet source-name-proven.

Equivalent metadata bytes from the other scene-source families are passed to the same `2B088` routine. This independently supports a generic scene-builder spatial parameter rather than item-specific gameplay state.

### Encoded domain

The routine has distinct behavior for values 1, 2 and 3:

```text
1  affect base cell only
2  base + immediate-neighbor ring
3  base + immediate ring + outer ring
```

Values greater than 3 are executable-accepted but receive only the base-cell and immediate-neighbor rules; the outer-ring special case is exactly `raw_step == 3`.

The supplied OBJECTS table uses only zero, so no shipped object exercises this path through OBJECTS.DAT.

## `+0x2C`: unresolved / no proved object consumer

All 336 supplied OBJECTS records contain zero at `+0x2C`.

A complete textual scan of the certified CODE.2 reconstruction found no OBJECTS-table-relative access to record byte `+0x2C`, including no unaligned word access beginning at `+0x2B` that would alias this byte. Other unrelated runtime structures naturally have their own `+0x2C` fields; those are not OBJECTS.DAT consumers.

Accordingly the safe format statement remains:

```text
+0x2C  uint8 unresolved_2C
```

It must be preserved verbatim. Zero in the supplied database is an observation, not a format requirement.

## Safe implementation boundary

A reader/editor may expose `+0x2B` as `scene_influence_step` with the exact encoded propagation behavior above. It should leave `+0x2C` raw and should not synthesize nonzero values for either field when round-tripping the supplied database.
