# Ravenloft: Strahd's Possession — World State, Levels, and Simulation Architecture

Status: **PHASE I / BASELINE 04 — RECONSTRUCTED FROM CERTIFIED CODE/DAT/RESOURCE EVIDENCE**  
Scope: level containers, cell topology, dynamic cell controls, world-record families, NPC/object definitions, event scripts, automap state, SAVE persistence, level-local versus global state, and generated renderer state.

## 1. Purpose

This unit reconstructs the boundary between Ravenloft's serialized world data and its live simulation state. The shipped engine does not keep the complete game world in one uniform structure. Instead it combines:

- compact baseline level records in `LEVELS.DAT`;
- optional expanded mutable standalone `LEVnnn.DAT` files;
- static type databases in `NPCS.DAT` and `OBJECTS.DAT`;
- global/cross-level state in `SAVE.DAT`;
- runtime-expanded 22-byte world records;
- generated renderer and automap caches derived from the preceding state.

The architecture is therefore a layered state system. A faithful port or editor must not flatten static definitions, per-level instances, global persistence, and generated presentation state into one false record model.

## 2. Principal data layers

Class: **RECONSTRUCTED from confirmed loaders/serializers**

```text
NPCS.DAT / OBJECTS.DAT
    static type definitions
             |
             v
LEVELS.DAT archive record
    compact baseline map + instances + events
             |
             | load / expand
             v
live level state
    cell words
    10-byte control records
    22-byte A/B/C world records
    event table
    mutable header/resource tables
             |
       +-----+---------------------------+
       |                                 |
       v                                 v
LEVnnn.DAT                         renderer / automap caches
expanded mutable level save       generated from live level state
       |
       +------------------+
                          |
SAVE.DAT                  |  cross-level/global persistence
party/global/runtime      |
state                     |
       |                  |
       +--------> next runtime session <+
```

The exact ownership split matters historically:

- `LEVELS.DAT` is an indexed archive of compact level baselines.
- `LEVnnn.DAT` is the standalone mutable representation preferred by the loader when present.
- `SAVE.DAT` does not replace the per-level file; it persists broader game state and runtime collections.

## 3. LEVELS.DAT archive and standalone level selection

Claim: the level loader prefers a standalone level image and falls back to an indexed `LEVELS.DAT` archive record.

Class: **CONFIRMED**

CODE.2 level loader: logical `181B4..18D8B`.

Archive structure:

```text
+0x0000  uint32 record_count = 32
+0x0004  uint32 record_offset[32]
+0x0084  first level record
```

For archived level `i`, CODE.2 seeks to the indexed record and then skips a common 29-byte (`0x1D`) preamble before loading the main payload.

Each record preamble stores seven boundary offsets:

```text
+0x00  uint8  marker/raw
+0x01  uint32 boundary_0 = 0x1D
+0x05  uint32 boundary_1
+0x09  uint32 boundary_2
+0x0D  uint32 boundary_3
+0x11  uint32 boundary_4
+0x15  uint32 boundary_5
+0x19  uint32 boundary_6
```

Those boundaries close exactly against the decoded sections for all 32 supplied records.

The marker byte is not consumed by the loader. Archived records store zero; supplied `LEV022.DAT` stores `0xDF`. Its producer-side meaning remains unknown and must be preserved rather than normalized.

## 4. Fixed level header and cell topology

### 4.1 Fixed header

Class: **CONFIRMED / substantially decoded**

At `boundary_0`, CODE.2 reads exactly `0x2A4` bytes to DGROUP `0x25618`.

Important fields:

```text
+0x001  int16 map_width
+0x003  int16 map_height

+0x00A  uint16 primary_resource_A[64]
+0x08A  uint16 alternate_resource_A[64]
+0x10A  uint16 primary_resource_B[64]
+0x18A  uint16 alternate_resource_B[64]
+0x20A  uint16 automap_style_control[64]

+0x28A  int16 family_A_count
+0x28C  int16 family_C_count
+0x28E  int16 family_B_count

+0x290  uint8 environment_profile_id_or_FF
+0x292  int16 special_npc_type[3]
+0x298  uint8 renderer_fill_base_A
+0x299  uint8 renderer_fill_base_B
+0x29A  int16 render_threshold_floor
+0x29C  uint8 level_music_sequence_id
```

