# CODE.2 Status-12 State/UI Ownership

Status: **ACTIVE — three binary-proven status-12 producers classified more tightly; final user-facing enum remains conservative**

This checkpoint follows the three direct CODE.2 producers of controlled process status `12` (`0x0C`) into their immediately owning application logic. The immutable CODE.2 bytes remain controlling evidence.

## 1. Producer at `0xCCE0`: completed modal/presentation sequence

The containing routine begins at `0xCBC8`. It constructs a nine-dword local table from `0xC6E0`, performs presentation/setup calls, then iterates `EBX=0..8` through a UI/resource sequence. The loop includes display/resource operations (`0x2E608`, `0x2E510`, `0x1EA04`), special handling for indices 0 and 6, and per-index presentation through `0x2E614` for the ordinary cases.

After all nine iterations complete, the routine deliberately waits for an input/state transition using `0x1A1F4` twice:

```text
0xCCCE: call 0x1A1F4
         loop while result != 0
0xCCD7: call 0x1A1F4
         loop while result == 0
0xCCE0: push 0x0C
0xCCE2: call 0x205B4
```

Thus this producer is not an error path. It is the terminal action of a completed modal/presentation interaction and intentionally requests the privileged status-12 transition after a release/press-style two-phase input wait.

## 2. Producer at `0x23E10`: reset/transition helper

The status-12 tail is:

```text
0x23DF0: mov byte [0xBF38],0
0x23DF7: call 0x1F1C8
0x23DFC: call 0x1EA78
0x23E01: call 0x28168
0x23E06: push 0x0A
0x23E08: call 0x13EF8
0x23E10: push 0x0C
0x23E12: call 0x205B4
```

This confirms the earlier classification: state byte `BF38` is cleared, several application cleanup/presentation routines run, application routine `13EF8` receives argument 10, and only then is process status 12 requested. The literal 10 is therefore an application argument, not the launcher status.

The code immediately following this non-returning-style transition tail is a separate predicate beginning at `0x23E1C`; it must not be folded into the status-12 semantics.

## 3. Producer at `0x26BD9`: gated high-level state transition

This site is the strongest ownership result in this checkpoint. Before status 12 can be requested, the routine requires **five independent application predicates** to succeed:

```text
call 0xF4B0(0x74)  != 0
call 0xF4B0(0x5C)  != 0
call 0xF4B0(0x76)  != 0
call 0xF4B0(0x8B)  != 0
call 0xF4B0(0x77)  != 0
```

If any predicate fails, execution branches to `0x26BF0`, writes state value `0x35` to global `BF44`, performs the ordinary return/cleanup path, and does **not** terminate the process.

Only when all five predicates succeed does the routine execute:

```text
0x26BB9: mov byte [0xBF38],0
0x26BC0: call 0x1F1C8
0x26BC5: call 0x1EA78
0x26BCA: call 0x28168
0x26BCF: push 0x0A
0x26BD1: call 0x13EF8
0x26BD9: push 0x0C
0x26BDB: call 0x205B4
```

This is the same transition tail used at `0x23E10`. Therefore those two producers converge on a common high-level reset/restart mechanism rather than representing unrelated process exits.

The surrounding dispatcher also assigns other explicit `BF44` state values (for example `0x35` on the failed gate and `0x6B` in the subsequent branch), confirming that this region is application state-machine logic.

## 4. Protocol-level conclusion

The three status-12 producers now divide into two ownership forms:

1. `0xCCE0` — terminal action after a completed modal/presentation and two-phase input transition.
2. `0x23E10` and `0x26BD9` — common reset/transition tail; the latter is reachable only after five successful application predicates.

All three deliberately invoke the application-controlled exit helper with 12. Combined with the already proven R.EXE behavior — delete `CHARS`, normalize supervisor state to 10, restart the module cycle — status 12 can now be promoted safely to:

**`RESTART_CODE_CYCLE_WITHOUT_CHARS`** (protocol semantic name)

This name describes only proven machine behavior. A stronger user-facing label such as `RESTART_CHARACTER_GENERATION`, `NEW_GAME`, or `RETURN_TO_CHARACTER_CREATION` is still withheld because the exact meanings of predicate IDs `0x74/0x5C/0x76/0x8B/0x77`, state values such as `BF44=0x35`, and the `0xCBC8` presentation content have not yet been correlated to their reconstructed strings/UI identities.

## 5. Current certified transition model

```text
CODE.1 completed character-state path
    -> serialize CHARS (0x920 bytes)
    -> status 10
    -> R.EXE launches CODE.2
    -> CODE.2 consumes/tests CHARS

CODE.2 controlled status 12
    -> R.EXE deletes CHARS
    -> R.EXE converts supervisor state to 10
    -> supervisor cycle restarts without the transferred CHARS state
```

For a native port, these are now sufficiently proven to model as typed state-machine transitions rather than DOS process codes/files. The user-facing cause of status 12 remains the next certification target.

## Next checkpoint

Resolve `0xF4B0` predicate IDs `74h`, `5Ch`, `76h`, `8Bh`, `77h`, identify `BF44`'s state-machine role, and correlate the `0xCBC8` nine-entry presentation with reconstructed strings/resources. If these converge on character creation/new-game UI, promote the protocol name to a gameplay-semantic enum; otherwise retain the protocol-level name.