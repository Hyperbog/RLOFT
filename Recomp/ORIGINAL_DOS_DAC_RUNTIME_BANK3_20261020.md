# Part 25 — Original DOSBox-X CODE.2 DAC write-chain capture (2026-10-20)

**Status: Original DOS game DAC writer execution VERIFIED; final RGB6 output matches independently saved VGA palette exactly. Original producer of the eight-entry in-memory change still NOT VERIFIED; save/port event atomic alignment NOT ATTESTED.**

## Authenticated private evidence (never commit the files)

- `active-dac-20261020T090210296Z-21816.jsonl`: 2,025,614 bytes, SHA-256 `5bd50f2eb80abbe3f4886e9c65696084447ec476763ca966cc215181b967c3cd`, 25,361 strictly sequenced VGA DAC port events, DOSBox-X `dosboxx-v2026.10.01-dac-v1` hook header. This is the **new gameplay** log, not the earlier BIOS-only `active-dac.jsonl`.
- `1.sav`: 958,827 bytes, SHA-256 `01fca5ce21f60802fb01d3a13ea5c8c6776effd8bf8ebb07f0f495326cd46689`; ZIP CRC verified; metadata `DOSBox-X 2026.10.01 (SDL2)`, `Program_Name=CODE`, `Machine_Type=MCH_VGA`, internal timestamp `2026-10-20 05:02`, 2,643,024-byte `Vga` component. DAC six-bit precision.
- `RLOFT-DAC-AutoLaunch-20261020T090210296Z-21816.conf`: confirms `C:` game mount at owner installation, `D:` mounted from `game.gog` via `imgmount ... -t iso -fs iso`, `r.exe` launch, `saveremark=false`.
- Independent certified original `CODE.2`: SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

## Original VGA output and actual execution

The hook captured **41** byte writes to index port `0x3C8` and **25,320** accepted six-bit color writes to DAC data port `0x3C9` (**25,361** events total).

- BIOS `CS=F000h`: 2,291 early events.
- Protected-mode game `CS=0180h`: 23,070 events, comprising **30 complete 256-color submissions**, each one index-0 write followed by exactly 768 RGB6 bytes.
- Each complete game submission is chunked into **four 192-byte segments**, with three segments at raw EIP hint `0x1EECCF` and one at `0x1EECE0`.
- Index writes carry EIP hint `0x1EECA0` (30 observations), 192-byte transfers at `0x1EECCF` occur 17,280 times and `0x1EECE0` 5,760 times.
- The independently authenticated `CODE.2` original LE executable object 1 (source file offset `0x3AF24`) contains palette writer **`0x29CA0`**, actual `OUT DX,AL` to port `3C8h` at `0x29CB6`, and `REP OUTSB` at `0x29CCF` and `0x29CE0`. All three raw EIP hints are these object-relative addresses plus common runtime offset **`0x1C5000`**.
- Note: the port-index EIP hint maps to the function **entry** `0x29CA0`, rather than its actual `OUT` instruction `0x29CB6`; EIP reported by the VGA hook is a CPU-state hint, **not independently guaranteed to be the exact executing instruction at the I/O call**. The source-signature and shared offset, however, now ground actual execution of this writer routine. The **caller that supplied its palette pointer** is still unresolved.

The 768-byte final DAC reconstruction from the **complete raw original port stream** has SHA-256 `0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812` and matches every one of the 768 RGB6 bytes extracted independently from `1.sav`. The last **two** game palette submissions, numbered 29 and 30, are exactly identical.

## Eight-entry transition, previously unresolved

Between complete **game submissions 28 and 29**, **only eight palette entries, indices 246–253**, change: all 24 RGB6 component bytes change. The remaining 248 colors are byte-identical. Submission 30 repeats submission 29 unchanged.

| Palette index | Submission 28 (RGB6) | Submission 29 / 30 and 1.sav (RGB6) |
|---:|---|---|
| 246 | 25,25,31 | 48,48,48 |
| 247 | 34,34,41 | 53,53,53 |
| 248 | 48,44,45 | 57,57,57 |
| 249 | 50,50,50 | 62,62,62 |
| 250 | 2,2,2 | 8,8,8 |
| 251 | 3,3,3 | 14,14,14 |
| 252 | 5,5,5 | 19,19,19 |
| 253 | 6,6,6 | 25,25,25 |

Submission 28 begins at event sequence 23,054; submission 29 at 23,823; submission 30 at 24,592. Each carries one 3C8 index-0 write and 768 3C9 channel writes. No separate 3C8 index-246 partial DAC update occurs in the protected-mode game segment.

**Interpretation bounded to original event evidence:** the eight-entry replacement has occurred **upstream of the full DAC output routine**, i.e. in a palette buffer or processing path, not in a separately observed hardware update targeting eight indexed colors. This is positive original runtime evidence for the 246–253 palette change, strengthening the previous provisional native saved-world slice while **not** identifying the CODE.2 routine/caller that made the change.

## Strict boundaries and next gate

