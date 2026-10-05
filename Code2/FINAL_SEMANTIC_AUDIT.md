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

## Checkpoint 5 — `02D70..03633`

**2,244 bytes.** Fresh disassembly from the immutable object closed the 16-entry CS dispatch table, its `02DB0..034AE` control-panel dispatcher, the `034AF` separator, the `034B0..03630` navigation/redraw helper, and the three-byte `03631..03633` alignment. Span SHA-256: `34d1aab387aaafb288749013481c975620f3d0ed2819a3cdb03d04b72a3ca373`. The mandatory overlap was regenerated across the prior frontier; `[0x02D30,0x02DF0)` hashes to `11b1b378f281c2685f0fad8f9046c8c7af5a83cfe35d2f3f8641c9fa2280171`.

PASS with no new semantic correction. The table entries were decoded directly as `2E59` x4, `2E82`, `2F5E`, `303A`, `3125`, `3210`, `32B4`, `337A`, `343D`, and `2E6C` x4. The dispatcher preserves inclusive rectangle bounds, the entry-12 sentinel behavior, the flag-2 remap of hits 0..3 to actions 12..15, and the encoded action-bit shifts. The modal actions retain their distinct `298F0`/`2975C` image offsets and copy dimensions, and the level-navigation paths keep signed clamps and the original page-flip/redraw ordering.

The `034B0` helper was checked specifically for first-set-bit priority, signed `-40` / `-4` bounds, the asymmetric dimension-derived limits (`height-0x2B`, `width-0x2C`, `height-0x2F`, `width-0x30`), 32-bit wrapping arithmetic on the stored offsets, and reloads of `12740h/12744h` before the redraw call. No CODE.1 defect class reproduced here. `03634` is the next independent routine.

## Checkpoint 6 — `03634..0518B`

**7,000 bytes.** Expanded audit across two original reconstruction batches. Fresh immutable-object disassembly covered map-mode setup/resource loading/nibble-grid logic (`03634..0461B`) and the complete four-party simulation/update loop plus classifier (`0461C..0518B`). Span SHA-256: `8fb9e029a196bcfae47729e08ad90d08bfc26cd8671f5b7f006d74cf65abe9ae`. Mandatory overlap `[0x035F4,0x036F4)` SHA-256: `c0a5b070819dc0380fc634dc1f8445dd684f6e699958fad502ff9c1fa7962645`.

Two new semantic defect classes were corrected. First, `043F4` had normalized `050DC` classifier indices into booleans. The binary compares the current row directly against each returned index, so the `+6A` grid is processed when `row != classifier(mode=1)` and `+BA` when `row != classifier(mode=0)`; `FFFFFFFFh` naturally excludes no row. Second, `044D0` incorrectly decremented `remaining` when a slot was already complete on entry. Binary equality at `4546..4554` jumps directly to the next slot; the decrement at `45F7..45FB` occurs only after the transfer-loop path reaches equality.

The remainder passed the high-risk audit. Signed map offsets/dimensions, variable resource-table construction, nibble-width truncation, full 16.16 arguments to `2C8E4`, level/proximity predicates, ordered movement attempts, and redraw flags were retained. The large `0461C` loop preserves its three distinct cleanup paths, signed tick/deadline comparisons, 16-bit current/target fields, and repeated classifier calls. `050DC` was re-disassembled through its RET at `0518B` and returns exact indices 0/1/2 or `FFFFFFFFh`. `0518C` is the next independent routine.

## CODE.1 cross-reference policy

The fully audited CODE.1 decompilation remains part of the active procedure. Its discovered defect classes determine where CODE.2 receives extra scrutiny: cdecl slot order, live-register call inputs, byte/word aliases, signed/unsigned branch drift, modulo-32-bit arithmetic, encoded loop entry, selector/interrupt preservation, global reloads, address-distinct thunks, self-modifying immediates, and DOS/runtime wrappers. Mechanically similar regions may be audited in larger coherent families, but any opcode/control-flow divergence is resolved from CODE.2 itself.

## Current result

- Audited object bytes: **20,876 / 239,698 = 8.7093%**.
- Semantic corrections found so far: **4 defect classes**.
- New corrections in checkpoint 6: **2 defect classes**.
- Current closed audited frontier: **`0518B`**.
- Next frontier: **`0518C`**.

The semantic-audit percentage is independent of the completed 100% decompilation/byte-accounting percentage.
