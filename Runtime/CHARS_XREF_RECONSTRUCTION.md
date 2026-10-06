# CHARS Cross-Executable Xref Reconstruction

Status: **BINARY-PROVEN PRODUCER/CONSUMER CLASSIFICATION**

This checkpoint uses the corrected object maps in `Runtime/CODE_OBJECT_MAP.md`. Data-object offsets, not the earlier erroneous object-1 linear addresses, are the xref keys.

## 1. Correct anchors

- CODE.1: `CHARS` file `0x3E7B8`, data object 2 offset `0x94`
- CODE.2 occurrence 1: file `0x76634`, data object 2 offset `0x710`
- CODE.2 occurrence 2: file `0x77064`, data object 2 offset `0x1140`

## 2. CODE.1 is a CHARS producer

Executable object 1 contains three direct 32-bit references to data offset `0x94` in one routine around logical `0x4FBF..0x500D`:

```asm
4FBF  push 0x94
4FC4  call 0x63A4

4FCC  push 0x94
4FD1  call 0x62D8
4FD9  push eax
4FDA  call 0x62F0

4FE2  push 0x94
4FE7  call 0x62C0
...
4FF2  push 0x920
4FF7  push 0x67F8
4FFC  call 0x6320
5004  push ebx
5005  call 0x62F0
```

The wrappers resolve to DOS file services:

- `0x63A4 -> 0xD8AA`: DOS `INT 21h`, `AH=41h` — delete/unlink pathname.
- `0x62F0 -> 0xD573`: DOS `INT 21h`, `AH=3Eh` — close handle.
- `0x62D8 -> 0xD55E -> 0xD313`: open/create-family wrapper using the DOS open path and flags `0x62`; the caller supplies mode `0x180`.
- `0x62C0 -> 0xD313`: reopens the pathname with flags `0x202`.
- `0x6320` enters the runtime's handle I/O family after the reopened handle is established.

The sequence is structurally unambiguous even before assigning exact Watcom symbolic names: CODE.1 deletes any prior `CHARS`, creates/reinitializes it, closes that handle, reopens it, performs handle I/O using a `0x920`-byte transfer argument, then closes it.

**Conclusion: CODE.1 is a producer/writer of the transient `CHARS` handoff file.**

## 3. CODE.2 occurrence #1 is a CHARS consumer

CODE.2 object-1 routine at logical `0x16CD0` directly references data offset `0x710`:

```asm
16CD0  push ebx
16CD1  push 0x710
16CD6  call 0x312A0
16CDE  push eax
16CDF  push 0x920
16CE4  push 0x218E0
16CEB  call 0x312EC
16CF3  push ebx
16CF4  call 0x312DC
16CFC  call 0x16ADC
16D01  pop ebx
16D02  ret
```

`0x312A0` passes flags `0x200` into the runtime open path at `0x37E77/0x37E99`, whose low-level implementation executes DOS `INT 21h`, `AH=3Dh` (open file). `0x312DC` is the corresponding close wrapper. `0x312EC` is the intervening handle-I/O wrapper and is called with a `0x920` transfer size.

The transfer size exactly matches the CODE.1 handoff I/O size.

**Conclusion: CODE.2 opens, consumes approximately/exactly the same `0x920`-byte CHARS payload produced by CODE.1, then closes it.** Exact short-read/error semantics remain to be named after the underlying I/O wrapper is fully classified.

## 4. CODE.2 occurrence #2 is an existence/access gate

At logical `0x204B3`:

```asm
204B3  push 0
204B5  push 0x1140
204BA  call 0x31218
...
204E4  test ebx,ebx
204E6  jne  0x204EF
204E8  call 0x19888
204ED  jmp  0x204F9
204EF  call 0x16CD0
```

`0x31218` calls `0x37E39`, which executes DOS `INT 21h`, `AH=43h`, `AL=0` to query file attributes and converts success/failure to a Boolean-style result. With the caller's second argument `0`, this is an existence/accessibility test for the pathname at data offset `0x1140`.

If the test succeeds, execution reaches `0x16CD0`, the proven CHARS reader above. Otherwise it takes the alternate initialization path at `0x19888`.

The two CODE.2 `CHARS` strings are therefore two pathname constants used by related startup/transition logic: one for the existence gate and one for the actual open/read operation.

## 5. Reconstructed lifecycle

```text
CODE.1
  delete old CHARS
  create/reinitialize CHARS
  close
  reopen CHARS
  write/transfer 0x920 bytes
  close
       |
       | process boundary via R.EXE
       v
CODE.2
  test CHARS existence/accessibility
       |
       +-- absent -> alternate initialization path
       |
       `-- present -> open CHARS
                     read/transfer 0x920 bytes
                     close
                     post-load processing
```

R.EXE independently deletes `CHARS` on its status-12 path and during final cleanup. The file is therefore confirmed as a transient cross-process state-transfer artifact rather than an ordinary persistent save file.

## 6. What is not yet promoted

The binary now proves producer, consumer, existence-test, transfer size, and launcher cleanup behavior. It does **not yet prove a user-facing semantic name** for launcher statuses 10 and 12. Those names remain withheld until the termination producers are traced into the CODE.1/CODE.2 application state machines.

## Next checkpoint

Trace callers of the CODE.1 producer (`~0x4FBF`) and CODE.2 startup gate (`~0x204B3`) into their high-level state transitions, then locate the application return/termination sites that feed R.EXE statuses 10 and 12. Correlate the status transitions with the now-proven CHARS lifecycle before assigning semantic enum names.