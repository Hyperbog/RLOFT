# Non-EH RES records

Status: **the 75 non-EH records in `RES9`/`RES12` are structurally identified as interaction/display wordcode containers; the 38-entry `RES9` bank has a direct CODE.2 loader/interpreter and exact `INIT`-derived base. The two four-byte `RES1` records remain unresolved sentinels.**

The immutable archives, `INIT`, and original CODE.2 bytes are controlling evidence.

## Corpus

The 77 non-empty records that do not begin with `EH` are:

- `RES1:381` and `RES1:427`: four bytes `60 00 00 00`;
- `RES9:517..554`: 38 wordcode containers;
- `RES12:370..406`: 37 wordcode-shaped containers.

The two `RES1` records are each immediately preceded by a zero-length archive entry. No direct consumer has yet been proven, so they remain preserved as four-byte sentinels rather than being assigned a speculative resource type.

## `RES9` / `RES12` container shape

All 75 larger non-EH records have the same exact outer shape:

```text
uint16 word_count
uint16 program_words[word_count]
uint16 zero_bank[word_count]
```

Therefore:

```text
entry_size == 2 + 4 * word_count
```

Every supplied `zero_bank` word is exactly zero. The counts are:

- `RES9`: 38 records, `word_count` 29..606, 11,396 program words total;
- `RES12`: 37 records, `word_count` 22..375, 4,820 program words total.

`analyze_res.py` validates this shape and the all-zero second bank for all 75 records.

The zero bank is **not** loaded by the proven `RES9` runtime path below. It is safest to describe it as an on-disk zero-initialized companion/state bank until a writer or alternate consumer proves its intended lifetime.

## Direct CODE.2 loader for the `RES9` bank

Routine `13F70` bypasses the normal `EH` resource loader:

1. calls `2EC74(9)` to obtain archive 9's raw file handle;
2. computes an entry index from two `INIT`-derived globals plus the requested program ID;
3. calls archive-entry seeker `31334(handle,index)`;
4. reads the leading 16-bit `word_count`;
5. reads exactly `word_count * 2` bytes into word buffer `0x13020`;
6. resets word instruction pointer `0x7FEC` to zero.

It does **not** read the second `word_count * 2` zero bank.

The entry-index calculation is:

```text
entry = 0x258CC + program_id + 0x7FCC
```

CODE.2 routine `16D04` loads both globals from the supplied `INIT` file:

```text
INIT +0x1E -> 0x258CC = 103
INIT +0x22 -> 0x7FCC  = 414
```

Thus:

```text
103 + 414 = 517
entry = 517 + program_id
```

This proves that `RES9:517..554` are program IDs `0..37` exactly.

The same `INIT` relationship also explains archive 9's layout: entries `103..516` are 414 typed resources immediately preceding the wordcode bank.

## Interpreter

CODE.2 `15534` fetches one 16-bit word from `0x13020`, increments `0x7FEC`, rejects values above `0x29`, and dispatches opcodes `0..0x29` through the 42-entry jump table at `1548C`.

The interpreter is not merely an animation sequencer. Proven handlers include:

| Opcode | Handler | Proven behavior |
|---:|---|---|
| `0x10` | `15A82` | draws an archive-9 typed resource using the current archive-9 sub-base and encoded coordinates |
| `0x17` | `15593` | sets archive-9 resource sub-base global `0x7FF0` |
| `0x18` | `15825` | absolute wordcode jump |
| `0x19` | `15575` | sets indexed-text base global `0x7FF4` |
| `0x1C` | `15D3D` | plays a sound-effect ID after adding the `INIT`-derived sound base at `0x258C8` |
| `0x1D` | `15D7A` | performs a music-sequence transition/start through the `28Exx/29xxx` subsystem |
| `0x1E` | `15983` | sets one bit in wordcode state flags `0x7FD0` |
| `0x1F` | `159AA` | clears one bit in `0x7FD0` |
| `0x20` | `159D1` | toggles one bit in `0x7FD0` |
| `0x21` | `158AE` | conditional branch if a state bit is set |
| `0x22` | `158E8` | conditional branch if a state bit is clear |
| `0x24` | `15DCA` | invokes the cinematic path `13EF8` and terminates the active interpreter run |
| `0x27` | `155B1` | sets an additional sound/index base global `0x7FF8` |

The table above is deliberately limited to handlers whose semantics are already cross-supported by CODE.2's known text/audio/resource/cinematic subsystems. Several other opcodes call helpers that consume additional words internally; a complete instruction-length specification remains future work.

### Text base (`opcode 0x19`)

`0x7FF4` is later added to indexed text IDs before calls to `1C368`, the same indexed-text reader already identified in DAT/cinematic work. It is therefore safely a per-program **text-record base**.

### Archive-9 resource base (`opcode 0x17`)

Draw handlers use:

```text
archive 9 entry = INIT_resource_base + 0x7FF0 + encoded_resource_delta
                = 103 + 0x7FF0 + encoded_resource_delta
```

so `0x7FF0` is safely a per-program archive-9 typed-resource sub-base.

### Sound/index base (`opcode 0x27`)

A dialog/audio path adds `0x7FF8` to `INIT +0x20` before direct sound playback. The supplied `INIT +0x20` value is 48. This makes `0x7FF8` a per-program sound/index sub-base; the exact historical source name remains unknown.

## Higher-level role: interaction/display wordcode

Routine `16524(program_id)` initializes an interaction/display state, calls `13F70(program_id)`, draws archive-9 resources, runs the interpreter, and later restores music/display state.

Its callers prove the programs are broader than NPC animation:

- several gameplay/story paths call fixed IDs such as `1`, `9`, `0x15`, `0x17`, `0x19`, `0x1A`, and `0x1C`;
- `21B11` derives the program ID from an interactive world-record type: type `0x52` maps to ID `0x25`, otherwise the eligible type is converted with `type - 0x22` before calling `16524`.

The safe semantic classification is therefore **interaction/display wordcode**. A numerical correlation between 38 `RES9` programs and 38 combat-capable NPC definitions is not used as a format claim.

## `RES12` relationship

`RES12:370..406` obeys the identical count/program/zero-bank shape. The 37 slots also show strong ordinal correspondence to `RES9` programs: for all 37 shared slot numbers, the early opcode `0x19` operand (text-record base) matches exactly.

However, the direct raw CODE.2 loader proven above obtains archive **9**, not archive 12. No equivalent raw archive-12 loader has yet been established in the audited CODE.2 object. `RES12` is therefore documented as an **ID-aligned alternate wordcode bank** without asserting when or whether this executable selects it.

## Remaining work

- finish all 42 opcode semantics and instruction-length/control-flow rules;
- determine the intended purpose of the serialized all-zero companion bank;
- trace `RES12` selection or prove it is unused/alternate data for this executable;
- resolve the two `RES1` four-byte sentinels.
