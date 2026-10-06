# CODE.2 Final Status-12 Producer — LE Fixup Audit

Status: **CORRECTION / BINARY-PROVEN NEGATIVE RESULT**

This checkpoint tests the hypothesis recorded in `STATUS12_FINAL_PRODUCER_XREF.md` that the otherwise unreferenced finale primitive at object-1 offset `0x23DF0` might be reached through an LE loader fixup / relocated function pointer.

## 1. Routine boundary

Raw object-1 disassembly from the immutable CODE.2 image confirms the routine starts at `0x23DF0` (not `0x23DEF`; the preceding zero byte causes a misleading linear disassembly when decoding across padding):

```text
23DF0  C6 05 38 BF 00 00 00    mov byte [BF38],0
23DF7  E8 ...                   call 1F1C8
23DFC  E8 ...                   call 1EA78
23E01  E8 ...                   call 28168
23E06  6A 0A                    push 0Ah
23E08  E8 ...                   call 13EF8
23E10  6A 0C                    push 0Ch
23E12  E8 ...                   call 205B4
23E1A  C3                       ret
23E1B  90                       nop
```

The semantic classification remains unchanged: this is a compact terminal primitive that clears `BF38`, performs teardown, runs numbered event 10 (`CINE10`/`CINE11`), and requests supervisor status 12.

## 2. Direct relative-call scan

The complete extracted object-1 image was scanned for x86 `E8 rel32` calls whose resolved destination equals `0x23DF0`.

Result: **zero direct calls**.

The same scan finds ordinary direct callers for adjacent routines (for example `0x23D84` has five direct callers), so the negative result is meaningful rather than a failure of the method.

## 3. LE fixup-table audit

CODE.2 contains an LE header at file offset `0x27BDC`. Relevant header-relative fields are:

```text
fixup page table   +0x235
fixup record table +0x371
loader/fixup region extends before application data pages
```

The fixup records visibly use the expected internal-fixup forms. Examples include source type `07h` with flags `00h` (word target offset) and `10h` (dword target offset), followed by source offset, target object number, and target offset.

A scan of internal object-1 targets found hundreds of valid object-1 fixup records (273 `07/00` candidates and 510 `07/10` candidates under the structural signature), including many known code/data targets. **None targets object 1 offset `0x23DF0`.**

The fixup region likewise contains no encoded internal target for `0x23DF0` in the expected object-number/offset forms.

Therefore the previous statement that invocation "appears to be indirect/fixup-mediated" is **not supported and is withdrawn**.

## 4. Correct current conclusion

What is proven:

```text
0x23DF0
    clear BF38
    teardown
    event 10 -> CINE10/CINE11
    status 12
```

What is also proven:

- no ordinary `CALL rel32` targets `0x23DF0` in object 1;
- no LE internal fixup targets `0x23DF0`;
- adjacent routines do have ordinary direct xrefs, validating the search method.

What is **not** yet proven:

- that `0x23DF0` is reachable in the shipped build;
- that it has an indirect caller;
- that it is referenced through a runtime-generated dispatch table;
- that it is dead/unreachable residual code.

The correct classification is therefore:

**`SHARED_FINALE_PRIMITIVE — reachability unresolved`**

It must not yet be counted as an independently proven gameplay trigger for status 12.

## 5. Status-12 taxonomy correction

The certified gameplay causes remain:

1. party wipe / no active party -> terminal failure presentation -> status 12;
2. five-artifact quest finale -> event 10 -> CINE10/CINE11 -> status 12.

`0x23DF0` duplicates the finale terminal behavior, but until an incoming edge is proven it is a **candidate shared/helper or residual implementation**, not a third certified gameplay cause.

## Next checkpoint

Trace non-`E8` control-flow forms capable of reaching `0x23DF0` (indirect `CALL/JMP`, computed dispatch, runtime-written pointers), and search reconstructed high-level dispatch ownership. If no incoming edge can be established, classify the routine as unreachable/residual and proceed to consolidate the runtime state-transition protocol.