- The original `.sav` and event log are plausibly from the same original DOSBox-X session and the final emitted palette agrees exactly. However, the v1 hook did **not** record an atomic save-event sequence number; the copied log can contain later events. **Same-instant capture: NOT ATTESTED**, despite equality.
- The original **DAC writer**, original palette segment transition, and saved RGB6 bytes are established. The original **palette source-bank selection/buffer writer**, **CODE.2 call ancestry**, and **native-vs-DOS indexed frame pixel parity** remain NOT VERIFIED.
- Next evidence requirement: hook the original code's **pre-3C8** submission with palette source pointer/768-byte buffer SHA and caller stack trace, or instrument guarded writes to the original palette staging buffer and prove the precise producer of the 246–253 replacement. Do not infer that the previously unresolved three palette helpers necessarily execute.
- Do not commit `CODE.2`, SAV files, raw JSONL logs, complete original palettes, or GOG media to public GitHub. The owner-private byte-level audit report remains a separate downloadable artifact.

Existing context: [Part 24 LE fixups and trace acceptance](ORIGINAL_LE_PALETTE_FIXUPS_AND_DOS_DAC_TRACE_20261020.md).


## Part 26 — original COLORS bank 0→3 source-producer resolution

New exact original media result (2026-10-20): The authentic 11,521-byte
COLORS file (SHA256 e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5)
contains 15 RGB6 palette banks. The bank at index 0 has SHA256
1f60d7831cd7b034ae67c894dc91662f15d4341fbf28e2a954b1b7e3e780f64f;
index 3 has SHA256
0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812.
These are **exact** independent SHA256 matches to original DOSBox-X full-bank
DAC submission 28 (bank 0) and submissions 29/30 (bank 3). The source file's
two palette banks differ at exactly eight indices **246–253**, 24 RGB6
components; the other 744 component bytes match. No reconstructed or
synthetic port history was involved.

### Authenticated CODE.2 machine-code path

- Object-1 0x18C85/0x18C8D reads a scene's 3-byte descriptor first byte
  from object-2 0x81AC + scene_index*3 and calls object-1 0x1F8E0.
- 0x1F8E0 selects the scene palette by storing the ID into object-2 0xC034
  and flags palette refresh at object-2 0xCF58.
- 0x1FAC4/0x1FADB is a guarded palette refresh that passes [0xC034],
  the original COLORS file name and buffer 0xC054 to 0x325B0;
  0x1FAE8 then calls the 0x29234 full-bank DAC submission wrapper.
- 0x325B0 seeks to 1 + palette_index*768 in COLORS and reads 768 RGB6
  bytes for a full-bank request, then calls 0x32150 for optional in-memory
  brightness adjustment. The hardware writer is 0x29CA0.
- 0x20510/0x2051D is a second authentic startup call path that
  loads [0xC034] from COLORS and calls 0x29234.

The authenticated retail CODE.2 object-2 scene table at 0x81AC contains
the **Level-5 triplet [3,10,3]**. Its first byte selects palette bank 3.
Do not assign semantics to the other two bytes without further evidence.

The owner's independent DOSBox-X 1.sav serializes a DWORD **3** at original
data offset 0xC034, and a full byte-exact COLORS bank-3 staging palette
at original 0xC054 (SAV Memory component offset 0x20D0DC); the SAV's VGA
hardware palette independently matches the same 768-byte bank.
The SAV Memory header consumes 0x88 bytes before MemBase, with observed
original guest data linear base 0x201000 in this capture.

### Evidence boundary

**Proven:** correct original retail source bank values, full runtime output
matching the bank 0→3 transition, saved scene selection state, exact original
source selection/loading code and hardware writer.

**Still not proven from existing v1 I/O hook:** which static 0x325B0
call site actually executed at submission 29; the first-generation
emulator hook recorded outgoing DAC bytes and raw EIP hints, not the
producer/caller stack. The source chain through 0x18C8D, 0x1F8E0 and
0x1FADB is a strongly supported static explanation, but not an independently
recorded temporal CPU call sequence. The owner's game save was captured
approximately one animation after load, so exact phase synchronization,
skeleton animation frames and native pixel parity remain open.

The original bank-3 value-level slice is now **source authenticated**;
the original CPU instruction ancestry and native graphics timing remain
NOT_VERIFIED. A skeleton RES3 image comparison is unnecessary to explain
this bank switch; it belongs to a separate sprite rendering investigation.

See tools/audit_original_colors_scene_palette.py for SHA-pinned reproduction;
its optional --trace/--sav inputs are owner-private, never committed to Git.

## Part 27 — Native renderer integration and retirement of eight-color splice

The palette-source investigation has now been integrated into the native
saved-world renderer. See
[SOURCE_SCENE_PALETTE_NATIVE_INTEGRATION_20261020.md](SOURCE_SCENE_PALETTE_NATIVE_INTEGRATION_20261020.md).

The previous bank-0-plus-246..253-bank-3 overlay is **retired** in source.
The native saved-world renderer selects a complete original COLORS bank
from the CODE.2 scene descriptor table, validates it, stages all 768
components into C054, invokes compiled 32150 and 29CA0, and emits a
full-bank DAC palette. For the original Level-5 data the bank is 3.
[Original-media regression #39419812617](https://github.com/Hyperbog/RLOFT/actions/runs/39419812617)
passed exact full RGB6 palette equality, complete frame/sprite/HUD and
malformed-input checks. No prior static/dynamic provenance boundary has
changed: exact original DOS caller timing and synchronized DOS/native pixel
parity are still NOT VERIFIED. Older references in this historical log to a
*provisional native slice* describe the superseded implementation, not the
current saved-world renderer.
