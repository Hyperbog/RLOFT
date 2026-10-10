# Level22 party/inventory background — original CODE.2 full-descriptor blit repair

**Development mandate:** finish entire original Level22 playably, through the original mist ending, before independent Level23 gameplay. No modernizations.

## Critical original archive-slot erratum (2026-10-10)

**Supersedes every claim below that the original `slot=2` party modal
images come from `RES0[INIT[0]+resource]`.** Source `Code2/Code2.cpp`
`2E7F0→2F29C`, independently matched original `CODE.2` instruction
windows, resolves slot 2 to original file **RES2**, entries 0..37.
The 200-frame × 320×1 strips observed in the first Build #649 ZIP
came from the port's **wrong RES0 namespace**, not the original
party-modal resource. The full-descriptor `2E510` semantics in this
historical report remain byte-verified; the old *resource selection*
in the original implementation was incorrect. The corrected native
route is `assets_.type1(2,resource)` at every actor-modal
source callsite, using no RES0 INIT offset. [Full six-window binary
proof and post-#651 field evidence](LEVEL22_CODE2_RES2_BANK_ROUTING_FIELD_AUDIT_20261010.md).

## Reproducible native failure

The user's first real `--dev-trace` ZIP, session `1791588336634-20980` from Full Windows Build #649, records black/partial inventory backgrounds, equipment and portrait graphics on isolated panel columns, and type-1 graphic draws of dimensions **320×1 with 200 frames**. `frame_0051.draws.tsv` shows repeated **frame 0 only** calls with row 0, 69, 114 and 296, and actor-lane columns 0, 80, 160 and 240. The native source `PartyModalPresenter::draw_res0_local` indiscriminately used `framebuffer.draw_frame(raster,0,...)`, so a 200-row original descriptor rendered only a one-pixel strip.

## Direct executable proof — source CODE.2

Immutable supplied `CODE.2` physical object-1 base `0x3AF24`, logical offsets below, disassembled with `objdump -D -b binary -m i386 -M intel`. The source semantics are **not guessed from filenames or appearance**.

- **`2E510..2E535`** resolves the original graphics descriptor with `2F29C(resource,bank)`, then calls **`2E4A4(row,column,first_frame=0,count=-1,descriptor)`**. The negative `count=-1` is a source sentinel requesting the descriptor's full frame count.
- **`2E4A4..2E4F3`** specifically checks for `count==-1` at `2E4BB`, substitutes `WORD[descriptor+2]` (frame count) at `2E4C0`, reads `WORD[descriptor+4]` (height) at `2E4C8`, and loops from first frame to count at `2E4D1..2E4EA`. Every iteration calls `2939C(frame,row,column,descriptor)` and increments **the output row by the descriptor's height** before the next frame. Thus an original decoded `count=200,width=320,height=1` descriptor MUST paint **200 consecutive scanlines**.
- **`2975C..29788`** is separate. It resolves the descriptor and calls `2A380(selected_frame,row,column,descriptor)` **once**. Original actor status badges/chrome elements use this individual-frame operation. Changing every Type1 blit to all-frames would incorrectly multiply those graphics.
- **`0FF3C:0FF48..0FF59`** active actor background calls `2E510(0,actor_lane,0,2)`; **`11268:11286..11297`** inactive actor background calls `2E510(0,actor_lane,2,2)`.
- **`1118C:111A7..111BB`** draws local RES0 resource **4** with `2E510(0x128,actor_lane,4,2)`; its other source chrome calls `2975C` and must keep one-frame semantics.
- **`10BBC:10BE1..10BFD`** draws body/equipment background resource `sex + side*2 + 5` with `2E510(0x72,actor_lane,resource,2)`.
- **`0FF3C:10077..100BF`** condition and status icon resources **0x13** and **0x12** go through `2975C`, correctly drawing just the selected frame.

## Native correction

`OriginalPartyModalBlit` is a typed two-operation source binding:
- `complete_2e510` uses the existing native `IndexedFramebuffer::draw`, which sequentially invokes `draw_frame` at `row+frame_index*height`, including original clipping.
- `frame_2975c` preserves the original single-frame `IndexedFramebuffer::draw_frame`.

`PartyModalPresenter` now invokes the complete operation **only** for active/inactive actor backdrops, inventory grid and equipment-body backgrounds at the exact original CODE.2 callsites. Other status/chrome icons and the rest of the game continue using single-frame routines. No change was made to the decoded original RES archives or framebuffer pixel engine; no synthetic graphics or alternative design was introduced.

## Regression and acceptance

New `original_party_modal_blit_tests` creates a synthetic raster identical in **descriptor shape** to the real trace (200×320×1), with unique pixel indices for all 200 rows, and verifies:
- All 200 rows are painted and the original per-frame draw audit emits 200 ordered operations.
- A `2975C` draw of frame 17 emits **one** operation and affects exactly one row of pixels.
- A three-frame, two-row sprite advances rows by two and clips at the native 320×400 buffer boundary without wrapping.
- Framebuffer draw-audit hooks remain optional.

**Native build status:** the last user-authorized Full Windows build #649 passed **179/179** earlier tests. Current source now registers **181 CTests** (the original RES2 bank fix test #180 and this draw-order regression #181) and is **unbuilt**. No Windows/manual original DOS visual comparison has accepted the updated inventory. After next explicitly authorized build, ask user to use `Diagnose-Level22.cmd` to inspect all four actor slots, encumbrance art, inventory grid, portrait layering, containers, modal switching and F12 captures.

**Readiness:** 49.8% provisional engineering-weighted (20 systems, 122 named sub-systems); original-DOS parity acceptance at current code revision 0%. Scores unchanged because code and unit assertions alone do not prove original pixel and interaction parity.
