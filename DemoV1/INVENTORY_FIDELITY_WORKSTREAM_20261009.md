# Ravenloft Demo v1 — Inventory-only full behavior reconstruction

**Checkpoint date:** 2026-10-09 · **Branch:** `demo-v1`
**User acceptance target:** all functionality visible in the user's
original DOSBox screenshots AND all CODE.2 inventory behavior, not
just a backdrop or a successful CTest.

**Important:** THIS IS NOT A FINISHED INVENTORY SYSTEM YET.
The changes below are new native source, uncompiled on current
HEAD. **Do not start the user-requested Windows build until all
inventory acceptance gaps are closed.** GitHub Actions remains
workflow_dispatch-only; no run triggered.

## Original evidence

- Nine original DOS screenshots show freshly created
  Halfling/Gnome Clerics, 4 vertical actor lanes (80 pixels
  each), two populated mannequins, original starting gear,
  12 ordinary 3x4 cells and nine equipped slots.
- Screenshots show the original character-sheet statistics
  and class XP, a standalone weight panel with
  `MAX: 140`, `CURRENT: 17`, `UNENCUMBERED`, and
  identified item-ribbon text for an Ivory Scroll Case,
  Sling Pouch and Blue Robe.
- Putting a container in hand shows its **source-specific
  2x2, 3x2 or 4x2** nested black slots with ornamental gray
  rim; putting items into those cells or removing them must
  preserve original inventory/known bits.
- CODE.2 source addresses: `13118` inventory mode;
  `12E70` pointer dispatch;
  `117A0` ordinary known-bit swap;
  `12180` equipment categories and hand compatibility;
  `21404` container insert; `11C3C` nested slot click;
  `12470` full party exchange;
  `12988` encumbrance dialog;
  `0FD84/0FDE4/0FE44` burden/maximum/rank;
  `0F790` item label;
  `CODE.1 5F4C` exact class/race starting kit.

## Source-implemented during inventory-focus passes

### 1. Original equipment for every class/race

- All **15 class families × 6 races**, 44 populated and
  46 original all-empty rows from CODE.1 table 0988.
- Complete 0x920-byte CHARS handoff, all original 21
  item slots, and the asymmetric second-actor duplicate
  category-1 container suppression.
- [Original class/race kit audit](
  ORIGINAL_CODE1_ALL_CLASS_RACE_STARTING_EQUIPMENT_20261009.md).

### 2. Item and cursor mechanics

- Normal 12-slot original 3x4 grid hit bounds
  for four actor columns; original item IDs and known bits
  transferred atomically through the cursor.
- Item description model reads actual OBJECTS names
  and half-pound weights; identified items reveal their
  enchantment suffix; concrete regression covers
  `IVORY SCROLL CASE: WEIGHT:0.5`.
- Win32 host now replaces the original RES0 mouse cursor
  with the held object's real RES1 type-1 image
  (`INIT+0x02` graphics base + item UI ID) and restores
  the original arrow when cursor empties or modal closes.
  Held item sprite is NOT painted permanently in background.
- This source path is not yet visually compared with DOS
  exact cursor/art offsets.

### 3. Hand, body, head, rings and special equipment

- Category 3 helmet→slot17; 4/8 armor/robe→16;
  5 rings→12/13; 6 accessory→18; 7 bracers→20;
  ordinary hands→14/15.
- Exact original hard-coded quiver IDs reroute side A to
  slot19; original class-4 single caster body-armor block,
  class-5 five-item armor whitelist, original paired-hand
  class predicate, temporary magic-touch pseudo-object
  `0x7E..0x86` pickup protection.
- Actual source RES1 item raster hit bounds for taking
  equipped objects; category-directed original destination
  for placing held items, with byte-exact known bits.

### 4. Nested containers

- Original CODE.2 category-1 descriptors and slot positions
  inherited from INIT, including capacity and source
  subtype 0–6.
- A category-1 item equipped into an empty hand opens
  its live nested container; removal closes the stale view.
- Native live modal now draws actual RES1 item icons inside
  descriptor-derived original `row=0xDB+0x22*y`,
  `col=actor*0x50+ui_x_offset+0x12*x` cells.
- Taking, inserting, replacing occupied container
  slots preserves both item ID and identified state.
- Automatic `117A0 -> 21404` item insertion when
  dragging onto an occupied ordinary backpack container
  leaves its outer slot untouched.
- Subtype 3 admits category13 sling stones,
  4 admits category14 scrolls,
  5 admits category12 keys,
  6 admits category15 arrows; general pouch/sack
  also checks source hand-pair code
  `OBJECTS+0x35 & 0x3`.
- Recursive capacity is checked in half pounds, with
  honest Full/Incompatible/TooHeavy outcomes;
  unsupported nested-container recursion is rejected
  rather than silently corrupting descriptor state.

### 5. Stats and encumbrance

- Existing source-backed character sheet, HP, six abilities,
  class labels, levels and XP records now also draws
  numeric class XP values and original actor AC.
- Original `CODE.2 DGROUP 7E8C` table has 21
  fixed rows × 5 thresholds for effective Strength
  3..23. No AD&D textbook formula is substituted.
