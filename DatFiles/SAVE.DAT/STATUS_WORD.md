# SAVE.DAT character status word

Status: **named player-facing conditions are closed through executable/resource cross-correlation; engine-only bits remain explicitly separated.** The field is serialized at character record `+0x010` and runtime `0x218F0 + character_index*0x248`.

```c
uint16_t status_word; // SAVE character +0x010
```

## Certified bit map

| Mask | Meaning | Evidence status |
|---:|---|---|
| `0x0040` | Vengeful-Spirit warning/coercion presentation/action-blocking state | behaviorally identified; original source name unknown |
| `0x0080` | recruited/template-character provenance | binary-proven |
| `0x0100` | possessed by the Vengeful Spirit | binary + story/resource proven |
| `0x0200` | Held | exact `T` status-display mapping |
| `0x0400` | Diseased | setter/curer + exact `T` mapping |
| `0x0800` | Poisoned | setter/curer + exact `T` mapping |
| `0x1000` | Paralyzed | setter/curer + exact `T` mapping |
| `0x2000` | engine-recognized action/eligibility blocking state; exact gameplay identity unproved | producer-unproven; preserve raw |
| `0x4000` | dead / zero-HP death state | binary-proven lifecycle |
| `0x8000` | empty/unoccupied active-party slot | binary-proven ownership |

Bits `0x0001..0x0020` are not assigned a gameplay name by the current direct-reference audit. Preserve them exactly rather than assuming they are padding.

## Exact status-screen mapping from CODE.2 + raw `T`

The character information/status display at physical `0x4DBD0..0x4DC58` initializes:

```text
mask    = 0x0100
T_index = 0x028D = 653
```

For each iteration it tests `status_word & mask`; when set it reads and displays the current T record. It then performs:

```text
T_index += 1
mask <<= 1
```

and stops after mask `0x1000`.

The supplied raw `T` resource decodes those five records exactly as:

```text
T[653]  POSSESSED
T[654]  HELD
T[655]  DISEASED
T[656]  POISONED
T[657]  PARALYZED
```

Therefore the direct serialized mapping is:

```text
0x0100  POSSESSED
0x0200  HELD
0x0400  DISEASED
0x0800  POISONED
0x1000  PARALYZED
```

This closes `0x0200` without needing to infer it from AD&D terminology or aggregate masks.

## `0x0040`: Vengeful-Spirit warning/coercion state

`0x0040` is **not** the Held condition; Held is independently `0x0200` above.

The only direct setter found for low-byte bit `0x40` is physical `0x528AC` inside the Vengeful-Spirit maintenance routine `0x5275C`. That routine:

1. scans the four active party records;
2. skips empty slots (`0x8000`);
3. requires possession bit `0x0100`;
4. tests persistent event flag `0x37` and current level to decide whether the warning can run;
5. clears `0x0040` before evaluating the warning path;
6. obtains the engine `rand()` value (`0..32767`) and computes `(rand * 4) >> 15`, giving exactly `0..3`;
7. selects `T[0x15F + value]`, i.e. `T[351..354]`;
8. displays the selected text with the affected character portrait through physical `0x576A0`;
9. sets `status_word |= 0x0040`.

The four exact raw-T messages are all Vengeful-Spirit coercion/warning dialogue. They warn the possessed character to fulfill the bargain, destroy the Ghoul Lord, return to the cemetery, or face destruction.

The same bit is cleared at physical `0x52792` on the next evaluation before any new warning is selected. The possession-resolution routine also clears `0x0040` together with `0x0100` via:

```text
status_word &= 0xFEBF
```

### Runtime consequence

The bit is not merely narrative bookkeeping:

- physical `0x5A320` maps `0x0040` to character presentation state code `1`, distinct from the normal state and from disease/poison/paralysis/death states;
- numerous actor-selection/action paths include `0x0040` in aggregate blocking masks such as `0xF240` and `0xD040`;
- physical `0x4FBFB` treats `0x0040 | 0x0100` as a combined exclusion condition in one party-selection UI path.

The safest preservation name is therefore **Vengeful-Spirit warning/coercion presentation/action-blocking state**. The historical source identifier and the exact duration semantics between maintenance passes are not asserted.

## `0x0080`: recruited/template provenance

Party-template installation copies a complete predefined `0x248` record into an active slot and sets low-byte bit `0x80`. RES9 recruitment reconstruction independently ties this flow to recruitable NPC/template characters.

This is metadata/provenance, not an ailment:

