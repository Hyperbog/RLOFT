# SAMPLE.AD — Miles AIL 2.x YM3812 Global Timbre Library

Status: **CONFIRMED.** The supplied `SAMPLE.AD` is a Miles Design IBM Audio Interface Library (AIL) Global Timbre Library (GTL) for YM3812/OPL2 playback. Its physical grammar matches the original Miles `GLIB.C` structures and writer logic exactly, and the supplied file validates end-to-end with no slack or unreferenced payload bytes.

## Controlling evidence

- supplied file: `SAMPLE.AD`
- bytes: **4,662**
- SHA-256: `79ee9f4ad3134be906ce493072ce0c3b7169c9013d3c25947ac385aaf3f9633b`
- Ravenloft `CODE.1` and `CODE.2` both contain the literal `SAMPLE.AD` and the adjacent runtime error text `Timber file "%s" not found`;
- the certified CODE.1 audit already preserves the timbre-file open/failure path;
- Miles Design `GLIB.C` identifies this format as a standard AIL Global Timbre Library used to assign instrument data to XMIDI Patch Bank Select / Patch Change values;
- original Miles source defines the packed header fields `patch`, `bank`, `offset` and the YM3812 `AD_instrument` register image used below.

Vendor-source reference used for nomenclature and field order:
`https://github.com/Tronix286/AIL2/blob/master/GLIB.C`

The repository mirror states that its contents are the real-mode AIL 2.14 / AIL32 source release made open by John Miles in 2000. Shipped Ravenloft bytes remain primary evidence; the Miles source is used to identify the third-party format and its intended field names.

## Physical grammar

The file is:

```text
repeat:
    uint8  patch
    uint8  bank
    if patch == 0xFF and bank == 0xFF:
        end header
    uint32 absolute_timbre_offset

repeat once for each header entry, in the same order:
    uint16 record_length
    byte   timbre_payload[record_length - 2]
```

For this supplied file:

```text
header entries       = 233
header-entry bytes   = 233 * 6 = 1398 / 0x576
terminator            = FF FF at 0x576
first timbre          = 0x578
record length         = 14 for every entry
record payload        = 12 bytes for every entry
final record end      = 0x1236 / 4662 == EOF
```

The first timbre offset equals the byte immediately after the two-byte `FF FF` header terminator. Every timbre record ends exactly at the next header entry's absolute offset. The final timbre ends exactly at physical EOF.

No alignment padding, slack, or secondary directory occurs in this file.

## Header namespaces

The 233 header entries are ordered into three contiguous MIDI bank/patch ranges:

| bank | patch range | entries |
|---:|---:|---:|
| `0` | `0..127` | 128 |
| `127` | `35..87` | 53 |
| `1` | `0..51` | 52 |

Header order is therefore bank 0, bank 127, then bank 1. Tools must preserve serialized order rather than sorting `(bank, patch)` pairs.

Miles' GTL format uses these two bytes as the XMIDI bank and patch selectors. They are lookup keys, not file-local ordinal IDs.

## YM3812 / OPL2 timbre record

All 233 supplied records have:

```text
uint16 record_length = 14
int8   transpose
uint8  mod_avekm
uint8  mod_ksl_tl
uint8  mod_ad
uint8  mod_sr
uint8  mod_ws
uint8  fb_c
uint8  car_avekm
uint8  car_ksl_tl
uint8  car_ad
uint8  car_sr
uint8  car_ws
```

The 12 bytes after `record_length` are exactly the structure emitted by Miles `GLIB.C` for a YM3812 timbre:

```c
struct {
    char transpose;
    AD_instrument AIL;
};
```

where the vendor `AD_instrument` is the eleven-byte OPL register image:

```text
modulator: AVEKM, KSL/TL, AD, SR, waveform
channel:   feedback/connection
carrier:   AVEKM, KSL/TL, AD, SR, waveform
```

These names describe encoded Yamaha OPL register fields, not DreamForge-specific semantics.

The supplied waveform-select fields are all in `0..3`, consistent with YM3812 waveforms. No OPL3-length timbre occurs in this file.

## Why the length is 14

Miles' GTL writer stores the complete record byte count, including the two-byte length itself. Ravenloft's entries therefore contain:

```text
2 bytes record_length
1 byte  transpose
11 bytes YM3812 instrument register image
-------------------------------------------
14 bytes total
```

This relationship holds for all 233 records.

## Engine role

Ravenloft stores its music sequences as XMIDI/XDIR resources (RES type 3). `SAMPLE.AD` supplies the programmable OPL2 instrument definitions needed by AIL's FM XMIDI drivers. It is therefore a **third-party Miles audio support bank used by the game's music-driver path**, not a DreamForge gameplay/resource container.

The shipped runtime also contains Miles audio drivers including `A32ADLIB.DLL`, `A32ALGFM.DLL`, `A32PASFM.DLL`, `A32SBFM.DLL`, and related FM variants. The presence of `SAMPLE.AD` and the explicit timbre-file error path are consistent with that driver architecture.

## Preservation boundary

For a modern port:

- preserve the original GTL bytes and key ordering as canonical evidence;
- a native OPL emulator may consume the decoded register images directly;
- General MIDI / SoundFont substitution is a presentation option, not a faithful replacement for the original FM instrument bank;
- do not normalize or sort the GTL header, because file order and absolute offsets are serialized data.

## Reproducible validation

`analyze_sample_ad.py` verifies:

1. immutable file size/SHA;
2. six-byte header entries;
3. terminating `FF FF` marker;
4. first timbre offset equals header end;
5. all offsets and record lengths are in bounds;
6. every record length is 14 bytes;
7. each record ends exactly at the next record offset / EOF;
8. the three observed bank/patch ranges;
9. exact final EOF closure.
