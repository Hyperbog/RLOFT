# Part 16 — CODE.1 to CODE.2 disk-based CHARS handoff

**Source owner:** `demo-v1`; **goal:** faithfully consume CODE.1's
serialized party when the supervisor re-enters CODE.2. No original game
file, saved party, binary extract or copyrighted game resource is committed.

## Exact original evidence

Owner `CODE.2`, length **558689**, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Its LE code base maps logical addresses to physical offsets `0x3AF24+offset`;
DGROUP file base is `0x75F24`. Both DGROUP names `+0710` and
`+1140` contain NUL-terminated `CHARS`.

Original `204B3→31218→37E39` checks DOS file attributes for
`CHARS`. If present, `204EF→16CD0` opens it with `312A0` using
the `+0710` string, passes destination `218E0` and length
`0x920` to `312EC`, closes the handle using `312DC` and calls
`16ADC` to import the party. All nine original code windows were
verified against the owner original file; the optional private oracle
repeats the SHA and byte checks.

The native normal character-creation controller previously built
the correct `CHARS` image, wrote it to the correct writable path,
then entered exploration with **the original in-memory CODE.1
image** rather than the CODE.2 disk-read bytes. That bypassed
the original module boundary.

## Correction and tests

After the original-status-10 supervisor transition, the native host now
reopens `CHARS` with `GameSource::read_runtime`, verifies the complete
0x920-byte transfer and hands the disk bytes to `run_exploration`.
The equality check against the just-written buffer is explicitly a
native file-transfer integrity guard; DOS does not do this equality
comparison. It rejects an inconsistent transfer rather than silently
loading a stale/corrupted character image. The ordinary existing-party
startup path already reads from disk. The `--front-end-only`
read-only showcase remains unchanged.

The synthetic `game_source_tests` fixture now writes, reads and
truncates a 0x920-byte CHARS file through the real path resolver;
no original game assets are required. The new read-back and truncation
checks throw on mismatch, including Release builds where `assert` can be
disabled. New Python CTest
`original_chars_disk_handoff_contract_tests` checks source call
ordering, guarded length and read-back, and rejects three source
mutations. `--original /private/CODE.2` also checks original SHA,
original instructions and both filenames.

**Separate gates:** local original instruction proof; portable C++
unit tests; Windows Full build/CTest; actual Win32 normal-launch field
test; independent original-DOS gameplay comparison. No Windows field
or DOS parity claim follows from the source-only verifier. Ordinary
startup still creates CHARS in the writable installation: protect
existing saves by testing on a **separate game installation copy**.
