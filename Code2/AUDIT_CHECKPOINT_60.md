# CODE.2 Semantic Audit — Checkpoint 60

## Scope

Immutable object-1 was directly disassembled from `39FED` through physical end `3A851`. This checkpoint establishes instruction boundaries and semantic ownership for the entire 2,149-byte physical tail, while deliberately withholding final 100% semantic-credit accounting until the previously identified 16-byte counter deficit is reconciled and explicit C++ tail reconstruction is staged.

Tail SHA-256: `f9bf5094a984627c7b0c742c0a87c3b23dfe0b378098efcfd8da7c36a664e47c`.

Ownership folds and hashes:

- `39FED..3A1B2` — 454 bytes — `d3bbcf4186008d90d240af1d27b43e91089f5abeb9b91dffda9c579a6fe2effc`
- `3A1B3..3A2AC` — 250 bytes — `295974dbdf1b3a0994b765cbe78bf2269747ef5a65b7931a0cd5b5eafdc8b6e8`
- `3A2AD..3A394` — 232 bytes — `2eaa689959601ec1f7aebd7b620d9d0a4f39aae130c7c0ab05e7b62d767e07c3`
- `3A395..3A528` — 404 bytes — `5d7184ac6e511ae9c46f8a2b8891d5016fc8906f3183c1cb6169d03fd071a735`
- `3A529..3A65B` — 307 bytes — `d27f93b5c897b8efd252ba55ba7754826c69d529eef56a59fd2112dcdc9225aa`
- `3A65C..3A7DE` — 387 bytes — `424a0da996ece3f86763b831e353e3275a736842228f44a81f59cf35d26803b2`
- `3A7DF..3A851` — 115 bytes — `eb38ee9fec60ae4867cb0343df2d9635204fda929d5fae106d5c6c1e754be629`

## Semantic findings

### `39FED..3A1B2`

Contains zero-return and errno-address stubs, lazy descriptor/device annotation, DOS write/append handling, IOCTL device probing, descriptor-table publication/close, stack availability and SS snapshot helpers, and address-distinct stack/selector guard wrappers.

The DOS seek/write/close paths encode failure by folding carry into the sign bit using `RCL/ROR`; reconstruction must preserve that error-state convention before passing the low DOS error word to `390D7` or publishing errno. `3A0A8` performs IOCTL `4400h` and derives its boolean from the encoded device bit after the carry-fold sequence; this is the same routine whose predicate polarity matters in the earlier `37EFA` defect.

`3A176` and `3A186` remain address-distinct `RET 4` paths. `3A189` compares the candidate stack address against ESP and the saved stack boundary, then compares SS against the snapshot at `126EC`; successful guard failure transfers to `3A536` rather than being representable as a normal local boolean.

### `3A1B3..3A2AC`

Initializes the stream/runtime list over the static 1Ah-byte records beginning at `F6FC`, allocating 8-byte link nodes as needed. `3A242` walks the list and conditionally closes streams through `3A689`; the lower bound is selected by a 26-byte stride (`argument * 26`), and static records are treated differently from externally allocated stream records.

### `3A2AD..3A394`

Builds a conventional argv/environment pointer vector from a far/selector-backed NUL-separated string block when global `12688` is zero. It counts strings, allocates a copied text block and pointer vector, publishes `12688` and `1268C`, terminates the vector with a null pointer, and calls `30D30` for final initialization. Allocation failure frees the first allocation. `3A38A` initializes global minimum arena size `1270C` to `8000h`.

### `3A395..3A528`

`3A395` swaps global `12708` and returns the old value. `3A3A5` is a backing-memory acquisition/ceiling helper with mode-sensitive DPMI/DOS paths; it preserves CF-derived success for both INT 31h and INT 21h, page rounding, DS-limit refresh, errno-address publication through `39FF0`, and fallback through `3A467`.

`3A467` is the lower-level memory-ceiling adjuster. It has distinct DOS-mode paragraph arithmetic and extender-mode page arithmetic, preserves DOS resize error folding, publishes errno 5 on failure, returns `FFFFFFFFh` on failure, and on success swaps `12634` with the requested boundary while returning the previous ceiling.

### `3A529..3A65B`

`3A529` is an address-distinct tail wrapper to `35632`. `3A536` first calls `3A7DF` with DS selector and caller data; if that gate returns zero it forwards to `3A529`. `3A564` allocates/reuses a stream record plus link node, preserving the static-record scan and dynamic 22h-byte fallback. `3A607` removes a stream link and moves it to free-list `2B74C`; `3A63E` drains/free-releases that free list.

### `3A65C..3A7DE`

`3A65C` locates a stream record in list `2BCF0` and closes it through `3A689`, returning `FFFFFFFFh` if absent. `3A689` calls the close implementation `3A738` and then unlinks the record through `3A607` while preserving the close return.

`3A6AF` converts a nibble to the runtime's digit/letter character mapping. `3A6BF` constructs the 13-byte temporary name `tXXXX_YY.tmp` from `3A80A()` and the supplied byte. `3A738` is the substantive stream close path: optional pre-close handling through `37D60`, position calculation through `3A810`, metadata/update call `38407`, optional DOS close, buffer release, and temporary-file cleanup path through `3A6BF`/`3A844`.

### `3A7DF..3A851`

`3A7DF` is gated by byte `12710`. On the enabled path it pushes the caller dword and a zero-extended selector word, executes INT3, and jumps over six embedded bytes `57 56 49 44 45 4F` = ASCII `WVIDEO`; it then returns one. Disabled path returns zero. The `WVIDEO` island at `3A7F8..3A7FD` is therefore proven embedded data, not instructions.

`3A80A` returns global `12640`. `3A810` computes a stream's logical position from `3967C` seek-to-end plus/minus buffered count depending on flag bit `10h`. `3A844` is an address-distinct wrapper around `38452`.

## Defect/repair implications

No new localized semantic defect is added beyond the already-known carry/flag-boundary class. The tail strengthens the requirement that DOS/DPMI interrupt boundaries expose carry/error state separately rather than treating zero/nonzero EAX as success. Address-distinct wrappers and the `WVIDEO` embedded-data island must be retained.

## Accounting blocker

Physical object offset `39FED` is decimal 237,549, whereas the source-explicit semantic numerator before the tail is 237,533. A 16-byte historical semantic-counter deficit therefore exists before this tail. The tail itself is 2,149 bytes, so crediting it against the current numerator would produce 239,682, not 239,698. The deficit must be located/reconciled before declaring 100% semantic inspection.

## Next

1. Stage explicit C++ reconstruction for the seven final-tail folds above, preserving embedded `WVIDEO` as data and all address-distinct wrappers.
2. Reconcile the 16-byte pre-`39FED` semantic-counter deficit against the historical checkpoint ledger.
3. Only after both are complete declare semantic coverage 239,698 / 239,698 and begin the accumulated repair/integration/compile gate.