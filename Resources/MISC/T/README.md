# T

- Bytes: 133,584
- SHA-256: `a3f1e19e40f226c8cf150a43aacd9c378ee35a1294d2c9dd72870bfdf1e16315`
- Source upload: `RES+OTHER.zip`
- Family: MISC / indexed game text

## Current reconstruction status

`T` is now structurally reconstructed as the game-wide offset-indexed text/message archive used by interaction wordcode, LEVELS event text, and ANM cinematic subtitle commands. Records contain a 16-bit payload length followed by bytes XOR-obfuscated with `0xDC`; CODE.2 `1C368` proves the lookup and decode path.

The supplied file contains 1,073 active indexed records. Its active offset table deliberately skips unreferenced byte regions, including stale DOS-era residue, so preservation tooling must retain slack bytes without treating them as text.

See `FORMAT.md` and `analyze_t.py`.
