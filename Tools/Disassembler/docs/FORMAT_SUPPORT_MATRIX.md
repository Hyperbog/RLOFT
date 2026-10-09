# Universal format support matrix and collection-derived regression order

The collection observations below are **reported header observations**,
not independently reverified by this source-only checkpoint.

| Format family | Example from collection | Implementation status | Correct next gate |
| --- | --- | --- | --- |
| DOS MZ `.EXE` | Hillsfar `MAIN.EXE`; Dark Sun `DSUN.EXE` | Header/relocation map and requested x86-16 linear decode in source | Real-file validation, complete MZ rebase and recursive CFG |
| MZ-bound LE | `CODE.1`, `CODE.2` | Validated object/page mapping and x86-32 candidates in source | Fixup coverage, indirect-flow completeness |
| Gold Box FBOV | Pool of Radiance `GAME.OVR` | Signature recognized only | Overlay directory, segment/code tables, runtime load mapping |
| Gold Box TPOV | Secret of the Silver Blades `GAME.OVR` | Signature recognized only | Independently validated alternative overlay mapping |
| Headerless COM | Pools of Darkness `SOUND.COM` | Unknown/headerless by default | Explicit 16-bit origin/PSP/entry assumption mode, no extension guessing |
| Headerless/structured DRV | Pools of Darkness `VGA.DRV` | Unknown/headerless by default | Driver calling/initialization conventions and detection |
| MZ-bound LX | `A32ADLIB.DLL`, other AIL/32 drivers | Signature recognized only | LX object/page/rebase/entry mapping |
| NE / PE | Other MZ-bound binaries | Signature recognized only | Dedicated NE/PE metadata loaders and host-platform distinction |
| AESOP/16 | Dungeon Hack `HACK.RES` | Resource signature recognized only | Container records + separate AESOP VM decoder |
| GFFI | Dark Sun `RESOURCE.GFF` | Resource signature recognized only | GFF directory, data references, scripts |
| DAX / Gold Box events | Gold Box resources | Unknown | Data container and interpreter-specific event decoder |
| Unlimited Adventures data | `.DAT`, `.TLB`, `.GLB` | Unknown | Proven resource/event schema, never automatic x86 |

## Source-driven priority

**P0:** reliable 16-bit MZ load semantics and seeded CFG; FBOV/TPOV
overlay parsing; explicit COM mode; signature detection and archive identity.
Continue universal indirect CALL/JMP analysis and unknown-gap reporting
in parallel.

**P1:** LX and NE loader details, DOS driver interfaces, packed and embedded
executable identification, entry point/rebase reconstruction.

**P2:** AESOP/16 interpreter disassembly, GFF/DAX and other data/bytecode
engines; read-only archive and disk-image inventory.

This list expresses **format support**, not hardcoded per-game behavior.
Many games share formats; one game may supply multiple executable and VM
container types. Game-specific SHA-256 known-answer fixtures must remain
external to the decoding engine.

## First original-file regression set, once manual testing is authorized

1. Hillsfar `MAIN.EXE` — standalone DOS MZ metadata/16-bit decode.
2. Pool of Radiance `START.EXE` — DOS MZ loader and relocation mapping.
3. Pool of Radiance `GAME.OVR` — FBOV container map and segment mapping.
4. Dark Sun `DSUN.EXE` — DOS MZ structural differences.
5. Menzo `A32ADLIB.DLL` — MZ-bound LX audio interface/loader.
6. Secret of the Silver Blades `GAME.OVR` — TPOV overlay variant.
7. Pools of Darkness `SOUND.COM` and `VGA.DRV` — explicit headerless contexts.
8. Dungeon Hack `HACK.RES` — AESOP VM instruction format.
9. Dark Sun `RESOURCE.GFF` — GFF resource mapping.

Exclude bundled modern Windows DOSBox executables, installers, uninstallers,
SDL libraries and GOG configuration helpers from the original-game benchmark;
they can be cataloged separately as supporting software.

**Source checkpoint only:** no Windows build, no original files processed and
no regression suite executed here.
