# External format / integration authorities

The master extractor uses the Ravenloft repository's certified reverse engineering
as the controlling source for Ravenloft-native formats, and official
format/project documentation for modern interchange formats.

## Ravenloft-native sources

Controlling project specifications:

- `Resources/ANM/FORMAT.md` — ANM cinematics.
- `Resources/RES/FORMAT.md` — RES container and EH types 1/2/3/4/6/7.
- `Resources/RES/TYPE6_RENDERER.md` — column-major first-person textures.
- `Resources/RES/TYPE7_RENDERER.md` — compiled scene shapes.
- `Resources/MISC/COLORS/FORMAT.md` — 15-bank VGA palettes.
- `Resources/MISC/T/FORMAT.md` — text/subtitle archive.
- `Resources/MISC/INIT/FORMAT.md` — resource namespace mappings.
- `Resources/MISC/SINTAB/FORMAT.md` — renderer tables/microcode.
- `Resources/MISC/SAMPLE.AD/FORMAT.md` — Miles AIL OPL2 GTL.
- `Resources/BNK/ARIA_BANKS.md` — GM1/GM2 ARIA identification and current boundary.
- `Resources/FINAL_RESOURCE_AUDIT.md` — certified 33-file source corpus.

Original shipped bytes remain primary evidence. Modern export code must not
normalize unknown/reserved source data.

## Images

Pillow project documentation is the implementation reference for image writers:

- https://pillow.readthedocs.io/en/stable/handbook/image-file-formats.html

Lossless asset-dump choices are PNG, TIFF, BMP, and lossless WebP. PNG is the
default for indexed Ravenloft art because Pillow supports `P` mode and a
256-entry palette directly. Canonical decoded palette-index bytes are retained
beside viewable images, so a conversion never becomes the sole preserved copy.

## Audio and video

FFmpeg project documentation is the integration reference:

- https://ffmpeg.org/ffmpeg-formats.html
- https://ffmpeg.org/ffmpeg-codecs.html

Sampled-audio defaults:

- original Creative Voice File (`.voc`) retained when requested;
- FLAC for compressed lossless interchange;
- WAV/PCM for uncompressed lossless interchange.

Cinematic preservation defaults:

- PNG for frame sequences;
- Matroska + FFV1 for a lossless rendered-video derivative;
- the original ANM plus JSON timeline remains canonical because rendered
  constant-FPS video does not replace Ravenloft's original timing/event model.

FLAC's format authority is Xiph / RFC 9639:

- https://www.xiph.org/flac/format.html
- https://www.rfc-editor.org/rfc/rfc9639

## MIDI

The MIDI Association is the authority for Standard MIDI File output:

- https://midi.org/standard-midi-files
- https://midi.org/standard-midi-files-specification

Ravenloft's stored music resources are Miles XMIDI/XDIR. XMI/XDIR is always
preserved exactly.

Standard MIDI derivatives are implemented from the original Miles Design
`MIDIFORM.C` writer (John Miles, 1991/1992) and `XMIDI.ASM` runtime behavior,
using The MIDI Association's Standard MIDI File rules for the output container.
Miles' default XMIDI quantum is 120 Hz. The extractor encodes SMF timing with
SMPTE division -30 fps and 4 ticks/frame, which is exactly 120 ticks/second and
therefore avoids tempo-dependent rounding.

Miles XMIDI FOR/NEXT controllers are retained as MIDI CC events. They remain
AIL-specific control-flow semantics; Ravenloft uses infinite loops, so the
original XMI is the canonical artifact for looping playback. The SMF output is
an editable one-pass derivative for mapping/remixing rather than a replacement
for the XMI program.

Original Miles source release mirror used to inspect the vendor-authored files:
- https://github.com/Tronix286/AIL2/blob/master/MIDIFORM.C
- https://github.com/Tronix286/AIL2/blob/master/XMIDI.ASM

## Miles / ARIA historical middleware

`SAMPLE.AD` is reconstructed from shipped bytes and original Miles AIL
structure evidence already recorded by the repository.

`GM1.BNK` / `GM2.BNK` are confirmed Sierra Semiconductor ARIA General MIDI
banks, but their inner grammar remains outside the certified reconstruction.
They are preserved byte-for-byte and are not passed through an unrelated
AdLib-BNK parser.
