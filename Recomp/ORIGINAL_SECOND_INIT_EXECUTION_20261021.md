# Part 32 - Original CODE.2 second INIT handoff (2026-10-21)

STATUS: Source callback and explicit driver committed; awaiting original-media opt-in execution evidence. NOT a playable recompilation.

## Verified preceding path

The original-media Windows, Linux and source-link regression run at commit 2a62048ce3 passed. That baseline follows original retail 353A0 -> 20608, host-backed mouse/DPMI/DOS services, original 2F468 resource selection and 290F4 video/palette, compiled 20294 page operations, original 5E38 GAME.OPT absent path, 28F58 sound volume, 31218 CHARS access and the 312A0 -> 37E77 DOS read-only file open. At that checkpoint the next original source call is 20440 -> 16D04.

The actual second 16D04 execution was NOT part of that baseline. The source stopped at an explicit host dependency rather than fabricate success.

## Current changes

- Recomp/src/canonical_code2_runtime_owner.hpp: borrow original CODE.2 Application30 INIT source callbacks and expose explicit continuation of the pending second 16D04 host boundary.
- Recomp/src/code2_startup_gate.cpp: after the original 20440 source call reaches the 16D04 callback, keep the actual C++ game-entry worker stack suspended; when host confirms the original INIT callback table is attached, execute compiled code2::function_16d04 on that worker stack against the SAME original Machine/DGROUP/heap. No synthetic source result replaces this function.
- Recomp/src/code2_original_res_startup.cpp: attach the already initialized Application30 table from the same persistent resource Session and verify that source 16D04 opens INIT for a second time without resetting the guest RAM base. Add opt-in driver to advance past second INIT and report next host service.
- Recomp/src/code1_code2_staged_boot.cpp: an explicit --advance-original-16d04 option for executing the additional source path; ordinary original-media acceptance remains unchanged.
- .github/workflows/recomp-milestone-windows.yml: separate bounded exploratory original-media Windows step records the actual source result or next blocker, without promoting an unverified experiment to gameplay acceptance.

The original INIT file remains read-only, original game media is taken from the user's repository source branch, and no proprietary bytes are committed in code.

## Next engineering boundary

Use the exploratory original-media CI output to determine whether source 16D04 returns, which next original host-service callback is requested, or whether the source runtime encounters a genuine unhandled C++/DOS dependency. Only add the observed callback to the same guest session. Do not mark 1FC14 game-loop execution or any live frame complete until original source calls them and maintains state across multiple ticks.

Existing original DOS R.EXE child-supervisor protocol remains CODE.2 first; CODE.1 launches only on genuine CODE.2 status 10, and authentic CHARS transfer is still required to connect real character creation.
