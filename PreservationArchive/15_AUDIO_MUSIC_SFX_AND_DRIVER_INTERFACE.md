# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 15 — Sound, Music, Sampled Effects, Cinematics, and Driver Interfaces

**Status:** PRIMARY GAME-FACING AUDIO RECONSTRUCTION — IN PROGRESS
**Branch:** code1-decompilation
**Frozen evidence baseline:** 373c7f10e6fc0f29c1e805125ef048150fcc7ba7

---

## 1. Scope

The original game uses several distinct, connected audio systems: configuration of Music and Digital Sound Effects hardware; a queued and directly callable sampled-SFX system; an independent XMIDI sequence/music manager; NPC sound identities; dialogue/interaction audio; and audio commands embedded in ANM cinematics. These ultimately call Miles AIL/32 hardware backend drivers. This chapter records the game-facing contracts and distinguishes required native-port behavior from optional third-party hardware archaeology.

The controlling files are the shipped CODE.1/CODE.2 binaries, SOUND.CFG, RES5/RES6/RES10 and legacy RES11/RES12, INIT, SAMPLE.AD, GM1.BNK/GM2.BNK, the ANM archive, and the A32/GUS driver set.

Sources: [Audio dependency audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/AUDIO_INTERFACE_DEPENDENCY_AUDIT.md), [resource grammar](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/FORMAT.md), [optional driver triage](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/OPTIONAL_EXECUTABLE_DRIVER_TRIAGE.md).

## 2. Game-level architecture

~~~text
CODE.1 / CODE.2 independently read SOUND.CFG
           |
           +-- Digital SFX
           |    queue 28110
           |    service 28168
           |    playback 27F24
           |
           +-- XMIDI music
           |    RES5 loader 286C4
           |    28Cxx..290xx sequence state
           |
           +-- NPC / interaction / cinematic audio
           |    NPCS sound IDs
           |    RES9 dialogue voice sub-bases
           |    ANM chunks 5,6,9
           |
           v
     game-facing Miles audio interface
           |
           +-- FM/OPL drivers + SAMPLE.AD
           +-- MIDI / MT-32 / ARIA / GUS variants
           +-- digital playback drivers + VOC blobs
~~~

The upper layer belongs to the game; ISA I/O/DMA/FM-device implementation resides in third-party driver code.

## 3. SOUND.CFG exact identity

~~~text
size       59 / 0x3B bytes
SHA-256    b373f84879c0dfe524009526989a3b6562f518a9a64280b4ee282673cb0474cc
type       binary, not INI/text
+0x16      a32mt32.dll
+0x24      a32sbpdg.dll
~~~

Supplied raw bytes:

~~~text
0000: 30 03 02 00 FF FF FF FF 78 00 20 02 07 00 01 00
0010: 01 00 7A 00 01 00 61 33 32 6D 74 33 32 2E 64 6C
0020: 6C 00 00 00 61 33 32 73 62 70 64 67 2E 64 6C 6C
0030: 00 00 04 00 03 00 08 00 02 00 00
~~~

The selected drivers in this sample are consistent with MT-32 music and Sound Blaster Pro digital audio. They are configuration data, not evidence that other shipped hardware paths are unsupported.

## 4. CODE.1 reads SOUND.CFG directly

Original CODE.1:

~~~text
filename physical file offset 0x3EA10, object2 +0x2EC
logical 9652: push 0x2EC (sound.cfg)
logical 9657: call file open
logical 9662: push 0x3B
logical 9664: push config buffer
logical 966A: call transfer
logical 9672..9673: close
~~~

The 59-byte transfer is an explicit machine constant.

## 5. CODE.2 independently reads SOUND.CFG

Original CODE.2:

~~~text
filename physical file offset 0x77360, object2 +0x143C
logical 2F37F: push 0x143C (sound.cfg)
logical 2F384: call file open
logical 2F38D: push 0x3B
logical 2F38F: push config buffer
logical 2F397: call transfer
logical 2F39F..2F3A0: close
~~~

Both CODE modules must initialize their own period audio state. A native unified process may reuse host devices internally but must reproduce the observable two-module transition/reset semantics.

## 6. Two configuration groups

CODE.2 passes the following fields together to two audio initialization routines:

~~~text
group A:
  uint16 at +0x00, +0x02, +0x04, +0x06
  string at +0x16

group B:
  uint16 at +0x0A, +0x0C, +0x0E, +0x10
  string at +0x24
  conditional path tests uint16 at +0x14

