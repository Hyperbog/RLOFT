# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 16 — ANM Cinematic Engine, Timeline, VGA Frames, and Story Integration

**Status:** FOUNDATION PASS — continuation required  
**Branch:** `code1-decompilation`  
**Frozen original-evidence baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`  
**Evidence classes:** A/B for stated executable/file geometry; C where CODE, ANM, T and runtime state independently connect; H for retained historical unknowns.

---

## 1. Boundary and call-chain overview

ANM is not merely an image format. Its on-disk stream interleaves image, palette, soundtrack, sampled effects, subtitle commands and frame timing. The native port must preserve ordered packet execution even if its host renderer and audio implementation differ.

```text
numbered presentation/event
    -> CODE.2 event entry 13EF8 / selector 13AD8
    -> numbered filename constructor 139AC
    -> CINE00.ANM ... CINE12.ANM
    -> frame reader 13834..139AA
       -> packet dispatcher 135C4..13831
          -> VGA/palette | music/SFX | T subtitles
       -> per-frame wait / alternate image timing
```

The ten-entry packet dispatch table is at CODE.2 `1359C..135C3`. The filename template `CINE00.ANM` is stored at `0x131D0`; `139AC` replaces the two decimal digits. These are reconstructed addresses, not original function names.

## 2. Immutable inventory and selection namespace

The supplied corpus has **13 files**, `CINE00.ANM` through `CINE12.ANM`. The per-file sizes, SHA-256 digests, frame totals, and packet frequencies are permanently recorded in [ANM inventory](../Resources/ANM/INVENTORY.md); this chapter does not substitute a mutable filename list for those original identities.

An event ID used in `139AC` is a *cinematic number*. It is not an RES archive index, an ANM packet type, or a T text record ID. In particular, the event-10 selector may change the cinematic number from 10 to 11 before constructing the filename.

## 3. Serialized stream contract

CODE.2 `13834..139AA` and `135C4..13831` establish this little-endian grammar:

```c
uint16_t frame_count;
uint16_t default_wait;
for each frame:
    uint16_t chunk_count;
    for each chunk:
        uint16_t chunk_id;
        int32_t  byte_count;
        uint8_t  payload[byte_count];
