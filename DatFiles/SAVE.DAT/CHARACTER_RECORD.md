# SAVE.DAT `0x248` character record

Status: **offset-normalized executable-certified reconstruction.** Runtime character base is DGROUP `0x218E0`; the same fixed image is serialized for four active-party slots and ten saved-roster slots. Runtime character `n` begins at `0x218E0 + n*0x248`.

## Offset-correction note

Earlier SAVE notes derived record offsets from an incorrect base of `0x218D8`. Runtime-derived serialized offsets in those revisions were eight bytes too high. The `0x248` stride and top-level SAVE boundaries were always correct.

The correction is independently anchored by runtime `0x218FC/0x218FE` = current/max HP (`+0x01C/+0x01E`), runtime `0x21900..0x21904` = three class levels (`+0x020..+0x024`), and runtime item storage `0x21920` = record `+0x040`.

## Current authoritative partition

```text
+0x000..+0x00F   fixed 16-byte name storage
+0x010..+0x011   status word; high bit participates in inactive-record filtering
+0x012..+0x014   identity/core metadata; semantics pending
+0x015..+0x017   int8 class_code[3]; -1 absent later class
+0x018..+0x01B   core/identity fields; semantics pending
+0x01C..+0x01D   int16 current_hp
+0x01E..+0x01F   int16 max_hp
+0x020..+0x025   int16 class_level[3]
+0x026..+0x031   int32 class_progress[3]; -1 absent sentinel
+0x032..+0x039   four paired ability-shaped byte fields; exact identities pending
+0x03A..+0x03F   three uint16/core fields; semantics pending
+0x040..+0x069   int16 item[21]
+0x06A..+0x0B9   spell matrix A: 8 x 10 packed cells; scribeable spellbook family
+0x0BA..+0x109   spell matrix B: 8 x 10 packed cells; second casting family
+0x10A           class/level-derived base attack target
+0x10B..+0x112   uint32 hand_ready_deadline[2]
+0x113..+0x116   Vampiric Touch absorption pool
+0x117..+0x11A   primary-effect-0 absorption pool
+0x11B..+0x122   reserved/unreferenced serialized bytes
+0x123..+0x126   reversible Strength modifier for primary effect index 12
+0x127..+0x12A   auxiliary dword; low byte participates in reversible Dexterity adjustment
+0x12B..+0x12E   uint32 item_known_bits; bits 0..20 map item[0..20]
+0x12F           selected spell bank
+0x130           spell-list scroll position
+0x131           class/casting capability selector/mask
+0x132..+0x135   uint32 primary active-effect bits
+0x136..+0x19D   uint32 primary_effect_expiry[26]
+0x19E..+0x1A9   uint32 timed_slot_deadline[3]
+0x1AA..+0x1B5   uint32 timed_slot_aux[3]
+0x1B6..+0x1B9   Stone Giant Gauntlets displaced/effective Strength auxiliary
+0x1BA..+0x1BD   serialized dword; no direct CODE.2 reference found
+0x1BE..+0x1C1   reversible modifier for primary effect index 23
+0x1C2..+0x1C5   uint32 temporary_hand_effect_bits
+0x1C6..+0x1E9   uint32 temporary_hand_effect_expiry[9]
+0x1EA..+0x1ED   consumable/additive persistent gameplay pool
+0x1EE..+0x1F1   recurring Strength-recovery deadline
+0x1F2..+0x1F5   accumulated state associated with primary effect index 6
+0x1F6..+0x247   82-byte serialized reserved/unreferenced terminal region
```

## Certified core fields

| Offset | Size | Meaning |
|---|---:|---|
| `+0x000` | `0x10` | character name storage |
| `+0x010` | 2 | status word; high bit distinguishes inactive records in audited party/UI path |
| `+0x015` | 3 | signed `class_code[3]`; `-1` marks absent later slots |
| `+0x01C` | 2 | current hit points |
| `+0x01E` | 2 | maximum hit points |
| `+0x020` | 6 | legacy `class_level[3]` words |
| `+0x026` | 12 | `class_progress[3]` dwords; `0xFFFFFFFF` absent sentinel |
| `+0x032` | 1 | Strength base/reference |
| `+0x033` | 1 | Strength current/effective |
| `+0x035` | 1 | Dexterity current/effective |
| `+0x03E` | 1 | exceptional Strength base percentile |
| `+0x03F` | 1 | exceptional Strength current percentile |
| `+0x040` | `0x2A` | signed `int16_le item[21]` |

## Record activity

The status word is runtime `0x218F0`, serialized `+0x010`. Party/UI iteration skips the populated-character path when the unsigned word is `>= 0x8000`.

```c
bool record_is_active = ((status_word & 0x8000u) == 0);
```

Unused fixture records contain `00 80` at `+0x010..+0x011`; an all-zero occupancy test is invalid. Known status masks include disease `0x0400`, poison `0x0800`, and paralysis `0x1000`.

## Class progression

