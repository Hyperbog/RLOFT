# Part 28 — host-ordered CODE.1 to CODE.2 source startup with real COLORS DAC (2026-10-20)

## Scope and evidence

The goal is to reduce reliance on saved-world diagnostic seeding. A new
owner-media-only source checkpoint explicitly drives:

1. original canonical CODE.1 `CC1C → CCDE → CD97 → FCE8 → FC98`
   startup/constructors to the existing `508C` game-entry **HOST STOP**
   boundary, using the existing stateful reconstructed guest/DPMI heap gate;
2. original CODE.2 source-backed `16D04` INIT and `2E768 → 2E7F0`
   INFO/eleven resource-archive setup in **one CODE.2 guest memory session**;
3. scene bank selection from the authenticated CODE.2 data object at
   `0x81AC + 3*level`, storing the selected ID at `C034` and flag
   `CF58` as an explicit **host-scheduled transition**, not claiming that
   canonical `1F8E0` ran in DOS order;
4. source-derived CODE.2 `325B0` open/seek/read/close path, which reads
   all 768 RGB6 components from the owner-original `COLORS` bank into
   original guest staging `C054`. The loader is coded in
   `src/original_code2_palette_loader.hpp`, not yet migrated into the
   monolithic canonical `Code2/Code2.cpp`; its seek/read and
   brightness callback semantics are original-byte-backed;
5. actual **compiled canonical CODE.2** `32150` brightness source
   and `29CA0` indexed hardware DAC writer, through the independent
   bounded VGA I/O adapter (exactly 769 port writes: one index and
   768 component bytes).

The native saved-world renderer now also uses this source-derived `325B0`
read-only transport instead of performing a direct host `std::copy`
of the selected bank. No original game media is included in the builds.
Read-only 15-bank formats remain a planned palette asset substitution seam
without changing historical original bank bytes.

## Execution contract

Build the non-playable research target:

```powershell
cmake -S Recomp -B build/recomp -A x64
cmake --build build/recomp --config Debug --target rloft_code1_code2_staged_boot
.\build\recomp\Debug\rloft_code1_code2_staged_boot.exe --root "D:\GOG Games\Ravenloft - Strahd's Possession\RLOFT" --cd-root "C:\Private\ExtractedCD\RLOFT"
```

The `--cd-root` path needs a **read-only extracted RLOFT CD file directory**
containing RES9 and RES10, not a `game.gog` image filename. This tool
does not mount ISO images; the existing original DOSBox-X launcher already
mounts GOG's `game.gog` using IMGMOUNT. Exact mount-source handling remains
in separate DOS/Windows boundaries.

The composite target has a source-order **barrier**:
`RLOFT_STAGED_BOOT_HANDOFF=HOST_ORDERED_NOT_DOS_GUEST`.
The original CODE.1 game entry `508C` still reaches an intentional host
stop, so this executable does **not** prove CODE.1 dynamically loaded or
called CODE.2. It does not claim to have booted character creation or
interactive gameplay.

The CODE.2 stage is substantial: original INIT/INFO/11 archive initialization,
the original source palette selector table and bank file transport, and
compiled CPU-equivalent palette normalization/DAC submission run on the
same machine's `memory`. No saved camera, native world frame, skeleton
sprite or palette splice is used to initialize that session.

## Verification boundaries

- Canonical reconstructed CODE.1 and CODE.2 source gates remain independent
  and continue to be built/executed in their preexisting CTest workflows.
- Original media identity is verified by the existing source gate for CODE.1,
  and by `is_original_code2` in CODE.2 resource startup. The COLORS file
  is opened read-only and checked for original 15×768 six-bit structure;
  custom compatible palettes are allowed, and no original palette is logged.
- CI integration is
  `.github/workflows/recomp-milestone-windows.yml`, with private
  original `DnD-Classics` sparse media checkout and no original media
  file or original palette/pixel attachments.
- Test pass means the source-backed initialization stages, file transport,
  full-bank video submission and sequence checks completed. It is **not**
  continuous DOS runtime call ancestry or pixel-perfect game acceptance.
- Original scene bank 3 and original saved VGA DAC agreement were separately
  verified from the independent earlier DOSBox-X port-event trace and SAV;
  the current checkpoint does not fabricate an independent DOS trace.
- A complete playable CODE.1-to-CODE.2 game requires the original DOS
  program-switch/overlay handoff, allocations/interrupts, event loop,
  input devices and world renderer to be truly connected rather than
  stop-boundary demonstrations.

## Next bridge acceptance target

Capture/implement the CODE.1 `508C` program handoff into the original
CODE.2 entry, then resume CODE.2 `20608` within the **same original
heap/resource/video session**, run original palette bank refresh from the
original scheduler rather than the host-set selector, and reconcile a
frame of original DOS VGA output. Never label the current composite
as guest-contiguous just because both source gates execute in one host
process.
