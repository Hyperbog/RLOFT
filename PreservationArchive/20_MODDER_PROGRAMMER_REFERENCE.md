# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 20 — Modder and Programmer Reference: Stable IDs, Structures, and Extension Rules

**Status:** ORIGINAL FILE CONTRACTS AND FLAG/ID CROSS-REFERENCE DOCUMENTED — native implementation regressions pending  
**Working branch:** `code1-decompilation`  
**Controlling evidence:** original CODE.1/CODE.2, fixed DAT fixtures, certified Resources corpus and the 19 preceding subsystem chapters.  
**Applicability:** the **supplied** executable and accompanying resource/data set, not every historical CD, diskette, non-English or non-DOS edition.

---

## 1. First rule: preserve a byte image before interpreting it

A safe editor maintains (a) original bytes and physical boundaries, (b) a parsed semantic view, and (c) an explicit edit mask identifying which bytes may change. Do not regenerate unnamed fields, invisible record tails, unused archive entries or caches merely because a typed model does not need them.

**Three operating modes must stay distinct:**

1. **Inspect / round-trip:** decode and re-encode without edits, require exactly identical bytes.
2. **Legacy-compatible modify:** make a documented change within the original format's capacities, then validate original-runtime consumers and cross-resource references.
3. **Native extension:** introduce modern capacity or functionality outside original constraints, using a new versioned format and conversion layer. Do not label that new format a faithful original DAT/RES/ANM.

Retain the original SHA-256 and release/media provenance of every input; do not overwrite the preservation originals. Chapter [19](19_RELEASE_MEDIA_BUILD_PROVENANCE_AND_DREAMFORGE_LINEAGE.md) supplies the per-file manifest.

## 2. Core file structure quick reference

| File | Original physical geometry | Key preservation constraint |
|---|---|---|
| `INFO.DAT` | 13 ASCII selector bytes + LE `uint16` path length + path bytes; supplied 17 B | Selectors 0–10 route RES0..10; 11/12 preserved but no active consumer proven |
| `OBJECTS.DAT` | `uint32 count=336`, then `336 × 0x41` byte definitions | Item IDs are record indices; retain raw name tail, flags and polymorphic state index |
| `NPCS.DAT` | `uint32 count=83`, then `83 × 0x45` byte templates | NPC type IDs are record indices; retain all `+0x39..+0x3C` residual bytes |
| `LEVELS.DAT` | `uint32 count=32`, 32 absolute LE offsets, variable records from `0x84` | Geometry, state, compact families, script table and cache belong to each level |
| `LEVnnn.DAT` | Direct mutable level form (e.g. `LEV022.DAT` = 6028 B) | **Not** an uncompressed byte-identical LEVELS member; preserve serialized cache/residue |
| `SAVE.DAT` | `0x77`-byte global prefix, 4 characters, counted `0x16`-byte transient records, other state and 10-character roster | File size depends on transient count; no universal offsets past that counted array |
| `CHARS` | **`4 × 0x248 = 0x920` bytes** | Transient CODE.1→CODE.2 party transfer, **not** a normal persistent save |
| `RES0..RES12` | `int32 count`, `count × int32` absolute offsets, entry payloads | Nondecreasing offsets; duplicates can be **valid zero-length entries** |
| `CINE00..12.ANM` | `uint16 frame_count`, `uint16 default_wait`, per-frame chunk lists | Packet dispatch semantics and original order are timing-sensitive |
| `INIT` | 18 fixed LE words (`0x24` B), counted 7-byte shape records, two environment profiles | Indexed RES sub-bases are **source namespaces**, not file-relative pointers |
| `T` | `uint32 count=1073`, 1073 LE absolute offsets, XOR-`0xDC` text records | Preserve unreferenced/slack bytes and zero-length records |
| `SINTAB` | Trig/perspective prefix plus 127 scale blocks and 512-byte trailer | Scale blocks contain executable x86 rasterizer microcode, **not** inert pixel data |

Sources: [INFO](../DatFiles/INFO.DAT/FORMAT.md), [OBJECTS](../DatFiles/OBJECTS.DAT/README.md), [NPCS](../DatFiles/NPCS.DAT/README.md), [LEVELS](../DatFiles/LEVELS.DAT/README.md), [SAVE](../DatFiles/SAVE.DAT/README.md), [RES](../Resources/RES/FORMAT.md), [ANM](../Resources/ANM/FORMAT.md), [INIT](../Resources/MISC/INIT/FORMAT.md), [T](../Resources/MISC/T/FORMAT.md), [SINTAB](../Resources/MISC/SINTAB/FORMAT.md).

