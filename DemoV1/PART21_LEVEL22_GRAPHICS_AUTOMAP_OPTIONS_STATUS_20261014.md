# Part 21 — Enhanced PC Alpha: Level 22 graphics and UI triage

**Date:** October 14, 2026  
**Product policy:** enhanced native PC port; source-DOS parity is optional research.  
**Release status:** Pre-Alpha. Owner has confirmed build #819 reached Level 22 with audio, but **has not accepted Level 22 graphics, inventory, automap or top menu**.  
**Latest source-matched Windows candidate:** `2e8bfae494fbfab34d3fc43a7f87621d6ac343c5`.  
**Windows CI:** [Windows CI run 38623194666](https://github.com/Hyperbog/RLOFT/actions/runs/38623194666); Windows compile, focused native tests, launcher parsing, and Alpha artifact publication **passed**. Artifact ID `11923650326`, `Ravenloft-Continuous-Alpha-Windows-x64`. Package ZIP and executable SHA-256 checked locally.  
**Current tested executable SHA-256:** `14c5e1ab06a92933c54fcca14a85ed3ce8084c61a58613bcb615e65a413a641a`.

## Separate defect categories (per owner)

There are **four Level 22 categories** and **one separate title-screen issue**. Do not collapse title Continue into a Level 22 issue.

| Area | Owner observation | New source remediation | Acceptance |
| --- | --- | --- | --- |
| Level 22 graphics | Trees/textures poorly placed; colors incorrect; overall rendering unpolished | Prior Part 19–20 native RES3/RES4 rasterization and per-sprite source-top projection retained. Current candidate's RES8 native backdrop shade path clamps indexed underflow instead of wrapping to palette indices 248–255; regression retains original arithmetic separately. | **Open**: actual output on the owner's PC must be visually inspected, including ground, sky, distance, tree/NPC placement and shadowed colors. |
| Inventory | Still buggy | Prior mirrored second bracer/gauntlet RES1 sprite omission fixed with a safe reflection, plus synthetic C++ regression. Ordinary/equipment/container state handlers remain distinct and incomplete actions are not silently fabricated. | **Open**: owner still needs to identify remaining wrong slots, pointer behavior or item state in live inventory. |
| Automap | Slightly bugged | Fresh indexed framebuffer clear before each automap presentation prevents leaked old world pixels; player marker and heading arrow clipped to actual map tile rectangle to avoid painting over chrome. Map tests extended. | **Open**: owner to test map boundary, arrow and controls. |
| Top menu | Only Automap and Quit work | Added **native in-game Options panel** in original indexed game window, real click regions for Music On/Off, native XMIDI stop/restart and return to game. Off-image/letterbox mouse input is no longer translated into first menu entry. | **Partially open**: Automap, Quit and Options now have handlers; **Rest, Pray, Memorize, Load and Save remain deliberately unimplemented** until supported by genuine gameplay/save systems. |
| **Title Continue** | Does not work | **No change in this phase.** Original title Load/Continue is separately blocked on safe complete native world save/restore. | **Separate open issue**: don't misrepresent a newly created character as restoring prior gameplay. |

## Guardrails and evidence

- The original CINE00/CINE12 audio, DreamForge/SSI/TSR presentation, lady/cat pacing, card-flip direction, and character creation remain source-preserved. Audio was **reported audible** by the owner; this is not an end-to-end audio timing certification.
- No separate Options utility window or folder picker. This is one continuous native game with its own indexed Options overlay. Music preference is **session-local**; permanent settings persistence is not yet implemented.
- New `enhanced_options_presenter_tests` and `enhanced_level22_ui_source_tests`, plus existing automap viewport/controls tests, run in the focused Windows suite.
- No real Windows capture has yet verified these latest changes. Passing MSVC and CTest does not imply that Level 22 was visually fixed.

## Next priority order

1. **Owner first-frame screenshot/video** of this exact source-matched artifact to assess colors and tree/NPC placement; fix the identifiable transformation/palette issue rather than adding invented geometry.
2. **Inventory repro** (which item/slot, click/drag behavior, and held cursor/state) and repair live state plus modal pixels.
3. **Automap screenshot** identifying the remaining chrome/arrow/tile interaction issue if ghosting/clipping changes are insufficient.
4. Finish actual safe **native save/load**, title Continue, and top-menu Load/Save; then complete Rest, Pray and Memorize with original rules-backed logic. Do not fake those actions with success messages.

The early continuous Alpha milestone remains owner-confirmed; complete Level 22 functionality and Alpha field acceptance remain pending.
