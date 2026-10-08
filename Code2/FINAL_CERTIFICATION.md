> **Historical certification notice — 2026-10-07:** this document records the certification snapshot exactly as it existed when closed. A later integration audit identified two corrected source-level semantic defects: routine `0139C` used constant automap draw coordinates where original CODE.2 increments them by 8 and 5; routine `2975C` consumes five stack arguments where the certified translation declared four. Byte coverage/accounting is unaffected. The current corrected source is identical on both branches (Git blob `87f6638921c53a32bde59735515fbc175e2c96ee`); original-byte evidence is documented in `Code2/POST_CERTIFICATION_ERRATA.md`. A new strict C++17 whole-file build is not asserted in this note. The hashes in this document therefore identify the historical pre-erratum snapshot.

# CODE.2 final certification

## Status

`Code2/Code2.cpp` is the canonical certified C++ reconstruction of Ravenloft: Strahd's Possession `CODE.2` object 1.

Certification completed 2026-10-05 after the end-to-end semantic audit, accumulated defect repair integration, terminal-tail reconstruction, stale-implementation sweep, and strict C++17 syntax/diagnostic verification.

## Immutable binary evidence

- Original `CODE.2` size: **558,689 bytes**
- Original `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 length: **239,698 bytes (`0x3A852`)**
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- Object-1 file start: `0x3AF24`
- Object-1 virtual base: `0x10000`
- Entry logical offset: `0x353A0`

The immutable CODE.2 bytes remain controlling evidence. CODE.1 was used only as an identification/defect-checking aid where independently re-established from CODE.2.

## Final accounting

- Verified translated instruction bytes: **235,022**
- Proven non-code/data bytes: **4,676**
- Total classified/verified object-1 bytes: **239,698 / 239,698 = 100.0000%**
- Unresolved object-1 bytes: **0**
- Semantic inspection coverage: **239,698 / 239,698 = 100.0000%**

## Certified source identity

The source uploaded to `Code2/Code2.cpp` was independently matched against the locally certified artifact before this document was written.

- File size: **1,304,770 bytes**
- SHA-256: `b4ab60f12e0d5a3c986ef61ec3c2e5c6d68f0193a0e535ef35eb7af6f3aed4aa`
- Git blob SHA: `c3f16122e79c3c464382e4511d9d932f6fcd694a`

## Final verification gate

The certified artifact passed:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic -fsyntax-only Code2/Code2.cpp
```

Result at certification: **0 errors, 0 warnings**.

The final audit also verified that the accumulated semantic-repair ledger was integrated, including the gate/radix/planar/stream/device fixes, formatter and floating-format helper repairs, incoming-CF behavior at `3908C`, rejected-stack behavior at `39589`, explicit DPMI carry/register contracts at `39C7B` and `39A8A`, and explicit ownership of the final runtime tail through `3A851`.

The six-byte `WVIDEO` island at `3A7F8..3A7FD` remains classified as embedded data, not executable instructions.

## Repository policy after certification

`Code2/Code2.cpp` is the canonical source. Audit checkpoints and final documentation are retained as provenance. Temporary integration scripts, staged C++ fragments, one-off repair drivers, and temporary certification workflows are not canonical artifacts and should not remain in the active workspace once their changes have been integrated. Git history is the archive for those intermediate mechanisms.

Future changes to `Code2.cpp` must preserve a recoverable prior commit, re-run the strict compile command above, and be justified against immutable CODE.2 bytes. Do not regenerate or destructively replace the certified source from intermediate fragments.
