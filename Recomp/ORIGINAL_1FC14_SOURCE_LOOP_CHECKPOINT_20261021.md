# Part 25 — original CODE.2 1FC14 keyboard and persistent source-loop frontier (2026-10-21)

**Status: implemented and validated source continuation, NOT a playable native Ravenloft game.**

## Authenticated original call path

The reconstructed original CODE.2 source shows `353A0 → 20608 → 20440 → 1FC14`, where `1FC14` contains its original uninterrupted keyboard/input and world-update `for (;;)` loop. The loop requests `30B40` through `Application45Boundary::call_30b40`. When that callback was null, the original reconstructed source used zero as the key and spun without a host suspension point.

Previous source execution in the SAME canonical 16 MiB guest advanced through the original INT33/DPMI mouse setup, DOS disk query, `2F468` archive selection, `290F4` VGA initialization, initial `20294` page clear, `5E38` absent options, `28F58` volume, `31218` CHARS probe, `312A0` read-only DOS open and a second, fully executed `16D04` authentic INIT read. Original `20294` page operations and `29234` palette then ran before entering `1FC14`.

## Work completed

- The canonical `Application45Boundary::call_30b40` now invokes a real stackful source callback. It suspends the running original guest C++ thread as `Original1FC14KeyboardEntry` (service 11), rather than replaying startup or returning a fabricated key.
- A host API `answer_original_keyboard_event` supplies an explicit 16-bit key response, maintains the original guest RAM location, and waits for the NEXT source callback on the same thread.
- The owner-original exploratory driver supplies **two zero-key (no keypress)** replies. Each resumes the original `1FC14` source loop and returns to `30B40` with a strictly incremented host request sequence and the identical guest memory base. No fake exit status or fresh session is generated.
- The Windows milestone workflow now surfaces and asserts `ORIGINAL_CODE2_1FC14_TWO_SOURCE_LOOP_ITERATIONS_PASS`, with `game_rendering=NOT_CONNECTED`. This is an original-media source execution witness, not a retail DOS trace or native scene/frame acceptance.

## Independent validation and limitations

At commit `4f98b57160e976b1853ea2caa27edb8416b53bca`, Linux source regression, CODE.1/CODE.2 source-link CI, and Windows source milestone passed. The original-media Windows workflow passed 85/85 CTests and its opt-in second-INIT source experiment. Workflow: https://github.com/Hyperbog/RLOFT/actions/runs/39698362110. A later CI assertion commit `51c4295db59817be37a76d706b397dea4db2090a` tightens the log proof for both loop iterations.

**Important authenticity boundary:** the canonical `20440` execution still has unconnected optional callbacks including source `3838` SINTAB, `16CD0/19888` initial character/world branch, source `325B0` scene palette, `2F844`, page/sprite operations and original `181B4` level load. They are bypassed when unset. Original `1FC14` rendering/tick callbacks also remain incomplete. Thus two observed `30B40` polls prove source loop entry and memory persistence only, **not** level initialization, input-controlled movement, rendered frame presentation or playable gameplay.

## Next strict execution frontier

Connect the remaining original `20440` initialization callbacks in their actual source order, borrowing the existing authenticated SINTAB, CHARS, RES and level-loader source implementations into the SAME canonical guest. Stop at each unsupported callback, implement it faithfully, then continue to `1FC14` frame work and live keyboard/mouse input. Accept playability only after a real initial world frame and stateful multiple input-driven frames, independently compared with original DOS behavior where reference captures exist.

No copyrighted original media or generated frames are committed.
