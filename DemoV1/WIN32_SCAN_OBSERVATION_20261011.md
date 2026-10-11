# Win32 scan-code observation and DOS IRQ bridge gate — 2026-10-11

## Native diagnostic boundary

The native model `Win32ScanObservationModel::decode` is a
**portable decoder of documented Win32 keyboard-message metadata**,
not an original DOS routine and not a live keyboard-to-DOS converter.
For `WM_KEYDOWN`, `WM_KEYUP`,
`WM_SYSKEYDOWN` and `WM_SYSKEYUP`, it
records repeat count (LPARAM bits 0–15), OEM scan (16–23),
extended marker (24), ALT context (29), previous key state (30)
and transition flag (31), independently of the virtual-key value.
The Microsoft Learn input-message tables specify these fields:
https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-keydown
https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-syskeyup

**Strictly non-authoritative raw candidates:** Only a single
nonextended message with a plausible original scan value and
consistent transition gets a candidate port 0x60 make/break byte.
E0/E1 extended sequence recovery, layout-dependent OEM scan data,
repeated key IRQ cadence, and missing/contradictory metadata
remain explicitly **unresolved**. A candidate is not accepted as
actual DOS hardware input.

## Normal Windows gameplay is not wired

`DemoV1/src/win32_main.cpp` now uses this decoder
**only when an existing developer session trace is active**.
The host records one `key.scan_observation` event
with two integer fields:
- a = physical OEM scan byte, not a virtual-key or text character.
- b bits 0–7 = observation status, 8–23 = repeat count,
  24 = extended marker, 25 = ALT context, 26 = previous-down,
  27 = key-up, 28 = system message.

This is passive forensic instrumentation. The original
`WM_KEYDOWN/WM_KEYUP` menu, character and exploration
handlers remain unchanged and the tracer does not call
`OriginalDosKeyboardSession::receive_port60`,
`dispatch_30950`, or change F571/F572/F588.
`WM_SYSKEYDOWN` and `WM_SYSKEYUP` are
only observed, not intercepted; default Windows processing remains.
No character text, names, file paths or original copyrighted data
are logged by this observation.

## Verification and acceptance

The standalone C++17 diagnostic decoder test passes locally under
GCC 14 with `-std=c++17 -Wall -Wextra -Wpedantic -Werror`.
It tests make/break, system ALT, nonextended versus extended
arrow/right-ALT paths, repeat counts, key transition flags,
missing scans and 64-bit sign-extended LPARAM input.
`win32_scan_observation_contract_tests` is a
**source-only CTest**, guarding the opt-in tracer and ensuring
the normal gameplay code does not call the DOS IRQ models.

This **does not** certify a Windows build, trace runtime behavior,
DOS PIC timing, E0/E1 prefix parity, real F0C8 callback installation,
full gameplay engine integration or M-PLAY-01. The native Windows
Full build remains manual and unrun for this source revision.

## Developer trace analysis for first Windows keyboard field check

The native opt-in session tracer records one `key.scan_observation`
JSON-line event per observed Windows keyboard message, with only an
OEM scan byte and a packed status/repeat/extended metadata integer.
The source-only tool `tools/summarize_win32_scan_trace.py`
reads the trace's `events.jsonl`, counts observed
single nonextended candidates, extended events, unresolved repeats
and invalid metadata, and identifies the **sequence number** of
the first unresolved event. It never outputs a scan sequence,
virtual-key codes, typed text or raw event contents.

The matching four-case Python regression passed locally. It checks
candidate/unresolved aggregates, first failure sequence, zero-key
captures, invalid status and out-of-order sequence rejection.
A zero-unresolved report is NOT evidence of original DOS parity:
the source stream, IRQ cadence, installed F0C8 callback and
game-facing input wiring still require independent validation.

Example internal developer use after a manually approved, new Windows
executable has been built and an opt-in trace captured:

`python DemoV1/tools/summarize_win32_scan_trace.py C:\path\to\events.jsonl`

This is an offline internal diagnostic, not a command-line
implementation of the desktop game, and changes no native controls.