## 3. IDs are meaningful only inside an explicit namespace

Never store an untagged integer under the generic name `resource_id`. Minimum supported typed coordinates are:

```text
ObjectId             -> OBJECTS.DAT record index, 0..335
NpcTypeId            -> NPCS.DAT record index, 0..82
LevelId              -> LEVELS.DAT record index, 0..31
ResEntry             -> (archive_index, entry_index)
InitBankLocalShape   -> (source_class, local_shape_index)
TTextIndex           -> T absolute logical text-record index
AnmEventNumber       -> numbered CINE file selector, conditional in CODE.2
LevelEventSlot       -> (level_id, slot 0..49)
Res9WordcodeIP       -> word index inside one interaction-program word array
PersistentFlag       -> SAVE/world-level event-flag namespace
WordcodeLocalFlag    -> interpreter-local flag namespace
```

An OBJECTS ID is **not** a RES1 UI image ID, even when the item has an image. A creature's NPCS class-0 local shape base is **not** its family-A live-instance ID. A RES9 wordcode instruction position is **not** a byte offset in `T`, and a LEVELS opcode-11 argument may denote a reserved story trigger rather than an ordinary wordcode index.

Use stable typed wrappers in native source and keep raw integers available for lossless serialization.

## 4. Essential INIT-derived resource bases

The supplied `INIT` file defines these arithmetic translations:

| Source field | Original value | Consumer meaning |
|---|---:|---|
| `INIT +0x00` | 15 | RES0 raster/image base after first 15 fonts |
| `INIT +0x02` | 168 | RES1 item-UI image base: `168 + OBJECTS[object_id].ui_graphic_index` |
| `INIT +0x04` | 336 | RES1 normal portrait base; possessed portraits start 46 later |
| `INIT +0x06/+0x08/+0x0A/+0x0C` | 0, 2169, 3316, 4158 | Four RES3 compiled-shape source-class bases |
| `INIT +0x0E` | 600 | RES4 secondary texture-bank split |
| `INIT +0x10` | 49 | Alternate RES5 sequence-bank displacement |
| `INIT +0x1C` | 32 | RES9 interaction graphics base |
| `INIT +0x1E` | 103 | RES9 interaction-resource base |
| `INIT +0x20` | 48 | RES10 interaction-voice base |
| `INIT +0x22` | 414 | RES9 active wordcode-bank displacement |

For RES3, bank partitions are precisely `[0,2169)`, `[2169,3316)`, `[3316,4158)`, `[4158,4234)`, totaling 4,234 compiled-shape entries. Do **not** embed these values as engine-wide universal constants independent of `INIT`; they are **the values in this supplied INIT**.

The physically serialized bank-local definitions and CODE.2's addition of the appropriate source-class base are jointly necessary for stable editors. [INIT executable field mapping](../Resources/MISC/INIT/FORMAT.md).

## 5. World-state, script and save constraints

`LEVELS.DAT` carries one 0x514-byte **level-local** event table per level: **50 slots × 26 bytes**, each with a command count `0..5` and five physical 5-byte command positions. Interpreter opcodes are `0..12` and *inactive command bytes are preserved*. The event selector `-1` is a no-op sentinel, not a valid array index. [Event table](../DatFiles/LEVELS.DAT/EVENT_SCRIPTS.md).

The **separate** RES9 interaction VM has **42 opcodes `0x00..0x29`**, 16-bit-word IPs and conditional/jump/choice targets. The current active bank comprises `RES9:517..554` (38 programs). Do not parse RES9 using the LEVELS five-byte instruction grammar. Opcode `0x05` takes one active 16-bit template-ID operand; the legacy RES12 generation instead uses a 141-word inline-template form. [VM grammar](../Resources/RES/WORDCODE_VM.md).

`SAVE.DAT` contains a **fixed `0x77` prefix** plus four `0x248` character images, followed by `uint32 live_effect_count`, then `count × 0x16` byte live-effect records; all later offsets move by `0x16 × count`. In the supplied zero-effect save, the gameplay-tick dword is at `0x09A3`; that is **not an absolute offset for every save**. Save/load must retain distinct IRQ tick, gameplay tick and 16.16 world-time clocks. [SAVE physical layout](../DatFiles/SAVE.DAT/README.md).

