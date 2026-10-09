# Every original CODE.1 starting equipment loadout — source-verified matrix

**Source authority:** unmodified `CODE.1` from the original
*Ravenloft: Strahd's Possession*, `OBJECTS.DAT`, and the
character-creation rules in CODE.1. Date: 2026-10-09.
**Build status:** committed native C++ source and regressions;
**not yet verified on current-HEAD Windows CI**.

## Correct source layout

Original `CODE.1` has 15 encoded starting class families
and six race codes. Routine `5F4C` uses the class family
from `5D90`, then indexes an original 14-word equipment
record at `DGROUP:0x0988 + family*0xA8 + race*0x1C`.
Its physical CODE.1 object-2 file offset starts at
`0x3E724 + 0x0988 = 0x3F0AC`.

All **90 rows are decoded**: **44 populated source rows** and
**46 all-FFFF rows**. The empty rows are prohibited
class/race combinations under the original CODE.1
creation rules. Do not assign invented generic starting
gear to them.

The exact 14-word -> 21-item character record mapping
is:

| Original word indices | Destination CHARS item slots | Interpretation |
|---|---|---|
| 4..13 | 0..9 | Ten ordinary inventory/backpack positions |
| — | 10..13 | Left empty (-1) |
| 2 | 14 | Hand A |
| 3 | 15 | Hand B |
| 1 | 16 | Body / armor / robe |
| 0 | 17 | Head |
| — | 18..20 | Left empty (-1) |

`0xFFFF` always means an originally empty slot.
Item names, categories, icons, weights and container
identity come from the indexed `OBJECTS.DAT` record.
The original shipped OBJECTS corpus contains **336
objects**, and every populated starter item ID belongs
to that corpus.

## Exhaustive original class/race availability

A check indicates a **populated** original CODE.1 kit.
An em dash means the source has all fourteen
words equal to `0xFFFF` (not a legal starter kit).
Race order follows the native CODE.1 choice codes.

| Class family | Human | Elf | Dwarf | Halfling | Gnome | Half-elf |
|---|:---:|:---:|:---:|:---:|:---:|:---:|
| Fighter | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| Paladin | ✓ | — | — | — | — | — |
| Ranger | ✓ | ✓ | — | — | — | ✓ |
| Cleric | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| Mage | ✓ | ✓ | — | — | — | ✓ |
| Thief | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| Fighter/Thief | — | ✓ | ✓ | ✓ | ✓ | ✓ |
| Fighter/Cleric | — | ✓ | — | ✓ | ✓ | — |
| Fighter/Mage | — | ✓ | — | — | — | ✓ |
| Cleric/Thief | — | ✓ | — | ✓ | — | — |
| Ranger/Cleric | — | ✓ | — | — | — | — |
| Cleric/Mage | — | ✓ | — | — | — | — |
| Mage/Thief | — | ✓ | — | — | — | ✓ |
| Fighter/Mage/Thief | — | ✓ | — | — | — | ✓ |
| Fighter/Mage/Cleric | — | ✓ | — | — | — | — |

Total: **44 populated / 46 empty**.

## Examples directly from the authentic source table

- **Human Fighter:** long sword in hand A; chain
  mail and chain coif; dagger and sack in ordinary
  inventory.
- **Human Paladin:** broad sword, shield, chain mail
  and plate helm; a sack.
- **Halfling Cleric:** mace and shield in hands,
  blue robe, sling, three different cleric scrolls
  and a sling pouch. Source object IDs in slots
  0..4: **49,206,220,221,311**; hands 14/15
  **37,34**; body 16 **8**.
- **Gnome Cleric:** warhammer, shield, leather armor,
  chain coif; cleric scroll of Negative Plane
  Protection and potion of Extra Healing. Source
  inventory IDs **217,60**; hands **256,33**;
  body/head **12,27**.
- **Human Mage:** mage spell book, ring and bracers
  of protection, dagger, mage scrolls, healing
  potion; white robe.
- **Half-elf Fighter/Mage:** mage spell book,
  composite bow, invisibility scroll, potion,
  quiver, ivory scroll case, bracers and ring;
  long sword and white robe equipped.

The full name+ID enumeration is not hard-coded
separately into the gameplay: the Windows source
audit executable reads the original 90 table rows
and original OBJECTS definitions to print **every**
populated class/race record and every original slot.

### Two-character starting-kit conflict rule

After populating both actor records, the
original routine `5F4C` compares the **class-family
and race** of the two created characters.
Only if BOTH match does it scan the second actor's
ordinary slots 0..9 and delete duplicate starting
objects whose `OBJECTS.DAT` category byte at
record `+0x2F` equals **1** (container objects).
It does not delete their equipped weapon, robe
or headgear. Native code preserves this asymmetric
rule rather than dropping every repeated object.

## Native implementation

The normal, default fresh-party path uses
`build_character_transfer` and now delegates
all equipment writes to
`CharacterTransferTables::starting_loadout(family,race)`.

`CharacterTransferTables::from_original_bytes(CODE.1,OBJECTS.DAT)`
parses **all 15×6×14** words directly from the
supplied game bytes, rejects shortened data and
invalid object IDs and does not involve `SAVE.DAT`.

New `character_starting_equipment_tests` checks:
- All 90 source rows and each of 21 runtime slots,
  through both the table model and real
  `build_character_transfer` generation.
- Every class family (including two-/three-class
  families), every race, both character records.
- Original Halfling/Gnome Cleric 14-word source
  values and resulting 21-slot character images.
- All-FFFF empty rows, invalid object indexes,
  truncated `CODE.1` and corrupted `OBJECTS.DAT`.
- The second-actor-only category-1 duplicate
  suppression; non-identical races keep their
  original starter records.

Existing `character_transfer_tests` additionally
cover status, names, portrait selectors,
ability/HP fields, equipped source word order,
and overall 0x920-byte CHARS transport.

## Windows original-media report

The **next manually built** Windows artifact
includes `rloft_starting_equipment_probe.exe`
and `Audit-Starting-Equipment.cmd`. Run the
launcher after extracting the artifact, optionally
passing an original RLOFT directory or `game.gog`
path:

```bat
Audit-Starting-Equipment.cmd "C:\Path\To\RLOFT"
```

It prints all **44 real source-populated class/race
kits**, original 21-slot indices, item IDs and
literal `OBJECTS.DAT` names, and writes the
audit to a text file under `%TEMP%`.
It never changes original assets, game saves,
characters, or inventories.

The updated manual-only Windows workflow includes
the new test and inventory/character-transfer
targets in its **Quick** verification mode.
**165 total registered CTests** now exist (when
the normal Python-enabled workflow config applies);
neither Quick nor Full has run on current HEAD.

## Remaining inventory acceptance

**This closes the source reconstruction of the
starter-equipment matrix**, not the whole inventory
system. Runtime container insertion, open-in-hand
container panels, object dragging tooltips, stat/
encumbrance, equipment hit controls and indexed-pixel
color parity against the supplied fresh DOS
screenshots still require separate validation.
The starting class/race records must remain
byte-authentic; the renderer must consume the
resulting party image, not substitute stock actor
inventory or unrelated SAVE.DAT equipment.
