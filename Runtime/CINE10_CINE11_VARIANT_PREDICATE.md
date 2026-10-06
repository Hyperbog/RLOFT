# CINE10/CINE11 Finale Variant Predicate

Status: **BINARY-PROVEN STRUCTURAL PREDICATE; gameplay names for the tested status bits remain unresolved**

This checkpoint resolves the exact CODE.2 condition that changes numbered cinematic event 10 into event 11. It deliberately does not assign speculative gameplay labels to the underlying character-status bits.

## 1. Event-10 special case

In the cinematic/event setup routine beginning at logical `0x13AD8`, the selected numbered event is held in `ESI`. After unrelated setup, CODE.2 checks:

```text
0x13B69  cmp esi,0x0A
0x13B6C  jne 0x13BAA
```

Only numbered event 10 enters the party-record scan below. Therefore this is a special finale-variant rule, not general cinematic selection logic.

## 2. Four active-party records are examined

The loop begins with `EDX=0` and `EAX=0`, reads the word at runtime `0x218F0 + EAX`, advances `EAX` by `0x248`, and stops at `0x920`:

```text
0x13B6E  xor edx,edx
0x13B70  xor eax,eax
...
0x13B95  add eax,0x248
0x13B9A  cmp eax,0x920
0x13B9F  jne 0x13B72
```

Thus exactly four `0x248` active-party character records are tested. The repository's independent SAVE.DAT reconstruction identifies runtime `0x218F0` as record offset `+0x18`, the 16-bit character status word.

## 3. Per-character predicate

For each party record CODE.2 executes the equivalent of:

```c
uint16_t status = character.status_word;

if ((int16_t)status < 0x4000) {
    if ((status & 0x0080) == 0)
        ++count;
}
```

Instruction evidence:

```text
13B72  movzx ebx,word [eax+218F0]
13B79  cmp   ebx,4000h
13B7F  jge   13B95

13B81  mov   bx,word [eax+218F0]
13B88  xor   bh,bh
13B8A  and   bl,80h
13B8D  movzx ebx,bx
13B90  test  ebx,ebx
13B92  jne   13B95
13B94  inc   edx
```

The `jge` is a signed comparison, although values reaching this structure should continue to be preserved as raw legacy status words until all high-bit semantics are certified.

A character contributes to `count` only when both conditions hold:

1. its status word is signed-less-than `0x4000`; and
2. status bit `0x0080` is clear.

## 4. Variant selection

After all four records:

```text
13BA1  test edx,edx
13BA3  jne  13BAA
13BA5  mov  esi,0x0B
```

Therefore:

```text
start with event 10

if ANY of the four party records satisfies
    status < 0x4000 AND !(status & 0x0080)
then
    retain event 10 -> CINE10.ANM
else
    rewrite event number to 11 -> CINE11.ANM
```

Equivalently, `CINE11.ANM` is selected only when **none** of the four active-party records satisfies that status predicate.

## 5. What is certified vs unresolved

Certified:

- the tested field is the per-character 16-bit status word at record `+0x18` / runtime `0x218F0`;
- four records are examined at stride `0x248` across exactly `0x920` bytes;
- mask `0x0080` and threshold `0x4000` control the event-10/event-11 split;
- event 10 remains `CINE10.ANM` when at least one record passes;
- event 10 is rewritten to event 11, hence `CINE11.ANM`, when no record passes.

Not yet certified:

- the exact gameplay name of status bit `0x0080`;
- the exact semantic category represented by status values `>= 0x4000`;
- therefore the human-facing description of the difference between the two finale cinematics.

Existing character-record work already proves that this word is a status/condition field and that its high byte contains multiple executable-managed condition bits. That is sufficient to identify the variant selector structurally, but not to label it as death, incapacitation, petrification, party survival, or another specific gameplay concept without further xref proof.

## Porting consequence

A faithful port should initially preserve the original predicate literally:

```c
bool cine10_variant = false;
for (int i = 0; i < 4; ++i) {
    uint16_t s = party[i].legacy_status;
    if ((int16_t)s < 0x4000 && (s & 0x0080) == 0) {
        cine10_variant = true;
        break;
    }
}

play_cinematic(cine10_variant ? 10 : 11);
```

The symbolic predicate should only be renamed after the `0x0080` and `0x4000` status semantics are independently certified.

## Next checkpoint

Trace all executable readers/writers of status bit `0x0080` and status bit/threshold `0x4000`, correlate them with combat, healing, death/incapacitation, portrait/UI, and SAVE.DAT behavior, then assign the CINE10/CINE11 ending variants their exact gameplay meaning if the evidence converges.