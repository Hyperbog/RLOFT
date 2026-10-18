# Ravenloft Asset Extractor

**Branch:** `Ravenloft-Asset-Extractor`. Windows WPF/Python extraction and asset-inspection workstream, using immutable original game media supplied by the owner.

**Start here:** [Tools overview](Tools/README.md), [WPF extraction GUI](Tools/WpfExtractor/README.md), [Python/ANM and resource extraction](Tools/anm/README.md), [RES/ANM format research](Resources/README.md) and [DAT formats](DatFiles/README.md).

Original graphics, palettes, audio, sprites and animation frames may be decoded to the owner's chosen local cache/export directory; do not publish extracted copyrighted media in Git or public Actions artifacts. Static format/source analysis does not prove every original asset is decoded correctly.

**Build/release:** the owning GitHub workflows are `.github/workflows/build-wpf-fluent.yml` and `build-windows-extractor.yml` and now reference `Ravenloft-Asset-Extractor`. Check the exact source and artifact SHA when validating a release.

[Project documentation index](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/INDEX.md) · [automated live branch/CI snapshot](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/LIVE_STATUS.md).
