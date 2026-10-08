# Ravenloft Tools

Modern preservation, inspection, extraction, and remastering utilities built
from the certified reverse-engineering work in this repository.

## Current tools

- `original_disassembly_cache.py` — hash-verified, local-only CODE.1/CODE.2 executable disassembly and address index generation; see [usage and fidelity rules](DISASSEMBLY_CACHE.md). Generated original-file text is gitignored and never becomes a substitute for original binary evidence.
- `anm/` — **Ravenloft Asset Extraction Suite**. The directory name is a
  historical artifact from its ANM-only first phase; it now extracts the full
  graphics/texture/font/shape, sampled-audio, music/timbre, cinematic,
  palette/text, and renderer-support corpus.

All tools treat shipped Ravenloft bytes as immutable source evidence. Decoders
follow the certified `Resources/` documentation, preserve canonical/native
bytes alongside modern derivatives where useful, and keep unresolved semantics
explicit rather than inventing them.
