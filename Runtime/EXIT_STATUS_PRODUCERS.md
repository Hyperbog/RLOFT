# CODE.1 / CODE.2 Exit-Status Producer Reconstruction

Status: **ACTIVE — binary-proven termination mechanism; gameplay labels still under certification**

This checkpoint traces the launcher-visible process statuses backward into the protected-mode application rather than assigning meanings from the numeric values alone.

## 1. CODE.2 CRT termination path

The CODE.2 Watcom startup at logical `0x353A0` ultimately terminates through DOS `INT 21h / AH=4Ch` at `0x35666..0x35668`. The value returned by the application/termination machinery is therefore exposed as the DOS child-process status observed by `R.EXE`.

The startup path calls application main at logical `0x20608` through the Watcom runtime. The ordinary main path performs initialization and eventually invokes the runtime termination machinery; launcher-visible transition statuses are produced by an application-owned controlled-exit helper rather than by accidental CRT failure.

## 2. CODE.2 controlled-exit helper at `0x205B4`

Binary reconstruction:

```text
205B4  call 1A1D0              ; application cleanup
205B9  push [C65C]
205BF  call 310B0
205C7  push [C660]
205CD  call 310B0
205D5  call 3810
205DA  call 2F4E0
205DF  cmp  [esp+4], 0Ah
205E4  je   205FA
205E6  cmp  [esp+4], 0Ch
205EB  je   205FA
205ED  push FFh
205F2  call 290F4
205FA  push [esp+4]
205FE  call 352BE              ; CRT/process termination
```

This is decisive protocol evidence. Statuses **10 (`0x0A`) and 12 (`0x0C`) are explicitly privileged application transition statuses**: both bypass an additional cleanup/reset operation (`0x290F4(0xFF)`) that is performed for ordinary termination values, then are passed unchanged into the process-termination routine.

Thus 10 and 12 are not DOS errors and are not artifacts of `R.EXE`; CODE.2's own application layer knows that these two values have special inter-process meaning.

## 3. Certified CODE.2 status-12 producers

Direct calls to `0x205B4` establish multiple controlled status-12 producers:

- `0xCCE0`: `push 0Ch ; call 205B4`
- `0x23E10`: `push 0Ch ; call 205B4`
- `0x26BD9`: `push 0Ch ; call 205B4`

The `0xCCE0` producer follows an application/UI sequence, waits across two opposite states returned by `0x1A1F4`, and then requests controlled status 12. The `0x23E10` and `0x26BD9` producers each perform additional application cleanup immediately before requesting status 12.

These independent call sites prove that status 12 is an intentional high-level transition used from more than one gameplay/UI path.

Other calls to `0x205B4` use `0`, `-1`, or context-dependent values and therefore belong to normal quit/error paths unless later evidence establishes otherwise.

## 4. Important negative result for status 10

Every direct call site to CODE.2 helper `0x205B4` was enumerated. No direct call passes constant `0x0A`, despite the helper explicitly testing for `0x0A`.

Therefore the status-10 producer must not be invented from nearby literal `10` values. At least one of the following remains to be established:

1. a status-10 path reaches the underlying termination routine through another wrapper;
2. a dynamic argument can reach `0x205B4` as 10;
3. another application-return path propagates 10 through the Watcom startup termination path.

The explicit `cmp ...,0Ah` inside `0x205B4` nevertheless proves that CODE.2 was designed to recognize status 10 as part of the same transition protocol.

## 5. Relationship to R.EXE and CHARS

The three independently proven layers now align:

```text
CODE application
    |
    | controlled status 10 / 12
    v
CODE.2 termination helper 205B4
    |
    v
Watcom / DOS AH=4Ch termination
    |
    v
R.EXE spawn/wait result
    |
    +-- status 10 -> launcher transition branch
    |
    +-- status 12 -> delete CHARS, normalize state to 10,
                     continue launcher cycle
```

`CHARS` has separately been proven as a 0x920-byte cross-process state-transfer artifact written by CODE.1 and consumed/tested by CODE.2. This makes a character-generation/module-handoff interpretation increasingly well supported, but the numeric statuses are not yet assigned final gameplay names here: that requires correlating the status-producing UI paths with the already reconstructed application functions/strings.

## 6. Porting consequence

A native port must preserve this as an application state machine, not as literal DOS process exit codes. The DOS implementation communicates module transitions through child-process status plus `CHARS`; a port can replace both mechanisms with typed in-process state while preserving the proven transition semantics and serialization boundary.

## Next checkpoint

Trace the three CODE.2 status-12 producer functions upward to their UI/menu/game-state callers and locate the status-10 producer path. In parallel, trace CODE.1's CHARS-write completion path to its process termination value. Only after those paths converge should statuses 10 and 12 receive stable semantic enum names.