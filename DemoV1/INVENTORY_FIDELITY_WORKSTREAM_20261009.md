# Ravenloft Demo v1 — Inventory-only full behavior reconstruction

> **Full-game integration invariant:** Every inventory,
> party, spellbook, original SAVE, actor container and
> quest-cinematic implementation in this workstream is
> part of the **single continuous Ravenloft game**.
> It must remain compatible with original startup,
> CODE.1 character creation, CODE.2 gameplay entry,
> and future level-to-level transitions without
> reinitializing or replacing persistent characters.
> The preview-only Level22 entry is not the release
> architecture. See
> [continuous-game integration contract](
> CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md).
> Development commits remain continuous; Windows
> builds are manual and user-authorized.


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


## Part 8 continuation — bounded live inventory SAVE session bridge

**Branch:** `demo-v1`. Independent local original-file check:
`SAVE.DAT` (9288 bytes, SHA-256
`f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d`)
has `u32[0x997]=0`, starts its container bank at
`0x9A7`, reaches `0xD51` after **938 bytes**,
ends with four `FF FF FF FF` inactive UI contexts,
and holds source level 22, coordinates `0x48000/0x48000`,
facing 270 and time `0xE0000`. Direct CODE.2
descriptor-table bytes confirm **63 descriptors**;
0..61 comprise **9 four-slot, 11 six-slot, 42
eight-slot** entries, totaling **438 item words**;
descriptor 62 is zero and remains unpersisted.

`InventorySaveCodec::apply_to_matching_session` now
copies a *validated* source snapshot's full four
0x248-byte actors, all 32 persistent event-flag bytes
and CODE.2-backed inventory runtime (held identity/known,
62 persistent containers, Mage table/source metadata)
into the live session, while resetting transient
scribe-feedback and open-container views. The reverse
`capture_matching_session` overlays the current
session/held/containers/known/flags on the original
byte-preserved snapshot; `encode` preserves unknown
and non-inventory source bytes unchanged.

Safety gates explicitly **reject without state mutation**:
- inconsistent source saved level, coordinates A/B, heading
  or time-of-day against the current initialized native world;
- any source `0x16` transient/moving-effect records
  (the *standalone byte codec* still supports them);
- unsupported `6FA0` contexts: interrupted phase-0/1/2
  animation replay, ambiguous/missing original hand-object
  evidence. Both fresh `FF FF FF FF` and post-close
  `FF FF 00 FF` load; **all four** phase-3 contexts may
  source-restore when each resolves uniquely from OBJECTS;
- a currently open native hand-container view **on import**.
  Export may persist an active view once it matches `6FA0`;
- damaged 62-descriptor geometry / malformed SAVE lengths.

`ExplorationRuntime::import_matching_save_inventory` and
`export_matching_save_inventory` are now the **actual
native runtime bindings**. Import requires initialized
gameplay, preserves the original byte image internally,
then invalidates stale world-hit frames by incrementing
`world_scene_revision_`. Export requires an imported
backing image, the *unchanged* world-scene revision, and
the same saved camera/clock state. Reinitializing
gameplay clears any prior backing. The runtime does
**not** wire these methods to the incomplete native
SAVE/LOAD menu, because doing so would falsely imply
that NPCs, level mutations, world effects, other globals,
roster and independent clocks have been fully restored.
These routines must not be used as general gameplay
save slots yet.

The existing `inventory_save_codec_tests` exercises
the new positive live copy/roundtrip with mutable
inventory/known flags, Mage matrix and story flags,
as well as failed imports for level/position/facing/time,
moving effects, stale UI states, and open views.
`exploration_runtime_tests` also asserts that an
uninitialized runtime refuses import/export without
acquiring a backing image. No new test executable:
**170 registered CTests; 19 Quick**.

**Unverified by native compilation or execution.**
The user's deferred Windows build has not been
dispatched. Inventory and full SAVE/LOAD are NOT
accepted; engineering tracker **56.25%**. Further
requirements: restore original 6FA0 open-container
phase, full moving/effect/roster/global serialization,
actual LOAD/SAVE menu/IO atomicity, native world
reconciliation, Level16 item 0x89, other special
inventory item use and original DOS UI comparisons.


## Part 8 continuation — exhaustive CODE.2 6FA0 context/phase integration

**Primary evidence:** original supplied `CODE.2`, object-1
SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
(file image code-object1 physical base `0x3AF24`).
The relevant x86 instructions were inspected directly with
`objdump -D -b binary -m i386 -M intel`:

