# RLOFT branch ownership

**Reviewed:** 2026-10-17. The project has **seven** retained owner branches; current/live names and heads are listed in [LIVE_STATUS.md](LIVE_STATUS.md).

| Branch | Primary purpose |
| --- | --- |
| `code1-decompilation` | **Current default:** `Decomp/`, CODE.1 / CODE.2 reconstruction, research and shared documentation |
| `recomp` | Recompilation host and original-source build tests, using CODE sources at stable paths |
| `demo-v1` | **Main playable enhanced Windows game**, not the decompilation branch |
| `native-engine` | Separate original-media-driven renderer/runtime prototype, not yet a shared KEX/OpenMW-style engine |
| `DosDis` | DOS/NES/SMS disassembler and WPF desktop |
| `Ravenloft-Asset-Extractor` | Ravenloft asset extraction WPF/Python tooling |
| `DnD-Classics` | Original preserved `MASTER/`; immutable |

**Requested rename:** default `code1-decompilation` → `Decomp`. GitHub's token returned HTTP 403 for the default-branch rename; an administrator must finish it. The default CI branch filters accept either name pending that action.

**Pull requests:** [DosDis draft PR #27](https://github.com/Hyperbog/RLOFT/pull/27) replaces closed PR #4 after the branch rename. The latter's discussion remains archived.

On 2026-10-17 the older eight `ci/native-*` branches and `port/asset-cache` were absent from the live branch list. The [October 11 branch retirement audit](BRANCH_CLEANUP_AUDIT_20261011.md) documents its own earlier 14 recovery tags; do **not** infer the October 17 retirement references were individually tagged unless verified.

Never rewrite `DnD-Classics`, force-update branch heads, merge independent products wholesale, or delete a branch with unique source changes before preserving and reviewing its SHA. CODE.1/CODE.2 source changes shared across branches must be synchronized narrowly and retested; see [source register](SHARED_SOURCE_REGISTER.md). No Windows/game build acceptance is implied by documentation changes.
