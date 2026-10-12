# Windows Full #659 — build success, source-only test failure

**Owner / branch:** `demo-v1` (M-PLAY-01, Part 14)  
**Authorized by user:** 2026-10-11 Eastern  
**GitHub Actions:** [#659](https://github.com/Hyperbog/RLOFT/actions/runs/38191874472)  
**Exact compiled checkout:** `fcb1d576763b9b75e58a7e33129c3b0d7b36fbdb`  
**Workflow state:** original manual-only blob `296f27bb893994c2862b11a7cfc98a948899378c` restored in commit `5c6dcf3d71cecf776c7c9866ff4e7732a09cbd27` before build completion.

## Independent Actions result

| Gate | Result |
| --- | --- |
| CODE.2 C++17 source syntax | Passed |
| Windows x64 CMake configuration | Passed |
| Full C++ target compilation | **Passed** |
| Full Windows CTest | **243/245 passed; overall failed** |
| M-PLAY-01 normal-launch contract | Passed (CTest #10) |
| M-PLAY-01 offline native trace integrity | Passed (CTest #11) |
| Windows artifact upload | Not executed (CTest failure) |
| Committed/replaced executable | Not executed (CTest failure) |
| Linux build | Skipped |
| Original DOS game/playthrough | Not attempted; not accepted |

Both failed CTests have the **same exact source-only text assertion**:

```text
original_ground_c050_page_toggle_tests (#17)
original_ground_source_proof_batch_tests (#20)
DemoV1/tools/verify_original_ground_c050_page_toggle.py:493
AssertionError: 42-way
```

The pinned source-proof batch reports 12/13 subprobes passed, with `c050_page`
as the sole failure. The C050 verifier checks that the exact phrase
`42-way` occurs in `LEVEL22_ORIGINAL_VGA_C050_PAGE_TOGGLE_AUDIT_20261010.md`;
the document already described 42 unique entries but omitted this exact
phrase. This is **not evidence of an original-byte mismatch or a native C++
compiler failure**.

The uploaded original `CODE.2` (558,689 bytes, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`)
was independently checked against original object-1
`0x1548C..0x15533`: **42 DWORD entries, all unique, all targets in
`0x15534..0x15E37`**, matching the existing source audit.
The audit's heading now says **42-way indirect dispatch**, preserving all
offsets, bounds, and open transitive callback uncertainties. The C050
source-only document assertion was rechecked for **every required term**,
including the formerly missing one. This is a local source-text check,
**not a second accepted Windows run**.

## Acceptance and next gate

Full Windows **#658 (214/214)** remains the previous accepted Windows
build on its older checkout; #659 shows the newer C++ code can compile but
does not clear the full 245-test gate. There is **no new artifact** and
`DemoV1/Windows/ravenloft_demo_v1.exe` remains from the earlier build.
Original DOS pixel, input, IRQ/RNG and campaign acceptance are unchanged.

A new manual Windows Full #660 (or successor) must be explicitly authorized
before dispatch. Use the **new build's exact Git SHA** for normal-path
`Trace-Normal-Launch.cmd` and diagnostic archive identity, and do not
overwrite original CHARS/SAVE.DAT outside an isolated runtime copy.