| Logical routine | Original 6FA0 behavior |
|---|---|
| `10A70..10BB4` | per-actor phase-switch renderer: phase 1/2 rim/panels, phase 3 descriptor-backed contents and category-specific item graphics |
| `10FAC..110B3` | initializes context bytes +0/+1/+3 and then sets phase byte +2 to **0, 1, 2, 3** in order; each pass calls `10BBC`, `1A724` and page logic; phase remains 3 |
| `110B4..1118A` | starts from **3**, renders 3,2,1,0, then clears ONLY bytes **+0, +1, +3** to FF; +2 remains **0** |
| `10EEC..10F06` | per-character inventory display reads signed +0 to detect active context and signed +2 to route phase rendering |
| `11268..11316` | draws all four independent actor panels rather than a single global open-container panel |
| `21648..216C4` | on the special item-to-hand path, clears a formerly active record to FF×4 and, when item category==1, copies descriptor +4 into +0, OBJECTS +31 into +1, stores **literal phase 3**, stores the **low byte** of object ID in +3 |
| `12E70..12F16` | accesses character-local context +0 before forwarding the active container cell rectangle to `11C3C` |
| `12470` | swaps complete four-byte `6FA0` contexts alongside the party actor images |
| `066E8` serializer / `06CBA` loader | persists/restores all **four** records as the final 16 bytes of original SAVE.DAT |

**Critical correction to the earlier partial implementation:**
an inactive per-actor context is **not** necessarily
`FF FF FF FF`. That form is the fresh initialization.
After `110B4` closes a container, the executable leaves
**`FF FF 00 FF`**, because it does not clear phase byte +2
after its last phase-0 draw. Previous fail-closed bridge logic
incorrectly rejected this reachable original state.
The normalized native compatibility model now explicitly
recognizes BOTH original inactive forms, while keeping them
different at the original byte layer.

**Another essential source constraint:** `6FA3` is a
**one-byte object ID**, despite the equipped hand being a
signed 16-bit OBJECTS ID. For example object 258 must be
saved in `6FA3` as 2, and object 329 as 73. The four-byte
context contains **NO hand index**. Restoring a native open
container must therefore inspect BOTH original actor hand
slots (item slots 14, 15), compare original low-ID bytes,
then verify full object identity via the loaded
`OBJECTS.DAT` catalogue: category 1,
`container_state_index` must select source descriptor
`6FA1`, and its descriptor subtype must equal
`6FA0`. If zero or two hands match, reject the live
restore rather than guessing or migrating an item. The
source state-61/62 mismatch remains **unmodified** and
selector 62 is not accepted as a serializable container.

**Implemented in native source (this checkpoint):**

1. `PartyContainerUiContext` preserves four raw bytes and
   exposes fresh/closed/phase3 states without losing
   `FF FF 00 FF`. Explicit `begin_open_10fac`,
   `advance_open_phase_10fac`, and
   `advance_close_phase_110b4` project the exact
   frame-phase transitions **without fabricating DOS IRQ
   timing or VGA rendering callbacks**.
2. `InventoryRuntimeState` now holds FOUR separately
   persistent actor `6FA0` contexts as well as its
   existing one-selected native interactive
   `open_container`. Category-1 item transfer/opening
   sets `[subtype,index,3,lowID]`; closing the selected
   view records `[FF,FF,00,FF]` for the same actor
   instead of discarding source UI metadata.
3. The original SAVE codec maps final 16 raw SAVE bytes
   into all four source runtime contexts on decode;
   during live export, it writes the **current runtime**
   contexts into the byte-preserved original SAVE image.
   A currently displayed native hand container must
   match the persisted actor/index/hand/object record.
4. `apply_to_matching_session` accepts two inactive
   historical states and independently validates each
   **fully-open phase3 view**. Each actor must have a
   unique hand matching original category/index/subtype/
   low-ID identity. A source OBJECTS catalogue is
   mandatory for active restoration.
   The entire restore is preflighted before mutating live
   inventory, event flags or the actor images.
5. `capture_matching_session` now supports exporting a
   genuinely open and uniquely resolved native hand
   container instead of rejecting **all** open containers.
   It still refuses a mismatch between the raw 6FA0
   records and the selected native view.
6. `PartyModalPresenter` now draws fully-open phase3
   contents from each actor's original source context,
   using original RES2 `9+subtype` surround and RES1
   item graphic IDs. This corrects a previous
   single-actor-only drawing assumption. Character
   mode still suppresses container art for its lane.
7. `InventoryPartyOrderModel::exchange` swaps all four
   raw bytes of the affected actor contexts; the
   selected native open context changes actor index
   simultaneously.

**Bounded unsupported cases:**
- original interrupted `10FAC/110B4` phase 0/1/2
  animation can be **decoded and byte-roundtripped**,
  but live restore rejects it until actual original
  10BBC/1A724 page-flip art and time ordering are bound;
- original SAVE may contain multiple concurrently open
  actor contexts. The subsequent checkpoint now supports
  source-validating and interacting with all four separately.
  The source still serializes NO selected native pointer,
  so the first open actor is selected on import, and click
  dispatch selects the actually clicked actor;
- the 6FA0 state alone does not prove all hand-side/menu
  input semantics or the complete DOS status frame.
  Exact mouse clicks, partial animation rendering and
  multi-lane content interaction remain open;
- **all previous full-world SAVE safety rules remain:**
  no moving effects in the live inventory-only bridge,
  same level/camera/facing/time, unchanged world revision,
  no full-game SAVE/LOAD menu claim.

