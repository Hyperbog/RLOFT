# Asset Extraction Suite Changelog

## ANM foundation

- Certified ANM parser and structural inspector.
- Type-2 full-frame RLE, type-3 sparse delta, type-8 planar frame reconstruction.
- Palette/reset handling, PNG frame extraction, timeline JSON.
- Synthetic decoder tests for RLE, delta, planar mapping, and palette expansion.

## GUI and modern export

- Responsive Tk GUI.
- Independent cinematic frame and movie destinations.
- PNG/BMP/TIFF/JPEG/WebP frame outputs.
- MP4/MKV/MOV/AVI/WebM/GIF video outputs.
- Lossless FFV1/MKV option.
- FFmpeg-backed video pipeline.
- Optional Strahd's Possession GUI theme using original RES0/COLORS artwork.

## Master media extraction

- Added certified 33-file source inventory with size/SHA-256 validation.
- Added RES type-1 graphics, type-2 fonts, type-6 textures, and type-7 shape dumping.
- Preserves native EH records and canonical decoded palette-index bytes.
- Added Creative Voice File extraction and lossless WAV/FLAC conversion.
- Added original XMIDI/XDIR extraction.
- Added COLORS palette export.
- Added all 1,073 T text/subtitle records.
- Added all 233 SAMPLE.AD OPL2 timbres.
- Added INIT mapping metadata.
- Added SINTAB scale-block extraction.
- Added exact GM1.BNK/GM2.BNK ARIA preservation and metadata.
- Added complete Dump All orchestration and manifests.

## Fidelity boundaries

- Type-7 scene shapes are not mislabeled as ordinary bitmap sprites; structural
  extraction is complete while faithful portable rasterization remains a
  separate renderer task.
- XMIDI-to-SMF conversion is not enabled until an authoritative Miles timing/
  event translation is implemented.
- ARIA bank inner records remain opaque and are preserved byte-for-byte.

## XMIDI / MIDI export

- Added original-XMI, exact-timing Standard MIDI, and combined export choices.
- Implemented FORM/XDIR/CAT/XMID discovery and EVNT conversion.
- Recreates note-off events from Miles XMIDI note durations.
- Uses exact 120 Hz SMF SMPTE timing (-30 fps, 4 ticks/frame).
- Retains AIL controller events and reports FOR/NEXT/branch counts.
- Corpus validation: 172 Ravenloft XMIDI resources, 350 sequences, zero parser
  failures; 249 infinite FOR_LOOP and 245 NEXT_LOOP controls observed.
- Original XMI remains canonical for AIL-specific looping/control-flow behavior.
