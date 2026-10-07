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
