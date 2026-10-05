# CODE.2 semantic audit checkpoint 23

Status: **PASS**

Audited object-relative range: **`1CE68..202EF`** (**13,448 bytes**).

Cumulative semantic audit: **131,808 / 239,698 = 54.9892%**.

Semantic defect classes remain **6**. No additional semantic defect was established in this ownership-complete four-batch span.

## Immutable evidence

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- Mandatory frontier overlap `[1CE28,1CF28)` SHA-256: `2a1131b7a7a82e332ca6e79cff7d09d94d512ee95ec513fe6e85c7f358c4132b`
- Audited span `[1CE68,202F0)` SHA-256: `aa659c15f0a188984c5898b736c0453b94078e298c5c9664933caf4bfc7cad50`

The overlap was freshly disassembled from the immutable object before extending the frontier; overlap bytes were verification-only and were not recounted. The right boundary was also re-disassembled: `202ED..202EF` is the encoded `LEA EAX,[EAX+0]` alignment and `202F0` is an independent routine entry.

## Ownership/family coverage

This checkpoint closes original reconstruction batches **42 through 45** in one higher-throughput semantic pass.

- `1CE68..1E49F`: preserved as one compiler-owned command dispatcher with shared selector tail `1E488`; splitting it would create false ownership boundaries. Selector-call ordering, live `BF44` reread in the `11h/12h` path, direct event-byte mutations, command `2Ch`'s zero second criterion, inventory-test ordering in `2Fh`, and `(r+r)>>15 + 6Fh` random-text arithmetic were rechecked.
- `1E4A0/1E534`: unsigned timer-expiry tests retained.
- `1E594`: phase-gated early return and both timed-text damage paths retained.
- `1E75C`: signed tick-delta comparisons and `438h/21Ch` cadence retained.
- `1E930/1E9A0`: partial-register return behavior retaining pointer high words retained.
- `1EA90`: status/item rendering retains the stack-carried zero that becomes `30508`'s fifth argument.
- `1EDAC`: one-replacement-per-lane mapping behavior retained.
- `1EEFC..1EF7B`: retained as the exact 32-entry CS table owned by `1EF7C`.
- `1EF7C`: phase-dependent random sound ranges retained.
- `1F120`: repeated live `29048` calls retained rather than cached.
- `1F1C8`: signed truncation and asymmetric presentation paths retained.
- `1F3FC`: encoded flag-priority order retained.
- `1F494`: dead/high-bit direct jump still bypasses the common actor-status refresh.
- The `2E6F0` one-cdecl-input signature and pre-pushed zero becoming `30508` argument 5 were rechecked against the surrounding stack ownership.
- `1F858..1F86B`: retained as the exact five-entry table.
- `1F86C`: state-4 fall-through through scripts `2Dh..31h` retained.
- `1F8E0`: backward shared-RET / forward fall-through ownership retained.
- `1FB80`: release-then-press polling retained.
- `1FC14`: non-returning main loop retains initialization of `258FC` only, live `258FE`, AX key dispatch, selected-axis damping skip when `direction*value >= 0`, and `C6E8/C6EC` low-32 IMUL + logical SHR + CWDE scaling.

Cross-cutting CODE.1 defect checks were reapplied for cdecl slot order, live-register call inputs, partial-register aliases, signed/unsigned branch drift, modulo-32 arithmetic, global reloads across calls, exact table ownership, shared tails, encoded oddities, and address-distinct control flow. No new CODE.2 mismatch was established.

Next semantic-audit frontier: **`202F0`**.
