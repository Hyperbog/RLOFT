# Ravenloft ANM Tool

Initial scope: inspect the certified ANM container and extract reconstructed video frames from the original cinematic stream.

Source specification: `Resources/ANM/FORMAT.md`.

## Commands

```bash
python rloft_anm.py inspect path/to/CINE00.ANM
python rloft_anm.py extract path/to/CINE00.ANM output_dir
```

`inspect` prints stream metadata and a per-frame chunk summary.

`extract` reconstructs the 320x400 indexed framebuffer, applies palette/reset/full-frame/delta/raw-planar image commands, and writes PNG frames. Pillow is required only for PNG output.

This first implementation intentionally does not synthesize music/SFX or encode a video. It records non-image commands in `timeline.json` so later phases can add subtitle/audio reconstruction without changing the image decoder.

### Requirements

- Python 3.10+
- Pillow for `extract`: `python -m pip install Pillow`

### Fidelity notes

- Type 2 is decoded as 320x400 row RLE exactly as reconstructed from CODE.2.
- Type 3 applies sparse planar updates directly to the current framebuffer.
- Type 8 writes the 320x256 four-plane region at framebuffer scanline 51.
- Type 0 updates the active 256-entry VGA palette.
- Type 1 clears the framebuffer and palette.
- Chunk type 4 is preserved in the timeline but rejected if encountered during extraction because no supplied ANM uses it and its higher-level semantics remain deliberately conservative.
