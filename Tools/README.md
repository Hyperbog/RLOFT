# Ravenloft Asset Extractor — tools

**Owning branch:** `Ravenloft-Asset-Extractor`. Active extraction applications:

- [WPF Fluent Windows extractor](WpfExtractor/README.md) — user-facing graphical application and local build wrapper.
- [Original ANM / resource extraction](anm/README.md) — Python asset decoding, soundtrack and graphics/media export tools. The historical `anm/` directory name does not limit its resource coverage.

All original files are read-only and are never bundled in public CI or release media. Decoder/regression outcomes are specific to tested source revisions; unknown source-format semantics remain explicit. The asset extractor is a separate tool, not the playable game.

[Original binary and palette research](../Resources/README.md) · [DAT research](../DatFiles/README.md) · [optional read-only REA static analysis](REA_ANALYSIS.md). Canonical original code remains on [`code1-decompilation`](https://github.com/Hyperbog/RLOFT/tree/code1-decompilation), with source recompilation on [`recomp`](https://github.com/Hyperbog/RLOFT/tree/recomp).