The three signed class-code bytes are followed by three level words and three progression dwords. CODE.2 counts class slots using `-1` as the absent class-code sentinel, selects class levels for combat progression, and updates corresponding progression dwords through class-indexed advancement logic. Player-facing names for numeric class codes remain a separate mapping target.

## Inventory

```text
item[0..11]   ordinary inventory grid
item[12]      ring A
item[13]      ring B
item[14]      hand A
item[15]      hand B
item[16]      torso/body garment
item[17]      head
item[18]      dedicated accessory
item[19]      quiver
item[20]      bracer/gauntlet
```

Entries are signed 16-bit values. `-1 / 0xFFFF` is the ordinary empty sentinel, but other negative values must be preserved.

## Dual spell-preparation matrices

Both matrices are 80-byte arrays:

```text
cell = matrix[bank * 10 + entry]
bank  = 0..7
entry = 0..9
selected/planned = cell & 0x0F
ready/current    = cell >> 4
unavailable      = ((cell & 0x0F) == 0x0F)
```

Matrix A is `+0x06A`; matrix B is `+0x0BA`. Resource expansion remains:

```text
matrix A T index = 0x42 + bank*10 + entry
matrix B T index = 0x92 + bank*10 + entry
```

## Post-spell combat state

```c
uint8_t  base_attack_target;                    // +0x10A
uint32_t hand_ready_deadline[2];                // +0x10B
int32_t  vampiric_touch_absorption_pool;        // +0x113
int32_t  primary_effect_0_absorption_pool;      // +0x117
uint8_t  reserved_11B_122[8];                   // +0x11B
int32_t  primary_effect_12_strength_adjustment; // +0x123
```

The attack-target byte is class/level-derived (`20 - progression`). Hand deadlines are absolute game-clock readiness values. The two protection dwords are layered damage-absorption pools.

## Item-known and spell-UI state

`+0x12B..+0x12E` is the item-known/identified bitset; bits `0..20` correspond to `item[0..20]` and move with item instances during inventory transfer. `+0x12F` is selected zero-based spell bank, `+0x130` is six-row spell-list scroll state, and `+0x131` is the casting capability selector/mask.

## Timed state

```c
uint32_t primary_effect_bits;       // +0x132
uint32_t primary_effect_expiry[26]; // +0x136
uint32_t timed_slot_deadline[3];    // +0x19E
uint32_t timed_slot_aux[3];         // +0x1AA
```

Primary effect bit `i` corresponds to expiry `i`; expiration resources map to `T[588]..T[613]`. The three paired timed slots are a separate subsystem.

## Late character state

`+0x1B6` is tied to `STONE GIANT GAUNTLETS`, preserving displaced Strength while the item forces effective Strength to 20.

```c
uint32_t temporary_hand_effect_bits;      // +0x1C2
uint32_t temporary_hand_effect_expiry[9]; // +0x1C6
```

Temporary-hand indexes map to objects `0x7E..0x86`: CHILL TOUCH, VAMPIRIC TOUCH, CLAWS OF THE UMBER HULK, CAUSE LIGHT WOUNDS, CAUSE SERIOUS WOUNDS, CAUSE CRITICAL WOUNDS, HARM, FLAME BLADE, and LICH TOUCH.

`+0x1EA` is a persistent additive/consumable protection pool, `+0x1EE` is the recurring Strength-recovery deadline, and `+0x1F2` is accumulated state associated with primary effect index 6.

## Terminal bytes

The complete final 82-byte record interval is now bounded:

```text
record +0x1F6..+0x247
runtime 0x21AD6..0x21B27
```

No direct character-field reference was found across this interval in the audited CODE.2 image. A literal `0x21B24` does occur, but disassembly at physical `0x5CAA7` proves it is a code jump-table base (`jmp *%cs:0x21b24(,%eax,4)`), not a character field. Character initialization references `0x21B28`, exactly `0x248` bytes after base `0x218E0`, independently confirming that the preceding eight bytes are inside the runtime record image.

All 12 populated quick-start records store zero in `+0x240..+0x247`; unused active-party records retain `0xFF` fill. Therefore the safe classification is **serialized reserved/unreferenced terminal state**, not disposable padding. Preserve all 82 bytes exactly.

## Native-port rule

Do not memory-map the legacy image onto a modern packed C/C++ structure. Decode explicitly into typed native state and serialize at exact legacy offsets. Preserve opaque/reserved bytes verbatim.

## Remaining semantic targets

1. `+0x012..+0x014` and `+0x018..+0x01B` identity/core metadata;
2. exact identities of remaining ability/core fields in `+0x034..+0x03D`;
3. exact player-facing identities for the 26 primary effects and three paired timed slots;
4. exact meanings of `+0x1BA`, `+0x1EA`, and `+0x1F2`.

Supporting evidence is maintained in the focused SAVE.DAT notes. Any note still carrying the old `0x218D8` base or pre-correction offsets is historical until normalized.