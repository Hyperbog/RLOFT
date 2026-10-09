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


## Subsequent original inventory D-key / loose-world-item implementation

The original inventory keyboard path has now been reconstructed
from actual `CODE.2`, including a shipped lookup-table audit:

- At physical `0x4E06C..0x4E0A5`, original `13118`
  calls `6BA64`, requires **logical key `0x44`**,
  requires an occupied held cursor, and calls `2195C`
  with **screen seed `0x80`**, the exact held object ID,
  identified byte, original player coordinate DWORDs,
  forward-offset flag 1 and cursor-clear flag 1.
- `6BA64` reads the original 32-bit key translation table
  at `DGROUP:0xF590 + scan*4`. Direct examination of
  the shipped bytes established **scan code `0x20`
  (D key) → logical `0x44`**, while scan `0x44`
  (F10) maps to **`0x144`**, and therefore F10 is
  NOT the inventory drop command.
- At `2195C` (physical `0x5C880..0x5C9FF`),
  the original heading is `facing -
  trunc((0x80-0xA0)/3) = facing+10` in
  source's 360-degree domain. The routine projects
  signed SINTAB Q16 direction by `0x00018000`,
  conditionally centers both low coordinate words
  when source `C02F` mode is nonzero, and calls
  placement predicate `2D5D0(y,x,0x190)`.
  Failed placement reverts to original player
  coordinates.
- The successful loose item is a genuine 22-byte
  family-C `LiveObjectRecord` using original
  `x=DGROUP:255D0`, `y=DGROUP:255D4`,
  cached world-cell index and OBJECTS ID. Source
  clears packed `0x05F8`, installs original
  `(known&1)<<10`, removes the **oldest** of
  exactly 300 loose records before appending a
  new one, and clears global held ID/known.
- Original object `0x89` invokes an additional
  `21880` gameplay effect after the drop.
  Until its effect is implemented, the native
  model returns `SourceSpecialActionPending`
  and **does not lose or mutate** that special
  item in world/held state.

Native `InventoryWorldDropModel::drop` implements
the above bounded source semantics; `ExplorationRuntime::
drop_held_inventory_item` owns the live mutation
and increments world-scene revision **only after
successful drop**.

Win32 modal `PartyPanels` now owns an explicit
inventory keyboard mode. It queues a single
logical `0x44` for a real **D keydown** (not
auto-repeat), drains it while inventory is
open, and discards queued inventory keys when
leaving the screen. Successful drops invalidate
previous world-hit/render snapshots; the next
normal world frame includes the new family-C item.

**Important remaining original parity limitation:**
the live caller's temporary collision predicate
checks the original surrounding map-cell words at
the original `0x190` footprint and returns a safe
collision fallback. This is NOT yet a full
implementation of original `2D5D0` dynamic
entity, scripted transition, and side-effect
rules. The special object `0x89` effect
also remains blocked. Both must close before
claiming the inventory world-drop parity gate.

The new `inventory_world_drop_tests` exercises
empty cursor, real signed direction and radius,
original x/y, known-bit10, collision fallback,
C02F Q16 center, 300-record FIFO and special
`0x89` rejection; new CMake total is
**169 registered CTests** and manual Quick
target is **19 focused tests**. No Windows
build or acceptance test has yet run.

The earlier open-world-drop checklist entry is
now **source-implemented with bounded exceptions**,
not an empty placeholder, but must NOT be called
fully DOS-accepted until original `2D5D0`
and special effects are verified.


## Subsequent source refinements: original encumbrance bitmap and Level16-only drop event

**Encumbrance art:** Direct original CODE.2 instruction review
of `12988` at physical `0x4D8D7..0x4D8E3`
shows the distinct modal weight-panel background
comes from exact source **RES2[0x25]**, rendered
at `row=0x73` and actor column `actor*0x50`
via source raster helper `69434`, *before* the
original T-text labels `0x266/0x267/0x268+rank`
are overlaid. The live
`PartyModalPresenter::draw_encumbrance_panel`
now uses the **actual RES2 bitmap**, removing
the earlier guessed solid `0x45` red panel fill.
This is source art selection, not a claim of
checked DOS color/pixel parity.

