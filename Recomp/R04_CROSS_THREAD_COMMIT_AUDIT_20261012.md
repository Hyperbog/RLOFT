# R04 cross-thread commit audit — 2026-10-12

## Provenance and scope

Canonical branch: `code1-decompilation` on `Hyperbog/RLOFT`.
Audited GitHub comparison:
`2ad11d17c73dab0d52e3a953070ac3313ac9dd49...54ba71e2840f88928c0a1c37763b940cb3249046`
(**16 commits**, chronological additions after the prior verified main-thread checkpoint).
GitHub commits do not identify the ChatGPT conversation that originated a
change, so this audit classifies the **entire branch range**, not a
purported per-chat author.

The 16-commit comparison touched **10 files, all under `Recomp/`**:
`CMakeLists.txt`; `CODE2_HOST_DPMI_HEAP_CHECKPOINT_20261012.md`;
`src/code1_original_dpmi_memory_gate.cpp`;
`src/code2_original_dpmi_heap_gate.cpp`;
`src/code2_startup_gate.cpp`;
`src/dpmi_dos_memory.hpp`;
`src/dpmi_linear_memory.hpp`;
`src/dpmi_selector_descriptors.hpp`;
`src/test_dpmi_dos_memory.cpp`;
`src/test_dpmi_selector_descriptors.cpp`.

No change to the canonical reconstructed original `Code1/Code1.cpp`,
`Code2/Code2.cpp`, original copyrighted assets, `demo-v1`, or
`native-engine` is part of this comparison. The purpose and tests
**align with R04 (DOS/DPMI memory runtime)**, not R05–R12.

## Findings by change group

| Commits | Evidence and verdict | Remaining limitation |
| --- | --- | --- |
| `c340a60`, `a497035`, `0be779a`, `8137d5f`, `6cd0f4d`, `104fe36`, `5e00083` | **Aligned.** Checked stateful selector-base table, registered 0100/0101 allocation lifetime, invalid selector errors and CX:DX translation. Independent host tests and compiled original CODE.2 mouse-source state checks. | Models host descriptor state; does not establish authentic DOS/4GW selector and limit policies. |
| `2c8393d`, `c01cafa` | **Aligned.** Executes original compiled CODE.1 `6F10/6FC0/6FF4/709C/705C/6F74` using host-owned DOS/linear memory; checks written ownership headers and stale selectors. | Does not join original CODE.1 character-creation process to CODE.2 runtime or complete hardware services. |
| `202e1c3`, `eb7fe7d` | **Aligned.** New compiled original CODE.2 `37B42→39D4D→39C7B→39BBC` heap test starts with no preseeded arena and exercises allocate, split, free, reuse and OOM. | Standalone source-bound test uses host callbacks for chain traversal; not the original startup allocation path or all free/coalesce logic. |
| `f4876bf`, `322a209`, `25977c8` | **Aligned with a test gap corrected below.** Optional shared guest-page aperture now gives original CODE.2 source and DPMI 0501 host-backed pages an identical byte image. Bidirectional writes and allocation teardown are checked. | Before this audit the shared-aperture DPMI 0503 resize/relocation behavior had no dedicated regression. Original CODE.2 `39A8A` and `39A2C` remain unexecuted through these services. |
| `54fedfd`, `54ba71e` | **Documentation only, appropriately conservative on acceptance.** | Wording about CI being pending became stale after the independent Windows runs below; corrected after this audit. |

## Independent build verification

- [Windows #89](https://github.com/Hyperbog/RLOFT/actions/runs/38250996809):
  **45/45 CTests**, code through `c01cafa`.
- [Windows #91](https://github.com/Hyperbog/RLOFT/actions/runs/38251181489):
  **46/46 CTests**, source heap witness registered.
- [Windows #93](https://github.com/Hyperbog/RLOFT/actions/runs/38251414499):
  **46/46 CTests**, host shared guest-aperture provider present.
- [Windows #95](https://github.com/Hyperbog/RLOFT/actions/runs/38251675815):
  **46/46 CTests**, bidirectional CODE.2 guest/provider byte checks.
- [Windows #96](https://github.com/Hyperbog/RLOFT/actions/runs/38252199410):
  **46/46 CTests** at source `898898dae8992978df728043d560c44eb4213601`.
  This audit adds regression coverage to `test_dpmi_linear_memory.cpp` for
  **shared-aperture DPMI 0503 growth/shrink relocation**, preserved original
  guest bytes, independent blocks, OOM rollback, stale handle/linear
  invalidation, and 0502 release.

Cancelled earlier workflow runs are **not** counted as passing. Passing CTests
are evidence of these specific original-source and provider contracts, **not**
an original DOS capture or a faithful running DOS/4GW host environment.

## Gate decision

**R04 remains UNACCEPTED; whole recompilation stays 3/12 = 25.00%.**
The 16 audited commits are compatible with the canonical workstream.
No revert or branch transfer is necessary from this evidence. Major missing
acceptance requirements remain:

1. Execute original CODE.2 `39A8A` 0503 relocation and `39A2C` 0502
   automatic reclamation, including error rollback and ownership validation.
2. Replace startup's seeded arena with original source-backed heap
   establishment in the continuous CODE.2 initializer/application path.
3. Remove the remaining CODE.1 release-bit/arena free-list test adapters and
   support full DOS/4GW selector/limit and memory ownership behavior.
4. Compare with independently captured original DOS memory/heap outcomes,
   using owner-local original media without committing commercial assets.

**Continue R04. Do not promote it to accepted merely because 46 CTests pass.**
