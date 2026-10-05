# CODE.2 explicit reconstruction — 39D4D..39FEC

## Scope

The post-Batch-142 interval `39D4D..39FEC` has been directly disassembled from immutable object-1 and staged as explicit C++ in `.audit_gap_39d4d.fragment.cpp`.

- `39D4D..39E1B`: 207 bytes — arena acquisition/publication
- `39E1C..39F76`: 347 bytes — alternate arena extension
- `39F77..39FEC`: 118 bytes — request normalization
- total: 672 bytes

## Fidelity points

The reconstruction retains the encoded partial-register alignment operations (`AND BL,FC`, `AND BL,FE`, `AND BH,F0`, `XOR BL,BL`), 32-bit unsigned overflow checks, mode-dependent DS-limit refresh, address-sorted arena search, adjacent-arena extension, owner allocation-count publication, `FFFFFFFF` owner marker, and the deliberate transfer of newly-created payloads through the existing `37C45` free/coalescing machinery.

The DOS AH=48h boundary in `39D4D` is represented with carry separate from EAX because the binary folds CF through `SBB EBX,EBX / NOT EBX / AND EAX,EBX`; failure is not semantically identical to `EAX==0`.

## Accounting

This explicit reconstruction closes the source-explicitness blocker identified in Checkpoint 58. It does not change the final translated/non-code aggregate; it makes the 672-byte interval eligible for semantic-audit credit. With Checkpoint 57's 236,861-byte base, cumulative semantic inspection becomes 237,533 / 239,698 = 99.0968%, leaving 2,165 bytes = 0.9032%.

## Integration status

The fragment is staged only. `Code2.cpp` is not claimed patched by this artifact. Main-source integration and compile verification remain a separate gate.

## Next

Resume direct semantic inspection at `39FED`.