- Original 0FE44 exceptional Strength band steps
  (1–50/+1,51–75/+2,76–90/+3,91–99/+4,100/+5)
  and two independent base/current Strength reads
  are represented. Recursive item burden uses all
  canonical party item slots and container content.
- At STR12, the exact source thresholds are
  `45,69,93,117,140` lbs, matching screenshot
  `MAX:140`; weight 34 half pounds prints CURRENT17
  and burden rank0.
- Source `12E70` encumbrance click rectangle
  row16..39, actor-local col68..77 maps to
  `12988` (different from the nearby status
  and active-effect icons). Dialog displays actual
  `T[0x266]`, `T[0x267]`,
  `T[0x268+rank]` source strings and exits on next click.

### 6. Party reordering

- Original `CODE.2 12470` drag-and-release across
  top 16 rows exchanges all 0x248 bytes of two actors;
  swaps native per-panel mode/context and correctly
  repairs 3-bit encoded actor bits11..13 in live NPC
  and family-C loose-object packed records.
- Family-B placed objects are intentionally untouched,
  as original disassembly shows.
- The live runtime increments world-scene revision
  so previously computed hit frames become invalid.
- Tests check full actor identity, reversible exchange
  and all actor references.

## Registered regression scope

`character_starting_equipment_tests`,
`character_transfer_tests`,
`inventory_runtime_tests`,
`inventory_item_description_tests`,
`inventory_encumbrance_tests`,
`inventory_party_order_tests`,
`inventory_screen_tests`,
`party_panel_controls_tests`,
`character_modal_stats_tests`, plus previous
party/hud/render/integration coverage.

With new encumbrance and party-order targets this branch
now has **168** registered CTests; manual Quick config
has 18 focused tests, Full covers all registered tests.

**Source-only; a successful old #640 Windows build cannot
validate changes made after it. Do not claim these
tests pass until actual current-source CI runs.**

## Open blocking acceptance requirements — DO NOT BUILD YET

1. **Original sprite and palette exactness.** Current nested
   container outline and red item/encumbrance panels use
   explicit provisional palette rectangles rather than
   verified original `RES0` decorative border resources.
   The user explicitly requires original colors, likeness,
   silhouettes and text placement.
2. **All nine equipment hit descriptor table cases.**
   The source uses actual sprite bounding boxes for
   pickup and authentic category routes for placement,
   but original `DGROUP 12C00` 10×24-byte alternate
   descriptor tables and side effects remain to be traced
   and compared against DOS pointer captures.
3. **Original item use actions.** CODE.2 special
   `0x35` potion, `0xA6` spell item, temporary touch
   effects, charges, spellbook/scroll hand use and
   item-triggered ability changes remain guarded,
   not silently replaced by ordinary swaps.
4. **Full general nested-container recursion.**
   Category-1 containers inside admissible container
   subtypes, their recursion/capacity tests and special
   same-type restrictions need source-verified handling;
   current handler conservatively rejects all
   category-1 nesting.
5. **Original loose-world item drop.** Inventory
   key `0x44` invokes original CODE.2
   `0x2195C` (physical CODE.2 offset 0x5C880)
   with pointer item, identification flag, heading-
   relative placement and world family-C update.
   This mutation must reproduce original geometry,
   count, cell-state and field flags, not simply
   remove the held item.
6. **Full persistent SAVE compatibility.** Closed/open
   per-character container state at original 6FA0,
   mutable descriptors, correct new-game-vs-load,
   carried cursor item, menu mode, party order and
   automatic SAVE.DAT output are not yet integrated
   end-to-end.
7. **Modal ancillary controls.** Exact active effect/
   character condition popup and their hit bounds,
   carried modifier changes, key shortcuts and
   original load/save/exit transitions are distinct
   gameplay features, only partly reconstructed.
8. **Live integration and DOS visual proof.**
   Current source must compile; tests must pass;
   verify original nine DOS references at matching
   race/class, source palette, inventory state and
   mouse actions. Pixel-perfect DOS output is
   currently unverified.

**Completion policy:** Only after all eight blocking
areas are resolved and native inventory completes
a continuous user-driven DOS-equivalent run should
a manual Windows **Full** build be dispatched,
its current-source test results checked, and a
finished inventory status announced. If Full fails,
fix and re-run instead of certifying the feature.

The original game branch remains immutable. Formal
Demo-v1 overall engineering 32-unit tracker currently
keeps inventory at PARTIAL while the above work remains.


## Subsequent direct-binary closure: nested containers and exact equipment-hit regions

The original `CODE.2` file was disassembled again, at the
instruction level, to remove two earlier native generalizations.

**Nested containers — CODE.2 physical 0x4CB60..0x4CECA
(original logical 0x11C3C):**

- Specialized descriptor subtypes 3, 4, 5, and 6 require
  incoming `OBJECTS.DAT +0x2F` categories 13, 14,
  12 and 15 respectively.
