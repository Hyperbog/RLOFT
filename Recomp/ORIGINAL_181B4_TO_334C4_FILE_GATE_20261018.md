# Original CODE.2 181B4 file-backed level load → 334C4 guest view preparation

**Owning branch:** `recomp`. **Code evidence:** `d4826defe84c263b7219815fafaa534e1e2593cc`. **Windows validation:** [run #39099508176](https://github.com/Hyperbog/RLOFT/actions/runs/39099508176), **72/72 CTests** plus a read-only original media gate covering all 32 levels.

## Verified executed source

`Recomp/src/code2_original_level_loader.cpp` includes canonical `Code2/Code2.cpp` and calls compiled `function_181b4` with the real `LEVELS.DAT` archive, rather than writing a host-decoded grid to guest memory. The fallback from missing `LEVnnn.DAT` to archive-level directory indexing occurs in original source; host `ReadOnlyDosFiles` provides only allowlisted DOS-style open, seek, read and close. The original source then reads the 0x2A4-byte header, row-by-row 16-bit grid, compact variable families, event-script block, automap and annotation tail; the test does **not** independently certify correctness of all reconstructed post-load actor/event semantics.

For all 32 original archived levels (115,584 packed cell words), the gate independently compares the resulting original guest `0x25618` header and `0x198E0` cell array with the separately validated read-only archive parser. The header's `0x29A` word is checked against the **actual original post-load rule**, which changes an initial zero scale word to one at guest `0x258B2`. Both source and expected bytes are checked, not silently normalized.

Without reloading the grid, the same guest machine executes compiled original `function_334c4` and verifies cell-count setup and clearing of only temporary `0x0040` visibility marks. Camera/trigonometric values remain explicitly synthetic, and the callback stops before `32954`. This run neither executes rays nor emits a world frame.

**Authentic file SHA-256:** `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`. [Engineering executable artifact](https://github.com/Hyperbog/RLOFT/actions/runs/39099508176/artifacts/12126151714) and [JUnit/test evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39099508176/artifacts/12125549535) contain no original media.

## Scope limits and next work

- The original standalone `LEVnnn.DAT` saved-level branch is **not** exercised. This gate intentionally forces the authentic archive fallback.
- Original actor state, NPC descriptor information, scripts and higher-level character/game initialization are not supplied by a complete CODE.1 → CODE.2 boot. Passing these checks does not prove full world-state restoration.
- The exact `32954` camera, orientation, SINTAB inputs, ray/depth record generation and its link to compiled `335DC → 3263C → 338C8` remain next.
- The continuous original CODE.1→CODE.2 handoff and R04 memory/selector acceptance remain open. The enhanced playable game is separately owned by `demo-v1`.

To reproduce with your own original assets:

```powershell
.\rloft_original_code2_level_loader.exe "C:\RLOFT\LEVELS.DAT"
```
