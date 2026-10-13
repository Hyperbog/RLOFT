# Part 16 — CODE.1 10AA9 original DS-break correction

**Owner:** `demo-v1`; **scope:** canonical source synchronization only.
**Prior demo source:** `Code1.cpp` Git blob `bdfe208db205fc3da467c3bf9d6710869096ddea`.
**Verified canonical source:** `code1-decompilation` blob `85a25d5298d6b88314fd571985f15cf8b7be0af5`.

## Byte-backed correction

The last original CODE.1 source mismatch in `function_10aa9` was its
mode-2..8 DS-break extension result. Original machine code obtains
the requested **absolute** break end from `ESI`, calls original
`10DD7` with that end, receives the **previous** break end in `EAX`,
then executes:

- `10B84: 03 35 F0 52 00 00`: ADD ESI,[52F0].
- `10BB4: 39 C6`: CMP ESI,EAX.
- `10BB6: 0F 86`: reject when desired end <= previous end.
- `10BBC: 29 C6`: SUB ESI,EAX; usable extension is desired - previous.

The earlier demo implementation incorrectly compared `size<=old` and
computed `size-old`, where `size` was the request increment.
The synchronized source now compares `desired<=old` and computes
`desired-old`. Other CODE.1 instructions are unchanged.

## Evidence and gates

- The owner-provided original CODE.1 has full SHA-256
  `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`.
- Independently inspected original bytes at physical file offsets
  `3D2A8,3D2BB,3D2D8,3D2E0` confirm these instructions.
- Canonical recompilation Windows #138 confirmed the corrected source
  allocator with 62/62 CTests; #153 passed its 63/63 current source suite.
- `tools/test_code1_10aa9_break_growth.py` detects a reversion of either
  expression. With `--original /path/to/CODE.1`, it checks the complete
  original file SHA and source opcode windows. Original media is optional,
  private and **not** published in CI.
- Demo CMake registers `original_code1_break_growth_contract_tests` in
  the existing Python regression group. Successful registration alone
  is not a Windows build or execution claim.

## Remaining game integration boundary

This source sync is **not** the same as wiring reconstructed CODE.1 heap
allocation into the DemoV1 game host. It does not implement menu options
0/1, save/CHARS round-trip, complete DOS/4GW memory service parity or a
normal gameplay startup. The published original-intro launcher still
stops at the explicit in-memory character transfer boundary. Full
CODE.2 source drift is a **separate** audit; do not overwrite its
current demo branch until individual differences are authenticated.

The next demo milestone is normal CODE.2 menu -> CODE.1 character creation
-> CHARS serialization -> ordinary CODE.2 level entry, with original-DOS
behavior separately verified. Neither reconstructed source tests nor
Windows CI alone qualify that gameplay milestone.