**Regression coverage committed (not executed):**
existing `inventory_save_codec_tests` gained source
FF FF 00 FF decode/encode, direct 0→3/3→0 transition
machine, live phase3 import/export, closing and saving,
low-byte aliasing (IDs 2/258), valid and ambiguous
hand matches, no OBJECTS evidence, subtype/index
mismatches, interrupted phase rejection, multi-actor
open-state rejection, and previously covered original
0/3/150 transient-effect dynamic SAVE byte preservation.
Existing `inventory_runtime_tests` checks live source
phase3 and exact post-close bytes; existing
`inventory_party_order_tests` verifies complete actor
context exchange and reversibility. No new target was
registered: **170 CTests, 19 Quick**. All added tests
remain uncompiled/unexecuted; Windows build remains
deliberately deferred under the user's request.

**Inventory fidelity remains PARTIAL; engineering progress
remains 56.25%.** Next gated steps: multiple interactive
character-lane container views; true 10FAC/110B4 VGA
animation/timing; native complete SAVE/LOAD through other
world/effect/clock domains; Level16 seed/item side effects;
remaining original item action categories and DOS visual
acceptance.


## Part 8 extended checkpoint — simultaneous actor containers, actual CODE.2 click routing and phase artwork

**This section supersedes the earlier "one native interactive
view" limitation, without claiming complete DOS animation.**
Directly re-disassembled the supplied CODE.2 object-1
at `12E70..12F1C`, `10EEC..10F06`, `10A70..10BB4`,
`21648..216C4`, and `110B4..11185`.

### Source instruction evidence

- `12E7B..12E89`: clicked VGA column is **signed-divided by
  0x50** to select character actor, *not* resolved through a
  previously selected global container context.
- `12EDE` reads `signed byte [6FA0+actor*4]` to check
  whether that actor's container UI is present. When
  mouse row is `0xDB..0x11F` and actor-local column is
  `5..0x4D`, `12F0C` dispatches `11C3C(actor,row,
  local_column,flags)`. Other lanes remain independent.
- `10EE0..10F01` reads both `[6FA0+actor*4]` and
  `[6FA2+actor*4]` and calls `10A70(actor,phase)`.
- `10A8F..10AA2`: phase `0` means no additional
  container drawing; phases **1 and 2** dispatch the
  original transition RES2 graphic at **row `0xC4`**.
  `10AA7..10AC6` computes the resource index exactly as
  `0x14+2*subtype+phase-1` (bank 2).
- `10ACB..10BAE`: phase **3** selects the original
  RES2 frame `9+subtype` at **row `0xCE`** and renders
  the descriptor's grid contents in its 80-pixel actor
  lane using the original `0x12` horizontal and
  `0x22` vertical cell spacing, rather than an
  invented procedural container UI.
- **Two different source close/reset paths:**
  `110B4..1117D` plays the animated
  `3→2→1→0` sequence, then clears only offsets
  `+0,+1,+3`, leaving
  `FF FF 00 FF`. In contrast,
  `21648..21670`, used when transferring hand items,
  clears all four bytes to **`FF FF FF FF`**, and
  optionally initializes a new `phase=3` context at
  `21693..216BD`. Conflating these two paths would
  corrupt SAVE bytes.

### Native source changes

1. `InventoryRuntimeState` retains all four original
   `6FA0` records; `open_container` is now explicitly
   the **current interaction selection**, not the sole
   source-open view. The new
   `select_container_actor(party,objects,actor)`
   matches the selected actor's saved subtype, signed
   descriptor selector, category-1 original hand object,
   8-bit low object ID and both possible full-width
   hand IDs. It fails without changing the selection if
   the match is ambiguous, invalid or inactive.
2. `swap_actor_container_slot` selects by actor and
   then invokes existing bounded `11C3C` item/known-bit
   transfer. The Win32 inventory pointer route now
   derives the actor from the **clicked column**, checks
   the actor's original phase3 `6FA0` context and
   uses its descriptor's grid geometry. Clicking a
   different open actor does not close the other panel.
3. The existing hand-container opening path now resets
   only the target actor's `6FA0` state. Other actor
   contexts continue to exist. `close_actor_container`
   retains `110B4` phase-zero close semantics. A
   distinct `reset_actor_context_21648` implements
   hand-transfer all-FF reset; taking a container out
   of a hand no longer incorrectly writes `FF FF 00 FF`.
4. SAVE input now validates **all four independent**
   source contexts and permits simultaneous phase-3
   views. It selects the first open actor only as a
   *native interaction default*; it does **not** emit
   an invented "selected actor" byte. A later click
   can select any valid open actor. SAVE capture
   validates every active context and accepts no
   selected pointer when remaining actors' valid
   contexts are open (e.g. following a close).
5. Modal presentation preserves all actor-lane phase3
   RES1/RES2 item displays, adds the EXACT phase1/2
   RES2 artwork at VGA row `0xC4`, and suppresses
   legacy fallback of a phase3 panel when a source
   context is actually in partial phase 0/1/2.
   `InventoryScreenModel` exposes the source
   `original_container_transition_res2_graphic()`
   formula with full subtype/phase bounds.