additional device-special-case input:
  uint16 at +0x08
~~~

The installer exposes distinct Music versus Digital Sound Effects groups. Exact source names for individual numeric words have not been established; do not guess hardware port/IRQ/DMA assignments solely from plausible values.

## 7. SINSTALL and A32 driver provenance

SINSTALL.EXE identifies itself as SSI Sound Flat Model Installer for Ravenloft 4.2.6b:

~~~text
size       381660
SHA-256    dcb28a9a38c822ded4392f6ff0ae7213b0e5e9a76dc69aa56bd0d1d48fd1ba4a
~~~

Its strings include SOUND.CFG, Music, Digital Sound Effects, address/IRQ/DMA prompts and original driver filenames. The exact config writer routine is not independently reconstructed.

All 18 shipped audio DLLs have LX/OS2-style protected-mode structures and represent hardware backends, not the owner of known Ravenloft gameplay state machines. Full driver decompilation is optional historical work unless a game-facing status/timing ABI requires it.

## 8. Digital SFX scheduling

Proved CODE.2 functions:

~~~text
28110  queue sound request in channel/priority table
28168  service pending request state
27F24  sampled sound playback path
280DC  sound mode helper
28098  user digital sound enable/disable
~~~

The request table uses 12-byte entries per channel and compares priority before replacing pending state. Do not treat a queued request as an immediate blocking sound playback call.

## 9. LEVELS opcode 5 requests sampled SFX

Command opcode 5 invokes:

~~~text
28110(
  request_id = unaligned_u32(command_bytes+3),
  aux        = 0xFF,
  channel    = 0,
  priority   = 0
)
~~~

The dword reads past the nominal 5-byte command boundary when necessary. All 20 supplied instances occur in level 5 and resolve ID 0xB8. The read width/adjacent bytes are part of certified executable behavior.

Source: [Event callees](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/EVENT_CALLEES.md).

## 10. ANM queued and direct SFX differ

ANM chunk 6:

- 2-byte payload;
- adds the encoded ID to audio base 0x258D0;
- queues through 28110 with channel 1, aux 0xFF, priority zero;
- deliberately ignores IDs 3, 15 and 0x88.

ANM chunk 9:

- 2-byte payload;
- calls direct playback 27F24 through wrapper 146B8;
- brackets playback with 280DC audio-mode calls.

This is separate from the level-event channel-zero queue. The exact distinction must survive native cinematic playback.

Source: [ANM format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/ANM/FORMAT.md).

## 11. Wordcode direct SFX is executable-supported

RES9 wordcode opcode 0x1C accepts one sound ID, adds the relevant INIT SFX base and enters a direct sound playback path.

The supplied 38 active wordcode programs do not instantiate this opcode, but CODE.2 supports it. Its absence from the corpus is not permission to delete its VM handler.

## 12. NPC sound identities are separate fields

NPCS.DAT:

~~~text
+0x3D  ambient/proximity ID
+0x3F  attack ID
+0x41  damage/reaction ID
+0x43  death ID
~~~

Ambient/attack/reaction sounds use creature audio base 0x258C0; death uses distinct base 0x258BC. A zero death ID requests the base death sound, not silence.

The creature definition supplies these IDs; active actor HP/action state decides when to trigger them.

## 13. Type-4 resources are Creative Voice blobs

The RES EH type-4 loader owns:

~~~text
uint32 blob_size
byte   VOC_blob[blob_size]
~~~

All 1005 shipped type-4 resources have Creative Voice File followed by 0x1A signature. Normal instances have exact declared sizes.

Active audio-oriented archives:

~~~text
RES6  266 type-4 VOC resources
RES10 219 type-4 VOC resources
~~~

INIT indexes multiple RES6 SFX sub-bases and an independent RES10 interaction voice/SFX base.

## 14. Important two-entry VOC size anomaly

Byte-identical historical entries:

~~~text
RES11:4877 and RES12:430

SHA-256  07db990a79ea8d02e89c150fec3d45a918a6efb563a99feb3a21f090315959eb

declared size  0x11669
physical bytes 0x01669
difference     0x10000
~~~

The two anomalous entries are the only ones among 1005 type-4 resources with that declared/physical mismatch. The original field is 32-bit. Preservation tooling must report and retain it rather than silently rewrite it.

## 15. INIT audio namespace bases

INIT contains:

