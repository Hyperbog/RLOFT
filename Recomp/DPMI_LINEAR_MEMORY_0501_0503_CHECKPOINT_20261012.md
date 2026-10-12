# R04 memory-runtime acceptance checkpoint — 2026-10-12

Branch: `code1-decompilation` · Workstream: **canonical `Recomp/` only**.

## Acceptance decision: R04 NOT YET ACCEPTED

R04 requires **DOS/4GW memory, original heap growth and DPMI selector lifecycle without test-only allocations**. Adding a host memory API or passing an isolated CTest is not evidence that the original game's heap has booted.

## This checkpoint — real, stateful memory transport

- `src/dpmi_dos_memory.hpp` implements existing host-owned DPMI **0100/0101** conventional DOS memory, tracking selector, real-mode paragraph and live allocation independently. Existing original CODE.2 `30D80 -> 30D52 -> 33B44 -> 30DE8` tests route through this service.
- `src/dpmi_linear_memory.hpp` adds host-owned protected-mode **0501/0502/0503** allocation, free and resize. A linear address is not a selector or handle. 0503 creates a new address/handle, copies retained guest bytes, invalidates the previous identity, and rejects invalid/zero/over-budget requests without mutating live data. Linear accesses are checked against committed pages.
- `src/test_dpmi_linear_memory.cpp` exercises 0501 zero/oversized failure, page accounting, independent mapping, 0503 grow/shrink relocation and content preservation, failure atomicity, stale-handle rejection and full 0502 release.
- `CMakeLists.txt` registers `canonical_dpmi_0501_0502_0503_linear_memory_lifecycle` as a new CTest. **Registration is not a verified Windows run.** Original game assets remain outside CI.

### Engineering limits

The deterministic host linear range begins at `0x01000000`, has a **16 MiB configured safety budget**, 4 KiB committed pages and a guard-sized address gap. These are host-adapter engineering choices, **not** claims about a measured DOS/4GW arena, free-memory capacity, guest page-fault implementation, or exact DPMI allocator policies. No guest LDT descriptors are created by 0501, in accordance with DPMI 0.9.

## Remaining mandatory R04 closure

1. Implement selector-base service `0006h` plus the DOS 48h/49h/4Ah backing memory services and required extender-specific behavior, including CODE.1 and CODE.2 call boundaries.
2. Execute actual original reconstructed heap establishment and growth (CODE.2 `39D4D/39E1C`, arena-chain and fallback paths; CODE.1 `1083F` and `109C1..10C45`) through these memory services. Replace—not just rename—the startup's seeded `0x2B800/0x2C000..0x2D500` allocation fixture.
3. Verify authentic LE image initial state, ownership headers (`selector,handle`), cross-page writes, OOM and resize rollback, selector/handle invalidation, and full source-backed allocate/free/coalesce/growth cycles.
4. Obtain an independently checked Windows source build, original-media execution evidence and negative/error-path tests before switching `ACCEPTANCE_GATES.json:R04` to `true`.

**Gate score stays 3/12 = 25.0%.** No original game boot, complete DOS runtime, or memory parity is claimed.

### Controlling references

- `PreservationArchive/02_PROTECTED_MODE_STARTUP_AND_MEMORY_RUNTIME.md` — original Watcom/DOS/4GW memory ownership and CODE.1 runtime mapping.
- `Recomp/CODE2_ORIGINAL_ARENA_CLOSURE_20261011.md` — original arena logic, confirmed request ABI, synthetic arena limitation.
- DPMI 0.9 functions `0501`, `0502`, `0503`: https://www.delorie.com/djgpp/doc/dpmi/api/310501.html, https://www.delorie.com/djgpp/doc/dpmi/api/310502.html, https://www.delorie.com/djgpp/doc/dpmi/api/310503.html.
