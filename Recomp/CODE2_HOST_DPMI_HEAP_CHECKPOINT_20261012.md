# R04 CODE.2 original heap establishment — source-backed checkpoint (2026-10-12)

Canonical branch: `code1-decompilation`, **Recomp/**. Not `demo-v1` or `native-engine`.

## Implementation committed, CI pending

- Source witness: `src/code2_original_dpmi_heap_gate.cpp`; registered
  as `canonical_code2_original_unseeded_host_dpmi_arena_growth` in CMake.
- No arena header, starting free-list node, or heap allocation is preseeded.
  Original DGROUP arena-chain head at `0x12624` begins at zero.
- Actual reconstructed `CODE.2:37B42` requests memory, reaches
  `39D4D -> 39C7B` through explicit source adapters, and invokes
  the stateful host DPMI `0501` page allocator. Original `39BBC`
  constructs and links the two arena headers.
- Original `39838` chooses blocks/splits free space; `37C37/398E0`
  frees them and verifies first-fit reuse. A request larger than the
  provider's memory budget must return zero without corrupting two
  existing allocations. Host-owned block handles must remain live
  throughout, and releases must invalidate translation.
- The `37B42` arena-chain traversal, growth callback wiring and free
  dispatch remain explicit host adapters rather than inlined fully
  reconstructed source. The host DPMI provider now accepts a bounded
  contiguous **shared guest-address aperture**, so original CODE.2
  `Machine.data + linear` and the provider's checked `translate` point to
  precisely the same host-owned bytes. Tests check writes in both
  directions and forbid any end-of-test guest/provider snapshot copying.
  This is host address-space translation, not actual DOS/4GW paging.
- The test directly releases the two surviving host handles at teardown;
  it **does not** claim original `39A2C` automatic reclamation or
  `39A8A` relocation. This is a partial R04 witness only.

## Verification

The test and CMake registration were committed to the canonical branch.
**New Windows CTest execution has not yet been independently verified.**
The most recent independently verified Windows suite is
[run #81](https://github.com/Hyperbog/RLOFT/actions/runs/38245588733),
**43/43 passed** on the earlier source SHA
`875617f4b6a47dcbb48bc209eeb0abfa2d2b4075`.
Do not attribute that result to the new test.

For Windows/MSVC verification:

```powershell
cmake -S Recomp -B build/recomp -A x64
cmake --build build/recomp --config Debug --parallel 2
ctest --test-dir build/recomp -C Debug --output-on-failure
```

No commercial game assets are required by the added CTest. The
owner-original `CODE.2` source-byte identity is archived separately;
this new Windows test does not independently compare original DOS heap traces.

## Acceptance

**R04 remains NOT ACCEPTED.** Fixed score **3/12 = 25.00%**.

Next: obtain independent green Windows execution of all newly
registered tests. Complete source `39A2C/39A8A` automatic reclamation
and 0503 resize, DOS fallback and full selector/limit/heap integrations;
eliminate the original game-entry seeded arena only when both startup
paths and rollback have authentic service coverage.

## Additional original-source and selector evidence

- Host `DpmiSelectorDescriptors` implements checked DPMI `0006`
  selector-base lookup (CX:DX), bounded descriptor lifetime and invalidation.
  DPMI DOS `0100/0101` binds/unbinds these descriptors. The existing
  original CODE.2 mouse-source DPMI tests check the live base and invalidation.
- New original CODE.1 `src/code1_original_dpmi_memory_gate.cpp` calls compiled
  original `6F10/6FC0/6FF4/709C/705C/6F74`; the original routines write
  selector/handle ownership headers on statefully allocated host bytes.
- `canonical_dpmi_0006_selector_base_and_descriptor_lifecycle` and
  `canonical_code1_original_6f10_6fc0_6ff4_709c_host_dpmi_memory`
  are registered alongside this CODE.2 source test.
- These additions raise the expected CTest registration count from the
  last verified 43 to **46** (with Python available). No new Windows pass
  count is claimed before an independently observed Actions result.

Remaining gate-blocking caveats: no original CODE.2 `39A2C` automatic
release, `39A8A` stateful 0503 source relocation, authenticated
startup under original media without synthetic seeded arenas, or full
DOS/4GW runtime/interrupt parity. R04 remains false.
