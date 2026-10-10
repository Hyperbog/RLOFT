# Level 22 RES4 selector — independent original-file cross-check (2026-10-10)

This is a **read-only original-file verification**, performed during the user's Windows #654 RES4 diagnostic follow-up. It does not repair the selected resource, change the original assets, or assert DOS/native pixel parity.

## Original file identities

| Original supplied file | Length | SHA-256 |
| --- | ---: | --- |
| `LEVELS.DAT` | 499,878 bytes | `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91` |
| `LEV022.DAT` | 6,028 bytes | `f88b033ff4cdafdd89620e0fac51ff2fb81bce103a6d31a26b91464532c9f51e` |
| `CODE.2` | 558,689 bytes | `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314` |

The packed `LEVELS.DAT` directory reports **32** levels. For zero-indexed level ID 22, its directory entry at `4+22*4` gives record byte range **[372,099, 376,843)** (`0x5AD83..0x5C00B`). The packed record header begins 29 bytes into this record, at file offset **`0x5ADA0`**. The separate direct level's header begins at byte **`0x1D`**. The first **32 header bytes agree byte-for-byte** between the two supplied original files.

Both original level headers have signed i16 at header `+7..+8` equal to **0x0037 = +55**. This is the relative RES4 selector copied by CODE.2 into `DGROUP:2561F`, before the separate loader base from the original `INIT+0x0E` is added by CODE.2 `1F388..1F398`.

Separately rechecked actual immutable original CODE.2 machine bytes for loader `16E55..16E90`, renderer `1F370..1F3C8` and type guard `2F29C`. The latter checks original resource type **1**.

## Consequence for the user diagnostic

The user-supplied older `Probe-Level22-Ground.cmd` output is precisely:

```text
Original RES4 probe error: RES entry is not an EH type-1 raster
```

Because that old probe threw during `resolve()` before printing archive metadata, this exception **does not establish** which physical original `RES4` index was attempted or why it failed. The independent direct-level-versus-packed-level comparison **rules out a difference in header+7 selector** for this supplied file pair. It does not prove that the runtime loaded identical global state from direct and archive levels, and does not prove `INIT` base, type-1 payload, original GOG variant or DOS startup parity.

The already committed forensic snapshot probe in `include/rloft/original_ground_res4_probe_snapshot.hpp` now inventories candidate 64×64 resources first and then prints `INIT` base, source-selected index and raw selected entry prefix even when type-1 decode fails. This fix needs a Windows build newer than #654. Do not guess a different ground image or accept the flat fallback without that evidence.
