# DreamForge resource bootstrap: cross-game original-byte reconstruction (Phase 4)

## What is implemented

The source-backed C++17 implementation at `include/rloft/original_resource_startup.hpp` and
`src/original_resource_startup.cpp` extends the Phase 3 application-entry lift:

- **Menzo CODE.1 `0x1044..0x10CB`**: first resource-prefix setup and title configuration
  calls, further initialization call order, creation of the `DGROUP + 0xE90` 256-byte
  identity lookup table, its subsequent single-byte mutation at `0xEA7` to `1`, and
  the following `0x0F48` call. This deliberately stops before the next instruction.
- **Stone CODE.1 `0x680C..0x68B5`**: resource-prefix and title calls, subsequent
  setup, and original 256-byte identity-table writes at `DGROUP + 0x68B8`.
- **Stone CODE.1 `0x6134..0x618A`**: zeroes DGROUP `0x8BC`, makes three
  source-observed early calls, computes byte `DGROUP[0x89FE] = DGROUP[0x8750] XOR 1`,
  then calls `0x7478`. Stops before the original menu/event-loop dispatch.
- **Menzo CODE.2 `0x23934..0x23976`**: calls its original resource prefix entry,
  reads a field at offset `0x72` from an original state object, conditionally
  calls `0x237EC`, and calls the title setup function `0x35544`.
- **Stone CODE.2 `0x2D45C..0x2D4B2`**: corresponding prefix/title calls,
  conditional `0x2D310`, and `CHARS` existence/open call `0x43338` whose return
  determines which original successor basic block to enter.

All original callees beyond the translated instructions are preserved as **explicit
callback interfaces**. This does *not* pretend a generic callback implements an
untranslated renderer, file I/O service, resource catalog, or original DOS ABI.

The generic `inspect_original_res0` preflight checks the original 32-bit count,
monotone entry offsets and all-`EH` first bytes in the three source variants.
Actual resource extraction and decoding remains owned by the already-existing
`ResArchive` and resource-specific decoders in `resource_assets.cpp`.

## Original resource and source authentication

| Game | Original RES0 bytes | Index entries | Source SHA-256 |
|---|---:|---:|---|
| Strahd | 2,186,991 | 141 | `5e30584947c9627e31b5dd2573ad75c2d3cc9fe796ac669e0ca5d02d6db7e61d` |
| Menzo | 580,712 | 175 | `7ffa8cc0544f15a27b9a935c2164e54e5600c7366882d99321956253bc7f6aba` |
| Stone | 497,722 | 174 | `f6529845c0aab9e26bb951bfe8f2619d661b8cd0e35f8c788e5314cd220bb306` |

Strahd's `RES0` was read directly from the owner-provided original archive.
Menzo's and Stone's `RES0` and their two CODE modules were fetched from
`DnD-Classics` and matched against the original `SOURCE_INVENTORY.csv`
SHA-256 digests before targeted opcode/call/embedded-string checks.
The full commercial files are not included in the C++ source package.

## Notable code differences

- Menzo CODE.1 constructs bytes `0..255` at DGROUP `0xE90..0xF8F`, **then**
  changes byte `0xEA7` from `0x17` to `0x01`. The final state is *not* a pure
  identity table. Stone initializes an identity table at `0x68B8..0x69B7`.
- The two CODE.2 implementations use different resource-prefix/title/state
  offsets and different helper functions; the shared interface does not erase
  these ABI differences.
- Stone checks `CHARS` during its CODE.2 initial-resource function, while the
  Menzo function's covered prefix does not check `CHARS`.

## Validation and limitations

Standalone C++17 tests compile with GCC and Clang (`-Werror`) and cover
ordered callback arguments, exact 256-element writes, both state-field branches,
Stone's early CHARS branches, bad RES0 indexes, and exception handling.
An optional original-media command runs the pinned opcode/string witnesses:

```
original_resource_startup_tests menzo /path/to/Menzo/CODE.1 /path/to/Menzo/CODE.2 /path/to/Menzo/RES0
original_resource_startup_tests stone-prophet /path/to/PROPHET/CODE.1 /path/to/PROPHET/CODE.2 /path/to/PROPHET/RES0
```

Full Windows native-game compilation, original-DOS differential runtime traces,
and gameplay resource initialization remain **not verified**. The new phase
adds source-backed native interfaces only; neither Menzo nor Stone gameplay
profiles are enabled, and no original asset is modified.

## Next objective

Reconstruct CODE.2 application main's post-environment DGROUP initialization
and connect its first original resource-init callee (`0x23934` / `0x2D45C`)
to the reusable loader without bypassing per-title file ownership. The remaining
CODE.1 menus and runtime handlers must be converted separately.