~~~text
+0x10 49   alternate RES5 XMIDI bank offset
+0x12 49   RES6 SFX boundary/base
+0x14 69   RES6 SFX boundary/base
+0x16 166  RES6 SFX boundary/base
+0x18 192  RES6 SFX boundary/base
+0x1A 266  RES6 SFX end / latent direct-SFX base
+0x20 48   RES10 interaction voice/SFX base
~~~

Caller context controls how local sound IDs are interpreted. It is incorrect to combine all SFX/voice IDs into one unqualified absolute integer namespace.

Source: [INIT reconstruction](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/README.md).

## 16. Music is a separate sequence manager

Recovered logical routines:

~~~text
286C4  type-3 XMIDI/XDIR loader
28C54  select/load/start music sequence
28DCC  begin timed music transition/stop
28EF0  poll/wait/finalize prior sequence
288FC  start sequence through low-level backend
29050  return current sequence ID
29048  active sequence slot/state
290A8  user music enable/disable
~~~

The current sequence ID is stored at 0xEF28. Digital SFX enable/disable instead routes through 28098.

## 17. Type-3 archive resources contain XMIDI/XDIR

Normal EH type 3 body:

~~~text
uint32 blob_size
byte   FORM_XDIR_payload[blob_size]
~~~

All 172 supplied type-3 entries match their stored lengths exactly and begin FORM ... XDIR.

Final RES5 contains 98 XMIDI/XDIR records arranged as two 49-entry banks. INIT +0x10=49 selects the secondary bank for certain driver-mode paths.

Do not label these resources simply Standard MIDI Files; their native container is XMIDI/XDIR.

## 18. Legacy music bank is not byte-equivalent to final RES5

Historical RES11 contains 74 type-3 records arranged as two banks of 37, versus final RES5 two banks of 49.

No complete legacy type-3 entry is byte-identical to a final RES5 entry; the XDIR sequence-count profiles also differ.

This supports a rebuild/recompile of the music bank during resource reorganization, but does not itself prove identical or changed compositions.

Source: [RES11/RES12 lineage](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/RES11_RES12_LINEAGE.md).

## 19. Level default music belongs to the level header

LEVELS fixed header +0x29C is the default music-sequence ID. Level activation compares it against the current sequence and transitions when needed; interaction music can later restore the header-selected default.

Across 32 supplied levels, the observed music IDs include 0..13, 34..43 and 45.

It is not a global SOUND.CFG field and not an RES5 file offset by itself.

## 20. LEVELS opcode 6 transitions between music sequences

The event handler passes the unaligned dword at command byte +3 to:

~~~text
28DCC(0x9C4)
28EF0()
28C54(requested_sequence, 1)
~~~

Two active commands, both in level 29, request sequence 0x2F and 0x2E.

The ordering is part of the original audiovisual/story sequence and should be preserved.

## 21. RES9 wordcode music changes are encounter-local

Wordcode opcode 0x1D selects a music ID through the sequence manager.

Active wordcode contains 36 uses. Per-program data associates distinct NPC encounters with different music IDs. This is independent from its RES9 graphics base, its indexed T text base, and its interaction voice base.

A native interaction runner should preserve restoration of default level music when encounters end.

## 22. ANM music packet is temporally significant

ANM chunk 5 has two signed 16-bit words and calls:

~~~text
28E58(sequence_id, second_word + 1, 0x9C4)
~~~

after the required music transition/finalization sequence.

It is an in-band cinematic command synchronized with frames, not a standalone XMIDI event file.

## 23. Sound and music options are different switches

CODE.2 options UI calls:

~~~text
28098  digital SFX enable/disable
290A8  music enable/disable
~~~

The original game retains distinct player preferences and distinct system states. A native backend must not implement both as a single mute flag when their initialization/playback/status semantics differ.

## 24. SAMPLE.AD is the FM timbre library

Supplied:

~~~text
name       SAMPLE.AD
bytes      4662
SHA-256    79ee9f4ad3134be906ce493072ce0c3b7169c9013d3c25947ac385aaf3f9633b
format     Miles AIL 2.x YM3812/OPL2 Global Timbre Library
~~~

The file is a third-party timbre bank used to map XMIDI bank/patch values to OPL instrument register images, not a DreamForge gameplay container.

Its format is independently corroborated by Miles GLIB.C from the released vendor source.

## 25. SAMPLE.AD physical header and payload grammar

~~~text
repeat header:
   uint8 patch
   uint8 bank
   if patch == FF && bank == FF: stop header
   uint32 absolute_timbre_offset

repeat one timbre per entry:
   uint16 record_length  // includes these 2 bytes
   int8 transpose
   11-byte YM3812 register image
