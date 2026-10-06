# Character-status ending predicate reconstruction

Status: **BINARY-PROVEN STRUCTURE; gameplay names remain conservative where writer ownership is not yet fully resolved**

This checkpoint follows the two status-word conditions used by numbered finale event 10 when selecting `CINE10.ANM` versus `CINE11.ANM`.

## 1. Finale selector

At CODE.2 logical `0x13B6E..0x13BA5`, event 10 iterates all four active-party records (`4 * 0x248 = 0x920`). For each record it reads the 16-bit status word at runtime `0x218F0 + character*0x248` and counts the record only when both conditions hold:

```text
status < 0x4000
(status & 0x0080) == 0
```

If at least one record qualifies, event 10 remains 10 and the cinematic constructor opens `CINE10.ANM`. If none qualify, the event number is changed to 11 and `CINE11.ANM` is opened.

## 2. `0x4000` threshold is a general actor-usability/life-state boundary

The `0x4000` comparison is not unique to the ending. CODE.2 repeatedly excludes characters with status values at or above `0x4000` from ordinary gameplay selection/processing.

A compact example is helper `0xD4E8`: it indexes a party record, loads `status`, and immediately returns a rejecting result when `status >= 0x4000` before considering other per-character state flags.

Other actor-selection paths at `0xD56C`, `0xD6F3`, and elsewhere use the same boundary before targeting or processing party members. This establishes `0x4000` as part of the engine's high-order unavailable/non-normal actor-state range. The exact original enum label is still withheld.

## 3. `0x0080` is an explicit low-byte character-state flag

CODE.2 has a direct writer at logical `0x14A53`:

```text
or byte ptr [character + 0x218F0], 0x80
```

This occurs at the end of a substantial per-character initialization/setup routine, after class/casting capability initialization. Therefore bit `0x0080` is persistent character state, not a temporary arithmetic artifact.

A second independent consumer appears in the cinematic/event logic around `0x13B46`: for event value 2, CODE.2 counts party records for which the **high-byte `0x80` bit (`0x8000`) is clear** and adjusts the numbered event accordingly. This demonstrates that the 16-bit status word contains multiple distinct high/low state classes and that the finale's low-byte `0x0080` test must not be conflated with `0x8000`.

## 4. `0x8000` is written as a complete terminal/special status

Several application/UI paths explicitly replace the entire character status word with `0x8000` (for example `0x2CB9E`, `0x2CC17`, `0x2CC63`, with parallel writers later in the same subsystem). These writes occur after per-character UI/action calls and mark the affected party record with a special high-order status.

This is distinct from the finale test's `status >= 0x4000`: the threshold deliberately treats `0x4000..0xFFFF` as outside the normal qualifying range, including but not limited to the explicit `0x8000` state.

## 5. Certified ending rule

The strongest safe interpretation is therefore:

```text
CINE10.ANM
    if ANY active-party record is in the normal (<0x4000)
    status range AND does not carry low-byte flag 0x80

CINE11.ANM
    if NO active-party record satisfies both conditions
```

This proves that the ending split is based on **party character status**, not inventory, random choice, or a generic global quest flag.

It is still premature to name the two variants `survival/death`, `good/bad`, or similar. To certify that language, the remaining task is to identify the gameplay action represented by the `0x14A53` writer of low-byte `0x80` and the specific producers of the `0x4000`-range statuses.

## 6. Preservation consequence

A native port must preserve the full 16-bit status word and the finale predicate exactly. It must not reduce character state to a single Boolean alive/dead field until the individual status encodings are completely reconstructed.

## Next checkpoint

Trace the owning routine/callers of `0x14A53`, classify its gameplay action, then enumerate direct writers that place party status into the `0x4000..0x7FFF` range. If those converge on incapacitation/death semantics, promote the CINE10/CINE11 selector to a user-facing ending classification.