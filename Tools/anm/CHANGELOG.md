# ANM Tool Changelog

## Phase 1

- Added certified ANM container parser.
- Added structural `inspect` command.
- Added type 2 full-frame RLE decoder.
- Added type 3 sparse planar delta application.
- Added type 8 raw four-plane frame application.
- Added palette and blackout/reset handling.
- Added PNG frame extraction and JSON timeline export.
- Added focused decoder unit tests.

Validation checkpoint: five synthetic decoder tests pass for literal/repeat RLE, sparse deltas, planar mapping, and VGA palette expansion.

## GUI / export foundation

- Added Tk GUI entry point.
- Added independently selectable frame and video outputs.
- Added frame formats: PNG, BMP, TIFF, JPEG, WebP.
- Added video formats: MP4/H.264, MKV/H.264, lossless MKV/FFV1, MOV/H.264, AVI/MPEG-4, WebM/VP9, GIF.
- Added independent frame-folder and movie-file destination selectors.
- Added configurable video frame rate and optional timeline output.
- Added FFmpeg-backed video encoding service.

## Optional Strahd's Possession theme

- Added runtime RES0/COLORS reader using the certified archive/type-1/LZSS/palette formats.
- Added optional Strahd-themed GUI styling sourced from original Ravenloft UI graphics.
- Theme artwork is decoded from the user's own game installation at runtime; extracted copyrighted assets are not stored in the repository.
- Verified the decoder against the supplied certified RES0/COLORS files and added synthetic resource-decoder tests.

Next validation checkpoint: run the GUI/export path over all 13 supplied ANMs, visually compare selected frames against original playback, and then add subtitle/audio reconstruction before calling rendered movies feature-complete.
