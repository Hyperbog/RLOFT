# CODE.1 Status-10 Propagation and CHARS Handoff

Status: **CONFIRMED — launcher-visible status 10 and CHARS serialization are joined in CODE.1**

This checkpoint closes the principal uncertainty left by `EXIT_STATUS_SEMANTICS_CHECKPOINT.md`: the value returned by CODE.1 routine `0x4978` is not merely an internal application result. It propagates directly into the CODE.1 controlled termination path and therefore becomes the child-process status observed by `R.EXE`.

## 1. CODE.1 application entry/termination chain

The Watcom startup path reaches application routine `0x508C` (startup call at `0xFCD9`). `0x508C` validates its first argument against `2`; on the accepted path it performs setup through `0x4DC4`, then executes:

```text
50AE  call 4978
50B3  push eax
50B4  call 5014
```

Therefore the return value from `0x4978` is passed unmodified as the sole argument to `0x5014`.

`0x5014` is a non-returning controlled termination routine. Its tail loads its argument, pushes it, and jumps to `0xCBF0`, the already audited termination path:

```text
5081  mov eax,[ebp+08]
5084  push eax
5085  jmp  CBF0
```

Thus `0x4978 -> 0x5014 -> 0xCBF0` is a direct application-to-process-status chain.

## 2. Status 10 from 0x4978 is launcher-visible

The terminal state test in `0x4978` is:

```text
49D4  cmp dword [04CC],-1
49DB  jne 49B6
49DD  cmp byte [04C4],0
49E4  je  49F8
49E6  cmp byte [04C5],0
49ED  je  49F8
49EF  mov eax,0Ah
49F4  ...
49F7  ret

49F8  xor eax,eax
49FD  ret
```

Consequently, when `[04CC] == -1` and both byte flags `[04C4]` and `[04C5]` are nonzero, CODE.1 returns **10** to `0x5014`, which terminates the process with that status. If either flag is zero, this path returns zero instead.

This is now a certified CODE.1 producer of launcher-visible status 10.

## 3. 0x5014 couples status 10 to CHARS serialization

`0x5014` explicitly distinguishes status 10:

```text
5050  cmp [ebp+08],0Ah
5054  je  5065
5056  push FFh
505B  call 64DC
...
5065  cmp byte [04C4],0
506C  je  5081
506E  cmp byte [04C5],0
5075  je  5081
5077  call 5F4C
507C  call 4EF0
5081  push [ebp+08]
5085  jmp  CBF0
```

The previously reconstructed `0x4EF0` path is the CODE.1 `CHARS` producer. Its late file sequence creates/opens the `CHARS` artifact and transfers exactly `0x920` bytes from the character-state buffer before closing the handle.

Therefore status 10 is not merely temporally adjacent to the handoff file. CODE.1's controlled-exit routine explicitly gates the `CHARS` serialization path on:

1. requested process status == 10; and
2. both `[04C4]` and `[04C5]` state flags being present.

Only after that serialization does termination continue with status 10.

## 4. Independent status-10 producer

CODE.1 also contains a direct status-10 request in the `0x9AE8` state/event routine:

```text
9AF5  test byte [2B01],02h
9AFC  je   9B28
9AFE  test byte [2B01],80h
9B05  jne  9B28
9B07  cmp  byte [2B02],0
9B0E  jne  9B28
9B10  mov  byte [2B02],1
9B17  mov  byte [2B01],0
9B1E  push 0Ah
9B20  call 5014
```

This proves a second application-owned route to the same status-10 termination helper. The protocol is therefore not an incidental consequence of the `0x4978` return convention.

## 5. Semantic promotion

The following interpretation is now binary-proven at the process boundary:

- **CODE.1 status 10 = successful module handoff/continuation.**
- On the normal completed-state path, CODE.1 serializes the `0x920`-byte `CHARS` transfer artifact immediately before terminating with status 10.
- `R.EXE` observes 10 and resumes the module cycle at CODE.2.
- CODE.2 independently tests/consumes `CHARS` on its receiving side.

Given the surrounding CODE.1 state model (two presence flags, actor-selection/generation routines, character-state records) and CODE.2's independently documented `GENERATING CHARACTERS` / `CHARACTER BASICS` string neighborhood, it is now safe to describe this specific path as the **character-state handoff from CODE.1 back to CODE.2**. This does not imply that every possible status-10 producer has identical user-facing context; status 10 remains the general launcher continuation code.

## 6. Porting consequence

A native port should model this transition as a typed state-machine result rather than a process exit:

```text
CODE.1 character-state workflow
    -> completed handoff condition
    -> serialize/transfer character state (DOS: CHARS, 0x920 bytes)
    -> transition result CONTINUE_TO_CODE2 (DOS status 10)
    -> CODE.2 imports transferred state
```

The serialized DOS file can be replaced by an in-memory typed object only after its `0x920`-byte schema is reconstructed; until then the binary serialization boundary remains preservation-significant.

## Next checkpoint

Trace CODE.2 status-12 producers to their owning dispatch/UI states and determine whether status 12 can now be promoted from generic controlled reset/restart to a specific character-workflow semantic. Then correlate CODE.2's CHARS-present and CHARS-absent startup branches with those status-12 paths.