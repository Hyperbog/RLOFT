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

## Checkpoint 7 — `0518C..06CAC`

**6,945 bytes.** The audit again used an expanded ownership-complete span, covering all of reconstruction batches 08 and 09: party/status UI buffer construction and interaction (`0518C..06007`), settings-panel drawing/interaction (`06008..066E5`), the CS jump table at `06520..06547`, save serialization (`066E8..06C66`), and the tile/record flag probe (`06C68..06CAC`). Fresh immutable-object span SHA-256: `21c33bde2993255fcbe994cef5122ec62fa2a859d6a318a9228ef04ed9a50dae`. Mandatory overlap `[0x0514C,0x0524C)` SHA-256: `4952dfc173872e6180dd638d23b25dd17ea26ee0576b0b03b44de6f52c17783d`.

**PASS with no new semantic correction.** The audit specifically rechecked the CODE.1 high-risk classes rather than relying on the reconstruction structure. `0518C` preserves the five-entry party display (four records plus the synthetic fifth entry), exact classifier-index gating, six row resource buffers, signed byte quantities, nibble extraction, palette remap bytes, and the unusual dword read at `C050h` passed to `1A5B0h`. `05698` retains signed mouse hit tests and signed `IDIV 12`, uses the selected output-buffer pair rather than the source grid for the mutable quantity, and writes only the low nibble back to the grid. The `05AA4` interaction loop keeps the encoded signed coordinate divisions and classifier validity test.

The settings family was checked through its complete dispatch. The `06520..06547` bytes are a ten-entry CS jump table, not executable fall-through; `06548` uses signed remainder rejection followed by the unsigned jump-table range test, which also rejects negative quotients. Toggle side effects (`28098`, `290A8`, conditional `28E58`) and page-flip ordering match the binary. The `06444` post-loop scaling retains signed `258B2h`, byte `C02Ah`, the compiler's signed divide-by-128 rounding sequence, and byte truncation into `C02Dh`.

The full `066E8` serializer was audited through every short-write failure tail. The 50-byte slot label, fixed state blocks, runtime-state local dword, 0x920-byte party block, record-count-scaled `0x248EC` block, 62 descriptor payloads, and final `F920/F91C/67EC/A808/6FA0` blocks all preserve encoded order and exact lengths. Each failed write closes the handle and returns 1; the success path closes it and returns 0. The special `slot == FFFFFFFFh` path intentionally copies only five bytes from `03A8h` into the 50-byte output buffer before writing the full 50 bytes; this odd partial overwrite is preserved rather than normalized. `06C68` retains signed coordinate loads and the exact type/flag/nonzero predicate before setting `CF6Ch`. `06CAD` is the next shared load tail and is not counted here.

## Checkpoint 8 — `06CAD..080AB`

**5,119 bytes.** Fresh immutable-object disassembly closed the backward-entered save-load tail, resource-panel construction/destruction, randomized background renderer, FCB delete wrapper, wildcard file copier, slot-label editor, save/load slot workflows, randomized confirmation dialog, modal message helper, and the `0808C..080AB` CS dispatch table. Span SHA-256: `983c5eb95aad3e3f2eb30fa7f9aa876f0f79df7e787a54b499dd38f700a5bc6e`. Mandatory overlap `[0x06C6D,0x06D2D)` SHA-256: `88ee2bc0bca3930a22167507f4e76441ea240cd18ed9e755e59a05a0d09ed475`.

Two correction groups were integrated. First, the randomized background families at `06008`, `071B8`, and `07D44` had repeated the previously identified cdecl live-stack defect: four draw arguments are pushed before `30CA2h`, remain live across that call, and then the scaled random result is pushed as argument 0 to `2975Ch`. The reconstruction called `30CA2h` with no arguments. This pass restores the four live arguments. The same review exposed a separate global-reload/source defect: `258DCh+35h` is reloaded before each randomized draw. `06008` and `07D44` had cached it, while `071B8` incorrectly derived the background image from its incoming resource argument. All three now preserve the encoded reload/source behavior. The `06008` portion is a retroactive correction to checkpoint 7; byte coverage is unchanged.

Second, `075CC` had the first-edit clear on the wrong side of the character-class test. On the printable-key path, after length and width checks, binary `76FE..7711` clears the inherited 50h-byte label whenever first-edit is still set, and only then classifies the key at `7713..772A`. Thus an invalid first key that reaches this path still clears the inherited label before first-edit is consumed. The reconstruction cleared only after a key passed classification. The clear was moved to its encoded position.

The remaining region passed the semantic audit. `06CAD` preserves the backward shared-tail entry, exact read ordering and widths, signed/byte party-state repairs, descriptor payload loop, temporary 248h-byte block copying, handle closure, post-load global resets, and final `181B4`/`06C68` restoration. `073A0` retains the exact 2Ch-byte DOS find block and filename at +1Eh, source/destination construction, early error exits, and per-file allocation/free ordering. Save/load slot workflows preserve their asymmetric failure reporting and intentional handle leaks on right-click exits. The confirmation hit boxes retain inclusive X and half-open Y bounds. `0808C` begins the next dispatch table ownership family; the table through `080AB` is audited as data and `080AC` is the next executable frontier.

## CODE.1 cross-reference policy