**Special world-drop item 0x89:** Original
`CODE.2 2195C` invokes `21880` after
dropping item `0x89`. The first test in
`21880` at physical `0x5C7B6` returns
immediately unless original level ID is
**16**. The native bounded event gate is
now appropriately narrow: item `0x89`
drops normally on all other levels,
including fresh Level22, but on Level16
does **not** mutate world/cursor until the
authored four-position event flag condition
and its `4EE1C/4FF74` effect can be
handled correctly. Expanded world-drop tests
exercise both branches.

The prior blanket block on item `0x89`
was too restrictive for Level22 and is
superseded by this exact source branch.

**Current source inventory test footprint:**
**169 registered CTests**, **19 Quick**
test executables, Full all tests.
Neither a new MSVC build nor a DOS
inventory-screen pixel parity run has
been performed. All GitHub builds remain
manual-only, and the user's request is to
build **only after** every inventory
acceptance blocker has been addressed.


## Part 8 — original mode-0x190 drop collision integrated in native source

Immutable `CODE.2` SHA-256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Verified by direct machine-code disassembly of the supplied
`2D5D0..2D9F8`, `2C800..2C8E1`, `2C9DC..2CA5B` and
`1E8C8..1E99E`. Logical CODE.2 object-1 file offset begins
at `0x3AF24`.

**Fixed source bug:** the original `2D5D0(y,x,0x190)`
third argument is actor/mode `0x190`, **not** the radius.
The original routine uses a constant Q16 footprint
`-0x5800..+0x5800` around each axis with inclusive signed
cell bounds (`2D5E2..2D629`). The previous Win32 path
incorrectly used `0x190` for the footprint and only checked
low-six-bit map geometry.

New `InventoryWorldDropModel::placement_passable_2d5d0`
models the source's group-1/group-2 descriptor precedence,
group-2 dynamic position, bits `0x0080`, `0x0800`,
`0x1000`, low-six solidity and control-type-1/type-8
exceptions. Off-map cells are skipped during traversal, as
in `2D693..2D6BB`. Subsequent `2C800` applies NPC
`NPCS+0x16 + 0x3E8` overlap, omitting inactive `+0x0B&2`
NPCs; mode `0x190` skips placed-object `2CC88` but
still applies player `2C9DC` proximity `+/-0x8FA0`.
Blocked placements keep original `2195C` player-position
fallback semantics.

`ExplorationRuntime::drop_held_inventory_item` now
uses this original-backed predicate by default; Win32
no longer supplies the approximate geometry lambda.
The prior explicit predicate remains injectable for
deterministic tests. Expanded existing
`inventory_world_drop_tests` covers source footprint
cell seams, modes, dynamic/group-2 variants,
bit-0x0800/0x1000, NPC overlap and player exclusion.

**Boundary:** original mutable `DGROUP:F048` scripted
early-skip state is not yet represented by this runtime
path; this is not a generalized replacement for every
`2D5D0` caller or authoring-time side-effect routine.
Other inventory blockers include Level16 special
item `0x89`, spell/item uses, SAVE/load persistent
inventory and live DOS palette, hit/visual and timing
comparisons. **No Windows build or DOS runtime
validation was performed.** Same 169 registered
CTests/19 manual Quick tests, 56.25% engineering
source coverage; inventory still PARTIAL.


## Part 8 — CODE.2 Tome of Evil Artifacts and Parchment Fragment effect

Independent read of the shipped immutable `CODE.2` object-1
and real `OBJECTS.DAT` (SHA-256
`f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0`)
closed one formerly guarded inventory action.
The `0x35` object is **Tome of Evil Artifacts and Cursed Items**;
`0x61..0x64` are four separate Parchment Fragments.
The earlier provisional label “0x35 potion” was wrong.

At logical `CODE.2:117E8..117F9` the dispatcher rejects
current temporary spell objects `0x7E..0x86`. At
`11808..11852` it tests **current-slot** object
`0xA6` Mage Spell Book. Held `0xA7..0xC9`
Mage Scrolls route to `11630` spell learning, where
duplicate/class/capacity state gates actual consumption.
That complete effect remains unsupported; never turn a
failed/unknown learn into a generic swap.

