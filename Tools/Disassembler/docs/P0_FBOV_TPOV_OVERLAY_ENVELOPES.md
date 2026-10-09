# FBOV/TPOV overlay envelope checkpoint

**Source only; no build/test execution or original GAME.OVR decoding.**

A `FBOV` or `TPOV` signature proves that an overlay container was
encountered; it does **not** establish which internal bytes are code.

`DosOverlayImage.LoadEnvelope` records a source-authenticated read-only
container envelope:
- `TPOV`: signature and opaque remainder. There is no assumed directory,
  relocation table or segment boundary.
- `FBOV`: checks a little-endian declared payload length against a complete
  8-byte or 16-byte envelope. The accepted envelope variant is explicit.
  The 8-byte and 16-byte layouts appear in distinct published overlay
  descriptions; neither is silently interpreted as the other's segment map.
- Inconsistent/truncated headers are rejected without speculative fallback.

`DosOverlayRegionManifest` provides a **manual evidence gateway** while
vendor-specific directory and fixup decoding remains incomplete. Each region
is bounded by original file offsets and a complete source SHA-256/byte length.
It must have a unique ID, an assumed 16-bit segment and offset, a role
(`code-candidate` or `data-candidate`), and an evidence ID. File ranges
must be disjoint, backed, and inside the opaque payload. The entire
region must fit one 16-bit offset space.

`DecodeExplicit` accepts only a code-candidate region, and returns
original-file-backed linear x86-16 *candidates*, never verified functions or
runtime overlay mappings. No overlay directory, jump stub or fixup is
invented. A future format-specific parser may produce independently
validated regions with stronger provenance than analyst-supplied maps.

External research for layout candidates:
- https://github.com/NationalSecurityAgency/ghidra/issues/5543
- https://github.com/gg-sl-oss/binary-comp (tpov-info)

Collection inventories on `DnD-Classics` name original overlay files but
do not contain their bytes. **Real FBOV/TPOV region recovery awaits the
corresponding original overlays and resident DOS executables.** No game
name, engine family or file extension controls parser logic.
