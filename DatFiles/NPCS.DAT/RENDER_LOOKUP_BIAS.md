# NPCS.DAT render lookup-index bias

Status: **record word `+0x27` is behaviorally decoded from certified CODE.2 as a bias added to the scene renderer's distance/scale-derived lookup index. Only PYRE ELEMENTAL uses a nonzero supplied value (`3`).**

The physical field is:

```text
NPCS.DAT +0x27  uint16 render_lookup_index_bias
```

The immutable executable is controlling evidence. The name deliberately describes the proven dataflow rather than assuming a historical source name such as glow, brightness, light radius, or palette bias.

## Serialized field -> scene record

The NPC first-person scene builder resolves the NPC definition through the family-A `type_id * 0x45` relationship already proved for `NPCS.DAT`.

At logical `0x2BACE` it reads the word at definition `+0x27`:

```text
2BACE  mov dx, word [npc_definition + 0x27]
```

and at `0x2BAD5` copies that value into generated 22-byte scene-record word `+0x0C`:

```text
2BAD5  mov word [scene_record + 0x0C], dx
```

The same builder marks these generated records as scene class 0 at `0x2BB4E`.

## Scene record -> rasterizer argument

The common scene-render loop later reads scene-record `+0x0C`. On the ordinary class path, logical `0x2C649` executes:

```text
movzx eax, word [scene_record + 0x0C]
push eax
```

as the fourth argument eventually supplied to renderer `0x36CEC`.

This is the same generated record bank beginning at runtime `0x27148`; the field is not recomputed or normalized between the NPC builder and the renderer.

## Exact renderer behavior

Inside `0x36CEC`, a depth/scale-derived index is first computed into a local value. The relevant sequence is:

```text
36D63  mov ebx, [depth_scale_index]
36D66  add ebx, [argument_4]
...
36D7D  mov edi, [0x258C4]
36D83  movzx eax, word [edi + ebx*2]
```

Thus the serialized NPC word contributes an **additive index bias** immediately before the renderer looks up a packed 16-bit render-table entry at runtime pointer `0x258C4`.

The table itself is initialized separately by the renderer setup code and consumed by the low-level scene/rasterizer family. The executable proves the index relationship; it does not by itself justify a stronger content-level label such as emissive intensity or lighting radius.

Safe semantic name:

```text
render_lookup_index_bias
```

## Supplied corpus

Across all 83 NPC definitions:

```text
value 0 : 82 definitions
value 3 :  1 definition
```

The only nonzero record is:

```text
NPC 3  PYRE ELEMENTAL  render_lookup_index_bias = 3
```

This makes the field a shipped, exercised capability rather than merely latent executable support.

## Preservation boundary

A faithful port/editor should:

- store the field as the exact serialized 16-bit word;
- copy it unchanged into the generated NPC scene-record `+0x0C` field;
- add it to the renderer's existing depth/scale lookup index exactly where CODE.2 does;
- avoid replacing it with a guessed boolean such as `glowing` or `emissive`;
- preserve zero values and any future/non-shipped values without clamping unless the original renderer does so downstream.

`analyze_render_lookup_bias.py` verifies both the supplied-record distribution and the controlling CODE.2 instruction signatures.
