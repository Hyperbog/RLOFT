# Part 31 - Stackful canonical CODE.2 source continuation (2026-10-20)

STATUS: Original CODE.2 execution now preserves the native C++ call stack over a real DOS host callback, and proceeds after an explicit mouse-reset reply to its next original DOS callback. NOT YET PLAYABLE.

## Unified state owner

Recomp/src/code2_startup_gate.cpp owns the canonical original 353A0 -> 20608 startup, source Watcom heap, DPMI state, CPU machine and original guest RAM. Recomp/src/original_guest_continuation.hpp preserves an actual running thread and its C++ stack at the DOS INT33 mouse-reset boundary. It does not replay source entry on each host service.

Recomp/src/code2_original_res_startup.cpp now borrows this exact canonical machine and guest RAM. Original 16D04 INIT, 2E7F0 INFO and 11 RES archives, palette staging and original VGA DAC calls run on the same guest/heap while CODE.2 source execution is suspended. Host-ordered source resource calls preserve interrupted CPU registers. Borrowed source heap calls use their own live setjmp frame and restore the source worker's escape target on return.

Important boundary: host-ordered resource calls are not evidence that the original guest scheduler invoked them in that order. The original 1FC14..20293 gameplay loop remains disconnected.

## Execution path and first two DOS dependencies

Original retail CODE.2 353A0 -> 20608 executes original environment, keyboard interrupt-vector and sound.cfg startup, reaching DOS INT33 mouse reset. The worker suspends the actual compiled call stack there.

An explicit TEST ONLY host-virtual mouse reply AX=FFFFh, button count=3, allows the source to continue. This is not a claim that an actual native Windows input driver has been connected. The original source performs INT31 AX=0100 for five DOS paragraphs using the tracked DpmiDosMemory allocator. This results in one authenticated DPMI DOS-memory allocation, then the original 33AA0 leads to its next unsupported real-mode mouse request, DPMI INT31 AX=0300. The source stack suspends again without synthesizing register-frame output.

The next dependency is correct DPMI 0300 / INT33 real-mode register-frame exchange, followed by the original tracked DOS-memory release and mouse-range path. Do not manufacture a successful mouse probe.

## Acceptance

Media-free original_guest_continuation_test: one native source entry, two ordered DOS callback waits, retained C++ local stack state, invalid reply rejection.

Owner-original original_code2_stackful_mouse_gate: authentic retail 353A0/20608 startup, real mouse reset, tracked DPMI 0100 DOS allocation, second suspended DOS host call at 0300. The virtual driver response is labeled a test fixture.

Windows original-media regression PASSED: https://github.com/Hyperbog/RLOFT/actions/runs/39523067077

Unimplemented: production Win32 input backend, DPMI 0300 register-frame response, subsequent mouse range/disk services, source gameplay loop, live frame presentation and full R.EXE child process cycling. No playable binary is claimed by this checkpoint.

No original retail binary/resource bytes, proprietary palette output, raw original captures or user saves have been committed.
