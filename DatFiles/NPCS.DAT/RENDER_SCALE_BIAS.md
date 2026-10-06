# NPCS.DAT projected-scale lookup bias

Status: **record word `+0x27` is now behaviorally identified as a bias added to the scale-dependent type-7 renderer lookup index. Only PYRE ELEMENTAL uses a nonzero supplied value (`3`).**

The physical field is:

```text
NPCS.DAT +0x27  uint16 projected_scale_lookup_bias
```

This is an executable-level behavioral name. It does not claim the original DreamForge source identifier.

## NPC definition -> scene record

The first-person family-A scene emitter resolves the current NPC definition at `0x2B73B..0x2B74A`.

After projecting the live NPC's world coordinates into a 0x16-byte scene record, it copies the definition word at `+0x27` into scene-record word `+0x0C`:

```text
2BACB  mov eax, npc_definition
2BACE  mov dx, word [eax + 0x27]
2BAD2  mov eax, scene_record
2BAD5  mov word [eax + 0x0C], dx
```

Placed objects and loose items initialize the same scene-record word to zero, making this an NPC-definition-specific renderer control in the supplied engine path.

## Scene record -> type-7 renderer

The scene draw routine passes scene-record `+0x0C` as the fourth argument to CODE.2 `0x36CEC`, the type-7 compiled scene-shape renderer.

For example at `0x2C4A3..0x2C4E1`:

```text
push scene_record[+0x02]    // final argument / projected-size-related value
push 0x58
push scene_record[+0x0C]    // renderer argument 4
push derived projected value
push derived screen/resource value
push scene_record[+0x00]
call 0x36CEC
```

The resource reconstruction independently establishes `0x36CEC` as the scale-dependent renderer for RES3 type-7 compiled scene shapes.

## Exact lookup-index behavior

Inside `0x36CEC`, argument 3 is the projected-size input used to derive the scale family. The function also derives:

```text
base_lookup_index = (projected_size & 0xFFF8) >> 1
```

and then performs at `0x36D63..0x36D87`:

```text
lookup_index = base_lookup_index + projected_scale_lookup_bias

if lookup_index > renderer_lookup_limit:
    renderer_value = 0
else:
    renderer_value = uint16(renderer_lookup_table[lookup_index])
```

The selected value is stored in renderer global `0x105E7` and capped at `0x0808` before the scale-dependent rasterizer proceeds.

Thus `+0x27` does not select an animation, sound, AI mode, or resource ID. It **biases the index of a renderer lookup table whose base index is derived from projected scale**.

Safe serialized name:

```text
projected_scale_lookup_bias
```

A stronger visual interpretation such as "height", "glow size", or "vertical offset" is not assigned because the external lookup table/rasterizer microcode contract does not require one for faithful implementation.

## Supplied corpus

All 83 supplied definitions use zero except:

```text
NPC 3  PYRE ELEMENTAL  projected_scale_lookup_bias = 3
```

This matches the other Pyre-Elemental-only renderer auxiliary (`scene_influence_step = 3`) but the two fields feed separate executable mechanisms and must not be merged.

## Safe implementation boundary

A faithful implementation should:

1. preserve `+0x27` as an unsigned 16-bit word;
2. copy it to the generated NPC scene record;
3. add it to the projected-scale-derived lookup index exactly as CODE.2 does;
4. preserve zero for ordinary NPC definitions and the supplied value `3` for Pyre Elemental.

Do not normalize this field away merely because only one supplied definition exercises it.