`CHARS` carries only `0x920` bytes of active party records. Original `R.EXE` translates child statuses **10** (continue CODE cycle) and **12** (restart without CHARS), then removes CHARS at the prescribed transitions. Do not serialize a full SAVE file as CHARS or suppress the restart distinction. [Supervisor contract](../Runtime/STATE_TRANSITION_PROTOCOL.md).

## 6. Raster, text and cinematic compatibility boundaries

RES typed entries normally start with `EH`, a one-byte type and **signed-interpreted** 16-bit type-header size. Type-1 raster `count` and `height` are sign-extended by CODE.2. Type 7 is compiled display-list data driven by SINTAB executable code; do not convert it to ordinary sprites without preserving the original coordinate and scale semantics. Resource type 5 and raster compression method 1 are executable-supported but absent from the supplied resource corpus.

`T` has **1,073** indexed text records, bytewise XOR-`0xDC` coding, and unreferenced gaps. In the original engine, subtitle packet 7 looks up `T[first_word + 0x324]`; never flatten that index into the ANM packet's own byte offset. Preserve original control bytes and encoding rather than treating the result as universally UTF-8.

ANM's chunk IDs `0..9` specify ordered palette, blackout, full-image, delta-image, dormant repeat/seek, music, SFX, subtitle, planar image and other sound actions. Chunk 4 is executable-supported but absent from the 13 original ANMs. The second stream header word is the **default tick wait**, not a version number. Preserve the original event-selection predicate and abort/finish results. [Cinematic chapter](16_ANM_CINEMATIC_ENGINE_AND_TIMELINE.md).

## 7. Compatibility hazards that an editor must expose

- **QUIVER:** OBJECTS ID 329 has container selector 62 even though the serialized state descriptors are indexed only 0..61; no automatic repair `62 → 61` is proved.
- **NPC names:** 20 of 83 names carry meaningful bytes after the first NUL; do not `memset` the entire fixed field when editing a displayed name.
- **RES1:** zero-length entries and raw four-byte `0x00000060` portrait sentinels are preservation evidence, not failed images to replace.
- **RES9 scripts:** a second word array of length `word_count` exists but is not consumed by the supplied CODE.2; preserve its exact bytes.
- **Level cell word:** `0x2000` has no proved direct cell-specific CODE.2 consumer; it is not license to erase the field.
- **Level-22 cache:** the supplied direct LEV cache differs in 40 cells from the certified recomputation. Import should preserve the original saved cache unless regeneration was requested.
- **Timing:** CINE and normal gameplay waits consume tick-driven deadlines, not an invariant display refresh rate; no measured real-time FPS has been certified.
- **Historical banks:** RES11/RES12 are older composite generations and not loaded by this executable pair's indexed RES setup.

[Quirk and UNKNOWN ledger](18_ORIGINAL_QUIRKS_LATENT_FEATURES_AND_LEGACY_DATA.md).

## 8. Suggested implementation modules and test gates

Suggested *native-program architecture*, **not** reconstructed original DreamForge source class names:

```text
OriginalByteImage
  -> LosslessReader / LosslessWriter
  -> TypedOriginalID namespaces
  -> Source-backed semantic models
      WorldLevel | ItemDefinitions | NpcDefinitions | PartySave
      ResourceArchive | ScriptVM | CinematicTimeline | RenderingTables
  -> OriginalBehaviorCompatibility
  -> NativeRenderer / NativeAudio / NativeInput
```

Required gates for a preservation-grade editor: file size, SHA-256 and object count; offset/table monotonicity; signed field promotion; exact byte-preserving decode/encode; script instruction and branch-target boundaries; referential integrity across RES/INIT/DAT; raw unknown-bit retention; fixture mutation tests; and a separate gameplay comparison test for any changed code path.

**Native extensions must be opt-in and versioned.** Unchanged originals should still round-trip identically, and importing an extended save or script into the original DOS executable must either be proved compatible or explicitly rejected. Do not silently truncate new capacities to fit the original fixed arrays.

## 9. Validation state and next reference work

All physical dimensions, source namespace bases and compiler-mapping claims above trace to the certified primary documents. The prior 33-file resource workstream independently certified its canonical corpus; this pass has **not** dynamically replayed all resource consumers in a DOS environment. The local original-file integrity check for nine provided files passed **15/15 tests** in the Chapter-19 closing audit.

