# Canonical recompilation: stateful DOS file/selector host-service checkpoint

2026-10-12. Owner branch: `code1-decompilation` / `Recomp/`.
No modifications to `demo-v1` or `native-engine`.

## Independently verified Windows source build

[Windows CI #60](https://github.com/Hyperbog/RLOFT/actions/runs/38242155253)
completed successfully from source SHA
`4f13ebda062f0db124acf52f8795ac368ab21696`.
**37/37 CTests passed.** This is a source link/execution test; no original
DOS emulator trace or interactive DOS game has been certified.

Relevant CTests:

- `canonical_host_readonly_dos_file_service_contract`: DOS `AH=3D`
  read-only open, `AH=3F` reads, `AH=3E` closes and now `AH=42`
  32-bit signed seeks. Independent descriptor positions, 64 KiB-crossing
  DX:AX positions, EOF recovery, invalid handles, negative/overflow seeks
  and preservation of the original host file are checked.
- `canonical_dpmi_dos_memory_selector_lifecycle`: bounded conventional
  memory with DPMI `0100h` paragraph allocation and `0101h`
  selector release; tests disjoint blocks, selector identity, bounds,
  rejection of stale descriptors, first-fit hole reuse and memory
  exhaustion. This is deterministic host-owned guest memory, not a real
  DOS memory map.
- `canonical_code2_original_dpmi_dos_allocation_via_host`:
  actual reconstructed `CODE.2` `33B44 -> 30D80 -> 30D52`
  requests five conventional-memory paragraphs. The provider owns and
  records the resulting real-mode segment/selector pair; execution
  stops at real-mode `INT31h AX=0300h` mouse simulation.
- `canonical_code2_original_dpmi_selector_release_via_host`:
  the original compiled source allocates using the above provider, then
  follows `33AA0` and `30DE8 -> 30D52` to release the LIVE selector.
  The `INT31h AX=0300h` no-driver response and mouse range responses
  are still explicitly controlled **synthetic** outcomes. The source
  reaches its original `INT21h AH=36h` disk-query boundary.
- `canonical_code2_original_sound_file_via_host_dos_provider`:
  original reconstructed `312A0 -> 37E77 -> 37E99 -> 312EC ->
  38106 -> 312DC -> 380F8` opens/reads/closes the actual file in a
  read-only installation directory instead of a synthesized file
  transport; CI uses a generated 59-byte SOUND.CFG.
- `canonical_code2_original_host_multifile_descriptor_and_seek`:
  after the original compiled startup stops before external mouse reset,
  a clearly marked **test postlude** calls the actual reconstructed
  `37E77`, `38106`, `380F8` on an additional independent
  `OBJECTS.DAT`. Two simultaneously live guest handles are opened,
  read independently, one is repositioned through AH=42, and the
  original read wrapper is called after the seek. Both read buffers
  are compared to an independently read host byte snapshot. This
  postlude does NOT imply the original application naturally opens
  OBJECTS.DAT at the mouse boundary.

## Exact architecture and boundaries

- `Recomp/src/dos_readonly_files.hpp` owns read-only host file streams
  and DOS guest cursors. Validated case-insensitive, flat 8.3 basenames
  only; no traversal, devices, symlinks, ambiguity or writes.
- `Recomp/src/dpmi_dos_memory.hpp` owns a conventional-memory range,
  guest real-mode segments, descriptor identities and checked selector
  translation. Only DPMI 0100/0101 is provided.
- `Recomp/src/code2_startup_gate.cpp` binds real original source
  callbacks to the opt-in providers. Host file opens now derive the
  filename from a bounded guest DGROUP NUL-terminated string (not a
  hardcoded `sound.cfg`), register per-handle descriptor flags, read
  into checked guest data, and close the correct descriptor.
- Earlier isolated fixture modes are retained as differential
  witnesses and remain explicitly classified as test-owned.
- Synthetic CI fixtures are generated under the build directory;
  game media are never committed or published as Actions artifacts.
  The separate owner-original CODE.1 and CODE.2 inputs were locally
  authenticated against known SHA-256 values, but a full original DOS
  run is not available for differential tracing.

## Reproduce on Windows

```powershell
cmake -S Recomp -B build/recomp -A x64
cmake --build build/recomp --config Debug --parallel 2
ctest --test-dir build/recomp -C Debug --output-on-failure
.\build\recomp\Debug\rloft_code2_startup_gate.exe --mouse-dpmi-to-real-mode
.\build\recomp\Debug\rloft_code2_startup_gate.exe --mouse-dpmi-to-disk-query
.\build\recomp\Debug\rloft_code2_startup_gate.exe --to-mouse-reset-host-multifile "C:\RLOFT"
```

The host-multifile mode needs an existing installation directory
containing a 59-byte SOUND.CFG and OBJECTS.DAT of at least 50 bytes.
Only the host file services read those files; the default test uses
synthetic original LE image/PSP/interrupt startup fixtures.

## What is NOT accepted

- **R04** (full DOS/4GW memory, heap growth, selector lifecycle) remains
  open: DPMI 0100/0101 alone is not all DOS extender memory services.
- **R05** (complete filesystem/streams) remains open: no DOS write,
  create, delete, rename, findfirst/findnext, directory services, full
  original-source seek caller map, or faithful error propagation.
- Mouse reset/real-mode `0300h` simulation, keyboard/timer interrupt
  effects, disk `AH36h`, full CODE.1 character creation, authentic
  CODE.1-to-CODE.2 handoff, original graphics/audio and gameplay
  remain pending. There is no playable original-code game.
- Guest 8.3 path checks are not a defense against an adversary mutating
  the host directory concurrently during opens. The stateful memory
  service's initial DOS-memory placement is deterministic test
  configuration, not an observation of DOS/4GW.
- **Progress is still 3/12 accepted major milestones = 25.00%.**
  The CTest count is not a substitute for whole-game completion.

## Next integration

1. Recover and implement source-required DOS/4GW heap and PSP block
   resize/selector operations (including CODE.1 original AH=4Ah)
   with stateful provider contracts, avoiding fabricated outcomes.
2. Implement source-required real-mode interrupt and mouse/keyboard
   services and track the first newly reached original instruction.
3. Expand source-backed file and resource calls, seek error propagation
   and DOS stream semantics while maintaining read-only installation
   safety until writes are required for authentic saves.
4. Join compiled original CODE.1 character creation and CODE.2 gameplay
   through authentic CHARS transfer; compare traces to DOS originals.
