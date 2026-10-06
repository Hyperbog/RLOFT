# CINE10/CINE11 Finale Variant Predicate

Status: **CORRECTED / BINARY-PROVEN.**

This document supersedes the earlier revision that modeled the threshold as an `int16_t` comparison and used the obsolete `+0x18` character-relative status offset.

## 1. Event-10 special case

CODE.2 routine `0x13AD8` special-cases numbered event 10:

```text
13B69  cmp esi,0Ah
13B6C  jne 13BAA
```

Only event 10 enters the four-record party scan.

## 2. Certified character geometry

The active-party image begins at runtime `0x218E0`. Each character record is `0x248` bytes and the four-record span is exactly `0x920` bytes.

The status word is therefore:

```text
runtime 0x218F0 = character record +0x10
```

not `+0x18`.

## 3. Exact predicate and signedness correction

The relevant instructions are:

```asm
13B72  movzx ebx,word [eax+218F0h]
13B79  cmp   ebx,4000h
13B7F  jge   13B95
13B81  mov   bx,word [eax+218F0h]
13B88  xor   bh,bh
13B8A  and   bl,80h
13B8D  movzx ebx,bx
13B90  test  ebx,ebx
13B92  jne   13B95
13B94  inc   edx
```

`jge` is formally a signed 32-bit branch, but the status word was first loaded with `movzx`. Therefore `EBX` is always in `0..65535`, and the machine behavior is equivalent to an **unsigned 16-bit threshold test**:

```c
uint16_t status = character.status_word;
if (status < 0x4000 && (status & 0x0080) == 0)
    ++count;
```

The former reconstruction:

```c
(int16_t)status < 0x4000
```

was incorrect because it would treat values such as `0x8000` as negative and qualifying, contrary to the zero-extended machine comparison.

## 4. Certified gameplay semantics

Later cross-subsystem work establishes:

- `0x0080` = party-template / recruited-character provenance;
- `0x4000` = zero-HP death state, set on lethal HP depletion and cleared by revival;
- `0x8000` = empty/unoccupied active-party slot.

Therefore event 10 selects:

```text
CINE10.ANM
    if at least one living original/non-recruited party character remains

CINE11.ANM
    if no living original/non-recruited party character remains
```

Dead original characters and empty slots fail the `< 0x4000` test. Living recruited/template characters fail the `0x0080` test.

## 5. Porting form

```cpp
bool use_cine10 = false;
for (int i = 0; i < 4; ++i) {
    const uint16_t s = party[i].legacy_status;
    if (s < 0x4000u && (s & 0x0080u) == 0) {
        use_cine10 = true;
        break;
    }
}
play_cinematic(use_cine10 ? 10 : 11);
```

This is the canonical predicate for reconstruction and preservation work.