The first four 64-word banks are first-person render/resource lookup tables indexed by the six-bit cell code. They are live mutable state, not merely immutable file metadata. The fifth table participates in automap presentation.

`+0x29A` is loader-normalized from zero to one; this explains the sole fixed-header byte difference between archived level 22 and the supplied standalone level-22 header.

`+0x29C` is the default level XMIDI sequence identifier and is distinct from temporary event-selected music.

### 4.2 Row-major 16-bit cell layer

Immediately after the header:

```text
uint16 cell_words[map_height][map_width]
```

Runtime base: DGROUP `0x198E0`.

This is the principal packed map/topology/state layer. The current behaviorally reconstructed bit model is:

```text
bits 0..5    0x003F  six-bit cell code
bit 6        0x0040  transient current visibility/raycast mark
bit 7        0x0080  special-geometry center / dynamic-collision gate
bit 8        0x0100  special-geometry frame / adjacent-code substitution
bit 9        0x0200  extra render-prepass enable
bit 10       0x0400  persistent automap explored/revealed mark
bit 11       0x0800  secret/hidden-passage topology marker
bit 12       0x1000  10-byte-record state resource-offset enable
bit 13       0x2000  preserved authoring/legacy marker; no direct runtime consumer proved
bits 14..15  0xC000  two-bit render shade/decrement level
```

Class: **CONFIRMED for encoded behavior; source-level names remain project labels.**

The low six bits can be replaced independently without disturbing the high state bits. The renderer and automap systems consume the high bits through separate control paths.

## 5. Visibility, exploration, and render state are not one concept

Ravenloft keeps several visually related states in the same cell word, but their lifetimes differ.

### `0x0040` — transient current-view mark

First-person traversal sets this while processing visible/raycast-reached cells. A map-wide renderer routine clears this bit and no other bit. Baseline archived maps have none set; the supplied standalone level 22 contains hundreds, demonstrating that it is mutable runtime-derived state.

### `0x0400` — persistent explored automap mark

The same traversal can set this bit, but it is not cleared by the view reset. Automap cache generation uses it as the primary explored/unexplored distinction.

### `0xC000` — render decrement selector

The renderer copies the two-bit value into generated render records and subtracts the selected amount from packed render values with per-byte zero clamping. It is therefore presentation-affecting cell state, but its broader gameplay origin remains deliberately unnamed.

This separation is important for a port: line-of-sight visibility, explored-map persistence, and render shading cannot be collapsed into one Boolean `visible` property.

## 6. Dynamic/special cell-control records

After the cell layer, the archive stores exactly `map_width` variable column lists:

```text
int16 count
record10 records[count]
```

Common record fields:

```text
+0x00 uint8 kind
+0x01 uint8 control/state flags
+0x04 uint8 row
+0x05 uint8 column
```

The loader partitions the archive records into two runtime groups:

```text
group 1 at DGROUP 0xCB3C : archive kind != 1
group 2 at DGROUP 0xCD3C : archive kind == 1
```

Standalone `LEVnnn.DAT` saves these two runtime groups separately, with an active-animation byte between them.

### 6.1 Kind 1 — dynamic barrier/door-like geometry

Class: **CONFIRMED for behavior**

```text
record +6  dynamic position
record +7  signed animation delta
```

On the traced collision path:

```text
position 0   open/passable
position 8   closed/blocking
position 1..7 transitional and blocking
```

Cell bit `0x0080` routes collision and automap behavior through this dynamic-state lookup when an associated group-2 record exists.

The class is broader than the source name "door" can safely express because `0x0080` cells can also exist without kind-1 records and with other special geometry.

### 6.2 Other group-1 controls

Proved examples include:

- kind 2: persistent two-state interaction; signed `+2` dispatches a level event-script slot;
- kind 4: conditional interaction/event path; signed `+2` dispatches on successful action and `+6` is a kind-specific accepted ID;
- kind 10: byte `+9` is passed as a resource/mode selector;
- low two bits of byte `+1` can select `0..3` resource/state variants when the owning cell has bit `0x1000`.

The engine therefore uses compact cell-attached control records as a second behavioral layer over the packed cell topology.

## 7. Compact world-record families and 22-byte live instances

The archived level then stores three compact families. Each archived record is 11 bytes:

```text
+0x00  uint16 type_id
+0x02  int16  x_cell
+0x04  uint16 x_fraction
+0x06  int16  y_cell
+0x08  uint16 y_fraction
+0x0A  uint8  compact_flags
```

CODE.2 expands these into 22-byte live records. Common reconstructed fields include:

```text
runtime +0x00 = (y_cell << 16) | y_fraction
runtime +0x04 = (x_cell << 16) | x_fraction
runtime +0x0C = y_cell * map_width + x_cell
runtime +0x0E = type_id
runtime +0x10 = live state/countdown field initialized by family logic
```

Class: **CONFIRMED**

Across all supplied levels there are 6,204 compact records, all with in-bounds stored cell coordinates.

The archive stores compact static placement. The runtime form adds mutable state, renderer linkage, movement/countdown fields, and behavior-specific flags.

## 8. Family A — NPC/creature instances

Claim: LEVELS family A contains NPC/creature world instances whose type IDs directly index `NPCS.DAT`.

Class: **CONFIRMED**

`NPCS.DAT` contains 83 fixed 0x45-byte definitions. Family A expansion multiplies `type_id` by `0x45` against the loaded NPC table. All supplied family-A IDs are `0..82`.

This creates a two-layer NPC model:

```text
NPCS.DAT definition
    name / scene base / collision extent
    animation parameters
    combat stats
    behavior/effect flags
    sound IDs
              |
              v
LEVELS family-A instance
    type_id
    world fixed-point position
    compact flags
              |
              | expand / initialize
              v
22-byte live NPC world record
    mutable action/runtime flags
    position/cell state
    animation/combat state
              |
              v
scene-record generation / combat / AI / audio
```

### 8.1 Static NPC definition highlights

`NPCS.DAT` record fields include:

```text
name[20]
local_shape_base
collision_half_extent
scene_frame_stride
animation_cycle_divisor
animation_span
scene_influence_step
damage_reaction_animation_flag
hit_dice_count
melee damage dice/sides/bonus
armor_class
thac0
experience_award
action_delay
render_scale_lookup_bias
directional_offset[4]
behavior_flags
initial_runtime_flags
secondary behavior bytes
four context-specific sound IDs
```

Static definition flags encode immunities, attack effects, undead classification, movement/obstacle behavior, and other behavior gates. Runtime initialization copies a dedicated definition dword into the live record's mutable flags rather than mutating the definition table itself.

Static type data and live actor state must therefore remain separate architectural layers.

## 9. Family B — placed object/item map instances

Claim: family B contains placed object/item records tied to `OBJECTS.DAT` definitions.

Class: **CONFIRMED / RECONSTRUCTED**

All 4,324 supplied family-B records use IDs `0..174`, inside the 336-entry OBJECTS table. Automap overlay code scans this family and emits object markers based on the referenced OBJECTS type.

Family B represents object/item presence anchored in the level, distinct from the loose/pickable item family below.

It is therefore unsafe to model all OBJECTS references as one uniform "item instance" collection: placement family changes behavior, serialization, renderer namespace, and automap handling.

## 10. Family C — loose/pickable/dropped/thrown items

Claim: family C is the mutable loose-item world family and is also reused for active dropped/thrown item motion.

Class: **CONFIRMED**

Runtime family-C array: DGROUP `0x17618`.

Executable behavior proves:

- insertion of a loose OBJECTS item at a world position;
- pickup/removal with array compaction;
- distinct first-person scene emission using OBJECTS definitions;
- moving-item position advancement and collision;
- periodic scanning/compaction of active moving entries;
- a runtime ceiling of 300 records, with oldest-record dropping when full.