6. Regression assertions in existing
   `inventory_save_codec_tests` cover importing
   two simultaneous actor contexts, switching click
   selection without closing the other view, clearing
   only the chosen actor, preserving both source
   records across original-format export, and rejecting
   a corrupt second context without session mutation.
   `inventory_runtime_tests` covers independent
   actor selections and original transfer/close byte
   differences. `inventory_screen_tests` verifies
   every phase1/2 graphic for all **seven** source
   subtypes, and rejects invalid phases and subtypes.

### Acceptance still open

The original `10FAC/110B4` multi-frame animation
includes drawing, page switching, and timing through
`10BBC/1A724/2F29C`. The source has phase progression
and correct per-phase graphic **identity**, but still
needs DOS-vs-native pixel and timing acceptance.
Mid-animation SAVE contexts remain losslessly decoded,
but live restoration rejects incomplete phase-0/1/2
animation instead of falsely treating it as a completed
panel. Unowned SAVE world/effect/clock domains and
remaining special-item actions are still incomplete.
No source/build provenance guarantees a fully usable
SAVE menu yet.

**Git commits are continuous and separate from builds.**
No Windows build or CTest was started. The registered
total remains **170 CTests / 19 Quick**, and engineering
progress remains **56.25%** with inventory PARTIAL.


## Part 8 extended checkpoint — CODE.2 Level16 object 0x89 four-Seed ritual

**This checkpoint upgrades the prior broad Level16 object 0x89
"special action pending" guard to an exact, source-backed
read-only condition model. The ultimate event/cinematic
side effects remain unimplemented and are still guarded.**

### Primary executable evidence

