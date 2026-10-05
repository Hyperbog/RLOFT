# CODE.2 Semantic Audit — Checkpoint 59

## Scope

The immutable object-1 tail from `39FED` through the physical end at `3A851` was disassembled directly. The physical tail length is **2,149 bytes**, not 2,165. This exposes a 16-byte discrepancy in the running semantic-inspection counter: the prior 237,533-byte numerator is 16 bytes behind the contiguous frontier `39FED` (decimal 237,549).

- object-1 physical end: `3A852` exclusive
- tail: `39FED..3A851`
- tail length: 2,149 bytes
- tail SHA-256: `f9bf5094a984627c7b0c742c0a87c3b23dfe0b378098efcfd8da7c36a664e47c`

The full tail is now instruction-bounded/direct-inspected, but it is not all source-explicit C++ yet. Therefore it is not silently credited as completed semantic reconstruction.

## Tail structure established

`39FED`, `39FF0`, and `39FF6` are tiny return/address stubs. `39FFC` performs lazy descriptor/device flag publication through `3A0A8`. `3A02F` is a DOS write path with optional seek-to-end, DOS-error normalization through `390D7`, short-write errno publication, and the encoded carry-folded DOS result convention. `3A0A8` performs DOS IOCTL 4400h device probing. `3A0C4` lazily annotates the runtime descriptor table and returns its dword record. `3A11A` publishes a descriptor-table record. `3A130` closes a DOS handle and clears the runtime slot on success. `3A165` returns stack availability relative to `12644`; `3A16E` snapshots SS to `126EC`; `3A176..3A1B2` are stack/selector guard helpers with address-distinct RET 4 paths.

The later tail contains runtime stream-list construction/destruction, exit/cleanup dispatch, DPMI/selector helpers, DOS/descriptor wrappers, temporary-name construction, stream close/flush helpers, and the final wrapper at `3A844..3A851`.

## Embedded non-code island

Within routine `3A7DF`, bytes `3A7F8..3A7FD` are jumped over by `EB 06` immediately after an `INT 3`. They decode as ASCII `WVIDEO` (`57 56 49 44 45 4F`) and are data embedded inside the code span, not executable instructions on the normal path. This six-byte island must remain classified as embedded data rather than translated code.

## Accounting discrepancy

Checkpoint 58/repair accounting reported 237,533 inspected bytes after making `39D4D..39FEC` source-explicit. But `39FED` itself is object offset 237,549. Therefore exactly **16 bytes** before `39FED` are already physically traversed yet absent from the semantic numerator. The final certification must locate and reconcile those 16 bytes rather than masking the difference by crediting the tail wholesale.

If the entire physical tail were credited, the counter would reach 239,682 / 239,698, leaving exactly those 16 bytes. This checkpoint deliberately does not claim 100%.

## Next

Make `39FED..3A851` source-explicit in ownership-complete folds, preserving DOS/DPMI carry state, partial-register behavior, selector state, address-distinct wrappers, and the six-byte `WVIDEO` embedded-data island. In parallel, reconcile the 16-byte historical semantic-counter deficit. Only after both are closed can the semantic audit legitimately reach 239,698 / 239,698.