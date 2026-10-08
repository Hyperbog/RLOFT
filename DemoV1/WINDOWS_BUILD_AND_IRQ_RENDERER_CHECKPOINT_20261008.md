# Windows build gate and original IRQ/VGA renderer continuation — 2026-10-08

## Actual Windows x64 build verification

The repository's own `.github/workflows/demo-v1-build.yml`
ran on the private `demo-v1` branch after explicit user approval.
A one-time, **marker-only** push trigger was installed because the
GitHub connector does not expose `workflow_dispatch`, and it was
restored to **manual-only** operation after each requested run.
The marker was subsequently deleted.

Three actual Microsoft Visual C++/CTest runs occurred:

| Run | Result | Source issue found / disposition |
| --- | --- | --- |
| [#632](https://github.com/Hyperbog/RLOFT/actions/runs/37785994267) | Build failed before CTest | Three test compilation errors: a duplicate `original` local; missing direct `<initializer_list>` include; obsolete `forced_minimum` result-field name |
| [#633](https://github.com/Hyperbog/RLOFT/actions/runs/37787041004) | Windows build passed; **129/130 CTests passed** | `combat_hand_pair_early_gate_tests.cpp` used actor0 status `0x4000` while expecting the ordinary-hand candidate to survive the original `0x7240` veto |
| [#634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281) | **Windows build PASS; 130/130 CTests PASS; EXE artifact and git binary commit PASS** | Corrected actor0 fixture status before ordinary-hand boundary tests, preserving original `2459C` mask behavior |

Original `CODE.2:245A2..245B7` directly confirms the
`status WORD & 0x7240` test and candidate clearing.
The correction changes only the fixture, **not** source game rules.

The successful run was pinned to source commit
`40e3f14b1072a7a0180ea1b0b4c1cea437137631`.
Windows artifact:
[Ravenloft-Demo-v1-Windows-x64](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281).
The Windows executable is stored at
`DemoV1/Windows/ravenloft_demo_v1.exe` alongside
`ravenloft_demo_v1.exe.sha256`. Verified EXE SHA-256:
`09a3ce4cb5e5d804eee4647413915be7c871789f22cf26e72f13e3f71ff59b35`.

This confirms actual Windows compilation and the 130
registered regression tests at the **pinned build SHA**.
It is **not** a reference-DOS gameplay, pixel/frame,
audio, IRQ/RNG or exact playable-combat equivalence test.
The Linux job remained explicitly skipped.

## Subsequent source-only renderer service execution bridge

After the green build, the following separate new source was committed:

- `include/rloft/combat_irq_display_runtime.hpp`
- `src/combat_irq_display_runtime.cpp`
- `tests/combat_irq_display_runtime_tests.cpp`
- CMake registration for `combat_irq_display_runtime_tests`

The existing `CombatIrqDisplayModel` is a **read-only event
plan**. The new `CombatIrqDisplayRuntimeModel::update`
executes the original `1C258` and `1C2C4` service
order against caller-owned mutable original globals:
`CF59` pending byte, `CF54` full IRQ deadline,
`C050` active mode, `2222C` background pointer,
and the `14580` message buffer.

1. If pending is nonzero, call `291B0(BYTE[C050])`,
   `2939C(0,0x106,0x20,DWORD[2222C])`,
   `3060C(0x107,0x22,buffer14580,1)`,
   `1A8C8(DWORD[C050],0x106,0x20,0x0C,0x100)`,
   then clear `CF59`. Render callbacks may update globals
   before later calls, so all live parameters are re-read.
2. **After** that draw, call the caller-supplied original
   `30700` IRQ service. If unsigned observed tick is
   **strictly greater** than the live `CF54`, call the
   page and background services, set `CF54=0xFFFFFFFF`
   **before** the final `1A8C8`, and composite again.
3. No pending message and no expiry requires only the
   original IRQ callback. Missing *actually required*
   collaborators or an unterminated bounded host copy
   are unsupported. Nothing is silently rendered/clocked
   from modern Windows defaults.
4. `CombatVgaPageSelectionModel::inspect` separately
   reconstructs `291B0 -> 29FEC -> 2AB6C` as original
   `EF4D=page`, `EFE9=page`,
   `F001 = ((0xA000 + page * DWORD[F005]) << 4)`
   with exact unsigned 32-bit wrap. The `F005`
   page stride **must be observed**, not hardcoded to
   an assumed modern framebuffer stride.

The injected collaborators are still **not connected to
the native framebuffer hardware emulation**. The new
source executes callback ordering and original projected
global writes; it does not yet produce or assert matching
reference-DOS pixels. The four full renderer-service
implementations are separate work.

The exact source-equivalent local C++17 execution-bridge
model and focused tests passed strict `-Wall -Wextra
-Werror -pedantic` Debug, optimized Release with
assertions enabled, and UndefinedBehaviorSanitizer.
The standalone harness is **not** the same as the
successful earlier 130-test Windows CTest run;
the newly registered extra CTest was added **after**
that Windows source SHA and has not yet run on Windows.

22 new exact original opcode signatures were independently
checked against uploaded `CODE.2`, raising the audit to
**509 named checks covering 501 distinct x86 instruction
addresses**, plus one direct Strength table, nine hashed
DGROUP data spans and one hashed CS data span.

## Next fidelity gate

Bind the injected `291B0`, `2939C`, `3060C`,
`1A8C8` collaborators to the already existing
`VgaPlanarMemory` and actual DOS image/font sources,
without eliding the `2939C -> 2A380/2E758` or
`3060C -> 303C8` dependencies. Compare original
framebuffer effects, pending/deadline transitions and
post-render IRQ samples against DOS reference traces.

Progress remains **20% (1 of 5 major acceptance gates)**.
No additional Windows build is claimed for the newly
added renderer bridge.
