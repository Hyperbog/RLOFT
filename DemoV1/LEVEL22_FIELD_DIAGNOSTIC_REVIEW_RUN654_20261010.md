# Field diagnostic review — Windows Full #654, Level22 — 2026-10-10

**Scope:** analysis of user-supplied native F12 session ZIP from installed original GOG game.gog media. **Evidence remains local to the user's report;** this repository checkpoint contains aggregate metadata, source identities and bounded source questions only, not installed proprietary files, party asset pixels, original binaries, player names or machine-local paths.

## Version and capture validity

- Trace manifest: \`rloft-level22-diagnostic-v1\`, \`native-preview-not-original-DOS\`, build source \`848ae8ba9e3cddde31bb5dd5ab8dbd26f97fc223\` (**the successful Windows Full #654**), indexed VGA 320×400, 256×3 VGA-6bit palette; original installed GOG CD image.
- Full log: **1,877 contiguous events** ending in \`session.finish\`; no diagnostic error events observed.
- Primary indexed VGA archive: **96/96** automatic captures (expected configured cap), **7** F12 marker captures with independent indexed pixels/palette and draw-command evidence, 16 distinct (screen,pixel-hash) states among 96 frames.
- Auto screen mix: **85 party_panels**, **6 viewport**, **3 top_edge_menu**, **2 automap**.
- Additional **12 renderer/ indexed VGA captures** exist in a separate namespace after the F12/early auto-capture window, together with source-state TSV rows. Do not conflate renderer/ frame_0012 with the primary frame_0012.
- 82 source-*projection* events \`f790.source.ordinary\`, exactly 82 following \`f790.source.flags\`: **79 × CODE.2 119FC** ordinary swap (saved source slot local), **3 × CODE.2 118FF** explicit-empty container insertion. All recorded source known/pending flags are (0,0); this session does NOT exercise F790 pending spell-scribe artwork, rejection, or expiry. Read-only trace events are not proof that the original F790 draw/RNG is live wired.
- \`party.encumbrance.open\` occurred twice. Party-screen panels, original portraits/figures, item descriptions, container grid, top menu and automap appear in captured indexed frames. Their pixel fidelity to DOS has not been accepted because **no paired original DOS capture is included**.
- Initial primary F12 markers 1–7 are viewport, party, party, party, party, automap, viewport. IRQ values **764, 800, 1387, 1794, 4267, 4339, 4413**.

## Priority P1 — original RES4 ground decoding fails (confirmed from all 12 renderer captures)

For **every renderer/frame_0001…0012.tsv**:

\`\`\`
native_lower_surface  flat-fill-fallback;other-modes-pending
ground_original_res4_verified  0
ground_original_res4_error  RES entry is not an EH type-1 raster
ground_original_res4_base_from_INIT  -1
ground_original_res4_absolute_entry  -1
original_C026_initial  1
\`\`\`

The actual frame pictures show the visually flat lower-ground fallback. The independent renderer captures follow native movement **cell (4,4) → (12,7)**, original fixed-coordinate metadata **(294912,294912) → (793789,469870)**, and approaching Assassin sprite. Thus, *movement is observed in the separate renderer series*: static F12 primary camera snapshots and keyboard actions after marker 7 must not be misinterpreted as a broken movement system.

Source authority is \`CODE.2 16E55..16E90\` establishing \`DGROUP:258F8 = u16(INIT+0x0E)\`, then \`1F388\` selecting signed level header +7 plus this base into RES4 bank 4. \`2F29C\` requires **type 1**, and \`34A74\` consumes the decoded 64×64 texture, not an unrelated type-6 wall texture. See \`ORIGINAL_LEVEL22_RES4_INIT_BASE_RESOLUTION_20261008.md\`, \`ORIGINAL_LEVEL22_RES4_GROUND_SOURCE_AUDIT_20261008.md\`, and \`ORIGINAL_CODE2_34A74_TEXTURED_GROUND_PIPELINE_20261008.md\`. Source original initial C026=1 expects the textured path, though dynamic C026/FF50/C027 still require original-state comparison.

**Root-cause remains unproven**: possible resource bank/index resolution, original GOG media variant, archive entry type/decoding, or source-specific loading state. The failure string alone does not identify which. Never substitute an arbitrary 64×64 candidate or accept the flat fallback as faithful. Next verification requires the **already built artifact's** \`Probe-Level22-Ground.cmd\` output on the same original media; compare the probe's RES4 candidate list, archive entries, INIT base and signed Level22 selector with the failed live lookup. No user-authorized new Windows build is needed simply to run this existing diagnostic utility. Correct only after immutable source and media evidence.

## Priority P2 — native open-container UI lifetime across screens (observed; fidelity pending)

F12 #5 is party_panels \`containers_open=1\`; F12 #6 is automap \`containers_open=1\`; F12 #7 is viewport \`containers_open=1\`. The active native \`win32_main.cpp\` host PartyPanels escape calls \`ui.close_screen()\` without a matching \`runtime.inventory().close_container()\`. The runtime has original \`110B4\` actor-container close lifecycle, and level transition guards against open containers. **Do not blindly add host close:** prove original \`13118/12E70\` modal exit, 6FA0 context persistence and 110B4 invocation first, including which DOS input exits the modal. Then verify that Level22 exit and level-transition continuity remain valid.

## Priority P3 — provisional held-object ribbon still drawn by native renderer (known incomplete)

F12 marker #3/4 indexed pictures and draw traces show the yellow, flat-background text strip at VGA row \`0x62\` (fill height 12), rather than source event-owned F790 2932C background restore plus original F66C RES2:3 tiles. Existing \`OriginalF790ModalEntry\` and \`OriginalF790OrdinarySlotDispatch\` compiled and passed isolated CTests in Full #654, but production Win32 **still uses its default legacy text-strip branch**, source event artwork/RNG not live wired. Do not infer F790 equivalence from trace projections.

## Next gates

1. Obtain original **RES4 ground probe** report for the same GOG media; compare resolution with original source and native loader.
2. Reconstruct and connect original modal close/11AD8/11B63/F790 precise source event paths; avoid 70-Hz RNG mutations.
3. Pair source-aligned original DOS video pixels/ticks and 320×400 native indexed captures before changing equivalence/readiness scores.
4. Revalidate any new native gameplay implementation with original bytes first, then CMake/Windows test build by user authorization. No code/CI build is initiated by this **documentation-only** checkpoint.

The locally verified diagnostic-file sanity checks accepted the 457 primary archive entries (including seven F12 saved recent-event files), contiguous event stream, independent 12-renderer frame series, correct sized 128,000-byte indexed pixels and 768-byte palette dumps; no untrusted instructions or original assets have been committed. Other native game versions must be checked separately.
