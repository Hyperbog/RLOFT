# Alpha startup: bypass logos after cinematic skip, restore title hit-testing

The original CODE.2 `19888` cold-start routine only enters its
logo/splash sequence when the `13EF8` cinematic result remains zero,
after source event 0 (CINE00.ANM) and event 12 (CINE12.ANM).
Original nonzero result goes directly to the common title menu.

The native Windows path had `skipped=true` on cinematic Escape but
nevertheless drew the first logo and waited up to 120 PIT ticks.
Now the first logo is inside the same skip gate as the later
logo/title animation stages. Ordinary unskipped intros are unchanged.

The Win32 title hit-test also formerly relied on a prior WM_MOUSEMOVE.
The actual first menu entry/replay can have an already-stationary
cursor, or a click can arrive with an obsolete hover. Native window
mode now samples the system pointer when opening the menu, and each
WM_LBUTTONDOWN maps its OWN coordinates through the native VGA
view rectangle before assigning a title action. All margin
coordinates are rejected; leaving the window clears selection.

This changes only the native host input bookkeeping for existing
original title menu action hitboxes and cold-start skip staging.
Original DOS mouse ISR input/timing and full page/palette/audio
parity remain unaccepted. The source-only Alpha startup CTest
requires both conditions and rejects six deliberate regressions.

The project remains **Pre-Alpha**, with first playable Level22
entry—not the entire Level22 quest/Level23 handoff—the accepted
Alpha scope. No original game assets or synthetic party data
are committed.
