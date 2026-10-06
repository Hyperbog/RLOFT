# Miscellaneous resource/support files

This family contains supplied support files that do not need their own top-level resource family. All five supplied files now have executable/vendor-backed classifications and dedicated documentation.

| File | Current status | Reconstructed role |
|---|---|---|
| `COLORS` | **structurally complete** | 15-bank VGA 6-bit RGB palette file; CODE.2 partial-range loader and brightness transform mapped |
| `INIT` | **structurally complete** | engine initialization header, RES3 source descriptors, interaction/audio bases, and two RES8 environment-profile panorama tables |
| `SAMPLE.AD` | **format/classification complete** | Miles AIL 2.x YM3812/OPL2 Global Timbre Library (GTL), 233 indexed timbres |
| `SINTAB` | **structurally complete** | renderer trig/projection tables plus 127 scale-specific x86 micro-rasterizer packages |
| `T` | **structurally complete** | 1,073-record XOR-encoded indexed text archive with preserved unreferenced slack |

Each child directory records immutable source size/SHA-256 plus its executable-backed format specification and, where practical, round-trip validator.

Historical field names remain conservative where the executable proves structure/behavior but not original editor terminology. Structural completion does not justify replacing such physical labels with guesses.
