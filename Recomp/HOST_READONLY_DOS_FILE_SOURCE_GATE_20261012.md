# Canonical recompilation: host-backed read-only DOS sound-file boundary

Checkpoint: 2026-10-12. Owner: canonical code1-decompilation / Recomp/.
Neither demo-v1 nor native-engine was changed.

## Verified scope

[Windows canonical build #49](https://github.com/Hyperbog/RLOFT/actions/runs/38208279160)
completed successfully at exact source SHA
e91263ff89dccf20b3335d4171ff5d76dfd3dbed:
**33/33 CTests passed**, including two new host-service gates.

- canonical_host_readonly_dos_file_service_contract tests a genuine
  host read-only DOS AH=3Dh/3Fh/3Eh transport. It checks independent
  handle cursors, 17+42 and 59-byte reads, EOF, close, invalid handle,
  write denial, DOS devices, traversal rejection and file preservation.
- canonical_code2_original_sound_file_via_host_dos_provider executes
  compiled canonical CODE.2 startup, then source calls
  312A0 -> 37E77 -> 37E99 -> 312EC -> 38106 -> 312DC -> 380F8.
  Original C++ source wrappers use the new host-file provider instead of
  the synthetic DOS file-data callback. Source DGROUP transfer, descriptor
  publication/close and the next actual mouse-reset boundary are checked.
- Windows CI generates a noncommercial 59-byte SOUND.CFG. No original
  media are packaged or published.
- The provider admits only a non-symlink installation directory,
  case-insensitive DOS 8.3 names and read-only compatibility-mode opens.
  It rejects traversal, DOS devices, ambiguous entries and write access.
  This is not a full DOS/4GW filesystem. It does not guarantee protection
  against malicious filesystem changes between checking and opening a file.

Owner execution on Windows:

    rloft_code2_startup_gate.exe --to-mouse-reset-host-file C:\RLOFT

The named folder must contain a 59-byte SOUND.CFG. CODE.2 image, DOS
PSP, arena, interrupt vectors and mouse remain bounded synthetic fixtures;
only the file service is host-backed. Execution stops before genuine DOS
INT33 mouse reset. Original-DOS behavioral equivalence is not claimed.

## Progress

Whole-game recompilation gate acceptance remains **3/12 = 25.00%**.
R05 remains unaccepted: a read-only 59-byte config source path does not
implement all DOS file/stream services. R04 and R06-R12 remain unaccepted.

Next: implement and test source-required seek/position, open mode, error
mapping, and additional file reads against authenticated original assets
without including originals in CI or distribution. Genuine DOS/DPMI memory,
hardware interrupts, CODE.1 gameplay handoff and interactive gameplay
remain outstanding.