Recognizable throwable definitions handled by the movement path include Axe of Hurling, Arrow of Slay Undead, Dagger of Throwing, and Javelin of Lightning.

Thus projectile-like item behavior is **runtime state on family C**, not a fourth serialized object family.

The loader also performs a family-C-only legacy type migration `0x54 -> 0x58`. In the current OBJECTS.DAT this converts the old compact value away from `STONE GIANT GAUNTLETS` and into current `ARROW OF SLAY UNDEAD` before runtime use. The semantic alias is therefore closed; only its historical database-revision provenance remains unknown.

## 11. OBJECTS.DAT — definition versus mutable state

`OBJECTS.DAT` is a 336-entry static database with 0x41-byte records.

Class: **CONFIRMED / substantially decoded**

Important definition fields:

```text
name[40]
local_shape_base
shape_stride
scene_influence_step
ui_graphic_index
item_category
weight_half_pounds
state_index
intrinsic_magic_marker
armor_class_adjustment
attack_profile
enchantment_bonus
small-target damage profile
large-target damage profile
short/medium/long range
```

The same definition table is consumed by:

- family-B placed objects;
- family-C loose/pickable/moving items;
- inventory/equipment UI;
- combat/equipment logic;
- persistent container and charged-device state selection.

### 11.1 Polymorphic `state_index`

A crucial ownership rule is that OBJECTS `+0x31` does not contain mutable state itself. It selects mutable state held elsewhere.

For category-1 containers:

```text
OBJECTS.state_index
    -> runtime container descriptor
    -> contents array + per-slot state bits
    -> SAVE.DAT persistent container record
```

For charged devices, the same physical byte selects one of 15 persistent charge counters serialized in SAVE.DAT.

A faithful model should therefore retain `state_index` as a polymorphic selector rather than falsely converting it into a universal property such as charges or container ID.

## 12. Level event VM — local scripted state mutation

Each level contains a fixed `0x514`-byte event table:

```text
50 slots * 26 bytes

slot:
    uint8 active_command_count
    command[5]

command:
    uint8 opcode
    uint8 arg1
    uint8 arg2
    uint8 arg3
    uint8 arg4
```

Interpreter: CODE.2 `002A8..006B8`.

Class: **CONFIRMED / RECONSTRUCTED**

Active opcodes `0..12` include:

- cell/control-record actions and state toggles;
- transient/moving-effect creation;
- six-bit cell-code replacement;
- queued SFX;
- music-sequence transition;
- indexed modal text;
- persistent global event-flag set/clear/toggle;
- numbered interactive event invocation;
- one small global-state assignment.

The VM is intentionally tiny and level-local. It does not replace the separate 42-opcode RES9 interaction/dialogue VM documented in baseline 03.

This distinction should remain explicit:

```text
LEVELS event table   -> cell/world/local event mutation
RES9 wordcode VM     -> interaction/dialogue/presentation scripting
```

Both can manipulate persistent game state and dispatch shared subsystems, but they have different storage, grammar, and ownership.

## 13. Persistent 256-bit event/game-state vector

LEVELS event operations manipulate a 32-byte vector beginning at DGROUP `0x22200`.

SAVE.DAT serializes exactly these 32 bytes at fixed prefix offset `0x32..0x51`.

Class: **CONFIRMED**

Therefore a level event can set global state that survives level changes and process restarts:

```text
level event opcode
       |
       v
DGROUP 0x22200 256-bit persistent vector
       |
       v
SAVE.DAT prefix +0x32
```

This is one of the cleanest cross-file examples of simulation state crossing from level-local scripting into global persistence.

## 14. Standalone LEVnnn.DAT — expanded mutable level image

Claim: standalone level files preserve an expanded live-level representation rather than duplicating the compact archive record byte-for-byte.

Class: **CONFIRMED**

The standalone writer is CODE.2 `171FC..17837`.

Differences from archive representation include:

