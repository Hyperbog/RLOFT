# Original CODE.2 source-ordered 22220 nearest-depth replay — 2026-10-10

**Scope:** Native workstream `demo-v1`; isolated source-backed adapter, **not live renderer wiring**. The immutable shipped original Strahd `CODE.2` is 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, object-1 logical base physical `0x3AF24`. The original `SINTAB` reports `C6D8=256` ray slots. No game assets, original binary instructions or user screenshots are checked into Git beyond short permitted code signatures in the **private** offline oracle.

## Original machine-code instruction windows checked independently

- `335C8` initializes **`DWORD [22220]=0x800`** before the original ray producer.
- `330E7..33110`, `33278..332A1`, `3337F..333A8`, and `33487..334B0` are four source-materializer tails. Each reads the **ordinary/main source** DWORD at materialized record `+8` (`15A20`). If this value equals **`0x93`**, the code **stores `0` into `22220`**. This fact was obscured by previous comments claiming an open-field depth of only `0x800`.
- Each tail next loads its materialized signed depth DWORD at record `+0` (`15A18`), **compares it with the mutable 22220 using signed `CMP` + `JGE`**, and stores a new minimum only when strictly smaller. The `0x93` zero-store occurs **before** that signed-min comparison. It depends on `resource_inner`, **not** a dormant alternate resource or +0C prepass-only flag.
- This is compatible with the [existing isolated FF50 source span](LEVEL22_ORIGINAL_FF50_SOURCE_CLIPPING_SPAN_20261010.md): nearest depth **zero** uses initialized lookup `15618[0]=0` and yields upper/lower **128/128**. The separately sourced initial `0x800` shortcut also yields **128/128**. They are distinguishable runtime states with the same output in the open-field case.

Independent original byte check:

```sh
python DemoV1/tools/verify_original_ground_nearest_22220.py --original-code2 /path/to/CODE.2
```

Five complete instruction windows match the immutable uploaded binary, including all four exact source branches. This is not mere C++ self-testing.

## Native bounded adapter and regression

`OriginalGroundNearest22220SourceModel::replay` is a header-only isolated model accepting precisely source-ordered materialized records, each with original signed depth, the original ordinary/main resource, and an **explicit `original_materializer_executed` provenance flag**. It replays the 335C8 initialization and four common materializer tails while retaining source zero-reset and signed-min update counters. A missing, synthetic or not-yet-materialized native placeholder fails closed as `std::nullopt`; it does not silently claim an original 22220 update.

`original_ground_nearest_22220_tests` covers initialized-sentinel depth, signed wall min, material=0x93 zero reset, negative signed comparison, missing provenance, wrong ray count and no ray samples. Standalone C++17 `g++ -Wall -Wextra -pedantic` regression **passed locally** before this commit. `original_ground_nearest_22220_binary_contract_tests` is an independent source CTest with optional exact original bytes. These registrations will bring the present **216** CTests to **218** on Windows with Python, but the new sources have **not** undergone an authorized Windows build.

## Explicitly unresolved original-DOS equivalence

The existing `ViewSweepTraversalModel::trace_ray` can synthesize `empty_record()` on bounds/no-material paths. It presently does **not** distinguish that native synthetic return from the actual original `330E7/33278/3337F/33487` materializer execution in every source branch. Do **not** feed all native `sweep_records` into the 22220 replay yet or attach FF50 clipped terrain to the live renderer: that would falsely claim source provenance.

Next gate: independently reconcile all four native trace return paths against original `32954` and materializer order, add explicit materialized-vs-synthetic record identity without changing game behavior, and only then connect source-derived dynamic `22220→FF50` to original `34A74` row clipping, preserving live `C026/C027` modes and `34DE8` order. Separate DOS framebuffer/IRQ/RNG comparison remains mandatory. Native open-field 34A74 Full #658 **64/64 successful field captures** remain accepted; no gameplay readiness score increase or new CI build is claimed.
