# CODE.2 character status `0x4000` death-state lifecycle

Status: **BINARY-PROVEN DEATH/REVIVAL STATE MECHANISM**

This checkpoint closes the remaining semantic ambiguity around the high-byte `0x4000` party-character status used by the CINE10/CINE11 finale selector.

## 1. Damage sets `0x4000` when hit points reach zero

The CODE.2 damage path at logical `0xCD44` operates on one active-party record (`index * 0x248`). After mitigation/absorption handling it subtracts the remaining damage from the 16-bit value at record offset `+0x1C` (runtime `0x218FC`):

```asm
CDF4  imul ebx,edi,248h
CDFA  sub  word [ebx+218FCh],si
CE08  cmp  word [ebx+218FCh],0
CE10  jg   CE93
CE16  mov  word [ebx+218FCh],0
CE1F  or   byte [ebx+218F1h],40h
CE27  and  byte [ebx+218F1h],E3h
```

Because `0x218F1` is the high byte of the 16-bit status word at `+0x18`, `or ... 0x40` sets word mask `0x4000` exactly when the character's current HP reaches zero or below. The HP field is clamped to zero.

A second independent transition around `0xD48D` also zeroes current HP and sets the same `0x4000` status, confirming this is an intentional actor-state transition rather than a side effect unique to one damage routine.

## 2. A recovery/revival path clears `0x4000`

The application action dispatcher beginning at `0x9D90` contains the inverse transition. After validating the target character and associated state, the path at `0x9F72` executes:

```asm
9F72  push edx
9F73  mov  word [eax+218FCh],1
9F7C  push [66B4h]
9F82  and  byte [eax+218F1h],BFh
9F8E  mov  byte [esi+CF5Ah],1
9F95  dec  byte [eax+21917h]
...
```

`0xBF == ~0x40`, so this path explicitly:

1. restores current HP from zero to **1**;
2. clears high-byte status bit `0x40`, i.e. word mask **`0x4000`**;
3. updates the character/UI state;
4. consumes/decrements an associated character value/resource.

This is the direct inverse of the zero-HP transition. Therefore `0x4000` is a reversible **death/revival state**, not merely a generic combat-selection exclusion.

The historical source-level name of the recovery action is not asserted here because its spell/item/action ID has not yet been tied to a named resource. Structurally, however, it is unequivocally a revival operation: zero-HP/death-state character -> 1 HP -> clear `0x4000`.

## 3. Certified status semantics

The following names are now safe for reconstruction:

```text
status & 0x4000 != 0  -> DEAD / ZERO-HP DEATH STATE
status & 0x4000 == 0  -> not in that death state
status & 0x8000 != 0  -> empty/unoccupied party slot (separate state)
status & 0x0080 != 0  -> recruited/template-character provenance
```

`0x4000` and `0x8000` must remain distinct. The latter is used by party-population code as an empty-slot marker, whereas `0x4000` is set by HP depletion and cleared by revival.

## 4. Finale meaning can now be promoted

Numbered finale event 10 counts a party record only when:

```c
status < 0x4000 && !(status & 0x0080)
```

Combined with the separately certified `0x0080` template/recruited-character provenance, the user-facing distinction is now supportable:

```text
CINE10.ANM
    at least one living original/non-recruited party character remains

CINE11.ANM
    no living original/non-recruited party character remains
```

Empty slots (`0x8000`) and dead original characters (`0x4000`) cannot satisfy the CINE10 predicate. Recruited/template characters are excluded by `0x0080` even when alive.

Thus the finale does not simply ask whether *any* party member survives. It specifically tests survival of the original/non-recruited party class.

## 5. Porting consequence

A faithful port can promote the legacy status bit to a typed death-state flag while preserving the original word for compatibility:

```c
bool is_dead(const Character& c) {
    return (c.legacy_status & 0x4000) != 0;
}
```

The original transition semantics should remain exact: lethal HP depletion clamps HP to zero and sets `0x4000`; the proven revival path restores HP to one and clears `0x4000`.

## Next checkpoint

Return to the two remaining status-12 ownership paths (`0xCBC8/0xCCE0` and `0x23DF0/0x23E10`) and correlate their presentation/resources and callers. The five-item finale producer is already independently classified; these remaining producers will determine whether status 12 has one gameplay-level meaning or is intentionally a shared supervisor restart primitive.