~~~

Supplied checks:

~~~text
233 timbre entries
6 bytes/header entry
header termination at 0x576
first timbre at 0x578
every record length 14
final end at EOF 4662
~~~

No file slack or unreferenced timbre payload exists.

Source: [SAMPLE.AD format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/SAMPLE.AD/FORMAT.md).

## 26. The FM timbre structure is a YM3812 register image

The 11-byte instrument part is:

~~~text
modulator:
  AVEKM
  KSL/TL
  AD
  SR
  waveform

feedback/connection

carrier:
  AVEKM
  KSL/TL
  AD
  SR
  waveform
~~~

With one signed transpose byte, this yields the 12-byte payload after its 2-byte length.

Header bank/patch ranges:

~~~text
bank 0    patch 0..127    128 entries
bank 127  patch 35..87     53 entries
bank 1    patch 0..51      52 entries
~~~

Order must be preserved, including bank 127 before bank 1. An FM-native playback option can use these values directly rather than a substituted General MIDI soundfont.

## 27. ARIA GM1/GM2 are separate vendor-format banks

Supplied files:

~~~text
GM1.BNK  23907 bytes
SHA-256 843f012e7980e0a58722aa5c7a6e88b652f2f8d404834638cd994ca0869df5a8

GM2.BNK  24273 bytes
SHA-256 b4af656059a6afba5db2da972c3506ca08d899ca04c65389e0bec7aaf6536339
~~~

Both begin:

~~~text
02 AF 00 32 07 02 4D 19 71 19 95 19 C5 19 F5 19
~~~

The files are Sierra Semiconductor ARIA General MIDI support banks, not AdLib Instrument Maker BNK files. Do not try to decode them with the unrelated AdLib BNK grammar.

## 28. ARIA bank selection follows period ROM capacity

Period ARIA documentation maps:

~~~text
GM1.BNK  512 KiB / 0.5 MiB ARIA ROM configuration
GM2.BNK 1024 KiB / 1.0 MiB ARIA ROM configuration
~~~

The original driver inventory includes A32ARXM.DLL (ARIA XMIDI family) and A32ARDG.DLL.

The inner grammar and vendor producer/build identities remain UNKNOWN, because the bank files have not been field-decoded against a verified ARIA consumer.

Source: [ARIA bank classification](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/BNK/ARIA_BANKS.md).

## 29. GUS patch-loading support is an optional hardware track

LOADPATS.EXE is an LZEXE 0.91-compressed DOS utility. PATCHES.INI explicitly contains Gravis UltraSound AIL32 patch mapping/configuration.

The supplied launch BAT files contain comments referring to LOADPATS revision/failure logic, but neither actually invokes LOADPATS before checking ERRORLEVEL. Therefore the precise activation path cannot be inferred solely from BAT comments.

This matters for truthful historical reconstruction, but does not block game-facing native sound playback.

## 30. Historical driver support should remain preserved

The original DLLs, SAMPLE.AD, ARIA banks, LOADPATS, PATCHES.INI, SINSTALL, and inactive audio-only resources are historically meaningful even when a modern platform never loads their original binary code.

They belong in the repository and preservation backup, distinct from the required first-port implementation of driver-facing audio services.

## 31. Native integration concept

~~~text
LegacyAudioConfig
    raw SOUND.CFG + two decoded config groups

DigitalSoundScheduler
    per-channel queued requests and priority

SampleResourceResolver
    INIT SFX base + RES6/RES10 indexed VOC

MusicSequenceManager
    current sequence ID, transition and enable flags
    INIT alternative bank + RES5 XMIDI/XDIR

CinematicAudioDispatcher
    ANM chunk 5/6/9 timing and channel differences

InteractionVoiceResolver
    RES9 voice base + INIT + RES10

HardwareBackendAdapter
    host audio device, not original ISA I/O implementation

VendorAssetCompatibility
    SAMPLE.AD / ARIA banks / original drivers preserved
~~~

These are proposed port boundaries, not recovered original type names.

## 32. Audio preservation boundary

A modern port may provide clean digital output, device mixing and safe resource bounds checks while remaining faithful to original:
- which asset is selected;
- when and by which actor/script an event fires;
- whether it is queued or direct;
- SFX channel/priority;
- sequence transitions and waits;
- enable/disable state;
- known resource quirks;
- file loading and error behavior exposed to the game.

It need not reproduce motherboard/hardware details except when explicitly operating a hardware-emulation mode.

## 33. Remaining core-facing audio questions

