# Shared reconstructed source register — 2026-10-10

**Purpose:** prevent silent CODE source drift without disturbing media preservation or G2 accuracy-validation snapshots. Exact Git blob reconciliation below is **not** complete semantic/DOS acceptance.

## Live shared-source identities

| Path | Git blob SHA | Active consumers checked | State |
| --- | --- | --- | --- |
| `Code1/Code1.cpp` | `226c563007ac…` | Decompilation, native, disassembler and other audited branches | Identical across all eight audited branches |
| `Code2/Code2.cpp` | `5b0b4dd7b6a91bfcc99068970c3152f0f62ce54e` | `code1-decompilation`, `demo-v1`, `disassembler-foundation` | **Identical** after original-byte-verified F790 sync |
| `Code2/verify_f790_stack_local_preread.py` | `4e2afcb39eb751be5a58e9963ba0e8bd1a405d52` | Same three active consumers | **Identical**; source-only test, optional original CODE.2/T oracle |
| `Code2/README.md` | `fc1681e828a0a6120d61e951549f1fd6345afeac` | Same three active consumers | **Identical**; correction and semantic caveat current |

### Original witness for the CODE.2 correction

Original CODE.2 length **558689**, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; object1 physical start `0x3AF24`. At logical `F7CC`, original bytes `6a3f8d44240450e890cb000083c408` show an original `T[0x3F]` stack-local read and call from `F7D3` to `1C368` **before** the `item==-1` check. Rechecked against supplied original local binary during synchronization.

The native `OriginalRandom` per-die/final-bonus regression is `demo-v1` test-only; do not copy it into the disassembler or decompiler.

## Intentional older CODE.2 source — snapshots are not live canonical copies

| Branch | CODE.2 blob prefix | Handling |
| --- | --- | --- |
| `disassembler-g2-build-20261010` | `87f6638921c5…` | G2 candidate with seven tool source/doc differences versus foundation; keep original candidate build evidence |
| `disassembler-g2-validation-20261010` | `87f6638921c5…` | G2 workflow validation candidate; preserve exact CI provenance |
| `DnD-Classics` | `87f6638921c5…` | Original-game media preservation takes precedence |
| `extractor-wpf-fluent` | `c3f16122e79c…` | Specialist extractor snapshot; inspect a real shared dependency before selective sync |
| `preservation-backup-code1-2026-10-07` | `c3f16122e79c…` | Historical backup kept deliberately unchanged |

**No source rewrite** is implied for original-media, backup, extractor or G2 candidate branches. The canonical disassembler **foundation** receives verified shared fixes; candidate review branches adopt them only after their author reconciles or promotes their distinct tool changes.

## Rules for maintaining this register

For every future CODE.1/CODE.2 correction: recheck original source, change the owning branch with a regression, list active live consumers, compare exact blob SHA, selectively synchronize relevant files, and update this register. Never mistake filename presence for semantic equality. Native-only and tool-only improvements stay in their own source area. A passed CTest on one checkout never establishes Windows/DOS parity on another.

See [ownership](BRANCH_OWNERSHIP.md), [current status](CURRENT_STATUS.md) and [documentation policy](DOCUMENTATION_POLICY.md). This is a **dated snapshot**: refresh hashes following any subsequent shared-source edit.
