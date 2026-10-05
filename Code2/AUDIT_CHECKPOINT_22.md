# CODE.2 semantic audit checkpoint 22

Status: **PASS**

Audited object-relative range: **`1A220..1CE67`** (**11,336 bytes**).

Cumulative semantic audit: **118,360 / 239,698 = 49.3788%**.

Semantic defect classes remain **6**. No additional semantic defect was established in this ownership-complete seven-batch span.

## Immutable evidence

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- Mandatory frontier overlap `[1A1E0,1A2A0)` SHA-256: `31e734215e44a6f0e38ff1dc58a6518073afbabf092d122ab2fb67f73e6fda72`
- Audited span `[1A220,1CE68)` SHA-256: `d608c97dbf0d636460446934421759ffed3e3332052f8ddd3abe6bb1bcb089c4`

The overlap was freshly disassembled from the immutable object before extending the frontier; overlap bytes were verification-only and were not recounted.

## Ownership/family coverage

This checkpoint closes original reconstruction batches **35 through 41** in one higher-throughput semantic pass.

- `1A220`: preserved byte-only `CF48` XOR followed by signed-word testing, saved/restored signed mouse coordinates, and INT 33h mode switch.
- `1A288`: preserved the large mouse-update family's non-contiguous shared cold tail at `1AB6C..1AC0B`, cdecl stack ordering, signed coordinate loads, and exact redraw/dispatch gating.
- `1A3B8`: preserved the two distinct cursor clamp regions rather than normalizing them into one generic clamp.
- Page/item helpers preserve active `XOR SI,1` versus fallback `XOR AL,1` behavior; `1AACC/1AB20` preserve the encoded word `DEC` before zero testing.
- `1AC18..1AC37`: retained as the exact eight-entry CS table owned by `1AC38`; `1AC38` preserves both hit-map families, `A6BC=index+3`, and axis-specific `1F7F8` dispatch values.
- `1AE30`: preserved disabled-path zeroing, INT 33h coordinate capture, right-button `C04C` state handling, partial-width fields, and the encoded input-state transitions.
- `1BBD8`: preserved retry count, type-9 word-width arithmetic, descriptor radius/divisor reads, and success-path orientation increment.
- `1BD9C..1BDAB`: retained as the exact four-entry state table owned by `1BDAC`.
- `1BDAC`: preserved descriptor timer reload after `0D550`, signed live-count reloads for all three record arrays, compaction retry through pre-increment `DEC ESI`, the encoded `CMP EDI,EDI` artifact in the type-8 path, and native zero-divisor x86 divide faults in metadata-driven orientation updates.
- `1C0C0`: preserved signed two-stage division.
- Bitset helpers preserve negative-index/remainder shift behavior and bit-31 arithmetic-shift result.
- `1C2C4`: preserved unsigned timeout comparison.
- `1C328`: preserved low-32 multiply/wrap arithmetic.
- `1C368`: preserved repeated handle reloads and backward `DCh` decode.
- `1C40C`: preserved partial-register size rounding (`AND AL,E0h` / `AND AL,F0h`), x86 signed half-offset calculation, separate random edge families, and signed loop termination.
- `1C77C`: preserved full-dword versus low-byte `C050` asymmetry.
- `1C924`: preserved the two-phase slot search.
- `1C9A0..1CA2B`: retained as the exact 35-entry table owned by the surrounding dispatcher.
- `1CD48`: preserved criterion-21h stack reuse/latch behavior.
- `1CE66..1CE67` is the final `MOV EAX,EAX` alignment before the independent command dispatcher at `1CE68`.

Cross-cutting CODE.1 defect checks were reapplied for cdecl slot order, live-register call inputs, partial-register aliases, signed/unsigned branch drift, modulo-32 arithmetic, sentinel widening, global reloads across calls, exact table ownership, encoded oddities, and address-distinct control flow. No new CODE.2 mismatch was established.

Next semantic-audit frontier: **`1CE68`**, the large command dispatcher intentionally kept intact for the next ownership pass.
