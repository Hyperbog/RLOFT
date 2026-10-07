# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 20 — Modder and Programmer Reference: Stable IDs, Structures, and Extension Rules

**Status:** FOUNDATION — supplied DOS 1.2a contracts indexed; executable/native regression integration pending  
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
| `SAVE.DAT` | `0x77`-byte global prefix, 4 characters, counted `0x16)-byte transient records, other state and 10-character roster | File size depends on transient count; no universal offsets past that counted array |
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
