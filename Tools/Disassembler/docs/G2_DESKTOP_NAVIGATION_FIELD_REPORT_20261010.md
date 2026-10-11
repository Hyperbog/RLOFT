# DOSDis desktop original-MZ navigation field report: direct CALL, Hex Go, References

**Owner observation (2026-10-10 local date):** In Windows desktop
build #43, the exact-original Dungeon Hack `CHECKSYS.EXE` loads,
analyzes, displays disassembly and exports JSON. Three follow-up
interaction observations:

1. Double-clicking the visible `call 017Dh` instruction did not navigate.
2. Switching to **Hexadecimal** and using **Go** did not visibly
   reposition that pane.
3. **References** shows a few references for the current initial
   instruction; double-clicking the existing references **does work**
   and navigates to their linked destinations.

This is a reproducible **desktop navigation/UI contract defect** and
limited-view discoverability issue. It is NOT evidence that the
original source is corrupt or that CALL decoding is inaccurate.
**Working resolved-reference double-click navigation must be retained.**

## Exact known source-backed mapping

`CHECKSYS.EXE` was individually original SHA/size verified:
SHA256 `60070e69c2ca61e1adb6fc82bf4a3ed2652a576e40d610ec4f2b83df237fdc22`,
15,882 original bytes; original MZ header 512 bytes and backed load
image 15,370 bytes. Module-relative initial CS:IP `0000:0000`.

User screenshot: `0000:0025` contains `E8 55 01`, an x86-16
direct relative near CALL. Its displacement `0x0155` is measured
from `IP=0x0028`, producing the candidate same-segment destination
`0000:017D`. Source physical coordinates are:

- CALL at `0000:0025` -> original `file+0x00000225`.
- Direct target `0000:017D` -> original `file+0x0000037D`.

These are **stored-file/module-relative** addresses, never proof
of an actual runtime loaded CS, callee execution or function boundary.

## Root cause confirmed from Windows WPF source

- Only `ReferencesGrid` had `MouseDoubleClick`. The
  `AssemblyGrid` had selection logic but NO double-click control-flow
  navigation event.
- `AssemblyGrid_SelectionChanged` replaced
  `ReferencesGrid.ItemsSource` with the currently selected
  instruction's incoming/outgoing subset. It had NO all-graph
  references presentation; this explains the initial few entries.
- `NavigateMz` selected a grid row, relying on
  `SelectionChanged` to move the hex text. Selecting the SAME row
  a second time does not trigger that event. Hex had NO physical
  position indicator and the text control could retain its old
  scroll position after replacement. It also sought decoded
  instruction rows even when the user only requested hex bytes
  at an arbitrary originally backed CS:IP position.
- These are source/UI event design issues, not evidence of an
  incorrect `Dos16ControlFlowAnalysis` candidate graph.

## Committed SOURCE-ONLY changes — NOT YET BUILT

- Added `Dos16Navigation.DirectTarget` in the shared .NET core:
  resolves ONLY a unique same-segment `call`, `branch` or
  `branch-taken` edge from the existing source-backed candidate
  graph. Ignores speculative `call-return-candidate`,
  `branch-not-taken`, fallthrough and unresolved far/indirect
  targets. Ambiguous/foreign-segment targets fail closed. NEVER
  parses assembly strings or guesses runtime addresses.
- Bound actual `AssemblyGrid.MouseDoubleClick` to the analyzed
  direct transfer. On `call 017Dh`, it navigates to
  `0000:017D`, opens Disassembly and scrolls to the destination
  if mapped; unresolved transfers show an explanatory status.
- Added labeled **Hexadecimal target** readout with source
  `CS:IP`, original physical file offset, and the aligned
  first displayed hex row. Every hex text replacement resets the
  caret/vertical scroll to the new first line. **Go** in the
  Hexadecimal tab now maps ANY validated source-backed MZ byte,
  without fabricating a disassembled instruction boundary,
  even if the selected Assembly row is unchanged.
  Physical `file:0x...` navigation remains supported; LE
  hex navigation also refreshes if the selected row has not changed.
- The References tab defaults to the **full candidate graph
  edge list** (e.g., 394 edges in the owner-supplied CHECKSYS JSON)
  with **Source**, **Destination**, **Kind**, **Evidence** and a
  count. An optional **Selected instruction only** checkbox
  restores the previous incoming/outgoing selection-based view
  without losing the functional double-click navigation.
  Unresolved dynamic/far references stay unresolved.
- Clearing/opening an LE project without a stored graph resets
  the global references to prevent stale results.
- Added synthetic source-only regression expectations for
  unique direct CALL, taken branch vs fallthrough, unresolved
  indirect call, foreign segment and ambiguous direct destinations.

All changes remain **UNCOMPILED / UNEXECUTED**.
The last successful compiled Windows build remains #43 at code/test
SHA `244718cc7f86ba7ee2815cfc1df2d27f9be271b0`, **before**
these fixes. This report is NOT a claim that the new controls work
on the owner's machine. The canonical workflow remains
`workflow_dispatch`-only and CLI distribution remains disabled.

## Acceptance checklist for the next owner-authorized desktop build

1. Compile/publish Windows WPF desktop only, run the existing core
   smoke/project suites plus the new synthetic navigation checks,
   and pin both canonical source and temporary PR merge SHAs.
2. Open original `CHECKSYS.EXE`, run Analyze, double-click
   `0000:0025 call 017Dh`. Confirm Disassembly visibly selects
   `0000:017D`. No runtime/ISA correctness inferred.
3. Switch to Hexadecimal, type `0000:0025`, click Go.
   Confirm the position readout says `file+0x00000225`
   and the first visible aligned hex row is
   `file+0x00000220`. Repeat with `0000:017D`: target
   `file+0x0000037D`, first visible row
   `file+0x00000370`. Check that repeating Go to
   the same address resets the view.
4. Use `file:0x225` Go and verify an original physical view;
   try an invalid/unbacked CS:IP and verify rejection.
5. References defaults to a list of all analyzed candidate
   edges, with source/destination/edge kind. Check an actual
   resolved reference double-click still navigates correctly;
   enable the Selected-instruction filter and confirm it
   narrows to the current instruction, including incoming edges.
   Unresolved destination rows must not invent targets.
6. Regression-check one LE executable's references, hex Go,
   analysis and project save/open. No original binary writes,
   no original disassembly or game bytes in hosted CI.

**Classification:** OWNER FIELD BUG DOCUMENTED; SOURCE FIX
COMMITTED; BUILD AND REAL GUI VERIFICATION PENDING.