At `11857..11891`, if the CURRENT ordinary slot contains
`0x35` and the HELD cursor contains `0x61..0x64`,
the source subtracts `0x61`, calls `1C124(index)`
and then `1A344(-1,0)` and `0F790(-1,0)`.
Routine `1C124` sets bit `index` in the first
DWORD of `DGROUP:22200`; SAVE persists this
32-byte 256-bit event vector. There is **no test
for a previously set bit**, so repeat consumption
is permitted. The Tome remains in place, its item-known
word unchanged, and held identification clears.
Otherwise `0x35` follows the ordinary source swap.

**Native source changes:** `InventoryRuntimeState::swap_ordinary_slot`
now accepts the actual session 32-byte persistent event vector
and returns `ConsumedStoryFragment` after the original atomic
flag/held update. The Win32 PartyPanels caller passes
`runtime.session().persistent_event_flags`; its successful
interaction branch refreshes the held UI. Missing flag state
fails without discarding the fragment. The overbroad ban
on moving held `0x35` or `0xA6` normally was removed,
while the current-slot `0xA6` special action remains guarded.

**Regression additions:** existing `inventory_runtime_tests`
checks all four event bits, unrelated flag bytes,
repeated fragment consumption, unchanged Tome/known
state, no mutation if the vector is absent,
ordinary Tome transfers, fragment non-Tome swaps,
nonconsuming Mage-book scroll boundaries and
ordinary placing of a held spell book.
No new CTest executable is introduced: **169 total**,
**19 Quick**. This is source-verified but **not yet
MSVC built or DOS compared**.

Remaining original inventory actions include complete
`11630` spell-list transfer and UI, special item/spell
effect timelines, Level16 item `0x89` scripted
side effects, live SAVE/LOAD persistence and
DOS visual/timing parity. **Do not dispatch Windows
CI until all inventory blockers close**.


## Part 8 continuation — original Mage Spell Book 11630 scribing integrated (source-only)

Direct disassembly of immutable CODE.2 SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
audited the executable at logical `11630..1179F`
(physical `0x4C554..0x4C6C3`), and its inventory caller
`11808..11852`. The original object-1 data literal
**at physical `0x3AF24+0xEEB0 = 0x49DD4`** has the
six literal DWORD pairs:
`[173,167], [179,174], [189,180], [193,190], [195,194], [201,196]`.
Caution: object-2's *different* `0x75F24+0xEEB0`
address contains zero-fill and **must not** be treated as
the mapping table. Independent original OBJECTS.DAT
names confirm `0xA6` Mage Spell Book and exactly
`0xA7..0xC9` (35) individual Mage Scrolls.

The source's actor `+0x39` byte indexes a
21-entry Intelligence-derived capacity table
in object-2 `DGROUP:6153`
(physical `0x75F24+0x6153`):
`9,6,6,6,6,6,6,6,6,6,7,7,7,9,9,11,11,14,18,18,18`.
The 8×10 Mage spell matrix starts at actor `+0x6A`.
`1168C..116B2` counts that bank's low-nibble
non-`0xF` cells and rejects only when **count equals
capacity** (the original compares equality, not >=).
`116C5..116CF` rejects an already available target
(low nibble other than `0xF`). On success
`11748` clears the **entire target byte** to zero
(high and low nibbles), and caller `1183A..1184F`
clears held scroll ID and known byte.

**Ordering matters:** `1166F..11679` invokes
`1C124(0xBA)` **before either failed check**,
setting byte 23 bit 2 of the persistent 32-byte
event vector even when no scroll is consumed.
The native `InventoryRuntimeState` now loads source
tables from the correct two original CODE.2 objects,
maps each Mage scroll, performs the exact bank-capacity
and low-nibble test, records the flag and updates the
real 0x248-byte party matrix/held cursor on successful
scribing. It returns distinct `LearnedMageSpell` and
`MageScrollRejected` statuses. Win32 PartyPanels
refreshes the inventory after either status. Missing
source table, missing session event vector and
corrupt Intelligence index produce an explicit
unsupported result **without consuming an item**.
The Mage book stays in its inventory slot.

