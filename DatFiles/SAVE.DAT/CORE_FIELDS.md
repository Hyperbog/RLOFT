# SAVE.DAT early/core character fields (`+0x010..+0x047`)

Status: **active executable reconstruction. This note separates certified early-record mechanics from still-unresolved identity/stat semantics.**

Runtime base for active character 0 is DGROUP `0x218D8`; character `n` is `0x218D8 + n*0x248`.

## `+0x018..+0x019`: status word also supplies the record-active predicate

The 16-bit field at runtime `0x218F0` is already certified as the character status word. A further executable audit establishes an important structural use of its sign/high bit.

Party/UI iteration at physical `0x51349..0x51355` performs:

```text
load uint16 status at 0x218F0 + character*0x248
compare status with 0x8000
if status >= 0x8000, skip the populated-character path
```

The supplied unused active-party records contain `0xFFFF` at `+0x018`, whereas populated records contain nonnegative status values. Thus the original executable has a usable record-activity discriminator: the status word's high bit separates inactive/unused record images from ordinary populated characters in this path.

This supersedes the earlier statement that no executable occupancy predicate was known. It remains unsafe to use an all-zero test.

Portable compatibility predicate for the audited path:

```c
bool record_is_active = ((status_word & 0x8000u) == 0);
```

This is an executable behavior, not a claim that every possible caller uses exactly the same predicate.

Known high-byte condition masks remain:

```text
0x0400 disease
0x0800 poison
0x1000 paralysis
```

Other status bits remain under semantic audit.

## `+0x01D..+0x01F`: three signed class-code slots

Runtime bytes:

```text
+0x01D -> 0x218F5
+0x01E -> 0x218F6
+0x01F -> 0x218F7
```

These are now structurally certified as three class-code slots rather than a single class discriminator plus unknown neighbors.

The helper beginning at physical `0x48AF8` reads the second and third bytes as signed values and compares each against `-1`. Its logic is:

```text
class_count = 1
if class_code[1] != -1: class_count = 2
if class_code[2] != -1: class_count += 1
```

Many other CODE.2 paths independently inspect all three bytes. Character/spell initialization at physical `0x51AFC` onward tests them against class codes used by the casting subsystems.

Portable representation:

```c
int8_t class_code[3]; // +0x01D..+0x01F; -1 marks an absent later class slot
```

The supplied fixture agrees: single-class characters use `-1` in slots 1 and 2; multiclass characters populate the additional slots.

Exact player-facing class names for every numeric code remain a separate mapping problem and are not assigned here without independent evidence.

## `+0x032..+0x03D`: six paired ability-shaped bytes

Fixture geometry shows six consecutive two-byte pairs before the already certified exceptional-Strength and equipment-related state. Direct executable consumers independently certify:

```text
+0x03A  Strength base/reference
+0x03B  Strength current/effective
+0x03D  Dexterity current/effective
```

The surrounding pairs have the same base/current fixture shape, but the exact identities of `+0x032..+0x039`, and the base byte paired with Dexterity at `+0x03C`, are not promoted to canonical names solely from AD&D stat ordering. They remain a focused target for UI/combat consumer tracing.

Safe current geometry:

```text
+0x032/+0x033  paired byte field A
+0x034/+0x035  paired byte field B
+0x036/+0x037  paired byte field C
+0x038/+0x039  paired byte field D
+0x03A/+0x03B  Strength base/current
+0x03C/+0x03D  paired field; current byte is Dexterity-related
```

## Direct-reference audit caution

Searching CODE.2 for little-endian runtime addresses is useful for finding consumers, but an address byte sequence inside instruction immediates, relative-call displacements, or data tables is not itself evidence of a field access. Candidate hits are only accepted after instruction-aligned disassembly confirms the operand.

This matters especially in the early record, where several apparent hits inside the `0x29xxx` data/table area are false positives.

## Current early-record boundary

```text
+0x010..+0x017  identity/appearance metadata unresolved
+0x018..+0x019  status word; high bit participates in inactive-record filtering
+0x01A..+0x01C  unresolved core/identity fields
+0x01D..+0x01F  int8 class_code[3], -1 absent sentinel
+0x020..+0x031  class/progression-related region; exact partition still under audit
+0x032..+0x039  four paired ability-shaped fields, identities pending
+0x03A/+0x03B  Strength base/current
+0x03C/+0x03D  paired field; +0x03D Dexterity current/effective
+0x03E..+0x045  unresolved paired/word state
+0x046/+0x047  exceptional Strength base/current percentile
```

## Next target

Trace the class/progression region `+0x020..+0x031` from its executable consumers and close the remaining ability-pair identities through actual UI/combat references rather than assumed AD&D field order.