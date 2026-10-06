# SAVE.DAT early/core character fields

Status: **active executable reconstruction. The earlier runtime-to-file offset translation in this note was corrected by direct comparison against the serialized fixture.**

## Correct runtime/file relationship

The active character runtime image begins at DGROUP `0x218E0`, with character `n` at `0x218E0 + n*0x248`. The previous `0x218D8` base was eight bytes early and therefore shifted runtime-derived record offsets by `+8` in the documentation.

This correction is independently checked against SAVE.DAT values and executable consumers. In particular runtime words `0x218FC` and `0x218FE` map to serialized `+0x01C` and `+0x01E`; the supplied records contain plausible current/max hit-point pairs at exactly those offsets.

## Certified core partition

```text
+0x000..+0x00F  fixed 16-byte name storage
+0x010..+0x014  identity/appearance metadata; semantics still under audit
+0x015..+0x017  int8 class_code[3]
+0x018..+0x019  status word
+0x01A..+0x01B  unresolved core field
+0x01C..+0x01D  int16 current_hp
+0x01E..+0x01F  int16 max_hp
+0x020..+0x025  int16 class_level[3]
+0x026..+0x031  int32 class_progress[3]
+0x032..+0x03F  ability-score/effect region; exact remaining names under audit
+0x040..+0x069  int16 item[21]
+0x06A..+0x0B9  spell matrix A, 8 x 10 packed cells
+0x0BA..+0x109  spell matrix B, 8 x 10 packed cells
```

## `+0x015..+0x017`: class slots

The executable treats these as three signed class-code slots. Later slots use `-1` for absence:

```c
int8_t class_code[3];
```

The fixture agrees: single-class characters have `-1` in slots 1 and 2, while multiclass records populate additional slots.

## `+0x01C/+0x01E`: hit points

The two signed words are current and maximum hit points:

```c
int16_t current_hp; // +0x01C
int16_t max_hp;     // +0x01E
```

Reference fixture examples:

```text
EHLIANNA     38 / 38
ANVIL DAWN   55 / 55
GLORIANNA    15 / 15
VELIKA       27 / 27
MISCHA        1 / 32
ANSHELM      43 / 51
FHALKEN      25 / 34
```

The non-full records are important evidence that these are current/max state rather than duplicated constants.

## `+0x020..+0x025`: per-class levels

The executable indexes the three words in parallel with the three class slots. Portable representation:

```c
int16_t class_level[3];
```

Absent class slots contain `-1` in the supplied records. Examples include EHLIANNA `[5,4,-1]`, ANVIL DAWN `[5,5,-1]`, and VELIKA `[4,4,5]`.

## `+0x026..+0x031`: per-class experience/progression

The three dwords align one-for-one with `class_code[]` and `class_level[]`:

```c
int32_t class_progress[3];
```

Absent slots use `-1`. Advancement logic modifies these values and derives levels through class-indexed progression thresholds, establishing them as per-class experience/progression accumulators. Fixture examples include GLORIANNA `[24000,-1,-1]`, VUKO `[41000,-1,-1]`, and VLADISLAV `[65000,-1,-1]`.

Until every advancement table is semantically catalogued, `class_progress` is preferred over claiming an original source-variable name; in native gameplay terms these are experience-point style quantities.

## Status/activity caution

The earlier note associated runtime `0x218F0` with serialized `+0x018`; that association was a consequence of the incorrect base. Runtime addresses must now be translated from `0x218E0`. Any previously documented early-record status-bit semantics are to be revalidated before canonical use.

## Offset-normalization consequence

Every character field whose serialized offset was inferred solely as `runtime_address - 0x218D8` must be shifted eight bytes earlier and rechecked. Examples:

```text
inventory             old +0x048 -> corrected +0x040
spell matrix A        old +0x072 -> corrected +0x06A
spell matrix B        old +0x0C2 -> corrected +0x0BA
```

The `0x248` record stride and top-level SAVE.DAT record boundaries are unaffected.

## Compatibility rule

Do not implement a native structure from the pre-correction offset tables. Normalize all character-record documentation against runtime base `0x218E0`, then re-run fixture and executable cross-checks before treating an offset as canonical.

## Next target

Perform the repository-wide SAVE.DAT character-offset normalization pass, beginning with `CHARACTER_RECORD.md`, spell-matrix documentation, post-inventory state, effect/timer documents, and analyzers. Semantic identities already proved by executable behavior should be retained, but their serialized offsets must be corrected independently.