# Original game research — supporting Ravenloft Enhanced

All original-source and asset-format research is retained to improve the
**single** native player in [Enhanced/](../Enhanced/README.md).

- [CODE.1 reconstructed source](../Code1/Code1.cpp) and
  [CODE.2 reconstructed source](../Code2/Code2.cpp): verified reference
  routines and human-readable original control flow.
- [SharedDecomp/](../SharedDecomp/) and [ResearchInfra/](../ResearchInfra/):
  source tests, original-byte analysis and source parity checks.
- [DatFiles/](../DatFiles/), [Resources/](../Resources/) and
  [Runtime/](../Runtime/): original data-format and runtime notes.
- [Recomp/](../Recomp/README.md): independently compiled C++ host adapters,
  original guest source tests, DOS/4GW and hardware execution diagnostics.
- [MOD_FORWARD_METHODOLOGY.md](../MOD_FORWARD_METHODOLOGY.md): reusable
  game-modification policy.

**Canonical game code:** `Enhanced/`. Research code does not create a second
player application. Verified parsers, algorithms, data contracts and behavior
should be extracted into the game with tests and player-visible validation.

The `Decomp` branch is preserved as historical research. Its original
[research documentation](https://github.com/Hyperbog/RLOFT/blob/Decomp/Decomp/README.md)
remains available, but it is not a separate active development product.
The `Recomp` branch is likewise preserved as the last independently
validated reconstructed-source baseline.

The preserved original game files in
[`DnD-Classics/MASTER`](https://github.com/Hyperbog/RLOFT/tree/DnD-Classics/MASTER)
are not bundled with source or native binaries. Disassembler tooling is out
of scope for Ravenloft Enhanced.
