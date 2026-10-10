# CODE.2 — byte-accounted executable reconstruction

> **Post-certification erratum (2026-10-07):** native Demo integration rechecked immutable CODE.2 bytes and confirmed two source-level semantic defects: constant automap draw coordinates in `0139C` and a four-versus-five argument wrapper at `2975C`. The current source implements the encoded increments and five-argument contract. The certification hashes below identify the historical pre-erratum snapshot, not the current `Code2.cpp`. Byte accounting remains unchanged. See `Code2/POST_CERTIFICATION_ERRATA.md`. This two-hunk branch difference is now fully reconciled; the full original-byte comparison is recorded in the main branch.

This directory contains the **fully byte-accounted but still semantically corrigible** C++ reconstruction of Ravenloft: Strahd's Possession `CODE.2`. Completed byte accounting is **not** a claim of original DOS behavioral equivalence.

## Historical accounting and verification record

- Canonical source: `Code2/Code2.cpp`
- Final certification record: `Code2/FINAL_CERTIFICATION.md`
- Reconstruction/accounting: **239,698 / 239,698 = 100.0000%**
- Historical instruction-inspection coverage: **100.0000%**; **not** a 100% semantic correctness or gameplay parity score
- Unresolved object-1 bytes: **0**
- Strict C++17 verification at certification: **0 errors, 0 warnings**
- Certified `Code2.cpp` SHA-256: `b4ab60f12e0d5a3c986ef61ec3c2e5c6d68f0193a0e535ef35eb7af6f3aed4aa`
- Original certified Git blob SHA (historical): `c3f16122e79c3c464382e4511d9d932f6fcd694a`
- **2026-10-10 synchronized corrected `Code2.cpp` Git blob SHA: `5b0b4dd7b6a91bfcc99068970c3152f0f62ce54e`** (identical to `code1-decompilation` when synchronized)

**2026-10-10 third semantic correction — F790:** original bytes at `F7CC..F7DA` unconditionally read `T[0x3F]` to a 16-byte stack local via `1C368` on the non-pending path, **before** the `item==-1` check. Stale `2C368` callback labels were also corrected. The corrected `Code2.cpp` has been synchronized to `demo-v1`. [Independent source contract and 21-call original E8 oracle](verify_f790_stack_local_preread.py) is registered in the native CTest catalog; local original binary disassembly confirms those 21 targets. Windows current-HEAD compile and original-DOS runtime sequencing remain unaccepted.

The detailed historical audit/checkpoint documents are provenance. Temporary integration scripts and staged fragments are not canonical source artifacts; Git history preserves them.

## Immutable CODE.2 identity

- Original file: `CODE.2`
- Size: **558,689 bytes (`0x88661`)**
- SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Bound LE header: file `0x27BDC`
- Executable object-1 virtual base: `0x10000`
- Executable object-1 size: **239,698 bytes (`0x3A852`)**
- Executable object-1 file start: **`0x3AF24`**
- Executable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- Entry logical offset: **`0x353A0`**
- Data object-2 virtual base: `0x50000`
- Data object-2 virtual size: `0x2DD10`
- Data object-2 file start: `0x75F24`
- Stored object-2 bytes: `0x1273D`

Authoritative object-1 extraction/disassembly:

```sh
dd if=/mnt/data/CODE.2 of=/tmp/code2_obj1.bin bs=1 skip=$((0x3af24)) count=$((0x3a852)) status=none
objdump -D -b binary -m i386 -M intel --start-address=0xSTART --stop-address=0xSTOP /tmp/code2_obj1.bin
```

Logical object-1 offset `X` maps to original file offset `0x3AF24 + X`.

## Fidelity rule

The immutable CODE.2 binary remains controlling evidence. CODE.1 and `Code1/Code1.cpp` may be used as identification aids and architectural context, but they never override CODE.2 bytes.

Preserve encoded widths, signedness, partial-register aliasing, selector/far-pointer behavior, exact cdecl slot consumption, overflow/truncation behavior, shared tails, address-distinct wrappers, interrupt carry state, embedded data, and unusual control flow.

## Verification

For any future modification run:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic -fsyntax-only Code2/Code2.cpp
```

Never destructively regenerate the byte-accounted reconstructed source. Preserve a recoverable Git commit before any integration or semantic change.

## Historical documentation

`DECOMPILATION_CONTINUITY.md`, `CODE2_CONTINUITY_ADDENDUM.md`, `PROGRESS.md`, audit checkpoints, closure records, and repair specifications document how the reconstruction was established. `FINAL_CERTIFICATION.md` records the **historical accounting closure**, not a guarantee of current source semantics or DOS parity. Consult the current native integration contract and newer original-byte corrections.
