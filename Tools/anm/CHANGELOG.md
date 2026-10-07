## 0.6.0

- Added semantic multipart RES3 extraction for NPCs/creatures, OBJECTS.DAT items, INIT class-1 scene descriptors, and class-3 transient/spell effects.
- Added Raw + grouped, Raw only, and Grouped only organization modes.
- Added per-group manifests with original RES3 IDs, generic variant/view coordinates, source-table provenance, and shared-resource relationships.
- Added contact-sheet PNGs, animated GIF previews, per-variant previews, and shared-resource alias maps.
- Added certified effect-producer labels for Fireball, Lightning Bolt, Ice Knife, Cone of Cold/Wand of Frost, Flame Arrow, Magic Missile, Burning Hands/Agannazar's Scorcher, Ice Storm/Freezing Sphere, Flame Strike, and Holy Symbol of Ravenkind classes.
- Added regression coverage for INIT/NPCS/OBJECTS multipart plan reconstruction.

## 0.5.6

- Added automatic original-game palette routing as the default Graphics & Textures behavior.
- Retained banks 0..14 as an explicit manual override.
- Added exact RES0 world-mode and modal-screen palette mappings reconstructed from CODE.2.
- Added environment-profile palette routing and per-asset palette provenance/alternative banks to graphics_manifest.json.
- Preserved canonical .indices output independently of preview-palette selection.

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
- Added synchronized sampled-SFX muxing: queued chunk 6 resolves through RES6 and direct chunk 9 through the original RES10 archive-switch path.
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


## GUI fidelity and complete scene graphics

- Restored the earlier original-art Ravenloft banner appearance as a baked,
  resource-independent GUI composition.
- Added delayed hover tooltips across interactive controls.
- Added visible native VFR timing/cadence information to the Cinematics tab.
- Added optional graphics sidecars: palette indices, type-7 metadata/shape
  bodies, transparency masks, and original EH records.
- Reconstructed all 4,234 active RES3 type-7 scene shapes to native-resolution
  transparent imagery, including NPCs/monsters, placed objects, loose items,
  and transient effects.
- Corrected direct ANM type-9 sample routing to RES10 while queued type-6 SFX
  continue to resolve through RES6.
- Added independent diagnostics/regression coverage for sampled-SFX muxing,
  Timeline JSON, and original-ANM preservation.
- Kept FFmpeg/helper processes hidden on Windows so extraction does not steal
  foreground focus.