**Next:** build a more exhaustive cross-reference for IDs, sentinels, flag domains, caller/producer ownership and per-file serializer constraints; then prepare the final project-wide contradiction and bounded-UNKNOWN audit. Preserve the difference between *known encoded behavior* and *unknown historical authoring names*.


## 10. Edit-impact matrix and original runtime limits

The following table identifies **compatibility consequences**, not permissions to expand original array capacities.

| Proposed change | Field or format | Required checks |
|---|---|---|
| Rename an item | OBJECTS fixed `name[40]` | Preserve unchanged tail bytes and ObjectId; do not disturb RES1 UI or RES3 shape IDs |
| Change an item's appearance | OBJECTS `+0x28/+0x2A/+0x2D` | Verify RES3 class-2 bank window and `RES1[168+local_ui_index]` |
| Modify NPC stats or shape | NPCS record `0x45` bytes | Check family-A type-index references, HP/action initialization, shape bases and unknown bits |
| Modify a door or wall | LEVELS cell words and 10-byte cell records | Check coordinates, collision flags, visual state, event triggers and automap cache |
| Modify level scripts | LEVELS 50 × 26-byte event table | Retain five physical commands, inactive bytes, and opcode range `0..12` |
| Modify interaction scripts | RES9 wordcode + T | Validate word-aligned IPs, conditional targets, text bases and flag namespaces |
| Change cinematic/audio | ANM packets and RES5/6/10 | Validate packet order, tick waits, SFX/music namespace and subtitle `T` IDs |
| Modify saved party | SAVE images or CHARS | Preserve `0x248`-byte image geometry and correct lifecycle for each file |
| Add an entirely new object, NPC, level or opcode | Original fixed executable tables | **Not automatically supported**: audit every consumer and index bound before claiming compatibility |

A variable-length archive does **not** imply its runtime supports arbitrary growth. Adding ObjectId 336, NPC type 83, level 32, a sixth command in a LEVELS slot, or a 43rd VM opcode may require **a new native format or executable modification**. Original data must remain independently importable.

## 11. Reproducible edit workflow

1. Record the original full-file SHA-256, edition and physical source (installation, CD image, save slot or extraction).
2. Decode then encode **without any changes**; demand byte-for-byte equivalence before attempting an edit.
3. Modify one typed field at a time while retaining raw unknown bytes, inactive commands, reserved rows and original IDs.
4. Validate counts, absolute offsets, signed widths, opcode targets, archive-local indexes and every affected cross-file dependency.
5. Emit a byte-range diff and a referential-integrity report; reject unproved capacity expansion rather than silently truncating.
6. Validate *observable* gameplay behavior separately. A successful parser round-trip cannot prove that an original DOS executable will accept an edited resource.
7. Export a new modded artifact; preserve the unmodified source and its hash.

**Security boundary:** original `SINTAB` contains executable x86 rasterizer microcode. Modern editors must treat modified SINTAB as untrusted machine code and should inspect, disassemble, bounds-check or safely emulate it instead of directly executing it in the editing process.

## 12. Validation checkpoint and next reference layer

The original-file integrity test recorded for Chapter 19 passed **15/15** local assertions. An additional five focused structural assertions passed for OBJECTS, NPCS, LEVELS, INFO and the supplied SAVE geometry. The larger 33-file RES/ANM corpus values used here come from the prior certified original-resource audit and were **not** newly rehashed in the current workspace.

**Next:** detailed status/flag and sentinel glossary; exact producer/consumer ownership index; a few original-file worked offset examples; and the eventual native-port regression checklist. Historical source terminology still classified UNKNOWN in Chapter 18 remains UNKNOWN.


## 13. Character status-word glossary: masks are not an enum

The physical status word is an unsigned 16-bit value at **character-relative `+0x010`**, runtime `0x218F0 + slot*0x248`, saved at `SAVE + 0x77 + slot*0x248 + 0x10` for active-party slots. These identifiers come from the [certified character-status map](../DatFiles/SAVE.DAT/STATUS_WORD.md), rather than guesses from AD&D conditions.

