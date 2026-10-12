# R04 memory-runtime integration and acceptance audit — 2026-10-12

Canonical branch: `code1-decompilation`, in `Recomp/`.
**R04 IS NOT ACCEPTED.** This is a precise record of progress toward its
fixed scope: "**DOS/4GW memory, heap growth and DPMI selector lifecycle
is implemented without test-only allocations.**"

## Final verified Windows build

[Windows canonical source build #133](https://github.com/Hyperbog/RLOFT/actions/runs/38320942043)
passed **60/60 CTests** at source commit
`9a1ab2a383127a8223879b5952bc6b71f993c390`.
The immediately preceding checkpoints were:

| CI run | Passing tests | New evidence |
| --- | --- | --- |
| [#125](https://github.com/Hyperbog/RLOFT/actions/runs/38320132906) | **59/59** | Four continuous original CODE.2 Phar Lap mode-2 DS-break `39E1C` startup, entry, sound, mouse cases |
| [#126](https://github.com/Hyperbog/RLOFT/actions/runs/38320331494) | **59/59** | DOS 48/49 probe/full allocation selector registered and retired in the same checked table as loader DS |
| [#130](https://github.com/Hyperbog/RLOFT/actions/runs/38320488669) | **59/59** | CODE.1 and CODE.2 DPMI `0006/0100/0101` use injectable shared loader-owned descriptor registry |
| [#132](https://github.com/Hyperbog/RLOFT/actions/runs/38320760435) | **60/60** | CODE.1 original `CCDE` extender startup resizes DGROUP and retains it into source `10AA9/104E0/DB5B/10590/107CB` lifecycle |
| [#133](https://github.com/Hyperbog/RLOFT/actions/runs/38320942043) | **60/60** | Authentic owner-media static CODE.1 heap-root words are checked, never silently overwritten when original file supplied |

These results are **asset-free Windows reconstructed-source execution
tests**. Passing CTests do not prove running original DOS/4GW software.

## A. Continuous CODE.2 Phar Lap `39E1C` — independently verified

The earlier `39E1C` source check was an isolated call. The new host
service makes the original reconstructed startup `353A0→35418` take
the **mode-2 Phar Lap** branch, then the original four initializers,
`20608` game entry, original sound allocation, and the `INT 33h`
mouse boundary, while using **unseeded, source-owned** `39E1C`
DS-break arenas with original `39BBC/39838/398E0` free-list behavior.

The host supplies a clearly labeled *controlled* original-compatible
DOS-version response identifying `4458h` and `BL='2'`, an initial
PSP:005Ch data-break address `0x30000`, `AH=4Ah` resize in **4 KiB
pages**, a bounded DS limit (LSL) and extension service. It does not
invent a successful heap allocation receipt: original CODE.2 selects
the growth request, arena, split, free and subsequent allocation.

An **independent GNU objdump x86 audit** of the supplied original
`CODE.2` file (558,689 bytes, SHA256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`)
verified:

- `35456 CMP AX,4458h`; `3545C SUB BL,30h`; `3546B` reads
  `ES:[005Ch]`; `35478` rounds the break to a 4 KiB boundary;
  `35484 SHR EBX,0Ch`; `3548C MOV AH,4Ah` and `3548E INT 21h`.
- `39E20` compares runtime mode `[12662h]` with 1;
  `39E29` compares subtype `[12663h]` with 0;
  `39E3F→39D4D` is the mode-1 default;
  `39E91–39E99` obtains LSL(DS)+1 for mode 2;
  `39EB4` calls the original data-break `3A467` service.

This is byte-grounded route selection, **not** an original DOS trace.
The protected-mode DS-break capacity and startup version responses are
still **host test policies**, not verified DOS/4GW policies.

## B. One host descriptor registry across CODE.1 and CODE.2

`DpmiDosMemory` now accepts an optional external
`DpmiSelectorDescriptors*`; its default private table remains
supported for isolated tests. When configured with a loader table:

- SOURCE DPMI `0100h` allocation installs a live selector + base/limit;
- SOURCE DPMI `0006h` retrieves that base in `CX:DX`;
- SOURCE DPMI `0101h` retires only its owned selector;
- loader's `DS=0030h` descriptor is retained during memory-block frees;
- original CODE.2 `39C7B` DOS `48h` probe/full allocation's
  test-defined numeric segment identity is also registered with the
  same table and retired on `49h` release.

The self-contained tests assert coexistence and isolation among
`DS:0030h`, allocated `0060h/0068h`, and the `1001h/1003h`
conventional-memory probe/retained handles, including exact bases,
checked limits, rejected overflow, and invalidated stale selectors.

**Important limitation:** using a DOS real-mode segment as the numeric
selector identity for the `39C7B` host fallback is an explicitly
bounded host policy. It does **not** demonstrate actual extender-created
LDT descriptor values, privileges, limits, or alias reuse on an
original DOS system. Similarly, CODE.1 and CODE.2 are separate
original executables and are checked with the same *provider contract*,
not as one simultaneously loaded process.

## C. Original CODE.1 `CCDE → 10AA9` live memory handoff

An opt-in `--extender-host-resize-to-source-heap` source test now
executes original CODE.1 `CC1C→CCDE` startup through the host
4 KiB-page `AH=4Ah` DGROUP resize to `0xA000`, then reuses the
**same guest DGROUP/session** and live host DPMI `0501/0502`
allocation service for original `10AA9`, source `104E0` split,
`DB5B→10590` free/coalescence and original `107CB` reclamation.
The DPMI arena starts empty. No test-owned user-block allocation or
allocated-bit shortcut substitutes for original source functions.

The post-CCDE heap-entry call remains **explicitly host-triggered**;
the original CODE.1 application has **not** naturally reached its
character-creation/heap setup from its full startup entry.

For the original CODE.1 object-2 static heap roots, an independent
audit of user-supplied `CODE.1` (277,787 bytes, SHA256
`fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`)
reconstructed the LE mapped object bytes and verified the root
DWORDs at `30C0h=30D4h`, `30C8h=FFFFFFFFh`,
`30D8h=30D4h` and `30DCh=30D4h`. The new host test
**requires** these words when real original media is provided.
An asset-free synthetic test seeds only these verified root bytes.
The runtime-mode byte `531Eh` is still host-initialized; this is an
acknowledged fidelity gap, not original boot evidence.

## Gate decision and exact missing evidence

The published accepted-gate count **remains R01–R03 = 3/12 (25.00%).**
R04 **must remain false** until at least:

1. CODE.1's heap-mode and ownership state are reached from a **natural
   original source startup path**, without the remaining host-injected
   mode/root preconditions.
2. DOS/4GW memory selector, base, limit, DPMI and DOS allocation,
   resize and release behaviors are compared with **independently
   captured original DOS/4GW** outcomes on authenticated owner-original
   media. Do not manufacture a purported original trace from host CTests.
3. The guest memory provider's DOS/4GW selector aliases, LDT limits,
   error codes, invalidation and cross-service ownership are proven
   against that independent oracle, instead of relying solely on the
   deterministic selector/physical guest alias test policy.

Original DOSBox/DOS runtime tooling and observed memory captures were
**not available in this execution environment**. No original game file
or derived commercial machine-code bytes were committed to CI.
Therefore the source-backed implementation has advanced, but **R04 is
not objectively closed**. Continue R04 alone; do not begin R05.
