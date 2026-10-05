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

For every audit checkpoint, immutable hashes are reverified, disassembly is regenerated directly from the immutable object, and the current reconstruction is checked for call/argument order, stack-slot consumption, signedness, partial-register effects, reloads across calls, overflow/truncation, shared tails, selector/far-pointer state, and address-distinct control flow. The fully audited CODE.1 reconstruction and `Code1/FINAL_BINARY_AUDIT.md` are used aggressively as reference models, never as substitutes for CODE.2 evidence.

The CODE.1 final audit is specifically treated as a defect checklist: cdecl slot order, byte/word aliasing, signed/unsigned branch drift, modulo-32-bit arithmetic, encoded loop entry behavior, selector and interrupt-frame preservation, global reloads, address-distinct thunks, self-modifying immediates, and DOS/runtime wrapper behavior are mandatory checks in CODE.2.

## Semantic audit checkpoint 1 — `00000..002A7`

Fresh disassembly was regenerated from the immutable CODE.2 object and reconciled through the complete early application block and the 13-entry dispatcher table boundary. Audit span: **680 object bytes (`0x2A8`) = 0.2837% of object 1**.

Rechecked routines and classifications:

- `00000..00002` trap stub and `00003..0000F` alignment.
- `00010..00072` type-54 record search/flag update.
- `00073` NOP alignment.
- `00074..000B7` type-8 cleanup and encoded runtime-call sequence.
- `000B8..001F9` slot-selection/state-machine routine.
- `001FA..001FB` MOV-EAX,EAX alignment.
- `001FC..00270` resource-46 slot allocation/flag rejection/table-bit clear.
- `00271..00273` LEA alignment.
- `00274..002A7` thirteen-entry CS jump table consumed by `002A8`.

One semantic defect was found in `000B8..001F9`. In the `1C1DCh(20h) != 0` branch, the binary executes `16524h(9)`, `1C124h(3Dh)`, and `1C160h(3Ch)` and then performs the four-slot flag-clearing loop. The fourth iteration branches directly to the function epilogue; there is **no** `1C124h(3Ch)` call on this path. The previous C++ incorrectly inserted that extra call after the loop. The audit removed it and preserved the live encoded EAX result (`0x920`). The alternate path still performs the real `1C124h(3Ch)` call at `001EE`.

**Checkpoint-1 frontier:** `002A8`.

## Semantic audit checkpoint 2 — `002A8..011A7`

The audit was widened from individual tiny routines to complete reconstructed families. Fresh CODE.2 disassembly was generated from `00280` through `01200`, with a full-object direct relative branch/call target scan used for separator validation. **No additional semantic correction was required in this checkpoint.**

### `002A8..006B8` — thirteen-way command dispatcher

PASS. The dispatcher table targets and case convergence were rechecked directly. High-risk details retained by the C++ include temporary bit-2 masking/restoration around `20C0C`, opcode-2 type-dependent byte/word toggles, opcode-3 signed `IDIV` remainder semantics, packed `8000h` coordinate values, opcode-7's long page/input sequence, live-EBX input to `1C160`, and opcode-11's stop path. These are exactly the kinds of details the CODE.1 final audit showed must not be normalized away.

### `006B9..006BF` — seven-byte zero island

PASS as non-code/padding. A full object-1 direct relative `CALL`/`JMP`/conditional-branch target scan found no incoming target to these seven bytes. They lie between the preceding terminal return and the independently decoded `006C0` entry. Linear objdump intentionally misdecodes across the zero island into the next instruction at `006BF`; boundary-aware decoding confirms that this does not create executable ownership of the island.

### `006C0..009AF` — coordinate and record-search family

PASS. `006C0` preserves signed camera-relative coordinate derivation and the `-40` stored clamp. `00700`, `00794`, `007E4`, `0083C`, and `00934` remain address-distinct searches with their encoded primary/alternate record sources, signed comparisons, `5FCB` layout choice, `6048` snapshot/writeback, `2ACDC`/`F03C`/`F03E` filters, table-bit `40h`, and signed ±10 coordinate tests. The audit specifically rejected any temptation to merge these into one generalized search helper.

### `009B0..00B6B` — four-neighbor tile classifier

PASS. The unusual branch structure is confirmed: kinds 2, 3, and 4 fall back to the raw neighbor mask. Only kind 1 and the ordinary kind-5 path consult `5FF0`. This is a high-value confirmation because a cleaner generalized lookup would be semantically wrong.

### `00B6C..00BF9` — tile draw helper

PASS. Partial-BX palette/index behavior was rechecked, together with temporary `764D/7651` palette-byte installation and restoration. The C++ preserves the encoded subregister effects rather than promoting them into unrelated full-register state.

### `00BFC..00CAF` — fixed-point marker/line helper

PASS. Eight-step fixed-point progression, signed arithmetic/truncation, local-width behavior, and the call into the reconstructed `29CEC` line rasterizer agree with the binary.

### `00CB0..010C2` — multi-family map renderer

PASS. This large block received explicit CODE.1-audit-style review for live globals across opaque calls. The reconstruction correctly retains live `5FCB` layout selection, address-distinct search families, repeated `258DC` reloads across draw calls, and asymmetric alternate-record type sources. The `3C..43` family and the `AD` test are not incorrectly normalized onto one source.

### `010C4..011A4` — fixed UI-frame decoration

PASS. All eight encoded draw calls and their setup were checked. Repeated `258DC` reloads remain intentional semantics and are not cached across opaque calls.

### Separators through `011A7`

PASS. The decoded no-op separators at `006FD..006FF`, `00792..00793`, `007E2..007E3`, `00839..0083B`, `00931..00933`, `00BFA..00BFB`, `010C3`, and `011A5..011A7` have no incoming direct branch/call target in the full-object scan and remain valid alignment boundaries.

## CODE.1 cross-reference policy in the accelerated audit

The fully audited CODE.1 decompilation is now part of the active audit procedure. Its discovered defect classes determine where CODE.2 receives extra scrutiny: live-register call inputs, partial-register aliases, global reloads after opaque calls, signed division/comparisons, stack-slot order, selector state, and branch convergence. Later runtime/graphics regions that are mechanically proven relocation-only counterparts can therefore be audited in much larger blocks than during decompilation, while any opcode/control-flow divergence forces a CODE.2-specific review.

## Current result

- Audited object bytes: **4,520 / 239,698 = 1.8857%**.
- Semantic corrections found so far: **1**.
- New corrections in checkpoint 2: **0**.
- Current audited frontier: **`011A7`**.
- Next frontier: **`011A8`**.

The semantic-audit percentage is independent of the completed 100% decompilation/byte-accounting percentage.
