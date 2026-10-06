# Exit-Status Semantic Correlation Checkpoint

Status: **ACTIVE — transition mechanism proven; final user-facing enum names still withheld**

This checkpoint continues the launcher protocol investigation by correlating the three known CODE.2 status-12 producers with their immediate machine-code context and by locating a concrete CODE.1 return-10 producer.

## 1. CODE.2 status-12 producer at 0x0CCE0

The bytes immediately preceding the controlled exit are:

```text
... call 1A1F4
    test eax,eax
    jne  <poll>
    call 1A1F4
    test eax,eax
    je   <poll>
    push 0Ch
    call 205B4
```

This confirms the earlier reconstruction: the transition is requested only after an application/UI polling sequence observes two opposite states. It is therefore a deliberate completed interaction path, not an error unwind.

The function tail after the call restores registers and returns. A neighboring routine begins immediately afterward. Direct-call xrefs to the neighboring `0x0CCF4` routine exist at `0x18D7F`, `0x2CC7B`, `0x2D2E7`, and `0x2D3B9`; these are not evidence that those callers own the status-12 path, so they are deliberately not promoted as callers of `0x0CCE0`.

## 2. CODE.2 status-12 producer at 0x23E10

The local sequence is now reconstructed more tightly:

```text
mov byte ptr [BF38],0
call <application cleanup>
call <application cleanup>
call <application cleanup>
push 0Ah
call <application routine>
push 0Ch
call 205B4
ret
```

The important distinction is that the literal `0Ah` immediately before the status-12 request is an **argument to another application routine**, not a process exit. This is direct evidence for why literal searches for `10` cannot be used to name the launcher status producer.

The path performs multiple application-level teardown operations before requesting status 12, consistent with a controlled mode/state restart rather than abnormal termination.

## 3. CODE.2 status-12 producer at 0x26BD9

The third producer has the same high-level shape:

```text
... conditional application checks ...
mov byte ptr [BF38],0
call <application cleanup>
call <application cleanup>
call <application cleanup>
push 0Ah
call <application routine>
push 0Ch
call 205B4
...
```

Again, `0Ah` is consumed by a different routine and `0Ch` is the value passed to the controlled process-transition helper. Two independently located status-12 paths therefore share a recognizable teardown/transition idiom.

## 4. Concrete CODE.1 return-10 producer

A direct CODE.1 machine-code scan located an application routine whose terminal decision is:

```text
... application polling/dispatch ...
cmp dword ptr [04CC],-1
jne <continue>
cmp byte ptr [04C4],0
je  <return_zero>
cmp byte ptr [04C5],0
je  <return_zero>
mov eax,0Ah
leave
ret

<return_zero>:
xor eax,eax
leave
ret
```

The `mov eax,0Ah` occurs at CODE.1 object-1 logical offset **`0x49EF`**. This is not merely a literal push: it is the function's return value.

This is the first concrete application-level CODE.1 producer of value 10 found during the launcher investigation. It proves that CODE.1 deliberately returns 10 from a state-dependent application routine when two byte state flags are both nonzero after a sentinel/state check.

What is **not yet proven** at this checkpoint is that this exact routine's return is the final process status observed by R.EXE. The caller chain from this routine to CODE.1 main/CRT termination must still be reconstructed before that promotion is made.

## 5. Protocol interpretation now justified

The evidence now supports these stable machine-level names:

- `10`: **continue/module-transition value** at the R.EXE boundary; additionally produced as a deliberate CODE.1 application return value.
- `12`: **controlled restart/reset transition value** deliberately produced by multiple CODE.2 application paths; R.EXE responds by deleting `CHARS`, converting the supervisor state back to 10, and restarting the cycle.

The stronger gameplay labels remain withheld. In particular, the repository should not yet encode `10` as `ENTER_CHARACTER_GENERATOR` or `12` as `NEW_GAME`/`RESTART_CHARACTER_GENERATION` until the caller chains and UI ownership are directly reconciled.

## 6. Next proof target

1. Trace CODE.1 logical `0x49EF`'s containing routine upward to the CODE.1 main/termination path and determine whether its return value becomes the child process status.
2. Identify the owning functions/start boundaries for CODE.2 `0x23E10` and `0x26BD9`, then enumerate their callers from the canonical reconstruction/binary.
3. Correlate those callers with UI strings/menu dispatch states and with `CHARS` creation/deletion.
4. Only then assign final semantic enum names to statuses 10 and 12.