| Mask | Proved meaning or evidence limit | Important ownership |
|---|---|---|
| `0x0001..0x0020` | Six individual low bits; gameplay identities unproved | Raw serialized state, not automatically unused padding |
| `0x0040` | Vengeful-Spirit warning/coercion and action-blocking state | Distinct from Held; possession maintenance sets/clears |
| `0x0080` | Recruited/template-character provenance | Set on template-party installation, retained through save |
| `0x0100` | Vengeful-Spirit possession | Story/event setter and resolver; adds 46 to portrait ID |
| `0x0200` | Held | Character status UI `T[654]` |
| `0x0400` | Diseased | Status UI `T[655]`, attack and cure handlers |
| `0x0800` | Poisoned | Status UI `T[656]`, attack and cure handlers |
| `0x1000` | Paralyzed | Status UI `T[657]`, attack and removal handlers |
| `0x2000` | Engine-consumed action-blocking tier; original identity **UNKNOWN** | Preserve bit; direct producer not proved |
| `0x4000` | Dead/zero-HP character | HP/death and revival lifecycle; **occupied** slot remains |
| `0x8000` | Empty/unoccupied active-party slot | Character replacement/recruitment code |

The exact player-facing condition range also includes **`T[653]` POSSESSED** for `0x0100`. Some callers test compound masks `0xF240`, `0xD040` or `0x0140` rather than any single status alias. Preserve the original expression at those callers.

**Crucial type distinction:** character status `0x2000` and LEVELS cell-word `0x2000` are **different fields in different owners**. The cell bit is a legacy/authoring marker with no proved bit-specific runtime consumer; the character bit is consumed in action blocking. A global enum named `BIT_2000` would be dangerous.

## 14. Flag namespaces and level-cell field glossary

The original SAVE fixed prefix `0x0032..0x0051` holds **32 bytes = 256 persistent flag bits**. CODE.2's flag operations at `1C124/1C160/1C1A0/1C1DC` and LEVELS event opcodes 8/9/10 or RES9 VM opcodes `09..0D` use this persistent namespace; RES9 VM opcodes `1E..22` instead use a separate **wordcode-local dword** at runtime `0x7FD0`. They are not aliases.

Concrete examples:

```text
Persistent global event flag 0x3C (decimal 60):
  SAVE byte offset = 0x32 + (60 >> 3) = 0x39
  bit within byte = 1 << (60 & 7) = 0x10
  semantic lifecycle = GHOST/vengeful-spirit possession story state

Persistent flag 0x20 (decimal 32):
  SAVE byte offset = 0x36
  bit within byte = 0x01
  setter = GHOUL LORD (NPC type 18) death path
```

These are **indexing illustrations** using the certified byte/bit vector, not evidence that either flag is set in the supplied quick-start fixture.

A LEVELS cell word is `uint16` at the level's packed row-major cell array. Certified masks:

| Mask | Meaning |
|---|---|
| `0x003F` | Six-bit cell code (not a boolean) |
| `0x0040` | Transient current-visibility/raycast mark |
| `0x0080` | Special-geometry center and dynamic-collision gate |
| `0x0100` | Special-geometry frame / neighbor substitution |
| `0x0200` | Extra render-prepass enable |
| `0x0400` | Persistent automap explored/revealed |
| `0x0800` | Hidden-passage topology marker |
| `0x1000` | 10-byte-record resource-offset state |
| `0x2000` | Preserved legacy/authoring marker; original semantics unproved |
| `0xC000` | Two-bit render shade/decrement level |

No modder should rename `0x0040` generically across these two types: character status `0x0040` is not map current visibility `0x0040`. See [LEVELS cell word](../DatFiles/LEVELS.DAT/CELL_WORD.md) and [SAVE persistent state](../DatFiles/SAVE.DAT/GLOBAL_STATE.md).

## 15. Producer/consumer ownership index