- two 10-byte control-record groups serialized separately;
- one-byte active-animation state between those groups;
- family A/B/C records written as expanded 22-byte live records instead of compact 11-byte archive records;
- mutable cell-word bits preserved;
- generated automap/display cache serialized in the common tail.

The fixed `0x514` event table remains byte-persistent.

This architecture allows the game to load an immutable compact baseline from `LEVELS.DAT`, expand it into mutable runtime structures, then preserve the modified level locally without rewriting the master archive.

## 15. Level 22 as the proved archive-to-live example

The supplied archive record 22 and `LEV022.DAT` describe the same `30 x 15` map.

Both carry family counts:

```text
A = 1
B = 110
C = 0
```

Key correlations:

- the family-B compact records expand to all 110 supplied standalone records exactly;
- the event table is identical and all zero;
- fixed-header difference is explained by loader normalization at `+0x29A`;
- cell words differ heavily because the standalone file contains runtime visibility/exploration/state changes;
- the generated automap cache differs correspondingly.

Thus the standalone level is demonstrably **runtime-mutated state descended from the archive baseline**, not an unrelated second level format.

One automap-cache branch cannot be reproduced from the serialized standalone header/cells alone for 40 unexplored code-1 cells. This remains a provenance/production-path discrepancy. Certified CODE.2 behavior is not altered to force a match.

## 16. Automap architecture — generated cache plus overlays

Claim: the tail byte grid is generated automap/display state, not an independent topology layer.

Class: **CONFIRMED**

Runtime flow:

```text
16-bit cell words + dynamic control records
              |
              v
cache classifier 01558
              |
              v
width*height uint8 cache at DGROUP 0x1760
              |
              v
connected wall/inset/door resolver
              |
              v
CP437 export conversion
              |
      +-------+--------+
      |                |
placed-object overlay  annotation overlay
      |                |
      +-------+--------+
              v
         exported map
```

The standalone writer calls the classifier immediately before serializing the cache.

### 16.1 Exploration source

Cell bit `0x0400` is the persistent explored/revealed mark. Current-view bit `0x0040` is separately transient.

### 16.2 Dynamic geometry

Special cells consult the dynamic group-2 records so open/closed/transitional state changes automap classification as well as collision/render behavior.

### 16.3 Placed-object overlay

The export path scans family B and overlays symbols for placed items/objects.

### 16.4 Annotation list

The common tail also stores a length-prefixed annotation blob. Runtime entries contain raw coordinates, payload length, and text. The exporter maps them to letter labels and produces a corresponding text legend. Annotation symbols have final overlay precedence.

All supplied archive levels and `LEV022.DAT` have zero annotation length, but the executable format and exporter are reconstructed.

Community research on Ravenloft's CP437 map bytes is retained only where it corroborates the conversion table already proved from the binary.

## 17. SAVE.DAT — cross-level/global persistence image

Claim: SAVE.DAT serializes application-wide and party state in a strict, variable-length order and is distinct from per-level mutable files.

Class: **CONFIRMED / supplied-build reconstruction archival-complete**

Supplied fixture: 9,288 bytes (`0x2448`). Its size is not universal because one middle section contains a counted 22-byte runtime-object array.

Top-level serializer order:

```text
fixed 0x77-byte global prefix
4 * 0x248 active-party character records
uint32 live_transient_effect_count
count * 0x16 class-3 transient/moving-effect records
uint32 spell_ui_selected_character[2]
uint32 gameplay_tick_counter
62 variable-width persistent container-state records
uint32 environment_shading_level
uint32 level_light_adjustment
uint8 charged_item_counter[15]
10 * 0x248 saved-roster character records
4 * 4-byte party container-UI contexts
```

The save serializer/deserializer order is certified from CODE.2.

## 18. SAVE fixed prefix — global world anchors

The fixed prefix contains:

```text
+0x00..+0x31  50-byte save description
+0x32..+0x51  persistent 256-bit event/game-state vector
+0x52          uint32 current level/map index
+0x56          uint32 world coordinate A
+0x5A          uint32 world coordinate B
+0x5E          uint32 facing/orientation
+0x62..+0x6B  shared primary-effect state/counters
+0x6C          interrupt-driven engine tick snapshot
+0x70          uint32 24-hour 16.16 world time-of-day clock
+0x74          int16 cursor-held OBJECTS.DAT object ID (-1 empty)
+0x76          uint8 cursor-held item known/identified state
```

This demonstrates that the save file persists the player's current world position/orientation independently from the current level's cell/object structures.

## 19. Three distinct saved timing domains

A fidelity-critical result is that Ravenloft persists three distinct timing domains rather than one undifferentiated game clock.

### Interrupt-driven engine tick snapshot

SAVE prefix `+0x6C` captures DGROUP `0xF58C`, which is incremented by an interrupt handler and restored under `cli/sti`.

### Pausable gameplay tick counter

Later in the variable middle section, DGROUP `0xA7E4` is serialized. It is used for:

- character effect expiration;
- hand/action deadlines;
- temporary spell-hand deadlines;
- recovery/protection deadlines;
- periodic healing/maintenance;
- rest-time advancement.

### 24-hour world time-of-day

SAVE prefix `+0x70` stores DGROUP `0xCF4C` as a 16.16 fixed-point hour value. The upper word is normalized modulo 24 while the lower word preserves fractional-hour state used by fine-grained time advancement and environment/day-night logic.

Class: **CONFIRMED**

A port must not collapse interrupt ticks, pausable gameplay ticks, and world time-of-day into one `game_time` value.

## 20. Character-state ownership

SAVE contains two arrays of the same 0x248-byte character image used by the inter-module `CHARS` handoff:

```text
active party  4 * 0x248
saved roster 10 * 0x248
```

The active-party image therefore participates in three separate architectural boundaries:

```text
live CODE.2 party memory
       |
       +--> SAVE.DAT active-party persistence
       |
       +<-> CHARS process-boundary transfer with CODE.1
```

This common record geometry should remain canonical even if a native port later provides typed fields.

Substantial character regions now include identity, status, portrait/sex/race/class, HP, progression, ability pairs, 21 inventory item slots, two spell matrices, and a large effect/combat/timer region. The final 82 bytes remain serialized reserved/unreferenced state rather than being discarded.

## 21. Live transient/moving-effect save section

Immediately after the active party:

```text
uint32 count
LiveTransientEffect22 live_effects[count]
```

Runtime source: DGROUP `0x248EC`; count DGROUP `0xC038`; record stride `0x16`; executable maximum `0x96` (150).

Class: **CONFIRMED structurally and behaviorally**

This array is distinct from the three per-level A/B/C expanded arrays. It is application-global class-3 transient/moving-effect state serialized in SAVE order. LEVELS opcode 3 plus spell/item paths create records in the same family consumed by the class-3 RES3 scene emitter.

Certified principal record fields are:

```text
+0x00  int32 coordinate B, fixed-point
+0x04  int32 coordinate A, fixed-point
+0x08  int32 heading/orientation
+0x0C  uint16 cached map-cell index
+0x0E  uint16 transient-effect type
+0x10  uint16 auxiliary
+0x12  uint16 type-specific auxiliary
+0x14  uint16 packed movement/direction/frame/source state
```

Late SAVE offsets shift by `count * 0x16`; tools must parse sequentially rather than relying on the quick-start fixture's fixed offsets.

## 22. Persistent container state

OBJECTS category-1 definitions select mutable container state using `OBJECTS.DAT +0x31`.

The executable has a descriptor table defining slot count, UI geometry, capacity, subtype, contents pointer, and per-slot state bits. SAVE serializes only the mutable contents and state byte for indexes `0..61`.

Across those descriptors:

```text
438 object-ID slots * 2 bytes = 876
62 per-container state bytes   =  62
                               ----
                                938 bytes = 0x3AA
```

This is a direct example of static definition/runtime state separation:

```text
OBJECTS definition
    -> state_index
    -> runtime container descriptor
    -> mutable contents/state
    -> SAVE.DAT
```