```text
status_word & 0x0080 != 0  -> recruited/template party member
```

The finale predicate uses this bit to distinguish original/non-recruited characters from recruited/template characters.

## Disease, poison, and paralysis lifecycle

Creature/status attack reconstruction independently establishes:

```text
0x0400 Diseased  <- NPC disease attack; Cure Disease clears it
0x0800 Poisoned  <- NPC poison attack; Neutralize Poison clears it
0x1000 Paralyzed <- NPC paralysis attack; Remove Paralysis clears it
```

The direct T mapping above independently confirms the player-facing names.

## `0x4000`: death state

Lethal HP depletion clamps current HP to zero and sets `0x4000`. The certified revival path restores current HP to `1` and clears `0x4000`.

This is distinct from `0x8000`: a dead character is still an occupied party record.

## `0x8000`: empty active-party slot

Party-slot population code scans for `0x8000`; when found it installs a complete character/template image into the slot. Unused quick-start active records also carry this high bit.

Do not infer occupancy from zero-filled bytes: unused records retain many nonzero/`0xFF` values.

## `0x2000`: engine-recognized but producer-unproven

A dedicated residual audit searched the certified CODE.2 image for direct status-word/high-byte writers and the literal masks needed to set or clear this bit. The direct-address status writers account for possession, Held-adjacent/story state, disease, poison, paralysis, death, revival, and empty/template ownership, but no direct `status |= 0x2000` / `status &= ~0x2000` lifecycle was found.

The bit is nevertheless consumed by shipped code:

- it is part of common aggregate mask `0xF240`;
- physical `0x47664` excludes status values at or above `0x2000` from one hand/action-selection path;
- other eligibility/action paths therefore reserve it as a higher-order blocking tier.

The named character-status display stops at `0x1000`; raw `T` supplies no sixth contiguous player-facing condition name for `0x2000`. The supplied quick-start fixture also has no populated record carrying this bit.

One physical path around `0x59A42` tests high-byte bit `0x20` only after an immediately preceding `status & 0xF240 == 0` gate. Since `0xF240` already contains `0x2000`, that branch cannot establish a distinct live meaning for the bit; it is best treated as redundant/dead legacy logic unless another producer is found.

The archival classification is therefore **engine-recognized action/eligibility blocking state with no proved producer or player-facing identity in the supplied build**. A computed-pointer writer cannot be ruled out solely by the direct-address audit, so the bit remains preserved rather than declared unused.

Do not label it `unconscious`, `petrified`, or another AD&D condition without producer evidence.

## Low bits `0x0001..0x0020`

The residual audit likewise found no defensible player-facing or lifecycle identity for status bits `0x0001`, `0x0002`, `0x0004`, `0x0008`, `0x0010`, or `0x0020`. They are outside the contiguous status-screen condition range and are not needed by the certified occupancy/provenance/possession/death mappings.

This is an evidence boundary, not proof that the bits were historically padding. A compatibility implementation must round-trip them exactly.

## Aggregate masks

Several important legacy predicates are compound masks rather than named-condition tests:

```text
0xF240 = 0x8000 | 0x4000 | 0x2000 | 0x1000 | 0x0200 | 0x0040
0xD040 = 0x8000 | 0x4000 | 0x1000 | 0x0040
0x0140 = 0x0100 | 0x0040
```

A native implementation should preserve the original raw predicates at compatibility boundaries. Do not replace them mechanically with a generic `is_incapacitated()` unless that helper reproduces the exact call-site mask.

## Native-port boundary

A high-level model may expose typed aliases for the certified bits while retaining the original word:

```cpp
struct LegacyCharacterStatus {
    uint16_t raw;
};

bool is_empty(uint16_t s)      { return (s & 0x8000u) != 0; }
bool is_dead(uint16_t s)       { return (s & 0x4000u) != 0; }
bool is_paralyzed(uint16_t s)  { return (s & 0x1000u) != 0; }
bool is_poisoned(uint16_t s)   { return (s & 0x0800u) != 0; }
bool is_diseased(uint16_t s)   { return (s & 0x0400u) != 0; }
bool is_held(uint16_t s)       { return (s & 0x0200u) != 0; }
bool is_possessed(uint16_t s)  { return (s & 0x0100u) != 0; }
bool is_recruited(uint16_t s)  { return (s & 0x0080u) != 0; }
```

Keep `0x0040`, `0x2000`, and every currently unnamed bit round-trippable even if the native rules layer does not expose them to gameplay code.