The fully audited CODE.1 decompilation remains part of the active procedure. Its discovered defect classes determine where CODE.2 receives extra scrutiny: cdecl slot order, live-register call inputs, byte/word aliases, signed/unsigned branch drift, modulo-32-bit arithmetic, encoded loop entry, selector/interrupt preservation, global reloads, address-distinct thunks, self-modifying immediates, and DOS/runtime wrappers. Mechanically similar regions may be audited in larger coherent families, but any opcode/control-flow divergence is resolved from CODE.2 itself.


## Checkpoint 9 — `080AC..0A790`

**9,941 bytes.** Throughput was increased across reconstruction batches 12–15 while retaining ownership-complete boundaries. Fresh immutable-object disassembly closed the left-panel dispatcher and status/event sweeper, moving-record construction/compaction and collision/effect resolution, opposite-buffer hit testing and selection, party effect-slot insertion/status application, and the complete party-effect animation/resolution/modal-apply family through `0A790`. Span SHA-256: `ea8476146e7851dfd335577d0bfbf538a889d793175ef89edb47ee73f75d7aa5`. Mandatory overlap `[0x0806C,0x0816C)` SHA-256: `db9a0033708da1ed5d9ec2436f792e75bab21a28228588a3347d46f09ddd1520`.

**PASS with no new semantic correction.** The audit explicitly rechecked the CODE.1 defect classes and the cdecl live-stack pattern that has already reproduced in CODE.2. `080AC` preserves the two distinct `1A1F4` polls, signed IDIV menu-row selection, per-action capture-refresh asymmetry, saved `D680h`, and action-specific state restoration. `0871C/0893C` preserve XOR status-bit clearing, signed deadline comparisons, address-distinct expiration side effects, and the second fresh timer read used when reinstalling refresh state. `08A80` retains 16-bit high-word tile arithmetic/truncation, owner-bit packing, unchecked sound-table indexing, signed owner range tests, angle normalization/orientation packing, and the collision loop's flag-controlled exit. `08C88` deliberately reports an out-of-range signed index but continues into the compaction copy.

The moving-record collision families preserve unsigned radius-box arithmetic, owner-derived multipliers, mode-specific filters, signed half-damage behavior, exact type-specific bit changes, and the proximity/orientation sound gates. The `0934C..09367` bytes remain a seven-entry CS sound-offset table. `09418` retains signed pointer-coordinate handling and its mode-dependent return contract. The opposite-buffer selector keeps the encoded low-byte/low-word toggle distinctions, descending order search, and the deliberate fall-through to record[count] when an ordering word is absent. `096E0` was rechecked at each six-argument call site for cdecl slot order; the mode and party-slot arguments remain in the encoded positions.

`09790..09B49` was audited as one complete status/equipment-effect transaction: signed old equipment words, F218-derived byte values, A5/80 special cases, the two address-distinct F238 refresh paths, redraw/sound ordering, and optional timed-event installation all match the immutable instructions. Batch 15 (`09B4C..0A790`) was then carried through its complete animation, encoded effect-resolution, modal apply, and four-party timed-state update paths. Signed party-state tests, byte/word truncation, random/result scaling, message-only failure returns, and timer-factor multiplication were checked without finding another semantic drift. `0A791..0A793` is alignment before the next independent family; next frontier is `0A791`.


## Checkpoint 10 — `0A791..0D54F`

**11,711 bytes.** The enlarged audit span closes the alignment at `0A791..0A793`, all of reconstruction batches 16 and 17, and the first complete Batch-18 family through the four-entry CS table at `0D540..0D54F`. Fresh immutable-object span SHA-256: `d7be8e8c6d82f446f8c25b0e5a56f0bb20dd08974152b6ed0c8f09180a562262`. Mandatory overlap `[0x0A751,0x0A851)` SHA-256: `a1c238a0b3e5eda7fce02986f0d8f76a61eabf7443fe4076242071c3df620311`.

**PASS with no new semantic correction.** The CODE.1 defect checklist was applied across the complete ownership units, with particular attention to cdecl stack slots, signed index/range tests, rank-indexed stack aliases, two's-complement arithmetic, table ownership, and address-distinct exits. The Batch-16 family retains the out-of-bounds hard return in `0B02C`, inclusive world-record scanning through `258A2`, the moving-type-`0Bh` special path, and `0B410`'s exact two redraw pairs followed by one final page flip after sound `+21h`.

Batch 17 preserves `0B518`'s rank-indexed stack aliases, 32-bit two's-complement absolute-value behavior, and the encoded `E284` loop-counter argument. The 132-entry CS dword table at `0B740..0B94F` remains data owned by `0B950`; all command values `00h..83h` retain their encoded handler/default mapping, including the distinct no-selection returns and command-`58h` forced `FFFFFFFEh`. The formerly ambiguous `0C6D7..0C703` boundary is retained exactly as nine padding bytes followed by the nine-dword source table copied by `REP MOVSD` at `0CBD5`.

The Batch-18 portion through `0D54F` was rechecked for the high-risk arithmetic and class-selection paths. `0CD44` keeps countdown state distinct from its damage buffer, `0CF88` retains the encoded signed divide edge, `0D0B0` preserves class-table indexing, and `0D1E0` preserves random-tie behavior. The four dwords at `0D540..0D54F` are data owned by the next routine. No new normalization, signedness, partial-register, global-reload, or call-slot defect was found. The next executable frontier is `0D550`.


## Current result

- Audited object bytes: **54,592 / 239,698 = 22.7753%**
- Closed frontier: **`0D54F`**
- Next frontier: **`0D550`**
- Semantic defect classes corrected so far: **6**
- Object byte accounting remains **239,698 / 239,698 = 100.0000%** with **0 unresolved bytes**.
