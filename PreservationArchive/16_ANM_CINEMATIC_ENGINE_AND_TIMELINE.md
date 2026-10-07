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

## 8. Tick-based cinematic timing — newly checked from original CODE.2

The frame loop at `13834..139AA` reads the two 16-bit ANM header words, records a frame-clock anchor with `2E608`, processes each packet through `135C4`, then dispatches pending subtitle, sampled-SFX service, palette upload and wait operations **in that order**.

Before decoding each frame, `138B4` sets byte `[0x7F80]=0`. Only chunk **3** (`136A2`) and chunk **8** (`136C6`) set it to one. At `13953..13973`, this byte selects the first argument to `2E614(wait,0)`:

```text
if frame had a chunk-3 or chunk-8 image update:
    wait = dword [0x12C10]
else:
    wait = dword [0x7F90]  // ANM default_wait
```

**Important address distinction:** `[0x12C10]` is a **data reference**, not a call to the unrelated executable bytes at object offset `0x12C10`. `13B1F` writes `6` to this wait variable for the normal numbered-event path, whereas `13B2E` writes `2` for event **12**. Thus the zero header waits in `CINE10/CINE11` do not imply every frame bypasses timing; their type-8/3 update frames use the alternate wait. A type-2 full-image packet by itself does **not** select that alternate branch.

## 9. Clock source, servicing, and interruption

`2E608` stores the current `30700()` value into `[0x2B640]`. `30700` simply returns `[0xF58C]`; the interrupt path at `30708..30735` increments that same counter. `2E614` constructs an absolute deadline by adding the chosen wait count to the anchored counter and loops until the tick value reaches the deadline, calling service routine `2F868` along the way. It clears the anchor on normal return.

The player also calls `2F868` after each packet, `2A020` and queued-SFX service `28168` after subtitle presentation, and again calls `2F868` after the wait. At `1397B..1398A`, a keyboard result of `0x1B` (Escape) terminates frame iteration and makes the player return `1`; normal completion returns the initialized `0`. Preserve that early-return distinction.

The machine **proves tick-count durations** (not seconds or video frames per second). The wall-clock rate of this interrupt counter, its interrupt installation details, and any platform-specific calibration belong to the dedicated timing chapter; do not multiply `6` or `2` by an assumed frequency here.

## 10. Chunk-4 repeat and seek state

The only dispatch edge into `13488` is packet handler `136B6`. On first encounter, when `[0x7F98]==0`, the routine reads `uint16 repeat_count` into `[0x7F9C]` and a following `uint32 relative_offset`; it combines the latter with saved stream base `[0x7FAC]` into `[0x7FA0]`. It then invokes the separate buffered image/drawing decoder `133CC`, captures a return file position in `[0x7FB0]`, seeks to `[0x7FA0]` and enables `[0x7F98]=1`.

While active, successive chunk-4 dispatches decrement `[0x7F9C]`. Nonzero iterations skip six buffered bytes, run `133CC` again and seek to `[0x7FA0]`. The terminal iteration seeks to saved `[0x7FB0]`, resets the buffered cursor to `0x1F400`, and clears `[0x7F98]` and `[0x7F94]`. The original also recalculates absolute positions using its `0x1F400`-byte buffer extent. These are **binary-proven state transitions**, not a claim of a well-defined artist/editor repeat instruction: chunk 4 occurs **zero times** in the 13 supplied ANMs. Its historical intended workflow, permitted ranges and malformed-input behavior are still **UNKNOWN**.

## 11. CINE11-only music substitution

Type-5 music command handler `136DA..13751` compares its first signed 16-bit word to `0x15`. Only when the current cinematic number `[0x7F78]` equals **11** does it substitute `0x30` for that music ID (`13701..1370A`). It subsequently calls `28DCC(0x9C4)`, `28EF0`, `29008(100)`, `28F58(100)`, then `28E58(selected_music_id, second_word+1, 0x9C4)`.

This is an executable conditional, even if a raw ANM extractor presents both variants with the same embedded first word. Avoid rewriting source ANM packets to bake in this runtime substitution.

## 12. Regression requirements for faithful execution

- Test frame packets in source order, then the post-packet subtitle, sound service, palette-commit and wait ordering; include more than one packet in a frame.
- Test wait selection separately for no image update, type 2 only, type 3, type 8 and mixed packets. Also test `CINE10/CINE11` zero header waits and event-12 alternate wait value.
- Anchor waits to the interrupt-counter timeline and assert Escape returns `1`, distinct from natural return `0`.
- Keep chunk-4 internal cursor, absolute seeks, repeat count, saved return address and two reset flags independently testable. Do not claim corpus round-trip validates execution of an absent packet.
- Test the first-word `0x15 -> 0x30` music substitution in cinematic 11, and the negative cases for other IDs/cinematics.

These cases supplement the format checks, not replace them. No ANM replay was executed during this source-inspection pass.

## 13. Numbered-event dispatch is conditional, not a one-to-one filename lookup

Routine `13EF8` is the high-level entry point for a numbered cinematic request, while `13AD8` is the stateful selector and `139AC` constructs `CINEnn.ANM`. Most event numbers remain unchanged, **but at least two event numbers undergo explicit game-state selection**. Event 7 may be rejected before opening any file. The byte-proven dispatch chain is `13EF8 -> 13AD8 -> 139AC -> 13834`; the first routine also preserves/reinstates display/audio state via `13C88`.

