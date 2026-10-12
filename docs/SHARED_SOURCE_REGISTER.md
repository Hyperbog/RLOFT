# Shared reconstructed source register — updated 2026-10-11

**Purpose:** prevent silent CODE source drift without disturbing media preservation or G2 accuracy-validation snapshots. Exact Git blob reconciliation below is **not** complete semantic/DOS acceptance.

## Live shared-source identities

| Path | Git blob SHA | Active consumers checked | State |
| --- | --- | --- | --- |
| `Code1/Code1.cpp` | `bdfe208db205fc3da467c3bf9d6710869096ddea` | `code1-decompilation`, `demo-v1`, `native-engine`, `disassembler-foundation`, `extractor-wpf-fluent` | **Exact shared blob after authenticated B01A alternate-entry terminator correction** |
| `Code2/Code2.cpp` | `b6d4fb44bda916f892b65d84cf4ec3005affdc88` | `code1-decompilation`, `demo-v1`, `native-engine`, `disassembler-foundation` | **Exact shared blob**, original `37BC2` unrounded allocator ABI correction; extractor receives selective patch below |
| `Code2/verify_f790_stack_local_preread.py` | `4e2afcb39eb751be5a58e9963ba0e8bd1a405d52` | Same three active consumers | **Identical**; source-only test, optional original CODE.2/T oracle |
| `Code2/README.md` | `fc1681e828a0a6120d61e951549f1fd6345afeac` | Same three active consumers | **Identical**; correction and semantic caveat current |


### CODE.1 B01A shared semantic correction — 2026-10-11

The owner-original CODE.1 SHA-256 and code-object B01A instruction span prove a deliberate branch into the second byte of A8 AA, executing STOSB at B04C. The previous canonical source omitted the byte terminator write on even-length source strings; correction was compiled and link-tested in Windows [run #38184924773](https://github.com/Hyperbog/RLOFT/actions/runs/38184924773), then selectively copied (exact new Git blob `bdfe208d…`) into all four active source consumers. [Detailed original-byte proof and downstream commit register](../ResearchInfra/CODE1_B01A_OVERLAP_CORRECTION_20261011.md). Preserved media/history branches remain intentionally older.

### 2026-10-11 original CODE.2 allocator ABI correction and synchronization

Original `37BC2: MOV EAX,EDI` and `37BC6: CALL 39838` prove the original request is *not rounded before dispatch*; source had double-rounded. The owner-canonical Windows [7/7 CTest gate #38190078813](https://github.com/Hyperbog/RLOFT/actions/runs/38190078813) passed after the source fix and original arena split/coalesce and rollback tests. All four active project consumers were updated without overwriting extractor-only drift. [Complete opcode, commit and original-file evidence](../Recomp/CODE2_ORIGINAL_ARENA_CLOSURE_20261011.md).

### Original witness for the CODE.2 correction

Original CODE.2 length **558689**, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; object1 physical start `0x3AF24`. At logical `F7CC`, original bytes `6a3f8d44240450e890cb000083c408` show an original `T[0x3F]` stack-local read and call from `F7D3` to `1C368` **before** the `item==-1` check. Rechecked against supplied original local binary during synchronization.

The native `OriginalRandom` per-die/final-bonus regression is `demo-v1` test-only; do not copy it into the disassembler or decompiler.

## Intentional older CODE.2 source — snapshots are not live canonical copies

| Branch | CODE.2 blob prefix | Handling |
| --- | --- | --- |
| `disassembler-g2-build-20261010` | `87f6638921c5…` | G2 candidate with seven tool source/doc differences versus foundation; keep original candidate build evidence |
| `disassembler-g2-validation-20261010` | `87f6638921c5…` | G2 workflow validation candidate; preserve exact CI provenance |
| `DnD-Classics` | `87f6638921c5…` | Original-game media preservation takes precedence |
| `extractor-wpf-fluent` | `a2312a69bf21…` | **Live active extractor:** independently different CODE.2 source; allocator `37BC2` fix patched *only* to matching source fragment, other drift preserved |
| `preservation-backup-code1-2026-10-07` | `c3f16122e79c…` | Historical backup kept deliberately unchanged |

**No original media or G2 review snapshot was rewritten.** The extractor is live, and its CODE.2 allocator correction was selectively synchronized as described above. The obsolete build/backup branch names listed historically are recovery **tags**, not among the seven live branches. The canonical disassembler **foundation** receives verified shared fixes; candidate review branches adopt them only after their author reconciles or promotes their distinct tool changes.

## Rules for maintaining this register

For every future CODE.1/CODE.2 correction: recheck original source, change the owning branch with a regression, list active live consumers, compare exact blob SHA, selectively synchronize relevant files, and update this register. Never mistake filename presence for semantic equality. Native-only and tool-only improvements stay in their own source area. A passed CTest on one checkout never establishes Windows/DOS parity on another.

See [ownership](BRANCH_OWNERSHIP.md), [current status](CURRENT_STATUS.md) and [documentation policy](DOCUMENTATION_POLICY.md). This is a **dated snapshot**: refresh hashes following any subsequent shared-source edit.