| Serialized/source identity | Producer / authoritative writer | Consumer / transformation | Safeguard |
|---|---|---|---|
| OBJECTS definition `id` | Static OBJECTS record | Inventory, equipment, combat, family-B/C world records | Keep original `id` stable; no implicit RES index |
| OBJECTS `+0x2D` | Definition/UI local index | RES1 entry `INIT+0x02 + index` | Verify actual UI bank range |
| OBJECTS `+0x28,+0x2A` | Definition scene base/stride | RES3 class-2 bank + scene variant | No offset into RES3 without class base |
| OBJECTS `+0x31` | Definition mutable selector | Category-specific container state or charged counter | Signed physical byte, polymorphic |
| NPCS type `id` | Static NPCS record | Family-A live actor generation, combat, sound | NPC type and instance index are separate |
| NPCS `+0x14` | Scene local base | RES3 class-0 compiled shapes | Retain local/absolute distinction |
| NPCS `+0x31` | Template behavior flags | Damage gates, AI/actor state | Preserve unsupported populated bits |
| Party `+0x10` status | Story, effect, HP and recruitment code | Occupancy, portraits, UI, action masks, ending | Multi-bit field, not single enum |
| SAVE `+0x32` 256-bit flags | LEVELS events, RES9 VM, story handlers | Script predicates/story transitions | Not wordcode-local state |
| RES9 wordcode IP | 16-bit program-word array | VM branches/calls/timers/choices | Branch targets at valid decoded word IPs |
| LEVELS event slot | Fixed `0x514` per-level table | 13-way event interpreter | Inactive command bytes must survive |
| `T` index | Offset-indexed XOR text archive | UI, story, dialogs, cinematic subtitle `+0x324` | Indexed logical text IDs, not UTF-8 offsets |
| ANM chunk stream | Per-cinematic packet authoring | Renderer/palette/audio/timing dispatcher | Packet order and original waits matter |
| `CHARS` image | CODE.1 party image writer | CODE.2 startup party loader | Raw `0x920`-byte handoff only |
| INFO selection/path | Original installation configuration | Indexed RES0..10 and numbered external resources | Preserve selector bytes 11/12 raw |

The original engine may contain multiple writer paths for a field; this table identifies the *proved ownership family*, not a claim of a single exclusive producer instruction. Exact source call edges are recorded in the linked subsystem chapters.

## 16. Worked offsets verified on supplied original files

The following numbers were checked against the original-file byte images. Distinguish **file-relative byte offsets** from **runtime DGROUP offsets** and **archive-local entry IDs**.

```text
OBJECTS.DAT: uint32 count=336
  object 329 record start = 4 + 329*0x41 = 0x538D
  object name[40]         = "QUIVER"
  selector field +0x31    = file 0x53BE
  selector value          = signed int8 62
  Runtime quiver-shaped state descriptor 61 is separate.

NPCS.DAT: uint32 count=83
  NPC type 18 start       = 4 + 18*0x45 = 0x04DE
  NPC name[20]           = "GHOUL LORD"
  Actor instances carry this type ID; they are not additional NPCS records.

SAVE.DAT: active party base file 0x0077, stride 0x248
  slot 0 status at file   = 0x0087, value 0x0000
  slot 1 status at file   = 0x02CF, value 0x0000
  slot 2 status at file   = 0x0517, value 0x8000
  slot 3 status at file   = 0x075F, value 0x8000
  transient count        = file 0x0997, value zero
  gameplay tick          = file 0x09A3, value 1735
  IRQ tick               = file 0x006C, value 1899
  world 16.16 time       = file 0x0070, value 0x000E0000 (14:00)

LEVELS.DAT: count=32 at file +0
  first absolute level offset = 0x84
  table of 32 uint32 offsets occupies file 0x04..0x83
INFO.DAT: 13 ASCII location digits, then uint16 path length=2
  trailing two bytes      = "D:"
```

These checks validate the stated **static fixture offsets and values**. They do not mean every SAVE contains zero transient effects or that all level, object and character tables can be expanded without updating the original executable. See [source provenance manifest](19_RELEASE_MEDIA_BUILD_PROVENANCE_AND_DREAMFORGE_LINEAGE.md).

## 17. Strictness tiers for native compatibility tests

**Tier A — lossless original data:** byte-identical parse→encode on unedited files; retention of unknown bits, name tails, RES duplicates, fixed command slots, raw cell markers and unknown wordcode companion arrays. The existing workstream audits have certified these properties for their respective supplied canonical source sets; new edited assets require their own tests.

**Tier B — deterministic source behavior:** original signed arithmetic and integer promotion; event/VM dispatch targets; index-base arithmetic; status and world gates; SAVE semantic setters; game-clock pause/nesting and scripted time advances. Verify against original executable traces where possible.

**Tier C — environment-dependent execution:** IRQ/PIT cadence, original audio-device service rates, DOS handles, external path routing and original video/audio hardware timing. These may be abstracted in a native port but require observable-gameplay equivalence claims to be separately tested.

**Tier D — extended assets:** additions exceeding original compiled capacities must use an explicit versioned native extension and import/export policy. Neither preservation tools nor a demo implementation can certify them as original-compatible without a consumer audit.

**Documentation state:** the present glossary is grounded in immutable original-file fixtures and certified subsystem chapters. It is an implementation reference, **not a completed native regression run**.

