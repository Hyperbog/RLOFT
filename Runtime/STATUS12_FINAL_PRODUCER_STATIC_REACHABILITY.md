# CODE.2 `0x23DF0` Static Reachability Audit

Status: **STATICALLY UNREFERENCED / RESIDUAL CANDIDATE — runtime-generated reachability not proven**

This checkpoint continues the LE-fixup audit of the standalone event-10/status-12 primitive at CODE.2 object-1 offset `0x23DF0`.

## 1. Entry boundary

The entry is `0x23DF0`, not `0x23DEF`. Raw bytes are:

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
```

`0x23D84` returns at `0x23DE4`; the intervening bytes are zero padding. `0x23E1C` begins a separate routine.

## 2. Direct control-flow audit

The complete extracted object-1 disassembly was searched for decoded control-flow instructions whose destination is `0x23DF0`.

No incoming decoded edge was found from:

- `CALL rel32`;
- near/short unconditional `JMP`;
- conditional `Jcc` branches.

The same disassembly contains normal direct calls to the adjacent `0x23D84` routine, so the absence is address-specific rather than a limitation of the scan.

## 3. Static pointer / jump-table audit

The object contains many compiler-generated indirect jump tables (`jmp DWORD PTR cs:[index*4+table]`). A raw object scan contains no 32-bit object-relative `0x00023DF0` entry, and the earlier LE audit found no loader fixup whose internal target is object 1 offset `0x23DF0`.

Therefore none of the ordinary statically encoded mechanisms establishes the routine as a jump-table/callback target:

```text
no direct CALL
no direct JMP/Jcc
no raw object-relative function pointer
no LE relocated internal pointer
```

## 4. Indirect CALL inventory

CODE.2 does contain indirect calls, including runtime/library callback forms such as calls through fixed memory slots, stack arguments, and registers. However, a runtime indirect call can reach `0x23DF0` only if some execution path first obtains that address.

No static producer of the `0x23DF0` address has been found in object 1 or in the LE internal-fixup records. Consequently the existence of generic indirect-call instructions is not evidence that this routine is reachable.

A runtime-generated address remains theoretically possible, but there is currently no binary evidence for one.

## 5. Correct classification

The standalone primitive's body is semantically clear:

```text
0x23DF0
    clear BF38
    teardown
    event 10 -> CINE10/CINE11
    status 12
```

Its reachability is not.

The strongest evidence-based classification is therefore:

**`STANDALONE_FINALE_PRIMITIVE — STATICALLY UNREFERENCED / RESIDUAL CANDIDATE`**

Do not count it as a third certified status-12 gameplay cause. Do not delete it from the preservation reconstruction either: runtime-generated invocation has not been mathematically excluded.

## 6. Certified status-12 causes

The shipped static control-flow evidence currently certifies two gameplay causes:

1. **party wipe / no active party** -> failure presentation -> status 12;
2. **five-artifact quest finale** -> event 10 -> `CINE10`/`CINE11` -> status 12.

Both converge on the already proven supervisor semantic:

```text
STATUS_12 = RESTART_CODE_CYCLE_WITHOUT_CHARS
```

`0x23DF0` duplicates the finale terminal sequence but has no certified incoming edge.

## 7. Preservation consequence

For a native port, retain the routine in the evidence/archive layer as residual candidate code, but do not create an additional gameplay transition solely because it exists in the binary. The typed runtime state machine needs only the two certified causes unless later dynamic evidence proves another caller.

## Next checkpoint

The runtime-control investigation can now move to consolidation. Build `Runtime/STATE_TRANSITION_PROTOCOL.md` from the certified launcher, CHARS, status-10, status-12, party-wipe, and finale evidence, while explicitly recording `0x23DF0` as a statically unreferenced residual candidate.