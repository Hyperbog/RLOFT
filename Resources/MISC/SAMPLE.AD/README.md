# SAMPLE.AD

- Bytes: 4,662
- SHA-256: `79ee9f4ad3134be906ce493072ce0c3b7169c9013d3c25947ac385aaf3f9633b`
- Source upload: `ANM+OTHER.zip`
- Family: MISC / Miles AIL audio support

## Classification

`SAMPLE.AD` is **confirmed** as a Miles Design Audio Interface Library 2.x **YM3812/OPL2 Global Timbre Library (GTL)** used by the XMIDI/FM music-driver path.

The supplied file contains 233 instrument definitions addressed by MIDI bank/patch pairs. Its six-byte GTL directory, `FF FF` terminator, absolute timbre offsets, and 14-byte OPL2 timbre records validate exactly against the original Miles `GLIB.C` format.

See:

- `FORMAT.md`
- `analyze_sample_ad.py`
