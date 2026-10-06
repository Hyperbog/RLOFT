# INIT

- Bytes: 1,879
- SHA-256: `cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276`
- Source upload: `RES+OTHER.zip`
- Family: MISC

## Current reconstruction status

CODE.2 `16D04..17049` directly reads the leading `0x24` bytes as 18 little-endian 16-bit initialization values. Several of the final words are now tied to the RES interaction/display subsystem:

- `INIT +0x1C = 32` -> sound-effect base global `0x258C8`;
- `INIT +0x1E = 103` -> archive-9 interaction-resource base global `0x258CC`;
- `INIT +0x20 = 48` -> secondary sound/index base global `0x25614`;
- `INIT +0x22 = 414` -> archive-9 wordcode-bank displacement global `0x7FCC`.

The last two archive-9 values prove the raw wordcode bank begins at `103 + 414 = RES9:517`, matching the first non-`EH` program exactly.

See `FORMAT.md` for the partial executable-backed reconstruction. The remainder of the 1,879-byte file is still under analysis and is preserved without speculative field names.