### 13.1 Event 2 selects CINE01 through CINE05

At `13B3D..13B69`, CODE.2 scans the four party status words at `0x218F0 + i*0x248`. For each it clears the low status byte, masks high-byte bit `0x80` (**status bit `0x8000`**), and increments a counter when the result is zero. After four records it subtracts one from that count and adds it to the requested cinematic number `2`:

```cpp
unsigned occupied = 0;
for (unsigned i = 0; i < 4; ++i)
    occupied += (party[i].status & 0x8000u) == 0;
unsigned cinematic = 2 + occupied - 1; // 1..5 for 0..4 occupied slots
```

This is a **party occupancy** selector, not a selection based on living-versus-dead characters or recruited provenance. The final filenames are `CINE01`, `CINE02`, `CINE03`, `CINE04`, `CINE05` for occupancy 0, 1, 2, 3, 4 respectively. An occupancy of zero maps to CINE01 mathematically; this does not establish that ordinary gameplay can invoke event 2 with an empty party. The existing event-10 `CINE10/CINE11` selector is a **different predicate**.

### 13.2 Event 7 has an early no-play return

At `13EFC..13F18`, the high-level entry tests requested event `7` and calls `1C1DC(0x25)`. When that predicate returns `1`, it returns zero immediately, **without** entering `13AD8`, opening an ANM, saving audiovisual state, or starting playback. Otherwise the normal path proceeds. Do not equate a zero return from `13EF8` with proof that a movie ran to completion; a deliberately suppressed request may also return zero.

## 14. Direct cinematic call-site inventory

An object-1 instruction scan identifies these direct `CALL rel32` edges into `13EF8`; descriptions are limited to what the nearby instructions prove.

| CODE.2 call address | Passed event | Evidence-bound context |
|---|---|---|
| `0x0052C` | dynamic (`EAX`) | Forwarded from a general dispatch branch after cases `0x0B..0x0D`. |
| `0x15DDE` | dynamic table word | Indexed `uint16` from `[0x13020 + index*2]`, advancing `[0x7FEC]`. |
| `0x194B7` | 0 | Immediate `push 0` in a separate presentation path. |
| `0x1992D` | 0 | Calls event 0 and examines its return before continuing. |
| `0x1999F` | 12 | Explicit event-12 request after music-state operations in that same path. |
| `0x21941` | 8 | Explicit request after a four-condition success gate and a text request. |
| `0x23E08` | 10 | Duplicate finale routine; established as having **no known reachable incoming edge**. |
| `0x26BD1` | 10 | **Live** five-quest-object success tail, followed by supervisor status 12. |

This is a list of **direct static calls**, not an exhaustive dynamic event-source census. An event number absent from an immediate call may still arrive through the two parameterized paths. In particular, the event-2 selector is real even without a direct `push 2; call 13EF8` site.

## 15. Interrupt and PIT evidence boundary for cinematic waits

The ANM frame loop awaits the `0xF58C` counter via `30700`, `2E608`, and `2E614`. The interrupt-vector installation routine at `30BB4..30C36` explicitly registers `30708` for **interrupt 8**; that handler increments `[0xF58C]`, executes `19C80`, and chains to its saved previous interrupt handler. This ties cinematic waits to an interrupt-updated clock, not to a fixed frame-delay loop.

A **second** timer infrastructure is also present. `360E8` installs handler `35FC6` at interrupt 8, using channel-state tables and periodic callbacks; `3616F` writes control value `0x36` to PIT port `0x43` and low/high divisor bytes to port `0x40`. `3619D` and `361CE` choose and program divisors from active timer channels. The two vector installations and their restoration paths must be ordered and traced before claiming a single fixed cinematic tick frequency.

Do **not** derive seconds per ANM tick from `default_wait=1`, alternate wait `6` or `2`, an isolated PIT divisor, or the conventional DOS BIOS tick rate. A trustworthy conversion requires the effective installed vector, PIT divisor, chaining behavior and active callback configuration during actual cinematic execution. These cross-module timing questions move to the dedicated timing chapter.

**Verification checkpoint:** 15 targeted original `CODE.2` machine-byte checks were rerun in this investigation and passed, covering event 2, event 7, immediate event 8/12 calls, IRQ0 handler installation/increment, and PIT programming. This checks binary identities and instruction anchors, **not** run-time cinematic execution or time-calibrated playback.

## 16. Validation limits and next evidence pass

The repository's `Resources/ANM/analyze_anm.py` records byte-exact unchanged parse/serialize and structural validation for all 13 supplied files, including all 73 type-2, 809 type-3 and 2,796 type-8 chunks. **This continuation has inspected the stored audit; it has not rerun corpus validation.** A raw-preserving parse/serialize check is not a certified mutation encoder.

Next: establish timer vector/PIT ordering in the dedicated timing chapter; correlate individual ANM SFX/subtitle packet parameters against the original cinematic payloads, indexed T and audio resource namespaces; then build frame-accurate runtime tests. Original chunk-4 authoring intent remains **UNKNOWN**.

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