A shipped mismatch must be preserved: OBJECTS ID 329 (`QUIVER`) selects state 62, descriptor 62 is zero-filled, while state 61 is quiver-shaped and is the last serialized state. No executable `62 -> 61` repair was found. This is consistent with an original off-by-one/static-data bug, but compatibility should not silently fix it.

## 23. Renderer/environment state persisted in SAVE

After container state, SAVE persists two dwords:

```text
environment_shading_level   // runtime 0xF920, clamped 0..10
level_light_adjustment      // runtime 0xF91C, clamped 0..8
```

These feed the palette/shading-table generation path reconstructed in the renderer/resource architecture.

For levels with an active environment profile, the shading value can be refreshed from a 24-entry time-of-day table. A party-wide lighting override derived from active primary effects is combined with the second value before shading-table generation.

Thus world simulation time/effects feed presentation through persistent renderer state rather than through a purely stateless draw layer.

## 24. Charged devices and other static-definition-selected state

The 15-byte charged-item counter vector follows renderer state in SAVE.

OBJECTS definitions for the relevant wand/rod/device family select these counters through the same physical `state_index` byte used by containers in another category.

The engine therefore repeatedly uses this architectural pattern:

```text
static definition field
        |
        v
selector into mutable global state bank
        |
        v
SAVE persistence
```

This is more accurate than embedding mutable charges or contents inside copied object-definition records.

## 25. Persistent versus generated versus transient state

Class: **RECONSTRUCTED summary**

### Baseline/static

- `NPCS.DAT` definitions
- `OBJECTS.DAT` definitions
- compact archived `LEVELS.DAT` records
- level event bytecode
- baseline header lookup tables

### Per-level mutable/persistable

- cell-word high state bits
- dynamic 10-byte cell-control state
- expanded 22-byte world families
- mutable header/resource tables
- active animation state
- generated automap cache as serialized snapshot
- annotation list
- standalone `LEVnnn.DAT`

### Cross-level/global persistable

- current level, player position, facing
- 256 global event flags
- party and saved roster
- live global transient/moving-effect array
- effect/timing state
- containers
- charged devices
- environment lighting/shading controls
- spell UI selection and party container UI contexts
- `SAVE.DAT`

### Generated/transient presentation state

- current-view cell mark `0x0040`
- projected scene records
- RES3 absolute shape IDs after source-bank translation
- type-6 selected texture-column pointers
- SINTAB scale-rasterizer selection/temporary code patches
- connected automap glyph resolution and overlay buffers

This four-way distinction is central to faithful architecture. Many values share source data or are serialized opportunistically, but their ownership/lifetime is different.

## 26. Level event / resource / save integration

A typical persistent scripted mutation can cross several subsystems:

```text
LEVELS cell/control interaction
          |
          v
26-byte level event slot
          |
          +--> mutate cell code/state
          +--> create transient effect
          +--> queue SFX / transition music
          +--> display indexed T text
          +--> invoke numbered interaction
          `--> set persistent global flag
                         |
                         v
                  DGROUP 0x22200
                         |
                         v
                  SAVE.DAT +0x32
```

This makes the event table part of the simulation/persistence architecture, not merely a presentation script format.

## 27. Relationship to rendering

World state is converted to renderer state through explicit translation stages:

```text
cell words + header resource tables
        |
        +--> first-person surface selection / RES4 type-6
        |
        +--> special-record state resource offsets

family A/B/C instances + definitions
        |
        +--> source-class scene records
        |
        +--> INIT source-bank translation
        |
        +--> absolute RES3 type-7 compiled shapes

LEVELS environment profile
        |
        +--> INIT environment table
        +--> RES8 backdrop placements
```

Renderer-visible resource IDs are therefore often **derived runtime values**, not values directly serialized in the level file.

Baseline 03 documents the resource and micro-rasterizer side of this boundary; this unit establishes the world-state producers.

## 28. Safe native-port model

A clean host implementation may use typed structures, but its compatibility model should preserve these distinct legacy layers:

```cpp
struct StaticDatabases {
    ObjectDefinition objects[336];
    NpcDefinition npc_types[83];
};

