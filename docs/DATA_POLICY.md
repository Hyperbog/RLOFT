# Original-data policy

RLOFT is intended to distribute only independently written source code, build scripts, tests, and reverse-engineering documentation required to reproduce compatible behavior.

## Do not commit

The repository must not contain original Ravenloft game content, including original archives/images, executable binaries, graphics, sound, music, maps, dialogue/text databases, save files, or bulk extracted assets.

Generated screenshots and extracted media are also excluded from the canonical source tree unless they are independently created project assets with clear provenance.

## Runtime model

The source port loads required content from a user-supplied original installation or disc/image extraction. Runtime loaders may understand original formats, but source releases should remain usable only after the user supplies those files.

## Verification material

Regression tests should prefer:

- hashes,
- sizes,
- offsets,
- small structural fixtures,
- deterministic state traces,
- independently generated synthetic fixtures,

rather than embedding copyrighted payloads.

When a test requires the original installation, it should be marked as an external-data/integration test and skipped cleanly when original data is unavailable.