The existing `inventory_runtime_tests` target now has
synthetic fixtures for both source table locations,
all scroll-bank offsets (including top bank),
successful scribing, previously-known and full-capacity
rejections, event flag `0xBA` on rejection, cursor
identity, and missing-source safety. They have been
**registered as assertions, not run or compiled**.

**Remaining fidelity gaps:** original `11630`
also selects message text via `1C368/3210F`,
updates `6DAC`, `6DA8 = IRQ_tick+0x36` and invokes
`0F790` with held object metadata. That timed
presentation/diagnostic UI is not reconstructed.
Spellbook persistence through native SAVE/LOAD, original
CODE.2 mutable global flag/clock scheduling,
all other item/spell-use actions and exact DOS screenshot
parity remain outstanding. Therefore the source-only
inventory milestone stays **PARTIAL**, engineering
progress **56.25%**, **169 registered CTests** and
**19 Quick tests**; the next Windows **Full** build is
still deferred under the user's explicit rule.


## Part 8 continuation — CODE.2 11630 message/ribbon and 13184 expiry

**Original byte verification:** immutable local
`CODE.2` SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Object-1 physical `3AF24+116D5..11798` calls
`1C368(T spell id 0x42+bank*10+entry)` for success,
appends the DGROUP `6DA0` source text, or selects
DGROUP `6D9C` for capacity failure and `6DA4`
for an already-known spell. The actual object-2
`75F24+6D9C/6DA0/6DA4` DWORDs are `05A4`,
`05D4`, `05E4`, referring to:

- `6D9C → "you do not understand this spell to scribe it."`
- `6DA0 → " SPELL SCRIBED."` (appended to T name)
- `6DA4 → "that spell is already in your spell book."`

Original `11717/1176A` writes
`BYTE[6DAC]=1` then `DWORD[6DA8]=30700()+0x36`,
and calls `F790`. Original `F790:F7B4` detects
the pending message before attempting ordinary held
item description; `F66C` draws it with an original
text call `30508(0x65,0,...)`. The original inventory
modal `13118` resets `6DA8=0`/`6DAC=0`
on entry. Every modal input iteration at
`13184..131BB` checks the unsigned IRQ tick
and expires *only if* `now > deadline`, then
clears both globals *before* calling `F790`
to restore the default description. Equality
is not expiry, nor is original arithmetic
wrap-corrected.

The native source now:

1. Resolves all three error/success strings from
   the **real object-2 pointer table** with bounded
   NUL termination; damaged or truncated data
   fail rather than supplying invented text.
2. Maintains `MageScribeFeedback` containing
   the original branch, T spell-name resource
   ID, copied source message and `6DAC/6DA8`
   projection. Only a supplied 30700 IRQ
   observation schedules a deadline.
3. Uses `code1_tick_count(exploration_epoch)`
   (existing host BIOS-cadence approximation)
   when called by Win32; no host wall-clock
   duration substitutes for the literal `0x36`.
4. Resets pending state at PartyPanels entry
   and expires after unsigned `tick > deadline`.
   The presenter draws feedback in preference
   to ordinary item descriptions even when
   the newly learned scroll leaves an empty
   held cursor, then restores the ordinary
   item text when the deadline passes.
5. Adds focused existing-target assertions
   for the pointer-derived exact strings,
   `T["ARMOR"]` success prefix, duplicate/full
   branch identity, event flag persistence,
   deadline at 100→154, no expiry at 154,
   expiry at 155, and original DWORD overflow
   behavior, plus pending-message reset.

**Limitations:** the VGA ribbon/ornament from
original `F66C/2975C` and exact font/color
palette still lack faithful DOS comparison;
the native renderer reuses the *existing
provisional inventory ribbon* with source text
at row `0x65`. The Win32 exploration-epoch IRQ
proxy is not proven identical to an observed
DOS IRQ phase. Other inventory actions,
Level16 item `0x89`, SAVE/LOAD roundtrips,
and timing/visual acceptance remain open.

