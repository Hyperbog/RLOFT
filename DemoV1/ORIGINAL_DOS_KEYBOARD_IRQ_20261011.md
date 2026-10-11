# Original DOS keyboard IRQ source audit — 2026-10-11

The uploaded original CODE.2 is 558689 bytes, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
The original binary verifier checks five bounded SHA-pinned regions,
45 exact opcode windows, four near calls, and all ten original DWORD
CS dispatch entries at `307C8`. It passed locally on the authentic file.

The held-key array resides at `2B6C4`, its byte count at `F588`,
the latest scan bytes at `F571/F572`, and modifiers at `F573`.
`30748` removes held scans, `307F0` appends/reorders scans and
`308BC` tests membership. The source dispatch at `30950`
processes make/break events. The ten-entry keypad table covers
`47h..50h` inclusive: scan `50h` changes `WORD[258FC]` and
`WORD[258FE]`; scan `51h` does not. A source-table regression
caught and corrected that native implementation boundary.

The special-event route at `30A1F` handles Alt+Q, scan `43h`,
scan `19h` and space. The native C++17 model and tests cover
these effects without pretending Windows events are already
DOS port-60 scan bytes. Exact native tested source blobs are
committed; the standalone strict GCC 14 test passes. Source-only
Python registration does not establish original DOS runtime parity.

**Not wired to Win32:** No native WM_KEYDOWN/UP mapping, port 0x60
hardware emulation, keyboard IRQ/PIC timing, F0C8 callback target,
or current Windows Full build was accepted. M-PLAY-01 still needs
an actual ordinary launch and user field test.

## Adjacent original modifier/Space queries

The two directly decoded leaf routines have been integrated into the
same native IRQ state. Original `30ABC` reads unsigned BYTE
`F573`, ANDs it with the caller's DWORD mask and returns the
masked integer without consuming the modifier state. Original
`30AC8` returns unsigned BYTE `F574` and then clears
that byte; it does not change the pending scan code.

The complete original source slice `30ABC..30AD6` has SHA-256
`67ea2baee9861d6009e9a9f0834aed232645999bcec90243b9a39e2ecd109d6d`.
Together with the previous evidence, the binary verifier now guards
**six** source slices, **45** opcode windows, four direct E8
edges and ten dispatch entries. These checks passed against the
uploaded original CODE.2.

The C++17 methods `modifiers_30abc` and
`consume_space_30ac8` passed the locally compiled strict GCC 14
test, including multi-bit masks, zero matches, repeated Space flag
consumption and preserving `F572`. No Windows event routing,
keyboard IRQ timing or original DOS gameplay acceptance is claimed.