- Generic subtype **0/1** admits an incoming object only
  when its original `OBJECTS.DAT +0x35 & 3` hand-pair
  class is **0 or 1**. Classes 2 and 3 are rejected.
  Previous native code incorrectly rejected class 1.
  The exact instructions are physical `0x4CC69..0x4CC83`.
- Original physical `0x4CC89..0x4CCC4` checks an
  **incoming category-1** nested container's descriptor
  subtype. If it matches the destination subtype,
  the operation is rejected; differing subtypes proceed.
  Previous native code rejected every nested container.
- Physical `0x4CD57..0x4CE18` computes the destination
  contents' current **recursive** burden, removes the
  old occupying object's full recursively contained burden,
  adds the new held object's full recursively contained burden,
  and checks capacity in original half-pound units.
  Prior native swapping accounted for direct weight only.
- Native `put_held_in_container_slot` now implements all
  these rules. In addition, to safely handle corrupt or
  recursively invalid modern/imported state, a bounded
  63-descriptor visited set rejects self and indirect
  container cycles *before* any mutation.
- Expanded `inventory_runtime_tests` covers accepted
  different-subtype container-in-container, rejected same
  subtype, rejected indirect cycle with byte-state
  preservation, original low-bit classes 1/2, and replacing
  a recursively heavy container with a lighter item.

**Equipment pickup — CODE.2 physical 0x4DDDF..0x4DE02,
0x4D252..0x4D2A9 (logical 0x12E70 / 0x12180):**

- Original source routine `0F368`, physical
  `0x4A28C..0x4A2B2`, returns **0** for race
  codes 2/3/4, else **1**.
- The modal sets live `DGROUP:0x12C00` to the exact
  10-descriptor array at `DGROUP:0x6EA0` for codes
  2/3/4, or `DGROUP:0x6DB0` for other races.
- Every descriptor is six little-endian 32-bit words:
  `slot, accepted mouse flags, min row, min local column,
  max row, max local column`. Physical source file
  offsets start at `0x75F24+0x6DB0` and
  `0x75F24+0x6EA0`. All twenty descriptors have
  source click-mask 1; duplicate slot20 rectangles
  represent separate left/right bracer graphics.
- Original source accepts **inclusive** rectangle
  bounds, skips empty equipment slots, tests the
  pointer flag, and scans all ten descriptor rows
  in original order.
- `OriginalEquipmentHitTables::from_code2` parses
  both immutable source tables, bounds-checks
  every descriptor, and exposes exact priority and
  mask semantics. `PartyModalPresenter::equipment_hit_at`
  now uses that source model for an **empty cursor**;
  it no longer guesses a pickup target based on sprite
  width/height.
- `party_panel_controls_tests` includes all 20 exact
  descriptor values from the original supplied
  executable, compares both races, and tests first
  and last pixels, gaps, mouse flags, and empty slot
  rejection on every region.

Source evidence: original binary `CODE.2`,
SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Both improvements are committed, **not yet compiled
or checked against the nine original DOS screenshots**.
They substantially close source-behavior unknowns in
earlier blockers 2 and 4, but do not certify
the full inventory without real Windows tests and
visual comparison. The user's full inventory build
remains deferred as requested.


## Subsequent source-art breakthrough: original RES2 container surround

A direct disassembly of `CODE.2` physical
`0x4B994..0x4BADF` (logical `0x10A70`)
established the original **decorative container surround**,
not just its interior item rectangles.

1. The original hand-container open routine at
   physical `0x4BED0..0x4BF09` stores the
   **container subtype** in actor-specific
   `DGROUP:6FA0`, the actual descriptor index
   in `6FA1` and the source object ID in `6FA3`.
   These are distinct fields.
2. When the container presentation selects mode 3,
   original `0x4B9EF..0x4BA03` uses raster bank
   **RES2** and absolute local entry **`9+subtype`**,
   draws at VGA `row=0xCE` and actor-column
   `actor*0x50` using `0x69434`.
3. `0x69434` obtains the authentic raster
   through `0x6A1C0`, then copies all original
   decoded lines through `0x693C8`: this is
   original **bitmap artwork**, not a solid-color
   gray box or a per-cell rectangle.
4. Only **after** the genuine RES2 surround does
   `0x4BA48..0x4BA65` invoke the original
   black indexed-VGA cell fill with dimensions
   32 rows × 16 columns for each descriptor slot.
   `0x4BA75..0x4BABD` then draws the matching
   original RES1 object graphic in occupied cells.

Native `PartyModalPresenter::draw_open_container`
now draws `assets_.type1(2, 9 + desc.subtype)`
at exact screen row `0xCE`, actor-column
`actor*80`, before the black 32x16 slots and
original RES1 inventory icons. The previously
fabricated individual gray cell frames have been
**removed**, avoiding a visible color/art difference.

`InventoryScreenModel::original_container_res2_graphic`
checks shipped subtype 0..6, returning exactly
entries 9..15. Expanded
`inventory_screen_tests` tests all seven mappings
and rejects invalid subtype 7.

This fixes the original source **container surround
selection**. Actual DOS visual palette, clipping,
the independent top item ribbon, and saved state
still require manual Windows verification.
No Windows build was initiated.
