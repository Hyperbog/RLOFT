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

Validation checkpoint: five synthetic decoder tests pass for literal/repeat RLE, sparse deltas, planar mapping, and VGA palette expansion. Full-corpus visual validation against the 13 supplied ANM files is the next phase.
