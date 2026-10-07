# SAVE.DAT primary effect catalogue

Status: **all 26 bits in the serialized primary-effect word have executable-backed source identities and direct raw-`T` text corroboration.** The catalogue is derived from CODE.2 application producers, spell dispatch, serialized `OBJECTS.DAT` spell-scroll names, item-use/equipment paths, the supplied `T` resource, and official manual behavior descriptions.

See `T_EFFECT_TEXT.md` for the exact `T[588..613]` strings and expiration-message assembly.

## Serialized structure

Each `0x248` character record contains:

```text
+0x132..+0x135  uint32 active_effect_bits
+0x136..+0x19D  uint32 effect_expiry[26]
```

For slot `i`, bit `1 << i` corresponds to `effect_expiry[i]` at `+0x136 + i*4`. The common expiration handler selects `T[0x24C + i]`.

Not every active effect is timer-driven. Effects such as Invisibility 10' Radius, Negative Plane Protection, Fortify, and the equipment-derived lightning-protection bit have special removal conditions and may not install an ordinary positive expiry.

## Complete catalogue

| Index | Bit | Source identity | Primary producer / evidence | Special persistent state |
|---:|---:|---|---|---|
| 0 | `0x00000001` | **Armor** | Mage dispatch 0, physical `0x46A3A`, generic timed-effect helper | `+0x117` Armor protection/durability quantity; initialized to caster-derived value + 8 |
| 1 | `0x00000002` | **Mage Light** | Mage dispatch 4, physical `0x46A58..0x46ACB` | renderer/shading rebuild on apply/expire |
| 2 | `0x00000004` | **Shield** | Mage dispatch 6, physical `0x46BC9` | none beyond normal timer |
| 3 | `0x00000008` | **Blur** | Mage dispatch 11, physical `0x46BFD` | none beyond normal timer |
| 4 | `0x00000010` | **Haste** | Mage dispatch 23, physical `0x46C28` | none beyond normal timer |
| 5 | `0x00000020` | **Invisibility 10' Radius** | Mage dispatch 27 calls `0x456B8`; physical `0x457AF` sets bit 5 | special-duration effect; no ordinary timer installed here |
| 6 | `0x00000040` | **Stoneskin** | Mage dispatch 32, physical `0x46C82` | `+0x1F2` accumulated/remaining Stoneskin state |
| 7 | `0x00000080` | **Dragon Scales** | Mage dispatch 55, physical `0x46C53` | none beyond normal timer |
| 8 | `0x00000100` | **Bless** | priest dispatch 80, physical `0x46CA1` | none beyond normal timer |
| 9 | `0x00000200` | **Invisibility to Undead** | priest dispatch 84, physical `0x46CC3` | none beyond normal timer |
| 10 | `0x00000400` | **Priest Light** | priest dispatch 85, physical `0x46AD3..0x46B46` | renderer/shading rebuild on apply/expire |
| 11 | `0x00000800` | **Aid** | priest dispatch 90, physical `0x46D1E` | `+0x1EA` temporary-hit-point/protection pool |
| 12 | `0x00001000` | **Draw Upon Holy Might - STR** | priest dispatch 94 -> `0x457C4`; bit set at `0x458B9` | reversible Strength delta at `+0x123` |
| 13 | `0x00002000` | **Draw Upon Holy Might - DEX** | priest dispatch 95 -> `0x45954`; bit set at `0x45A49` | reversible Dexterity delta in low byte of `+0x127` |
| 14 | `0x00004000` | **Negative Plane Protection** | priest dispatch 102, physical `0x47394` | special duration/until-hit behavior; ordinary expiry may remain zero |
| 15 | `0x00008000` | **Fortify** | priest dispatch 112, physical `0x46B9D` | special one-shot state consumed by cure handling |
| 16 | `0x00010000` | **Slow Poison** | priest dispatch 93, physical `0x46CF2` | none beyond normal timer |
| 17 | `0x00020000` | **True Seeing** | Mage dispatch 54, physical `0x4749A..0x474FF` | increments/decrements shared dword `0xFD08` |
| 18 | `0x00040000` | **Detect Undead** | Mage dispatch 3, physical `0x46B4E..0x46B98` | increments/decrements shared word `0xF03C` |
| 19 | `0x00080000` | **Potion of Fire Resistance** | item-use object 61; bit set at physical `0x61F67` | ordinary timer; no dedicated expiration auxiliary field |
| 20 | `0x00100000` | **Detect Magic** | priest dispatch 83, physical `0x47548..0x475B0` | increments/decrements shared word `0x6DAE`; party display refresh |
| 21 | `0x00200000` | **Prayer** | priest dispatch 103 -> `0x4558C`; bit set for active party at `0x45682` | party-wide synchronized effect; expiration clears corresponding state across party |
| 22 | `0x00400000` | **Shield of Lightning Protection equipped-state** | equipment update tests object ID 103 and sets bit at physical `0x4C9DE` | equipment-derived; cleared when qualifying shield is absent |
| 23 | `0x00800000` | **Potion of Giant Strength** | item-use object 62; bit set at physical `0x62002` | reversible Strength modifier at `+0x1BE` |
| 24 | `0x01000000` | **Potion of Speed** | item-use object 64; bit set at physical `0x62086` | ordinary timer |
| 25 | `0x02000000` | **Wand of Enemy Detection** | item-use object 75; bit set at physical `0x61D7C` | increments/decrements shared word `0xF03E`; consumes charged-item counter |

Bits 26..31 are not part of the 26-entry primary timer array and must not be synthesized as extra effect slots.

## Spell-dispatch name proof

