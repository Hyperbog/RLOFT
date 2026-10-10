# Upper callback and FF50 indirect-write boundary audit — 2026-10-10

**Source:** original uploaded Strahd `CODE.2` length 558,689, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. This is a **bounded near-call and literal-address** audit, **not** a proof that pointer aliases or nested routines never mutate `FF50`.

## Original CODE.2 callgraph

The outer `1F1C8` renderer invokes an upper callback before `1F377` **reloads the mutable FF50**. The full source entry ranges and direct E8-rel32 calls were disassembled independently:

| Upper callback | Bounded body [start,end) | Direct E8 calls | Literal FF50 references |
| --- | --- | --- | --- |
| `29B40` rectangle overlay | `29B40..29C9E` | 1 | 0 |
| `314B0` animated overlay | `314B0..318B1` | 14 | 0 |
| `33E28` default fill | `33E28..33E6B` | 0 | 0 |
| `34A74` source floor/ceiling texture | `34A74..34BAD` | 2 | 2 reads |
| `2AB38` destination pointer helper | `2AB38..2AB6C` | 0 | 0 |

**17 exact E8 edges:** `29B68→2AB38`; `314EF/318A4→296C0`, `314FD/31620→30CC4`, `31532/3158D/315EA→30CA2`, `31553/315B3/31610→29838`, **`3161A→30700`** (original IRQ/timer), `31766/3180B/31889→2986C`, `34B1C→342E4` and `34B84→34954`. `33E28` makes no near calls; it fills a plane-addressed area using `rep stos` and VGA registers.

Original `34A74` dynamically patches its own code-byte immediates at **`34AF9`, `34B69`, `34AF6`, `34B66`**, via code instructions `34A7E`, `34A83`, `34A8D`, `34A92`. These are **not** stores to `FF50` but affect exact raster sequencing. The original CODE.2 direct-FF50-operand census still yields seven references total: three writes, one comparison and three reads.

## What can be concluded

All three overlay/fill callback bodies have **zero direct literal `FF50` references**. The original `34A74` raster has exactly two FF50 reads, no direct writes. Original `1F377` nonetheless reloads mutable `FF50` following the upper callback. The animated overlay `314B0` explicitly invokes `30700` (time/IRQ source) as well as rendering helpers; it cannot be assumed pure.

This **does not rule out** writes through indirect addresses, register pointer aliases, computed framebuffer destinations, nested callbacks, runtime segment aliases or source callgraph side effects. Especially, full transitive closure of `296C0/29838/2986C/30CA2/30CC4/30700/342E4/34954` and exact `C026/C027` mutation remain open. Do not replace the source-correct post-upper `FF50` reload with an unconditional saved value or enable unsupported partially occluded terrain.

## Independent regression and next gate

`tools/verify_original_ground_upper_callback_boundary.py --original-code2 /path/to/CODE.2` validates original file identity, the five complete code entry-to-RET ranges, their literal FF50 reference counts, 17 exact near-E8 destinations, original `1F377` reload and four original 34A74 self-modifying targets. The script passed against the uploaded immutable original. Registered Python CTest without original media checks the accompanying documentation and source registration. This is **source-only**: CI Windows build remains manual-only and last certified Windows Full #658 passed 214/214 on earlier checkout.

Next: investigate transitive write destinations and source-global register/segment aliases of the above helpers, especially `314B0→30700→30CC4`; preserve all original rendering/IRQ/RNG sequencing. Independent DOS matched indexed framebuffer/palette/ticks and variable-height ground remain **unaccepted**.