Required follow-up evidence is narrowly defined:

1. exact SOUND.CFG numeric word meanings;
2. precise CODE.1/CODE.2 initialization callee behavior;
3. audio driver status/completion results visible to game logic;
4. per-driver RES5 alternate-bank selection;
5. sequence transition timing/state when a new request arrives during an active sequence.

These are **open implementation-level questions**, not justification to infer hardware port, IRQ and DMA labels from the example config bytes.

## 34. Fidelity boundary for modern hardware

A faithful native game should preserve which sound or sequence is requested, its namespace, timing, priority, channel, enable state, and expected completion status.

A modern backend may map these events into a host audio mixer and MIDI/XMIDI renderer. Emulating ISA registers, DMA, interrupts or the original hardware detection utility is optional unless that behavior materially affects gameplay timing.

## 35. Drivers and installers remain preservation assets

The source set includes INSTALL.EXE, SINSTALL.EXE, LOADPATS.EXE, PATCHES.INI and eighteen shipped protected-mode driver DLLs.

Preserve all of them even when none is loaded by a modern default audio configuration. **Conditionally inactive**, **historically retained**, **not called by a particular executable**, and **unrelated to game logic** are not interchangeable classifications.

## 36. Regression: both CODE config reads

For CODE.1 and CODE.2 require:

~~~text
SOUND.CFG read size   0x3B
binary fixture bytes  59
driver string #1      +0x16
driver string #2      +0x24
group A words         +0,+2,+4,+6
group B words         +0xA,+0xC,+0xE,+0x10
conditional selector  +0x14
~~~

A no-op import/export must preserve all 59 bytes even when some numeric words remain unnamed.

## 37. Regression: separate SFX scheduling and direct playback

Cover:
- request into an empty channel;
- higher, equal and lower priority on occupied channels;
- separate channels zero and one;
- delayed queue servicing;
- direct 27F24 invocation;
- 280DC mode bracketing;
- user audio-disable behavior.

The exact priority comparison must be tested against original 28110, not guessed from abstract mixer expectations.

## 38. Regression: distinct game callers

Require correct caller-specific behavior for:

~~~text
LEVELS opcode 5    queue channel 0, aux FF, priority 0
ANM chunk 6        add 0x258D0 base, queue channel 1
ANM chunk 9        direct playback with 280DC wrapper
wordcode opcode 1C direct sound path, though no active use
NPC death           distinct audio base and zero-ID fallback
~~~

The ANM chunk-6 ID exclusions 3, 15 and 0x88 require separate fixtures.

## 39. Regression: XMIDI and music sequence selection

For all 172 type-3 entries:
- preserve the outer EH type-3 record;
- require the stated 32-bit payload extent;
- verify FORM/XDIR signature;
- preserve raw bytes, not merely conversion into host MIDI;
- retain RES5's two 49-entry sequence banks and INIT offset 49;
- preserve separate music-enable and current-sequence state.

Transitions must exercise 28DCC, 28EF0 and 28C54 ordering.

## 40. Regression: VOC resources and original anomaly

For all 1005 type-4 blobs require the Creative Voice File signature.

For 1003 ordinary records verify declared length equals stored payload extent.

Retain exactly the two byte-identical anomalies at RES11:4877 and RES12:430, preserving the original 0x11669 length field against the 0x1669-byte payload.

Report the mismatch; do not silently repair or drop the historical resource.

## 41. Regression: sampled-audio namespace mapping

Verify distinct resolution of:
- RES6 game SFX;
- RES10 interaction sound/voice;
- INIT SFX boundary values 49, 69, 166, 192, 266;
- INIT interaction voice base 48;
- NPC ambient/attack/reaction versus death sound bases.

A numeric sound ID alone is not sufficient without caller/namespace context.

## 42. Regression: SAMPLE.AD timbre library

Require:

~~~text
233 header records, six bytes each
FF FF header terminator at 0x576
first timbre begins at 0x578
every timbre record length 14
12 bytes timbre payload per entry
all 233 records close at EOF 4662
bank 0    patches 0..127
bank 127  patches 35..87
bank 1    patches 0..51
~~~

Validate YM3812 modulator/carrier register fields against the independently identified Miles format. Preserve source header order and absolute offsets.

## 43. Regression: ARIA preservation-only boundary

Verify exact GM1/GM2 sizes, hashes, common signature and shared first 77 bytes.