```

The decoder rejects `chunk_id > 9`; packet body interpretation depends on the ID. The second file-header word is **default_wait**, not a file format version. The original engine passes it to `2E614` after a frame unless the active image path selects timing from `12C10`.

Both finale files (`CINE10`, `CINE11`) store `default_wait=0`; the other eleven supplied ANMs store `1`. This difference is observable content, not permission to rename or normalize the field.

## 4. Exact packet dispatch, IDs 0 through 9

| ID | CODE.2 handler | Original operation |
|---:|---|---|
| 0 | `1362D` | Copy 768 palette bytes to staging buffer `C355`; defer palette application through `1393F -> 29234`. |
| 1 | `1364B` | Clear image to zero, zero the 768-byte palette buffer and apply the blackout palette immediately. |
| 2 | `13689 -> 27BD8` | Decode a full **320 × 400** chunky image with per-row RLE; `27A90` scatters it into four VGA planes. |
| 3 | `1369C -> 27AEC` | Apply sparse planar pixel deltas: x advances, vertical groups and runs. A two-byte zero outer count means no updates. |
| 4 | `136B6 -> 13488` | Executable-defined repeat/seek controller; first-use six-byte count/offset body. Not instantiated in supplied ANMs. |
| 5 | `136DA` | Two signed words drive music finalization/transition then `28E58(sequence_id, second_word+1, 0x9C4)`. |
| 6 | `13752` | Queue SFX via `28110`, channel 1, auxiliary `0xFF`, priority zero; ID uses base `0x258D0`, except suppressed IDs 3, 15, `0x88`. |
| 7 | `137C0` | Clear subtitle when first word is zero; otherwise read indexed T text at `first_word+0x324`. Pass second word to renderer `13FD4`. |
| 8 | `136C0 -> 27B84` | Copy four consecutive `0x5000`-byte planes to VGA at page offset `0x0FF0`. |
| 9 | `1379F -> 146B8` | Direct SFX call `27F24(id)` bracketed by `280DC`; not the type-6 queue path. |

Packet handlers are **ordered timeline effects**. In particular, packet 0 palette changes are staged, packet 1 blackout is immediate, and packets 6 and 9 do not share scheduling semantics.

## 5. Native geometry and image decoding

VGA pixel `(x,y)` maps to plane `x&3`, column `x>>2`, row stride **80 bytes per plane**. Type-2 RLE emits exactly 320 pixels in each of 400 rows; low-bit opcodes copy `op+1` literals, high-bit opcodes repeat the next byte `0x101-op` times.

Type 3 instead applies vertical runs directly to the selected plane. The corpus contains **809** type-3 packets: 790 two-byte zero-update records and 19 substantive records, all substantive instances belonging to `CINE12.ANM`.

Type 8 is **320 × 256**, drawn at framebuffer rows **51..306**, not a 640-wide movie frame: `0x0FF0/80=51` and `0x5000/80=256`. Resizing to 640 pixels is a modern export/display transformation, not original ANM geometry.

## 6. Shared subtitle and audio ownership

Type-7 text does **not** embed a subtitle string. It references the independently indexed `T` resource via `1C368`; that reader follows the active dword offset table and XOR-decodes record bytes with `0xDC`. Its `+0x324` cinematic namespace offset is part of the game contract.

Type-5 music, type-6 queued SFX, and type-9 direct SFX use different game-facing routes. A native timeline cannot reduce these to unordered audio events or infer a voice/music asset from the bare numeric ID without its caller namespace. Chapter 15 owns the detailed audio interface; this chapter owns synchronization with image and subtitle packets.

## 7. Finale choice and process lifecycle

The certified five-quest-object producer runs event 10 and subsequently requests DOS status 12. CODE.2 `13AD8` scans four `0x248`-byte party records at runtime base `0x218E0`. For each status word at record offset `+0x10`, the qualifying machine-equivalent predicate is:

```cpp
uint16_t status = party[i].status;
bool qualifies = status < 0x4000u && (status & 0x0080u) == 0;
```

If **any** party record qualifies, play `CINE10.ANM`; otherwise use `CINE11.ANM`. `0x0080` marks recruited/template provenance, `0x4000` marks death and `0x8000` marks an empty slot. The `<0x4000` comparison is unsigned in effect because CODE.2 zero-extends the original word before its comparison. After playback, supervisor status 12 means **restart without CHARS**, not “cinematic succeeded.”

The other routine at `0x23DF0` duplicates finale behavior but has no established incoming executable edge; it must not be presented as another live trigger.

## 8. Validation limits and next evidence pass

The repository's `Resources/ANM/analyze_anm.py` records byte-exact unchanged parse/serialize and structural validation for all 13 supplied files, including all 73 type-2, 809 type-3 and 2,796 type-8 chunks. **This continuation has inspected the stored audit; it has not rerun corpus validation.** A raw-preserving parse/serialize check is not a certified mutation encoder.

Next: trace `13488` repeat/seek state and the `12C10` alternate-timing branch instruction-by-instruction; map event callers and cinematic SFX/subtitle IDs; then add frame-accurate regression fixtures. Retain chunk-4 authoring intent and timing units as **UNKNOWN** until proved.

## Primary source ledger

- [Executable-backed ANM format](../Resources/ANM/FORMAT.md) and [full inventory](../Resources/ANM/INVENTORY.md)
- [ANM structural validator](../Resources/ANM/analyze_anm.py)
- [Indexed T text grammar](../Resources/MISC/T/FORMAT.md)
- [Game-facing music/SFX interface](15_AUDIO_MUSIC_SFX_AND_DRIVER_INTERFACE.md)
- [Planar VGA and palette behavior](04_VGA_RENDERING_PIPELINE.md)
- [Event-10 cinematic construction and correlation](../Runtime/EVENT10_CINEMATIC_CORRELATION.md)
- [Corrected unsigned CINE10/CINE11 predicate](../Runtime/CINE10_CINE11_VARIANT_PREDICATE.md)
- [Canonical module lifecycle/status-12 protocol](../Runtime/STATE_TRANSITION_PROTOCOL.md)
- [Immutable CODE.2 certification](../Code2/FINAL_CERTIFICATION.md)
