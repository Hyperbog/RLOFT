# SSI Runtime / Ravenloft module architecture

The production port is a clean source reimplementation. It does **not** execute CODE.1/CODE.2 disassembly.

- `src/engine/`: reusable platform/runtime systems. Must not contain Strahd-specific rules.
- `src/games/ravenloft/`: Ravenloft parsers, simulation, scripting, UI rules, and compatibility behavior.
- `tools/`: developer inspection tools. They may expose original data but must not become runtime authority.
- Original Ravenloft data is loaded at runtime from the user's installation/CD extraction. No original copyrighted data is bundled.
- The decomp archive and original shipped files are the behavioral authority. Viewer patches/guesses are excluded.
- Faithful compatibility behavior must remain separable from modern renderer, bug-fix, QoL, and mod layers.
- Reusable discoveries belong in the common SSI/runtime layer; Strahd-specific behavior stays in the game module so the framework can be reused by the sequel.

The immediate milestone is the complete executable reconstruction ledger followed by systematic CODE.1/CODE.2 C/C++ implementation.
