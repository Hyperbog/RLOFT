# Part 31 — canonical CODE.2 source runtime + original resource state, single owner (2026-10-20)

## Verified milestone and exact boundary

The previously separate original source checks have been connected to one retained CODE.2 owner. The source compiled from the authenticated original Code2/Code2.cpp now executes startup at object-1 0x353A0, the original application main at 0x20608, and the original sound/interrupt-vector initialization. On the owner-original 59-byte SOUND.CFG, source 375FE keyboard vector selection and source 30BB4 keyboard interrupt installation execute before the first unimplemented external mouse driver reset, DOS INT 33h function 0.

**Hard stop: STOP_BEFORE_DOS_INT33_MOUSE_RESET.** No driver result, user input or completed gameplay frame is fabricated. The recovered game loop at 0x1FC14..0x20293 is NOT reached.

### Shared state and heap

The C++ original startup path now retains its actual runtime Session, Machine/CPU fields, DPMI memory allocation transport, selectors and full 0x01100000-byte guest aperture across the source stop. The resource adapter **borrows** the exact machine and memory vector from the canonical startup owner instead of creating a separate 16 MiB guest. This is enforced by object-base-pointer and resource-cache integrity tests.

The owner retains the original source Runtime129Boundary arena callbacks. When SOURCE INIT/INFO/RES requires memory, the resource adapter invokes the reconstructed original CODE.2 0x37B42 allocation path, with its DPMI-backed arena growth where necessary. Releases use the reconstructed 0x37C37/0x398E0 path. A new valid setjmp escape frame is installed for allocator calls after canonical startup returns, avoiding jumps into the old source startup frame. Source failures stop, rather than fabricating DOS allocation results.

The authenticated original CODE.2 data object stores 0x12708 = 1 before original 0x3A395 (Watcom heap-enable setter); synthetic fixtures start at 0. The source run now tests the authenticated prior-state contract rather than assuming the value 0. This corrected a real original-media integration failure and did not modify original bytes.

After the bounded original mouse stop the same retained guest executes original compiled 0x16D04 INIT, 0x2E7F0 INFO and all 11 RES archive headers, source-derived 0x325B0 original COLORS selected bank, compiled 0x32150 brightness and 0x29CA0 VGA DAC output. This resource orchestration is explicitly **host-scheduled after the stop**; it is not a claim that the original CPU executes these functions in that exact order. Source RAM and the heap remain shared throughout, and two native supervisor Pending dispatches do not create new children or reset the guest.

### Compilation and test constraints

Code2/Code2.cpp contains non-inline compiled function definitions and **cannot be linked twice**. The resource implementation intentionally includes the canonical startup implementation in one translation unit, with resource helpers in a distinct named namespace. Original CODE.2 machine-code reconstruction is unchanged. The standalone source startup target retains its CLI; the integrated resource executable uses one copy of compiled source and the separate LE image loader.

Owner-original Windows source acceptance:
- https://github.com/Hyperbog/RLOFT/actions/runs/39520092333 : canonical original 353A0 and 20608 sharing the original heap and source resource/palette state; passed
- https://github.com/Hyperbog/RLOFT/actions/runs/39520572141 : original CODE.2 375FE/30BB4 source keyboard vector path and original SOUND.CFG read to mouse INT33 boundary; passed
- heap-release integration follow-up under Recomp source gating.

This is source-runtime progress, **NOT** a bootable game. The source code's C++ call stack is unwound by the bounded host stop; guest RAM and Watcom heap are retained, but the original CPU call stack/PC cannot just resume from that same instruction on the next Pending dispatch. Correct continuation requires implementing DOS INT33 as a native host service and letting original execution continue **without a longjmp stop**, then completing subsequent dependency callbacks until genuine CODE.2 1FC14..20293 ticks and frame presentation run in one active source invocation. Do not describe host Pending probes, a saved-scene BMP or a fake mouse response as live gameplay.

### Next strictly scoped blocker

Implement a native mouse service for original DOS INT33 reset, range setup, actual pointer button/coordinate sampling and event delivery, connected to a real game window/input backend. This must preserve original source 33B44/33D18/33D38 and associated DPMI transfers. If no real host mouse/window service is available, fail closed. Continue original compiled 20608 from startup with that service in place; do not try to resume an already-unwound source stack. After mouse and host disk-free services, connect the original source world tick 1FC14 to preserved RES/palette and the presentation scheduler.