struct LevelBaseline {
    LevelHeader header;
    CellWord cells[];
    CompactWorldRecords compact;
    EventTable events;
};

struct LiveLevelState {
    CellWord cells[];
    CellControlGroups controls;
    NpcInstances family_a;
    PlacedObjectInstances family_b;
    LooseItemInstances family_c;
    EventTable events;
    AutomapCache cache;
};

struct GlobalGameState {
    PartyState party;
    PersistentEventFlags flags;
    WorldPosition player;
    GameplayClock gameplay_ticks;
    RuntimeWorldObjects dynamic_objects;
    ContainerState containers;
    DeviceCharges charges;
    RendererEnvironmentState environment;
};
```

These type names are modern explanatory abstractions, not claimed original DreamForge declarations.

Compatibility requirements:

1. preserve legacy numeric IDs and bank-local namespaces;
2. reproduce archive compact-to-live expansion and family-specific initialization;
3. preserve standalone-level expanded serialization where needed;
4. maintain two saved time domains;
5. keep static definitions separate from mutable selector-addressed state;
6. preserve raw/unknown bits and shipped anomalies;
7. treat automap cache as generated state but preserve serialized cache bytes on legacy round trips;
8. do not "repair" the quiver state-61/62 mismatch or level-22 cache discrepancy by default.

## 29. Material unknowns retained

The following remain open without blocking the reconstructed architecture:

- original editor/source name for cell-word bit `0x2000`; direct CODE.2 audit finds no runtime consumer, so it remains preserved legacy/authoring state;
- some 10-byte record kind-specific historical/source-level names;
- exact historical object-table revision behind the semantically resolved family-C `0x54 -> 0x58` Arrow-of-Slay-Undead alias;
- provenance/production reason for the 40-cell `LEV022.DAT` automap-cache discrepancy;
- historical source names for family A/B/C, the two 10-byte control groups, and selected bounded auxiliary fields.

Unknowns are preserved as bounded fields/behaviors instead of being filled by analogy to other DreamForge titles.

## 30. Canonical evidence

Primary repository evidence for this unit:

- `DatFiles/LEVELS.DAT/FORMAT.md`
- `DatFiles/LEVELS.DAT/SEMANTICS.md`
- `DatFiles/LEVELS.DAT/CELL_WORD.md`
- `DatFiles/LEVELS.DAT/CELL_RECORDS.md`
- `DatFiles/LEVELS.DAT/DYNAMIC_CELL_RECORDS.md`
- `DatFiles/LEVELS.DAT/EVENT_SCRIPTS.md`
- `DatFiles/LEVELS.DAT/AUTOMAP.md`
- `DatFiles/LEVELS.DAT/AUTOMAP_OVERLAYS.md`
- `DatFiles/LEVELS.DAT/HEADER_TAIL.md`
- `DatFiles/OBJECTS.DAT/FORMAT.md`
- `DatFiles/NPCS.DAT/FORMAT.md`
- `DatFiles/SAVE.DAT/README.md`
- `DatFiles/SAVE.DAT/GLOBAL_STATE.md`
- `DatFiles/SAVE.DAT/CHARACTER_RECORD.md`
- `Resources/MISC/INIT/FORMAT.md`
- `Resources/RES/TYPE7_SOURCE_BANKS.md`
- certified CODE.2 reconstruction/audit records.

## 31. Next evidence unit

`TechnicalArchive/05_PARTY_CHARACTERS_ITEMS_COMBAT_AND_EFFECTS.md`

The next Phase-I unit should integrate the reconstructed 0x248 character record, OBJECTS/NPCS combat fields, inventory/equipment ownership, spell/effect timers, damage/death/revival status, NPC effect immunities, item mutable-state banks, and the combat/effect flows already cross-linked in the SAVE/OBJECTS/NPCS work. It should explicitly distinguish AD&D-facing gameplay semantics from the underlying integer/state implementation and retain unresolved status/behavior bits as bounded unknowns.
