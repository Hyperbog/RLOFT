# CODE.2 faithful decompilation

This directory is the canonical workspace for the faithful C++ reconstruction of Ravenloft: Strahd's Possession `CODE.2`.

`Code2/DECOMPILATION_CONTINUITY.md` is a CODE.2-adapted carry-forward of the CODE.1 continuity/playbook so none of the methodology or critical lessons are lost. The complete append-only CODE.1 historical log remains at `Code1/DECOMPILATION_CONTINUITY.md`. For current CODE.2 state, always read this file and `Code2/PROGRESS.md` after the continuity playbook.

## Immutable CODE.2 identity

- Original file: `CODE.2`
- Size: **558,689 bytes (`0x88661`)**
- SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Bound LE header observed at file `0x27BDC`
- Executable object 1 virtual base: `0x10000`
- Executable object 1 virtual size / conservative denominator: **239,698 bytes (`0x3A852`)**
- Executable object 1 file start: **`0x3AF24`**
- Executable object 1 page count: 59
- Executable object 1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- Initial CS object: 1
- Entry logical offset: **`0x353A0`**
- Data object 2 virtual base: `0x50000`
- Data object 2 virtual size: `0x2DD10`
- Data object 2 first page: 60; page count: 19
- Data object 2 physical file start: `0x75F24`
- Stored object-2 bytes: `0x1273D` (18 full 4 KiB pages plus final `0x73D` bytes), ending exactly at file `0x88661`
- Automatic data object: 2
- Initial SS object: 2; initial ESP: `0x2DD10`

Local extraction used for authoritative object-1 disassembly:

```sh
dd if=/mnt/data/CODE.2 of=/tmp/code2_obj1.bin bs=1 skip=$((0x3af24)) count=$((0x3a852)) status=none
objdump -D -b binary -m i386 -M intel --start-address=0xSTART --stop-address=0xSTOP /tmp/code2_obj1.bin
```

Logical object-1 offset `X` maps to original file offset `0x3AF24 + X`.

## What CODE.1 gives us

CODE.1 and `Code1/Code1.cpp` are major accelerators, but never controlling evidence for CODE.2. Use them as a function-signature and compiler/runtime knowledge base, then prove every CODE.2 conclusion from CODE.2 bytes.

High-value reusable lessons from CODE.1 include:

- exact Watcom/DOS4GW runtime idioms and likely library-function byte signatures;
- the established `Machine`/boundary style for DOS calls, hardware I/O, selectors, guest memory, far pointers, and callback frames;
- explicit modeling of partial registers, promoted stack slots, signed-vs-unsigned branches, x86 shift masking, modulo-32-bit arithmetic, `DEC/JNE` entry-before-test loops, and REP string-instruction pointer state;
- treatment of jump tables, embedded runtime strings/data, padding, alignment islands, shared tails, interrupt/far-call frames, and self-modifying code;
- formatter, heap, stream/file I/O, resource, VGA, input, timer, and startup/CRT semantics already reconciled against the original compiler output.

A preliminary cross-binary anchor scan already finds repeated exact instruction sequences between the completed CODE.1 executable object and CODE.2 at shifted addresses. Treat those matches as leads for rapid function identification, not as automatic translations.

The most useful CODE.1 records to consult are:

- `Code1/Code1.cpp` — completed semantic model and reusable implementation patterns;
- `Code1/FINAL_BINARY_AUDIT.md` — catalog of semantic mistakes caught during the end-to-end audit;
- `Code1/DECOMPILATION_CONTINUITY.md` — full methodology and historical lessons;
- `Code1/RESIDUAL_LEDGER.md` — evidence standard for code/data/padding classification;
- `Code1/FormatterMaterializer.map.md` — detailed formatter/runtime mapping evidence;
- `Code1/PROGRESS.md` — final accounting model.

## Clean-start rule

Do not pre-populate this folder with CODE.1 staging scripts or historical intermediate C++ files. Create staging only when a specific CODE.2 ambiguity requires it, and delete it after verified integration. Git history is the archive.
