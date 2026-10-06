# SAVE.DAT primary-effect text corroboration

Status: **raw-resource closed.** The supplied `T` binary was rechecked directly against the repository's certified `T` format and matches the archived source identity exactly:

```text
bytes    133,584
SHA-256 a3f1e19e40f226c8cf150a43aacd9c378ee35a1294d2c9dd72870bfdf1e16315
records  1,073
```

This closes the previously pending `T[588..613]` corroboration for the 26 serialized primary-effect slots in `SAVE.DAT`.

## Expiration-message construction

CODE.2 physical `0x43640` removes one primary effect and constructs the displayed expiration message. The sequence is:

```text
1. format character prefix using DGROUP pointer 0x669C -> "%s's "
2. read T[0x24C + effect_index] into the end of the buffer
3. append DGROUP pointer 0x66A0 -> " wears off."
```

Therefore the visible message template is:

```text
<character name>'s <T effect label> wears off.
```

The `T` records are effect labels, not complete standalone sentences.

## Exact `T[588..613]` labels

All 26 records are physically contiguous in the supplied `T` file with no skipped slack between them.

| Effect | T index | Exact decoded T label | Producer-backed identity |
|---:|---:|---|---|
| 0 | `588 / 0x24C` | `Armor` | Armor |
| 1 | `589 / 0x24D` | `Light` | Mage Light |
| 2 | `590 / 0x24E` | `Shield` | Shield |
| 3 | `591 / 0x24F` | `Blur` | Blur |
| 4 | `592 / 0x250` | `Haste` | Haste |
| 5 | `593 / 0x251` | `Invisibility` | Invisibility 10' Radius |
| 6 | `594 / 0x252` | `Stoneskin` | Stoneskin |
| 7 | `595 / 0x253` | `Dragon Scales` | Dragon Scales |
| 8 | `596 / 0x254` | `Bless` | Bless |
| 9 | `597 / 0x255` | `Invis. to Undead` | Invisibility to Undead |
| 10 | `598 / 0x256` | `Light` | Priest Light |
| 11 | `599 / 0x257` | `Aid` | Aid |
| 12 | `600 / 0x258` | `Draw Strength` | Draw Upon Holy Might - STR |
| 13 | `601 / 0x259` | `Draw Dexterity` | Draw Upon Holy Might - DEX |
| 14 | `602 / 0x25A` | `Neg. Plane Prot.` | Negative Plane Protection |
| 15 | `603 / 0x25B` | `Fortify` | Fortify |
| 16 | `604 / 0x25C` | `Slow Poison` | Slow Poison |
| 17 | `605 / 0x25D` | `True Seeing` | True Seeing |
| 18 | `606 / 0x25E` | `Detect Undead` | Detect Undead |
| 19 | `607 / 0x25F` | `Fire Resistance` | Potion of Fire Resistance |
| 20 | `608 / 0x260` | `Detect Magic` | Detect Magic |
| 21 | `609 / 0x261` | `Prayer` | Prayer |
| 22 | `610 / 0x262` | `Lightning Prot.` | Shield of Lightning Protection equipped-state |
| 23 | `611 / 0x263` | `Giant Strength` | Potion of Giant Strength |
| 24 | `612 / 0x264` | `Speed` | Potion of Speed |
| 25 | `613 / 0x265` | `Detect Enemy` | Wand of Enemy Detection |

## Cross-check result

The raw text range produces **no contradiction** with the producer-side catalogue.

It also demonstrates why executable producers were necessary for exact semantic ownership:

- effects 1 and 10 both display the generic label `Light`, so `T` alone cannot distinguish Mage Light from priest Light;
- effect 5 shortens `Invisibility 10' Radius` to `Invisibility` in the expiration UI;
- effects 19 and 22-25 use generic effect labels even though their serialized bits are created by a potion, equipped shield, potion, potion, and wand respectively.

Thus the resource labels are strong independent corroboration, but they are not a substitute for producer tracing.

## Native-port rule

For faithful presentation, preserve the original label strings separately from normalized internal effect names. The compatibility model should retain stable primary-effect indexes `0..25`; presentation can then reproduce the original message template exactly or route the same labels through a localization layer.