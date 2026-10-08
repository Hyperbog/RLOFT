# CODE.2 post-certification semantic errata

## Erratum 001 — routine 2975C argument contract

**Date discovered:** 2026-10-07  
**Discovery context:** Demo v1 native automap presentation integration  
**Binary authority:** immutable supplied `CODE.2`, object-1 logical `2975C..29789`

### Historical certified translation

The certification snapshot declared:

```cpp
function_2975c(boundary, a1, a2, key, set)
```

and resolved the resource with `2F29C(key,set)`, then invoked
`2A380(a1,a2,key,image)`.

That source signature is semantically incorrect.

### Immutable instruction evidence

At logical `2975C`, the binary consumes **five** cdecl stack arguments.

Before calling `2F29C` it pushes:

```text
[entry ESP+14h]  arg5
[entry ESP+10h]  arg4
```

After the lookup returns, it pushes the resolved image and then forwards the
first three incoming arguments to `2A380`. Accounting for the temporary
pushes, the call is equivalent to:

```text
image = 2F29C(arg4, arg5)
2A380(arg1, arg2, arg3, image)
```

The independently reconstructed `2A380` contract is:

```text
(frame, y, x, image)
```

Therefore the exact source-level contract is:

```cpp
function_2975c(boundary, frame, y, x, key, set)
```

with:

```cpp
image = call_2f29c(key, set);
call_2a380(frame, y, x, image);
```

The map/render call sites in the same object already push five arguments, and
the earlier `Application03Boundary::call_2975c` callback type was already
five-argument. That internal mismatch helped expose the defect during native
integration.

### Physical verification

Object-1 begins at file `0x3AF24`, so logical `0x2975C` maps to physical
file offset `0x64680`.

A reproducible disassembly is:

```sh
dd if=CODE.2 of=code2_obj1.bin bs=1 skip=$((0x3AF24)) count=$((0x3A852))
objdump -D -b binary -m i386 -M intel \
  --adjust-vma=0x10000 \
  --start-address=0x3975c --stop-address=0x3978a \
  code2_obj1.bin
```

The adjusted display address `0x3975C` corresponds to object-relative
logical `0x2975C`.

### Impact

- Executable-byte ownership/accounting is unchanged.
- The historical `239,698 / 239,698` coverage result remains valid as byte
  coverage.
- The original claim of a fully correct source-level semantic audit requires
  this erratum: the certified source hash identifies the pre-correction
  snapshot.
- Native callers can now reproduce the encoded frame/Y/X/resource/archive
  contract without relying on a boundary adapter that happened to retain the
  correct five-argument shape.

### Correction status

`Code2/Code2.cpp` on the current branch has been corrected. The historical
certification documents are retained as provenance and carry a notice pointing
to this erratum rather than having their original closure data silently
rewritten.


## Erratum 002 — routine 0139C automap draw-coordinate accumulators

**Date discovered:** 2026-10-07  
**Discovery context:** Demo v1 native automap presentation integration  
**Binary authority:** immutable supplied `CODE.2`, object-1 logical `0139C..01556`

The historical certified translation called both cell passes with constant draw
coordinates `6,4`. Immutable disassembly proves two loop-carried coordinates:

```text
draw0 = 6 + 8 * (column - viewport_column)
draw1 = 4 + 5 * (row    - viewport_row)
```

Both live values are passed to `00B6C` and `00CB0`.

Object-1 begins at file `0x3AF24`; logical `0x0139C` maps to physical
`0x3C2C0`. Reproduction:

```sh
dd if=CODE.2 of=code2_obj1.bin bs=1 skip=$((0x3AF24)) count=$((0x3A852))
objdump -D -b binary -m i386 -M intel \
  --adjust-vma=0x10000 \
  --start-address=0x1139c --stop-address=0x11557 \
  code2_obj1.bin
```

The first pass advances the inner coordinate by 5 at displayed `0x11437`
and the outer coordinate by 8 at `0x1143F`. The second pass repeats those
increments at `0x114D7` and `0x114DF`.

Executable-byte coverage/accounting is unchanged. The source-level automap
placement semantics are corrected on the current branch; historical
certification hashes remain provenance for the pre-erratum snapshot.


## Cross-branch source reconciliation (2026-10-07)

A full text comparison of the `code1-decompilation` certified snapshot (`c3f16122e79c3c464382e4511d9d932f6fcd694a`) and the demo reconstruction (`87f6638921c53a32bde59735515fbc175e2c96ee`) found **exactly the two semantic edits detailed in Errata 001 and 002**, with no other changed source region. Independently hashed original `CODE.2` has SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Both ranges were re-disassembled directly from that original file. The historical main branch adopted the existing exact demo blob in commit [`c952e9a`](https://github.com/Hyperbog/RLOFT/commit/c952e9adb152007208951da3b0030c7439fd72b9); GitHub file SHA checks confirm both branches now share `87f6638921c53a32bde59735515fbc175e2c96ee`.

The correction is instruction-supported and changes no original executable bytes or byte-accounting totals. A new whole-file strict C++17 syntax check and complete original-vs-native gameplay verification remain separate, unclaimed acceptance tests.