Until the inner ARIA bank record grammar is recovered from a trustworthy spec or driver, field-level round-trip must remain **NOT CERTIFIED**. Passing a raw byte-preserving copy test does not certify decoded ARIA timbres.

## 44. Regression: cinematic audio and image timeline

Exercise ANM chunk IDs 5, 6 and 9 in the original frame/packet order and relative to image/palette packets and the frame wait value.

ANM chunk 7 is a T subtitle/text lookup, not itself a music or audio command.

This prevents modern playback from drifting when cinematic commands coincide with graphics or mode changes.

## 45. Correction C-33 — sampled SFX and music are not one play function

CODE.2 has independent queues, direct sampled playback, music-sequence loading, timed stop/finalization and distinct user settings.

Flattening every caller into an immediate play() loses the original scheduling/transition logic.

## 46. Correction C-34 — SOUND.CFG is actively consumed by both CODE modules

Both executables load exactly 59 bytes into their own audio configuration state. SOUND.CFG is not only an optional installer byproduct.

A native port may replace the legacy hardware UI while retaining the original configuration import contract.

## 47. Correction C-35 — SAMPLE.AD and ARIA BNK are different formats

SAMPLE.AD is a Miles OPL2/YM3812 Global Timbre Library.

GM1.BNK and GM2.BNK are Sierra Semiconductor ARIA hardware banks. They are not AdLib Instrument Maker BNK archives and should not be parsed with that unrelated grammar.

## 48. Correction C-36 — inactive legacy audio resources are historically meaningful

RES11 and RES12 preserve earlier music/VOC resource generations, including byte-identical legacy sampled banks and a real 0x10000 declared-length anomaly.

An inactive resource is not automatically redundant, especially during historical source/format preservation.

## 49. Correction C-37 — BAT comments do not prove GUS patch-loader execution

The supplied RLOFT.BAT/SOUND.BAT contain LOADPATS-oriented comments but neither actually invokes LOADPATS.EXE before checking ERRORLEVEL.

The activation path is not established by those comments alone.

## 50. Historical unknowns retained

- exact numeric SOUND.CFG field labels and writer rules;
- full original installer/card detection workflow;
- driver-side timing/completion ABI where not proved by CODE;
- precise per-driver RES5 bank selection predicates;
- ARIA GM1/GM2 inner grammar;
- LOADPATS activation path under every original installation configuration;
- exact original source-function names for the game audio interface;
- hardware-specific synthesis behavior beyond the original source asset/driver role.

These remain bounded questions rather than invented facts or grounds for modifying original files.

## 51. Source ledger

- [Runtime audio dependency audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/AUDIO_INTERFACE_DEPENDENCY_AUDIT.md)
- [Optional executable/driver triage](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/OPTIONAL_EXECUTABLE_DRIVER_TRIAGE.md)
- [Resource type-3/type-4 format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/FORMAT.md)
- [Resource inventory](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/INVENTORY.md)
- [Historical RES11/RES12 lineage](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/RES11_RES12_LINEAGE.md)
- [INIT audio namespace bases](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/README.md)
- [SAMPLE.AD vendor-format reconstruction](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/SAMPLE.AD/FORMAT.md)
- [ARIA bank classification](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/BNK/ARIA_BANKS.md)
- [ANM audiovisual packet grammar](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/ANM/FORMAT.md)
- [LEVELS SFX/music event callees](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/EVENT_CALLEES.md)
- [NPCS sound fields](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/SOUND_FIELDS.md)

## 52. Architectural checkpoint

~~~text
SOUND.CFG 59 bytes
     |
     +-- CODE.1 independent audio setup
     +-- CODE.2 independent audio setup
                |
                +-- sampled SFX request queue
                |     RES6 / RES10 VOC
                |     NPC events
                |     LEVELS events
                |     ANM queue/direct SFX
                |     RES9 interaction voice
                |
                +-- music sequencer
                      RES5 XMIDI/XDIR
                      INIT 49-entry alternate bank
                      LEVELS default sequence
                      scripted changes
                      ANM music commands
                |
                v
           Miles/AIL driver boundary
                |
                +-- SAMPLE.AD OPL2 timbres
                +-- MT-32 / MIDI
                +-- ARIA GM1/GM2
                +-- Sound Blaster / GUS / other hardware
~~~

The game-facing audio architecture is now documented to the established supplied-build evidence boundary. The vendor/configuration unknowns remain explicitly open.

Next component: **ANM cinematic engine, frame/audio synchronization, subtitle/T integration, VGA palette and planar frames, and finale CINE10/CINE11 selection.**