The player spell executor at physical `0x46874` dispatches a compact legacy spell ID through the jump table at logical object-1 offset `0xB740`.

Prepared spell runtime records form that ID as:

```text
spell_dispatch_id = spell_level_index*10 + entry_index + family*80
family 0 = Mage
family 1 = priest
```

and the corresponding name index is:

```text
T_index = 0x42 + spell_dispatch_id
```

The category-14 spell-scroll dispatcher provides an independent serialized-name cross-check. Representative mappings include:

```text
object 167  MAGE SCROLL OF 'ARMOR'                -> dispatch 0
object 171  MAGE SCROLL OF 'LIGHT'                -> dispatch 4
object 173  MAGE SCROLL OF 'SHIELD'               -> dispatch 6
object 175  MAGE SCROLL OF 'BLUR'                 -> dispatch 11
object 183  MAGE SCROLL OF 'HASTE'                -> dispatch 23
object 187  MAGE SCROLL OF 'INVISIBILITY 10 FOOT' -> dispatch 27
object 192  MAGE SCROLL OF 'STONESKIN'            -> dispatch 32
object 200  MAGE SCROLL OF 'TRUE SEEING'           -> dispatch 54
object 201  MAGE SCROLL OF 'DRAGON SCALES'         -> dispatch 55
object 202  CLERIC SCROLL OF 'BLESS'               -> dispatch 80
object 206  CLERIC SCROLL OF 'INVISIBLE TO UNDEAD' -> dispatch 84
object 207  CLERIC SCROLL OF 'LIGHT'               -> dispatch 85
object 209  CLERIC SCROLL OF 'AID'                 -> dispatch 90
object 212  CLERIC SCROLL OF 'SLOW POISON'         -> dispatch 93
object 213  CLERIC SCROLL OF 'DRAW STRENGTH'       -> dispatch 94
object 214  CLERIC SCROLL OF 'DRAW DEXTERITY'      -> dispatch 95
object 217  CLERIC SCROLL OF 'NEGATIVE PLANE PROT' -> dispatch 102
object 218  CLERIC SCROLL OF 'PRAYER'               -> dispatch 103
object 222  CLERIC SCROLL OF 'FORTIFY'              -> dispatch 112
```

Priest Hold Person is an intentional shared-handler complication in the scroll path and is not a primary character-effect bit; see the NPC hold-status documentation.

## Raw `T[588..613]` corroboration

The supplied `T` file directly decodes the expiration-label range as:

```text
 0 Armor                 13 Draw Dexterity
 1 Light                 14 Neg. Plane Prot.
 2 Shield                15 Fortify
 3 Blur                  16 Slow Poison
 4 Haste                 17 True Seeing
 5 Invisibility          18 Detect Undead
 6 Stoneskin             19 Fire Resistance
 7 Dragon Scales         20 Detect Magic
 8 Bless                 21 Prayer
 9 Invis. to Undead      22 Lightning Prot.
10 Light                 23 Giant Strength
11 Aid                   24 Speed
12 Draw Strength         25 Detect Enemy
```

The raw range produces no contradiction with the producer catalogue. It also proves why producer tracing remains controlling for exact ownership: Mage Light and priest Light both use the same display label `Light`, and the item/equipment-derived upper slots display generic effect names rather than their source object names.

CODE.2 assembles expiration UI text as:

```text
"%s's " + T[0x24C + effect_index] + " wears off."
```

## Behavioral corroboration

Official manual spell descriptions match the executable auxiliary state:

- Armor's saved protection quantity is depleted by incoming hits and clears the effect when exhausted; direct CODE.2 damage ordering shows this quantity is durability/protection state rather than an HP-damage absorption pool.
- Aid grants temporary hit points consumed before normal HP, matching effect 11's `+0x1EA` pool.
- Draw Upon Holy Might separately raises Strength and Dexterity, matching effects 12/13 and their reversible saved deltas.
- Negative Plane Protection lasts until duration or an undead hit, consistent with effect 14's special handling.
- Fortify affects the next cure spell, consistent with effect 15 being a one-shot bit rather than an ordinary fixed-duration timer.
- Stoneskin absorbs a limited number of attacks, consistent with effect 6's separate `+0x1F2` state.

The executable remains the controlling source for serialized slot ownership; `T` and the manual provide independent semantic corroboration.

## Non-spell effects 19 and 22..25

```text
19  Potion of Fire Resistance       OBJECTS ID 61
22  Shield of Lightning Protection  OBJECTS ID 103, equipment-derived
23  Potion of Giant Strength        OBJECTS ID 62
24  Potion of Speed                  OBJECTS ID 64
25  Wand of Enemy Detection          OBJECTS ID 75
```

This is why the 26-slot representation must not be replaced with a simple spell-ID array.

## Expiration and cleanup consequences

The expiration audit remains authoritative:

- effects 1 and 10 rebuild renderer lighting state;
- effect 6 decrements Stoneskin auxiliary state at `+0x1F2`;
- effect 11 clears Aid's temporary protection pool `+0x1EA`;
- effects 12/13 reverse Draw Upon Holy Might ability deltas;
- effects 17, 18, 20 and 25 decrement shared global counters;
- effect 21 clears Prayer state across the party;
- effect 23 reverses the potion Strength modifier at `+0x1BE`.

See `EFFECT_FLAGS.md` for exact cleanup branches.

## Native-port rule

Use stable legacy effect indexes at the compatibility boundary. Keep normalized internal source identities separate from exact legacy display labels. Do not replace the 26-bit/timer representation with spell IDs: several entries are item/equipment-derived and several spell effects have special removal semantics rather than ordinary timer expiry.