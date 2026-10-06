# ANM format reconstruction

Status: **outer container and image packet grammar confirmed from CODE.2 and all 13 supplied ANMs; command semantics partly confirmed.**

The immutable ANM files and original CODE.2 bytes are controlling evidence. Community decoders are secondary references only.

## Outer stream

CODE.2 `13834..139AA` reads the cinematic stream as:

```text
uint16 frame_count
uint16 default_wait
repeat frame_count times:
    uint16 chunk_count
    repeat chunk_count times:
        uint16 chunk_id
        int32  chunk_size
        uint8  payload[chunk_size]
```

`135C4..13831` reads each chunk header and payload, resets the payload cursor, rejects IDs above 9, then dispatches IDs 0..9 through the ten-entry table at `1359C..135C3`.

The second header word is deliberately named `default_wait`, not `version`. After each frame, CODE.2 passes this value to timing routine `2E614` unless the active image path selects the alternate timing source at `12C10`. `CINE10.ANM` and `CINE11.ANM` store zero in this field; the other supplied files store one.

## Chunk dispatcher

| ID | CODE.2 handler | Supplied size(s) | Confirmed behavior |
|---:|---|---|---|
| 0 | `1362D` | 768 | Copies exactly `0x300` bytes from the packet into palette staging buffer `C355`, marks the palette dirty, and the frame loop applies it at `1393F` through `29234`. |
| 1 | `1364B` | 0 | Reset/blackout path: invokes the full-screen zero-fill path, zeroes the `0x300`-byte palette buffer, and immediately applies that palette. |
| 2 | `13689 -> 27BD8` | variable | Full-frame row RLE. Decodes 400 rows of exactly 320 chunky pixels, then `27A90` distributes every fourth byte into the four VGA planes. |
| 3 | `1369C -> 27AEC` | variable; 2 means no updates | Sparse planar delta. Encodes x advances, vertical-run groups, y advances, run lengths, and one color byte per updated pixel. Writes directly to the selected VGA page. |
| 4 | `136B6 -> 13488` | none supplied | Repeat/seek controller. The first-use path consumes a 6-byte control body (`uint16` count plus `uint32` offset) and manipulates stream position/state around the type-3 delta path. No supplied ANM exercises it, so higher-level meaning remains conservative. |
| 5 | `136DA` | 4 | Music-sequence command. Reads two signed words, performs the music transition/finalization sequence, then calls `28E58(sequence_id, second_word + 1, 0x9C4)`. DatFiles analysis independently establishes `28E58` as the music-sequence family. |
| 6 | `13752` | 2 | Queues a sound-effect request through `28110` on cinematic channel 1 with aux `0xFF` and priority 0, after adding the encoded ID to base `258D0`. IDs `3`, `15`, and `0x88` are intentionally ignored. DatFiles analysis independently establishes `28110` as the sound-effect request queue. |
| 7 | `137C0` | 4 | Subtitle/text command. First word zero clears the subtitle region; otherwise CODE.2 reads text record `first_word + 0x324` through indexed `T` reader `1C368` and marks subtitle state active. The second word is retained as a renderer parameter and passed to `13FD4`. |
| 8 | `136C0 -> 27B84` | `0x14000` | Raw four-plane image packet. Copies four consecutive `0x5000`-byte planes directly to VGA memory at page base + `0x0FF0`. With 80 bytes/scanline, this is a native 320x256 region beginning at scanline 51 inside the 320x400 framebuffer. |
| 9 | `1379F -> 146B8` | 2 | Direct sound-effect playback command. `146B8` brackets `27F24(id)` with `280DC` sound-subsystem mode calls; DatFiles analysis independently identifies `27F24` as the sound-effect playback path. |

## Type 2: full-frame row RLE

CODE.2 `27BD8..27C6D` performs this decode once per scanline:

```text
for y = 0..399:
    output = 0
    while output < 320:
        op = u8()
        if op < 0x80:
            count = op + 1
            copy count literal bytes
        else:
            count = 0x101 - op
            value = u8()
            repeat value count times
```

Each row must terminate at exactly 320 decoded bytes. The engine then passes the row through `27A90`, which selects four VGA planes and writes bytes 0,4,8... to plane 0, bytes 1,5,9... to plane 1, and so on. Thus the encoded geometry is **320 pixels wide**, not 640. A decoder that doubles each encoded byte for display is performing presentation scaling rather than reproducing the engine's native pixel geometry.

All 73 supplied type-2 chunks consume their payload exactly and decode to `320 * 400 = 128000` pixels.

## Type 3: sparse planar delta

CODE.2 `27AEC..27B82` defines the payload grammar:

```text
uint16 outer_count
x = 0
repeat outer_count times:
    uint16 delta_x
    uint16 group_count
    x += delta_x
    y = 0
    repeat group_count times:
        uint16 delta_y
        uint16 run_count
        y += delta_y
        uint8 colors[run_count]
        y += run_count
```

For every color byte, CODE.2 selects VGA plane `x & 3`, byte column `x >> 2`, writes at the current y, then advances one scanline. `delta_x` is used directly; it is **not multiplied by two**.

Of the 809 supplied type-3 chunks, 790 are exactly two bytes containing `outer_count = 0`. The 19 substantive type-3 chunks all occur in `CINE12.ANM`; all consume their payloads exactly and remain within the engine's 320x400 bounds.

## Type 8: raw planar viewport geometry

`27B84` selects each VGA plane in turn and copies `0x5000` bytes from the packet into the same destination address. The destination starts at VGA byte offset `0x0FF0`:

```text
0x0FF0 / 80 bytes_per_scanline = 51 scanlines
0x5000 / 80 bytes_per_scanline = 256 scanlines
```

Therefore type 8 represents four planes of a 320x256 image region at framebuffer y=51..306. This packet occurs heavily in `CINE00` through `CINE11` and does not occur in `CINE12`.

## Supplied-set validation

`analyze_anm.py` currently proves for all 13 supplied ANMs:

- complete parse reaches EOF with no trailing or missing bytes;
- parse -> serialize reproduces the original file byte-for-byte;
- all type-2 packets satisfy the executable-defined 320x400 row decoder and consume exactly their payloads;
- all type-3 packets satisfy the executable-defined delta grammar and consume exactly their payloads;
- every type-8 packet is exactly `0x14000` bytes.

This is structural round-trip verification. It does **not** yet certify an editing serializer, because no mutation/re-encoding path has been implemented.
