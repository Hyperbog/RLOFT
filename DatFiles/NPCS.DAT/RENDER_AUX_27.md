# NPCS.DAT `+0x27` render-scale lookup bias

Status: **the formerly generic `render_aux_27` word is behaviorally decoded from certified CODE.2 as a bias added to the type-7 renderer's projected-size-derived lookup index. Only PYRE ELEMENTAL uses a nonzero supplied value (`3`).**

The physical field is:

```text
NPCS.DAT +0x27  uint16 render_scale_lookup_bias
```

This is a deliberately behavioral name. The executable proves where the value enters the renderer and how it changes the lookup index; it does not expose an original source identifier or justify a stronger visual label such as glow, translucency, or height.

## 1. NPC scene emission copies the word verbatim

The first-person NPC scene builder is CODE.2 logical `0x2B6D0..0x2BB91`.

For each visible family-A/NPC world record, it resolves the corresponding 0x45-byte `NPCS.DAT` definition and builds a 0x16-byte generated scene record in the double-buffered array based at `0x27148`.

At the end of the common NPC setup path:

```text
2BACE  mov dx, word [npc_definition + 0x27]
2BAD2  mov eax, scene_record
2BAD5  mov word [eax + 0x0C], dx
```

Thus NPC definition `+0x27` becomes generated scene-record word `+0x0C` without transformation.

Other major scene-source builders initialize this same generated word to zero:

```text
2BC5A  placed-object source -> scene +0x0C = 0
2BEAF  loose-item source    -> scene +0x0C = 0
2C0A4  additional source    -> scene +0x0C = 0
```

This makes the value an NPC-specific renderer input in the supplied scene-generation model.

## 2. Scene record `+0x0C` is passed to the type-7 renderer

The final first-person scene draw loop calls renderer `0x36CEC` repeatedly for generated records.

For example, the NPC/type-0 path at `0x2C4A3` pushes six cdecl arguments. The third push from the top is:

```text
2C4AD  mov eax, scene_record
2C4B0  movzx eax, word [eax + 0x0C]
2C4B4  push eax
...
2C4E1  call 36CEC
```

With cdecl ordering, scene-record `+0x0C` is renderer argument 4 (`[EBP+0x14]`). The same scene-field value is passed at the other `36CEC` call sites (`2C4F6`, `2C58C`, `2C5E3`, `2C616`, `2C649`, `2C697`).

`Resources/RES/TYPE7_RENDERER.md` independently establishes `36CEC` as the scale-dependent renderer for the compiled archive-3/type-7 scene-shape format.

## 3. Renderer use: projected-size lookup bias

Inside `36CEC`, argument 3 (`[EBP+0x10]`) is the projected-size/scale input already used to select the scale-dependent renderer block.

The routine derives an integer lookup index from that value:

```text
36CFA  eax = argument3
36CFD  eax &= 0xFFF8
36D02  eax >>= 1
36D04  derived_index = eax
```

It then applies argument 4 directly:

```text
36D63  ebx = derived_index
36D66  ebx += argument4
36D69  compare against lookup-table bound
...
36D7D  edi = [0x258C4]
36D83  value = word [edi + ebx*2]
```

Therefore the serialized NPC word does not select a sprite frame, sound, combat behavior, or world-state value. It biases the index used by the scale-dependent type-7 rendering lookup table.

Safe project name:

```text
render_scale_lookup_bias
```

## 4. Supplied corpus

All 83 definitions use zero except one:

```text
NPC 3  PYRE ELEMENTAL  render_scale_lookup_bias = 3
```

So this is an exercised shipped feature, but only for Pyre Elemental.

The nearby NPC field `+0x1B` is also uniquely nonzero for Pyre Elemental (`scene_influence_step = 3`), but the two values enter different code paths. They must not be merged into one inferred "elemental glow" property.

## Safe implementation boundary

A faithful high-level model may expose:

```text
uint16 render_scale_lookup_bias
```

and reproduce the renderer behavior exactly:

1. copy NPC definition `+0x27` to generated scene record `+0x0C`;
2. pass scene `+0x0C` as argument 4 to `36CEC`;
3. add it to the renderer's projected-size-derived lookup index;
4. index the table addressed by runtime pointer `0x258C4`.

Do not replace the field with a guessed effect name. Its proven contract is the lookup-index bias itself.