Direct inspection of the uploaded CODE.2 object-1
(SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`)
identified the complete `21880..2195A` test, called
directly by world-item constructor `2195C` after
appending OBJECTS ID **`0x89` / decimal 137 /
`SEED OF MORNINGLORD`**.

The original conditions are exactly:

1. `21892` compares the live level `C6C8` to
   **0x10 (Level 16)**. Other levels return immediately.
2. `2189F..218AB` calls `1C1DC(0x44)` to test
   persistent event/story bit **`0x44`**. If already
   set, it exits. Native bit projection:
   `persistent_event_flags[0x44/8] & (1 << (0x44%8))`.
3. `2188B..21892` copies eight original embedded
   object-1 bytes from logical `0x207E0`:
   **`0D 13 0B 15 0D 17 0F 15`**,
   interpreted as four `(x,y)` site coordinates:
   **(13,19), (11,21), (13,23), (15,21)**.
   `30D30(...,0,0x10)` zeroes four DWORD hit markers
   without overwriting the adjacent eight-byte site list.
4. The loop `218C7..21918` scans all live
   `0x16`-byte family-C loose-item records at
   `0x17618+index*0x16`, comparing the
   **object ID word at +0x0E** to `0x0089`.
   It compares each unsigned source x byte against
   `WORD[record+6]` (high half of Q16 coordinate A)
   and each unsigned source y byte against
   `WORD[record+2]` (high half of coordinate B).
   **Fractional coordinate bits do not matter.**
   Matching an item at a site sets that site's own
   distinct DWORD marker; repeated items at just
   one site cannot complete the other three.
5. `2191A..21933` requires **all four** marker
   DWORDs nonzero. On success `21935..21950` calls
   `1C124(0x44)` (set persistent story flag),
   `13EF8(8)`, then `15050(0x73)` in that order.
   Cross-references certify `13EF8` as the numbered
   interactive presentation runner, so **8 is the
   original numbered event**. Direct disassembly of
   `15050..15142` establishes that `0x73` is a
   granted OBJECTS ID, **HOLY SYMBOL OF RAVENKIND**
   in the supplied object database. It first offers
   the reward to an empty cursor, otherwise the first
   free party inventory slot, otherwise family-C
   world placement. Do not skip or prematurely set
   the persistent quest flag without those actions.
6. The caller at `21AC7..21AD6` invokes this
   check after a successful `2195C` append only
   when the incoming item ID is `0x89`. The world
   loose-item array is bounded at **`0x12C` (300)**
   entries, with first-record eviction before append.
   A faithful *candidate* check must therefore ignore
   an evicted first seed rather than count it.

### Native implementation

`Level16SeedRitualModel::evaluate_21880()` now
projects the exact source behavior without mutating
world state or event flags:
- source level and `0x44` flag gates;
- original four site coordinates;
- source loose-item object ID and Q16 high-word
  comparisons, ignoring low fraction;
- four independent hit markers and
  `ReadyForSourceActions` only when all four present;
- optional newly inserted seed candidate and correct
  300-record first-item eviction modeling;
- the original follow-ons: numbered interactive
  event **8** through `13EF8`, followed by
  `15050(0x73)`, granting object 115
  **HOLY SYMBOL OF RAVENKIND**.

`InventoryWorldDropModel::drop()` now performs
ordinary original position/geometry/overflow
validation and constructs the source-correct
candidate record for Level16 as for any other
level. It then checks the POST-append ritual
state *without first changing the world*:

- no completed ritual: **ordinary world drop
  succeeds**, adds item 0x89 and consumes the
  cursor, including Level16;
- event flag 0x44 already set: original returns
  early from the ritual and the ordinary item
  drop proceeds, with no repeated story action;
- a new placement would complete all four
  source sites while flag 0x44 is unset:
  **`SourceSpecialActionPending`**, with
  original world array and held cursor unchanged,
  because native numbered-event-8 presentation
  and original HOLY SYMBOL OF RAVENKIND reward
  grant bindings are still absent.
  The native port does NOT mark the story flag,
  drop the triggering fourth seed, or falsely
  claim the original sequence occurred.

New focused assertions are in the existing
`inventory_world_drop_tests` target:
zero/one/three/four sites, wrong objects, duplicates,
Q16 fractional-coordinate independence, event
flag gate, original effect-argument identity,
successful non-triggering Level16 drops,
blocked fourth triggering drop without consuming
cursor, success after flag-set, max-300 eviction,
and not-on-Level16 bypass.

**Acceptance:** predicate and non-triggering
Level16 world drops are source-modeled;
the fourth-seed cinematic/effect transition remains
a hard missing integration, not an implied
completed quest. No Windows build or CTest
run was initiated. Registered CTests **170**,
Quick **19**, engineering tracker **56.25%**,
inventory **PARTIAL**.


### Source-bounded Holy Symbol reward transfer (CODE.2 15050)

Further original CODE.2 disassembly at `15050..15142`
confirmed exactly how the reward item from
the four-Seed ritual is distributed.
The supplied OBJECTS.DAT record **0x73 (115)**
is the **HOLY SYMBOL OF RAVENKIND**.
For this argument, unrelated `15050` special
cases for object IDs `0x90/0x78/0x9B` do
not fire. The source follows this sequence:

1. `150AB..150C8`: if signed cursor item at
   `0x8600` is **-1**, call `1A344(0x73,0)`,
   equipping the reward on the *cursor* as
   **unidentified (known byte zero)**. No party
   or world item is modified.
2. Otherwise `150C9..1511D` walks **active**
   actor records in exact order **0,1,2,3**,
   skipping character status `0x8000`.
   For each actor it checks **ordinary inventory
   slots 0..11** (NOT all 21 equipment/hand
   slots), selects the first signed `-1`
   item ID, writes `WORD=0x0073`, then calls
   `0F238(actor,slot,0)` to **clear only the
   corresponding known/identified bit**. The
   previously held cursor is left unchanged.
3. If the cursor is occupied and every eligible
   ordinary slot is occupied, `1511F..1513C`
   invokes `2195C` to append a **family-C
   loose/world reward** at the current position
   with source action `0x80` and no fabricated
   inventory slot. This invokes the usual
   authored placement/eviction semantics.

The new `HolySymbolRewardModel::grant_15050()`
implements only this verified `0x73` case:
the empty-cursor and first-free-party routes
mutate exactly their source-owned item/known
bytes; `NeedsWorldSpawn` returns without any
mutation when the original world-placement
fallback is required. The latter remains
a separate effect handoff, not a dropped reward.

**Important**: the Level16 4-seed completion
still returns `SourceSpecialActionPending`
without setting event `0x44`. The new
reward helper is *not invoked prematurely*
because original numbered event `13EF8(8)`
must execute before reward routine `15050`.
This avoids skipping original story presentation
while preserving a fully source-backed future
reward-transfer implementation.

`inventory_world_drop_tests` now additionally
covers cursor-known reset on receipt, first
empty ordinary slot, actor 0→3 priority
with inactive characters skipped, slot
known-bit clearing, held cursor preservation
during party insertion, and nonmutating
full-party/full-cursor world fallback.
This is an expansion of the existing CTest,
not a new registered test. Registered
**170 CTests / 19 Quick**, still unexecuted
on current source. The engineering score
remains **56.25%** and inventory is PARTIAL.


## Part 8 continuation — source-correct 15050 world fallback and CINE08 event identity

**Primary binary source revalidation:** CODE.2 object-1
`2195C..21AD9`, `21880..2195A`, `15050..15142`,
`139AC..13A5C`, `13AD8..13BB1`,
`13EF8..13F60` were freshly inspected as original
x86 instructions. The original immutable upload remains
SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
No generated or guessed bytes substitute for the source.

### 15050 reward's WORLD fallback is now source-bound

The prior model implemented 15050's empty-cursor and
active-character first-free ordinary-slot routes but
returned `NeedsWorldSpawn` without placing the original
HOLY SYMBOL OF RAVENKIND when all those destinations
were unavailable.

Direct `1511F..1513C` disassembly establishes the exact
downstream `2195C` argument sequence:

```text
push 0          ; do NOT clear live cursor (last 2195C arg)
push 1          ; forward-position attempt enabled
push [255D4]    ; source current player Y (Q16)
push [255D0]    ; source current player X (Q16)
push 0          ; object known/identified byte
push 0x73       ; object ID 115: HOLY SYMBOL OF RAVENKIND
push 0x80       ; source screen-direction seed
call 2195C      ; original family-C loose-item constructor
```

By contrast, the modal **D** inventory drop at
`13118` invokes `2195C` with clear-held flag 1.
The original 2195C implementation computes heading,
attempts `2D5D0(...,0x190)`, falls back to player
location when denied, appends a 22-byte family-C
record with `(known&1)<<10` in its packed state,
and if already at 300 loose items evicts the oldest
record before appending.

This checkpoint implements:

- `InventoryWorldDropModel::spawn_holy_symbol_reward_15050`
  reuses the **same** previously reconstructed
  `2195C` placement/validation/eviction logic using
  ephemeral original source parameters
  `held={0x73,0}`, so the **actual** held cursor
  is never passed to a clearing drop routine.
  The method returns the real spawned family-C
  index or an explicit invalid-position rejection;
  it doesn't run story event 8 or alter flag 0x44.
- `HolySymbolRewardModel::grant_with_world_15050`
  completes the original inventory transfer precedence:
  empty cursor → first free ordinary slot in active
  actor order → source `2195C` world fallback.
  Successful fallback returns `SpawnedInWorld`
  with its actual index. Rejected world placement
  returns `WorldPlacementRejected` with **no**
  change to party, held item, known flags, or
  original family-C records. The earlier
  `grant_15050` pure inventory route and
  `NeedsWorldSpawn` status remain available
  to callers that have not yet bound a live
  `LevelWorldState`.
- Regression assertions verify the world reward's
  object ID 0x73, unidentified bit, exact Q16
  fallback to player coordinates, source 300-entry
  evict-first behavior, unchanged actual cursor
  and party state, corrupt-world no-eviction
  guarantees, and cursor/party precedence over
  even an invalid world backing.

**IMPORTANT:** the combined reward helper is NOT
yet wired into a ritual completion. The original
`21880` performs its three steps synchronously
*after the fourth 0x89 seed is appended and cleared
from the cursor*: sets bit `0x44`, plays numbered
event 8, **then** calls `15050(0x73)`. The current
native `InventoryWorldDropModel::drop` deliberately
returns `SourceSpecialActionPending` before the
fourth seed is committed because the live native
cinematic/event handoff is still incomplete. The
source-backed grant helper must never be called
before that event has been completed.

### Event 8 resource and extant native ANM support

Original `139AC` copies its eleven-byte
`CINE00.ANM\0` filename template from CODE.2
logical `0x131D0`; replacing its two decimal
digits with number 8 results in **`CINE08.ANM`**.
The call chain is unambiguous:

```text
21880 -> 13EF8(8)
          -> 13AD8(8)
              -> 139AC(8)  => CINE08.ANM
          -> 13834 frame/packet interpreter
          -> 13C88 post-presentation cleanup
       -> 15050(0x73) reward grant
```

Repository `Resources/ANM/INVENTORY.md` records
`CINE08.ANM` as a 9,255,472-byte original animation
containing **182 frames** (SHA-256
`cfbc8b5835906ceedcf74e8c202430cd0b5b4e4d4c7c00054d42b934def817f3`).
The existing native `AnmCinematic` decoder and
Win32 `play_cinematic()` already support the original
packet/timeline presentation for startup event 0
and event 12; they are not yet connected to the
inventory modal's fourth-seed completion. Code now
pins `Level16SeedRitualCheck::kNumberedEventFilename`
to the source exact `CINE08.ANM`, and tests its
identity alongside event ID 8 and reward ID 115.

**Next integration requirement:** thread a safe, explicit
game-session event-8 presentation handoff through
the native inventory modal, preserving original
story-flag order, consuming the last seed exactly
once, giving exactly one Holy Symbol reward through
`grant_with_world_15050`, and then rebuilding
native gameplay/audio/scene state after cinematic
teardown. The general `run_exploration` entry does
not currently receive the Win32 audio/MIDI presenters;
blindly invoking a startup-only cinematic helper
would be an unsafe partial implementation.
Cinematic Escape/quit, resource failure, and
world fallback must also be distinctly handled.
No such live completion is claimed in this checkpoint.

**Git policy:** every code/test/doc change committed
to `demo-v1`. **Build policy:** user-managed;
no Windows CI, local compile, or test execution
initiated. The same existing `inventory_world_drop_tests`
target is expanded; **170 registered CTests,
19 Quick** remain. Engineering tracker **56.25%**,
inventory milestone **PARTIAL** pending full
acceptance and runtime validation.


## Part 8 continuation — live Level16 CINE08 ritual event and source-order reward (October 9)

**Supersedes the preceding "pending CINE08 integration" checkpoint.**
CODE.2's original 0x89 fourth-seed event is now connected
across the native inventory model, exploration runtime,
existing Win32 cinematic player and original audio/MIDI bridges.
No invented scene text, reward object, or playback resource
is substituted. The executable evidence remains:
`2195C -> 21880 -> 1C124(0x44) -> 13EF8(8) ->
15050(0x73)`. Numbered event 8 resolves through
`13AD8/139AC` to original **`CINE08.ANM`**, whose
source archive contains 182 original animation frames.

### Source-order completion transaction

New `Level16RitualCompletionModel::complete_21880`
is invoked ONLY after a genuine pending ordinary D-key drop:

1. Validate `level==16`, held item `0x89`, original
   loose-object site configuration, placement geometry,
   existing persistent event bit and source world capacity.
   On a copy of the original live state, first verify that
   the regular `2195C` call returns
   `SourceSpecialActionPending`.
2. Preflight actual fourth-seed placement on copies.
   Set `0x44` on a copied session before invoking the
   existing faithful `2195C` constructor so its recursive
   `21880` probe terminates; verify the fourth seed was
   appended/oldest item evicted as in the original source.
3. Preflight the subsequent `15050(0x73)` allocation on
   copied state, including the identified-bit and 300-item
   family-C world fallback. Failed placement returns
   `RewardPlacementRejected` WITHOUT showing the event
   or mutating original world/party/cursor/story flags.
4. Commit the fourth seed to the live world, consume the
   actual held `0x89`, and set live persistent flag
   `0x44` BEFORE beginning numbered event 8. The event
   callback sees the original source-order intermediate state:
   seed placed, cursor now empty, flag set, and no Holy
   Symbol prematurely granted.
5. Call the supplied event-8 presenter synchronously.
   On `Completed`, run `15050(0x73)` AFTER cinematic
   completion: reward the previously empty cursor, else
   the first eligible active actor/ordinary slot, else
   source `2195C` world spawn. Report the actual reward
   destination; never grant the reward twice.
6. On `Escaped`, `Quit`, presentation exceptions, or
   failed post-event grant, restore the complete pre-event
   in-memory party, source world, held cursor, and 32
   original story flag bytes. This is a **deliberate
   native fail-safe policy**, NOT yet a proven emulation
   of 13EF8's exact DOS Escape semantics. A missing
   presentation callback is rejected as invalid, rather
   than being treated as successful silent playback.

Separate strongly typed playback outcomes and ritual status
records disambiguate `NotReady`, `InvalidPlacement`,
`RewardPlacementRejected`, `Escaped`, `Quit` and
`Completed`.

### Native live-world and Windows integration

- `ExplorationRuntime::complete_pending_seed_ritual()`
  injects its existing CODE.2 `2D5D0(y,x,0x190)`
  collision predicate, original object definitions,
  renderer tables and C02F coordinate-rounding mode.
  Success, Escape and Quit invalidate previously
  displayed world hit tests by incrementing the scene
  revision; presentation exceptions do likewise.
- `win32_main.cpp` passes the SAME native
  `Win32AudioPlayer` / `Win32MidiPlayer` instances
  already used at startup into `run_exploration`.
  All three existing exploration entry callers
  (preview, CHARS-present startup, new-character entry)
  received the additional arguments.
- On the precise `SourceSpecialActionPending` D-key
  result in the inventory modal, the host loads and
  parses `CINE08.ANM` before starting the transaction.
  The existing `play_cinematic()` displays authored
  ANM frames, palettes, subtitle/raster packets,
  audio/MIDI commands and waits. During playback,
  live modal hit input is detached, the inventory
  pointer art is hidden, and the native inventory
  mode is suspended. On normal return or error the
  original modal input and cursor are reattached,
  audio/MIDI are stopped, the rendered-scene cache
  is invalidated and host frame pacing reset.
- `CinematicResult::Completed` awards the actual
  source Holy Symbol and resumes the modal; Escape
  aborts without recording a completed quest; Quit
  aborts and closes exploration. Failure to load
  or decode the original ANM is guarded and reports
  a **host-only diagnostic title**, never invented
  original DOS story dialogue.
- The existing native cinematic player still needs
  the exact original `13C88` teardown / CINE08
  interaction-result / palette timing acceptance
  audit. World state is currently rolled back on
  canceled presentation for safety rather than
  claiming that these cancellation semantics are
  certified by DOS traces.

### Regression tests — committed, NOT executed

The existing `inventory_world_drop_tests` was
extended with the source-order live callback
assertions (fourth seed before callback, bit 0x44
set during callback, cursor emptied, reward absent
until callback completes), successful once-only
Holy Symbol award, repeat-event suppression,
Escape and Quit rollback, exception recovery, and
missing-callback rejection. The existing
`exploration_runtime_tests` checks that an
uninitialized world cannot invoke an event
callback or change gameplay state. No new CTest
target was created; **170 CTests registered,
19 Quick** remain.

**Honest acceptance boundary:** this is the first
complete native *source-wired* Level16 ritual
pipeline. Neither compilation nor any Windows
or DOS behavioral playback of the new source
has occurred. Exact original CINE08 interactive
presentation semantics, cinematic cancellation/
teardown, IRQ scheduling, level transitions,
and full game save/reload continue to require
actual acceptance testing. Engineering tracker
remains **56.25%**, inventory **PARTIAL**, and
the user's manual-build-only rule remains in force.
Every source, test and documentation edit was
committed directly to `demo-v1` as it was made.


### CODE.2 13C88 exit audit — event 8 uses shared cleanup, not a transition branch

A new direct `objdump -D -b binary -m i386 -M intel`
audit of the supplied `CODE.2` object-1 code at
`0x13AD8..0x13C61`, `0x13C88..0x13EF5`, and
`0x13EF8..0x13F60` closed an important source
branch-selection uncertainty.

- `13EF8` calls `13AD8` with the numbered
  event and runs the `13834` ANM processor.
  It then forwards the returned interaction
  result and saved setup state into `13C88`
  before returning to the caller at `21880`.
- `13AD8` saves current audio state through
  `29048`, `29050`, `29098`, and `290A0`;
  it configures event resource/audio startup
  (including globals `7F78/7F7C/7F84`).
  Event 8 follows the ordinary numbered-event
  path, not special event-2, event-10, or
  event-12 selection branches.
- `13C88` indexes a **nine-entry, DWORD
  jump table at logical 0x13C64** by
  `(event_number-1)`. The exact decoded
  entries are:

  | Event ID | post-event dispatch target |
  |---|---|
  | 1 | `0x13D0E` |
  | 2 | `0x13D1D` |
  | 3 | `0x13D1D` |
  | 4 | `0x13D1D` |
  | 5 | `0x13D1D` |
  | 6 | `0x13D69` |
  | 7 | `0x13DB5` |
  | **8** | **`0x13E33`** |
  | 9 | `0x13E17` |

  Thus **event 8 does not take the event-2/6/7/9
  hard-coded coordinate/level relocation branches**
  and does not call any of their `181B4` level
  loaders from this post-event dispatcher.
- At `13E33`, event 8 takes the ordinary
  post-presentation graphics/audio restoration:
  `325B0` data/palette path over C054,
  `29234` raster/resource refresh, conditional
  `29050/28DCC/28EF0/28CE8` music handling,
  `29008`, `28F58`, `28E58` saved-state
  application when permitted, followed by
  common `1F8FC` input reset and `19CF4`
  teardown. Earlier in `13C88` it also calls
  `310B0`, `312DC`, `27FF0`,
  `28DCC(0x9C4)`, `20294`,
  `30D30(C054,0,0x300)`, `29234`,
  and `1A288`.
- Therefore the new native Win32 event-8
  wrapper is **correct not to hard-code a
  level relocation at cinematic exit**.
  However the current host's
  `audio_player.stop_all()`, `midi_player.stop()`,
  restored pointer input and normal exploration
  render refresh are **NOT** a fully source-exact
  reconstruction of the `13C88` palette/music/
  VGA and cursor-state restoration. They are
  provisional safety cleanup pending a live
  DOS-vs-native transition trace.

Additional regression assertions in existing
`inventory_world_drop_tests` now deny
`CINE08` if the fourth Seed would not complete
all four sites, the source level cell geometry
is corrupt, or the active level is not 16.
These negative calls must not dispatch an
event callback, consume a seed, or set bit
`0x44`. Still **170 CTests / 19 Quick**.
No compilation/CI/user-triggered build has
occurred. All source and docs committed to Git.



## Cross-workstream checkpoint — noninvasive Level22 development state inspection

The entire inventory reconstruction is one subsystem of the
reusable full-game Level22 vertical slice, not a second isolated
inventory application. A new opt-in native flag
`--explore-level22 --dev-inspect` exposes a read-only caption
view of the actual live authoritative game state. It captures
separately:

- all four original `0x248` actor records;
- current cursor and 63 original container descriptor runtime
  contexts, including every actor's `6FA0` phase bytes;
- original 32-byte story flag bank including Level16 bit 0x44;
- live LevelWorldState NPCs, loose/placed items, Q16 coordinates,
  level cell words, dynamic group1/group2 10-byte controls and
  authored animation counts;
- original level/camera/facing/time data.

Each domain is fingerprinted with explicit little-endian
64-bit FNV-1a; the opt-in host caption shows shortened
eight-digit hexadecimal diagnostics, item/NPC and flag counts
and a change counter. Hashes are **developer diagnostics only**,
not a replacement for source SAVE bytes, a security checksum,
or DOS gameplay data. They do not allocate/drop items,
modify CHARS, mutate story flags, write disk captures,
change original indexed pixels, or alter production startup.

The new `dev_level_diagnostics_tests` checks one-domain
mutations, stable repeat captures and restored fingerprints.
It becomes CTest registration **171**, while manual Quick
remains 19. None of the current source has been built or
executed. Fidelity score **56.25%**, inventory PARTIAL.
This milestone aids eventual original cold-start →
character-creation → continuous level-transition testing
without claiming any currently missing inventory behavior
is completed.

[Permanent developer testbed acceptance matrix](
DEV_LEVEL_TESTBED_20261009.md) ·
[Continuous-game integration contract](
CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md).
