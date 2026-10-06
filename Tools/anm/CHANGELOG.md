# Asset Extraction Suite Changelog

## Version 0.2.0

- Made the Strahd's Possession aesthetic the sole GUI design.
- Removed the theme selector and all runtime theme dependence on RES/COLORS.
- Removed the "Certified source corpus" group caption.
- Added multi-select cinematic extraction: one, several, or all CINE files.
- Added per-RES graphics archive selection while retaining category-wide dump.
- Added per-RES sampled-audio and XMIDI selection while retaining category-wide dump.
- Retained global Dump All.
- Added reconstructed native ANM variable frame timing from the CODE.2 wait path.
- Retained optional custom constant-FPS presentation export.
- Added synchronized sampled-SFX muxing for proven ANM chunk-6/chunk-9 RES6 events.
- Music commands remain XMI/MIDI/timeline data rather than being rendered with an
  arbitrary soundfont.
- Suppressed FFmpeg/helper console windows on Windows using no-console process flags.
- Corrected RES12 intentional zero-count raster handling.
- Added regression coverage for native timing and cinematic SFX mapping.
- Updated Windows packaging/self-test for the baked-only GUI.

## ANM foundation

- Certified ANM parser and structural inspector.
- Type-2 full-frame RLE, type-3 sparse delta, type-8 planar frame reconstruction.
- Palette/reset handling, frame extraction, timeline JSON.
- Synthetic decoder tests for RLE, delta, planar mapping, and palette expansion.

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

## XMIDI / MIDI export

- Added original-XMI, exact-timing Standard MIDI, and combined export choices.
- Implemented FORM/XDIR/CAT/XMID discovery and EVNT conversion.
- Recreates note-off events from Miles XMIDI note durations.
- Uses exact 120 Hz SMF SMPTE timing (-30 fps, 4 ticks/frame).
- Retains AIL controller events and reports FOR/NEXT/branch counts.
- Corpus validation: 172 Ravenloft XMIDI resources, 350 sequences, zero parser
  failures; 249 infinite FOR_LOOP and 245 NEXT_LOOP controls observed.
- Original XMI remains canonical for AIL-specific looping/control-flow behavior.

## Fidelity boundaries

- Type-7 scene shapes are not mislabeled as ordinary bitmap sprites; structural
  extraction is complete while faithful portable rasterization remains a
  separate renderer task.
- ARIA bank inner records remain opaque and are preserved byte-for-byte.