**Build/test status unchanged:** current source
has not been compiled, CTest-run, or DOS
pixel-compared. 169 registered CTests; manual
Quick 19; source engineering 56.25%; inventory
is PARTIAL. Do not dispatch the Windows build
until all inventory acceptance gates are closed.


## Part 8 continuation — original SAVE.DAT inventory section lossless codec

**Independent original-file identity check** (supplied local uploads):
- `SAVE.DAT` SHA-256 `f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d`
- `CODE.2` SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- supplied SAVE length `0x2448` / 9,288 bytes
- transient class-3 effects `u32[0x997]=0`
- persistent 62-container bank `0x09A7..0x0D50` / `0x3AA` bytes
- source CODE.2 `DGROUP:7330` descriptor widths exactly 438
  16-bit item slots, plus 62 known-bit bytes (876+62=938)
- 5,879 bytes after the bank: two renderer dwords, 15 device
  counters, ten 0x248 roster records and 16 final UI-context bytes
- original held item at `0x74=-1`, known byte `0x76=0`,
  32 event bytes `0x32..0x51` all zero, final 16 contexts FF
- original 63rd CODE.2 descriptor (`62`) has zero slots and is
  NEVER part of the 62-record SAVE bank. Its OBJECTS ID 329
  source mismatch remains intentionally uncorrected.

The new `InventorySaveCodec` is a **strict inventory projection
over a retained original SAVE.DAT byte image**, not a new native
format and not a general SAVE/LOAD gameplay implementation:

- It finds the middle bank by reading the authentic 32-bit
  effect count at `0x997`. Every transient record is 22
  bytes; the following eight spell-UI bytes and four gameplay
  tick bytes precede the first descriptor. Save file size must
  be exactly `0x2448 + count*0x16`, with `count <= 0x96`.
- It accepts original CODE.2-parsed
  `InventoryRuntimeState` as the descriptor template;
  validates exactly 62 4/6/8-slot records summing to 938
  bytes, and copies out container item IDs and per-slot flags.
- It decodes signed 16-bit held cursor ID and known byte,
  copies the entire 4×0x248 party array including item-known
  bits and the Mage scribing matrix, and all 32 persistent
  story flag bytes. It also captures the four last 4-byte
  party container UI contexts as **raw, not guessed hand UI**.
- When writing back it overlays **only** those owned fields,
  preserving all original description, map position,
  independent clocks, transient class-3 records, renderer
  controls, charged devices, saved roster, and unowned
  character bytes verbatim. The 63rd descriptor stays out
  of the serialized image.
- Pending `6DAC/6DA8` Mage feedback and an active
  `OpenInventoryContainer` runtime view are transient;
  import clears them. The four original raw UI contexts
  are retained, but safe hand/menu reconstruction on LOAD
  remains a separate unresolved behavior.

New `inventory_save_codec_tests` covers unchanged byte-for-byte
encode(decode(...)), mutable cursor/known bits, Mage spellbook
bytes, four fragment story flags, descriptor 0 and 61,
unserialized descriptor 62, source UI tail bytes and exclusion
of all non-inventory bytes. Synthetic saves with **0, 3 and
150 transient records** exercise dynamic offsets. It rejects
truncated files, invalid counts, wrong descriptor geometry
and stale snapshot offsets. This is a **new registered CTest**,
bringing source-defined tests from 169 to **170**;
manual Quick remains 19.

**Important engineering/acceptance boundary:** the codec
is not yet wired to a fully original CODE.2 general
save/load world-state and UI controller, nor are all
per-actor container-open transition phases reconstructed.
A returned snapshot is an inventory-scoped in-memory
projection; no user-facing save menu or complete session
restore is claimed. The other effects, Level16 item 0x89,
item-use actions and DOS visual/timing parity remain open.
Current source is **uncompiled/unexecuted on Windows**
since old run #640. No manual CI run initiated, per user.
Overall milestone score remains **56.25%** (inventory partial).
