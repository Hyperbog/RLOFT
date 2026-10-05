# CODE.2 final semantic audit

## Source identity

- Original file: `CODE.2`
- Size: **558,689 bytes (`0x88661`)**
- SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 file start: `0x3AF24`
- Object-1 length: **239,698 bytes (`0x3A852`)**
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`

## Audit method

This is a semantic audit of the already byte-closed CODE.2 reconstruction. Coverage accounting remains **235,022 translated instruction bytes + 4,676 proven non-code/data bytes = 239,698 / 239,698**, with **0 unresolved bytes**. Audit progress is tracked separately and never changes byte ownership unless a prior ownership error is discovered.

Every closed checkpoint is regenerated from the immutable object and checked for call/argument order, stack-slot consumption, signedness, partial-register effects, reloads across calls, overflow/truncation, shared tails, selector/far-pointer state, and address-distinct control flow. The fully audited CODE.1 reconstruction and `Code1/FINAL_BINARY_AUDIT.md` are used as a defect checklist and cross-reference model, never as substitutes for CODE.2 bytes.

## Checkpoint 1 — `00000..002A7`

**680 bytes.** Early application block and 13-entry dispatcher table. One semantic defect corrected: the `1C1DCh(20h) != 0` branch of `000B8..001F9` had an invented trailing `1C124h(3Ch)` call after the four-slot cleanup loop. The binary branches directly to the epilogue and preserves live EAX (`0x920`).

## Checkpoint 2 — `002A8..011A7`

**3,840 bytes.** Thirteen-way command dispatcher, zero island, coordinate/record-search family, four-neighbor classifier, tile draw, fixed-point line helper, map renderer, fixed UI-frame decoration, and separators. PASS with no new correction. High-risk CODE.1 defect classes checked explicitly: live-register inputs, partial BX, signed IDIV/remainder, global reloads after opaque calls, address-distinct searches, and asymmetric alternate-record sources.

## Checkpoint 3 — `011A8..01C4C`

Fresh disassembly was regenerated from the immutable object after the interrupted audit was resumed. Immutable hashes matched exactly. Closed span: **2,725 bytes (`0xAA5`)**. Span SHA-256: `07c14c3cccd8062c5fec56fcb70fa96d7cd88e7b51b959afef5dcb6bb701f867`.

A later cdecl-stack-order review in checkpoint 4 found one repeated semantic defect in this already-closed span: the `30CA2h` result is pushed on top of four still-live arguments before `2975Ch`, so that result is **argument 0** to `2975Ch`, not argument 4. The affected scaled-border calls in `01834..01C07` were corrected during checkpoint 4.

### `011A8..01226` — frame/background setup

The encoded draw setup was checked as a complete routine. It reloads `258DCh` for each draw pass rather than retaining the value across opaque calls; the reconstruction must preserve that CODE.1-audit-sensitive behavior. The three horizontal passes advance EBX by `0x60`, terminate at `0x120`, and use the encoded `+0x3E` source offset. The final `296C0h` rectangle call closes the routine at `01226`; `01227` is a NOP separator.

### `01228..01398` — record/text renderer

The complete variable-length record walk was audited instruction-for-instruction. The six-byte fixed record header is followed by a signed 16-bit length field; a zero length terminates the list. X/Y fields and length are promoted with `MOVSX`, not zero extension. Clipping uses signed comparisons and includes the text-width result from `302CCh`. Character rendering preserves carriage return (`0Dh`) as a line transition: X advances by 12, Y resets to the saved origin, and the per-character width table is not consulted for CR. Ordinary characters call `29804h`, then advance by the unsigned byte width at table offset `+4+character`. The next record starts at `header + 6 + signed_length`. No normalization to an unsigned record format is permitted.

### `01399..01556` — bounded tile-region renderer

`01399..0139B` is alignment; `0139C` is the independent entry. The routine calls `011A8` first, then performs two bounded tile passes. Signed row/column clipping against globals `12754h/12756h`, the five-byte horizontal stride, eight-byte vertical stride, and calls to the address-distinct `00B6C` tile helper were checked. The two passes intentionally use separately derived local origins; they are not a duplicate loop that may be merged.

### `01557..01830` — map-cell/state rebuild family

`01557` is a NOP separator and `01558` is the next entry. The routine copies signed map dimensions from the `25619h/2561Bh` state into `12756h/12754h`, then walks the map using signed bounds. Audit emphasis was on partial-byte state manipulation and the encoded table addressing: row indexing multiplies the signed width, cell state is manipulated bytewise, and the loops retain their original signed termination tests. No CODE.1-style width/signedness drift was found.

### `01831..01C07` — UI/map redraw family

`01831..01833` is LEA alignment and `01834` is the independent entry. Repeated reads of `258DCh` across draw/copy calls were confirmed to be real reloads. The nested drawing loops preserve their distinct source offsets, dimensions, and iteration limits; they are not interchangeable generalized blits. Partial-register and byte-state behavior remains encoded. The routine terminates at `01C07`.

### `01C08..01C4C` — variable-record terminator scans

Two short address-distinct helpers walk the record list rooted at `5760h`, adding `signed_word([record+4]) + 6` until the zero-length terminator. `01C08` returns the terminator pointer; `01C28` additionally subtracts `5760h` and therefore returns the occupied byte count. Their similar loops must remain distinct because their return contracts differ. `01C4C` is the second helper's RET.

The next bytes `01C4D..` begin alignment and a new family and are intentionally not counted in this checkpoint.

## Checkpoint 4 — `01C4D..02D6F`

**4,403 bytes.** Fresh immutable-object disassembly covered the text editor, border redraw, mouse-driven editor loops, packed-record deletion, status-frame query, cell-label generation, text serialization, three-plane map export, numbered resource load, and the `02D70` data-table boundary. Immutable source hashes matched exactly.

One repeated cdecl argument-order defect was found while applying the CODE.1 call-stack checklist. At `018B1..018CB` and the analogous `01F94..0203B` border loops, four arguments for `30CA2h` remain on the stack; after `30CA2h` returns, the scaled result is pushed and `2975Ch` is called without first removing those four slots. Therefore the scaled result is `2975Ch` argument 0, followed by the four pre-existing slots. The reconstruction had represented these calls in the opposite order, placing the scaled result in argument 4. All ten affected calls in `01834` and `01F6C` were corrected. This is one defect class with ten call sites.

The newly closed `01C4D..02D6F` region otherwise passes the semantic audit. Particular checks included signed text-length traversal, CR handling, signed mouse-coordinate bounds, IDIV-based line count, record deletion copy lengths, the 17h status-frame bit predicate, signed `/8` and `/5` label coordinate arithmetic, A..Z / space-A label rollover, CR/LF serialization, overlay precedence in block mode, signed map dimensions, allocation/free paths, numbered filename decimal construction, and stream read ordering. `02D70` begins an address table and is the next independent ownership family.

## CODE.1 cross-reference policy

The fully audited CODE.1 decompilation remains part of the active procedure. Its discovered defect classes determine where CODE.2 receives extra scrutiny: cdecl slot order, live-register call inputs, byte/word aliases, signed/unsigned branch drift, modulo-32-bit arithmetic, encoded loop entry, selector/interrupt preservation, global reloads, address-distinct thunks, self-modifying immediates, and DOS/runtime wrappers. Mechanically similar regions may be audited in larger coherent families, but any opcode/control-flow divergence is resolved from CODE.2 itself.

## Current result

- Audited object bytes: **11,632 / 239,698 = 4.8528%**.
- Semantic corrections found so far: **2 defect classes**.
- New correction in checkpoint 4: **1 repeated cdecl argument-order defect affecting 10 call sites**.
- Current closed audited frontier: **`02D6F`**.
- Next frontier: **`02D70`**.

The semantic-audit percentage is independent of the completed 100% decompilation/byte-accounting percentage.
