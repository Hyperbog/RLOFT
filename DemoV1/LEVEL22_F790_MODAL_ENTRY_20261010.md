# CODE.2 13118 / 112EC / 1134C native modal-entry boundary — 2026-10-10

**Branch:** \`demo-v1\`. **Status:** verified original-source integration code, still isolated from Win32. No source gameplay implementation or DOS compatibility claim is inferred from an unbuilt C++ test.

## Original binary and sequence

Immutable supplied CODE.2 (558,689 bytes, SHA-256 \`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314\`) owns the behavior. Object-1 starts at file \`0x3AF24\`.

- Original \`13118..1312A\` clears 6DA8 deadline and 6DAC pending feedback, then \`13133\` calls \`112EC\`.
- \`112EC\` calls \`11268\` four times (0x50-pixel actor lanes), allocates \`0x1400\` bytes, and calls \`292BC\` with (row=\`0x62\`, column=0, height=\`0x10\`, width=\`0x140\`) to retain exactly 5,120 indexed pixels at 7F68.
- \`1133C..11351\` loads global held item WORD 8600 with sign extension and known BYTE 8602 with zero extension, then E8 \`1134C\` targets \`F790\`.
- Original \`F790\` restores 7F68 through \`2932C\` **before** pending/held checks. Initial 6DAC is zero, so empty held item restores only, while a held item drives F66C, source 30CC4/30CA2 and final 30700 tick reseed.

These source ranges were independently checked against uploaded original CODE.2 and match existing certified reconstruction/report references. The 11316..11350 machine window is now pinned by the existing automated binary verifier.

## Implemented and tested interface

\`include/rloft/original_f790_modal_entry.hpp\` composes the existing source-backed components into a ONE-time modal entry: check 6DAC cleared, resolve 1134C source arguments, preflight text/RES2:3/font, reset source modal, invoke a caller-provided \`112EC\` actor-column painter, capture the one 320x16 page, then draw the first true F790 event with the caller's **single existing OriginalRandom** and 30700 observer. A second capture without modal exit is rejected. It performs no source F790 call on a host frame refresh.

\`PartyModalPresenter::draw\` gains a default-false \`source_f790_owns_ribbon\` parameter. When true, this allows actor columns to be drawn WITHOUT the legacy provisional 70-Hz text strip contaminating the source 7F68 capture. The default false path remains unchanged, so there is **no change to the running native UI** in this checkpoint.

Existing \`original_f790_runtime_description_tests.cpp\` exercises empty first-call background, exactly one original event, duplicate-entry rejection, held-item shared RNG/tick reseed, and 128 RNG-free host redraws. Original binary/offline source assertions are added to \`tools/test_original_inventory_f790_cadence.py\` without registering a new CTest.

## Limitations and next gate

This batch does not wire \`OriginalF790ModalEntry\` into \`win32_main.cpp\`, nor replace the current provisional live ribbon. Actual original 11268 full party-column pixels remain separately DOS-pending. Complete F790 event fanout for equipment, container, actor toggle and selected item, actual source RNG ownership at live event boundaries, and real native-vs-DOS frame/IRQ comparison are open.

**No Windows build was triggered.** Last authorized Full #651 passed 182/182 on older code; the current registry still has 210 CTests when Python is present. Current source changes are unbuilt. No subsystem percentages should be increased for this isolated interface. Follow \`ORIGINAL_EXECUTION_FIDELITY_CHARTER.md\` and manual build authorization.
