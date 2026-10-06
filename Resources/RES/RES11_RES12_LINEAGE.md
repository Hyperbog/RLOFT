# RES11 / RES12 historical lineage and format evolution

Status: **RECONSTRUCTED / HIGH CONFIDENCE from canonical resource bytes, certified CODE.1/CODE.2 behavior, CD timestamps, and deterministic cross-generation correlations. The exact historical executable/editor that consumed or produced these legacy composite archives remains UNKNOWN.**

This document records the historical relationship of the two CD-shipped archives that are not opened by the certified CODE.1/CODE.2 indexed RES setup.

## Evidence boundary

The following are controlling primary evidence:

- canonical RES0..RES12 byte images;
- canonical INFO.DAT;
- certified CODE.1 and CODE.2;
- CODE.2 static character-template records;
- timestamps preserved with the supplied CD-derived resource corpus.

External CD listings and GOG release metadata are corroborative historical evidence only.

The safe conclusion is not that RES11/RES12 belong to a different retail disc. They are physically present on the supplied CD image. The evidence instead identifies them as **legacy/pre-split composite resource generations retained alongside the later active archive layout**.

The exact older program or producer that originally selected these files has not been recovered and must remain UNKNOWN.

## Both certified game modules use only RES0..RES10

INFO.DAT serializes thirteen location digits:

    0000000001111

Both certified game modules read and normalize all thirteen digits.

CODE.2 then initializes the indexed archive system with base name RES and count 11.

CODE.1 independently does the same thing:

- function 8978 reads thirteen selector bytes and the path payload from INFO.DAT;
- function 8A04 builds indexed RES filenames;
- the character-generation bootstrap reaches 95BC with resource base RES and group count 11.

Therefore the supplied CODE.1/CODE.2 pair actively opens only:

    RES0 .. RES10

This closes the possibility that supplied CODE.1 is the missing RES11/RES12 consumer.

The existence of thirteen serialized selector positions alongside thirteen CD archives is historically significant, but selector positions 11/12 are not promoted to active runtime semantics for the certified executable pair.

## RES11 physical partition

RES11 contains 5,114 entries in four exact contiguous banks:

    0..4127       4,128 type-7 scene-shape resources
    4128..4779      652 type-6 first-person textures
    4780..4853       74 type-3 XMIDI/XDIR sequences
    4854..5113      260 type-4 Creative Voice resources

This family order mirrors the later active archive sequence RES3, RES4, RES5, RES6.

### Type 7: legacy scene-shape generation

Of the 4,128 RES11 type-7 entries, **2,402** are byte-identical to at least one entry in final RES3.

Sequence alignment exposes large exact anchors including:

    legacy 3221 -> final 3316    at least 249 consecutive exact entries
    legacy 4055 -> final 4158    at least 65 consecutive exact entries

Final RES3 source-class bases are:

    0 / 2169 / 3316 / 4158

The alignment supports reconstructed legacy partition transitions near:

    class 0 start     0
    class 1 start  ~2118
    class 2 start   3221
    class 3 start   4055

These legacy boundaries are **alignment reconstructions**, not recovered old INIT values. No historical INIT corresponding to RES11 has been identified.

### Type 6: legacy texture generation

RES11's texture bank is:

    entries 0..584    585 decoded 8192-byte / 64x128 textures
    entries 585..651   67 decoded 4096-byte / 32x128 textures

Final RES4 is:

    entries 0..599    600 decoded 8192-byte / 64x128 textures
    entries 600..666   67 decoded 4096-byte / 32x128 textures

Thus the later archive adds fifteen wide textures while retaining the same narrow-bank count.

Across the full bank, **609/652** legacy entries have an exact match in final RES4. The narrow bank is especially stable: **65/67** legacy narrow textures match after the old 585 -> final 600 boundary shift.

### Type 3: legacy XMIDI generation

RES11 contains **74 = 2 x 37** XMIDI/XDIR records. Final RES5 contains **98 = 2 x 49**, with INIT selecting the second final bank through offset 49.

The two halves in each generation have internally identical XDIR sequence-count profiles:

    legacy 37-entry half: 14 records with 2 sequences, 23 with 1
    final 49-entry half:  26 records with 3 sequences, 23 with 2

No complete legacy XMIDI entry is byte-identical to a final RES5 entry. That means this family was globally rebuilt/recompiled rather than simply copied into the split archive. Event-level musical equivalence is not asserted without a dedicated XMIDI semantic comparison.

### Type 4: legacy sampled-SFX generation

RES11 entries 4854..5113 and RES12 entries 407..666 are **260/260 byte-identical**.

Of those 260 legacy VOC resources:

- **236/260** have an exact match in final RES6;
- only **5/260** have an exact match in final RES10.

This strongly associates the shared legacy bank with the later RES6 sampled-SFX namespace rather than the later RES10 interaction-voice namespace.

## RES12 physical partition

RES12 contains 667 entries:

    0..369        370 type-1 graphics
    370..406       37 raw interaction-wordcode containers
    407..666      260 type-4 VOC resources

The 37 raw records align ordinally with active RES9 programs 0..36.

### Graphics lineage into RES9

Of the 370 legacy graphics, **255** have an exact match among final typed RES9 entries 0..516.

Important exact sequence anchors include:

    legacy 0  -> final 0      first 31 entries remain contiguous and exact
    legacy 51 -> final 103    exact interaction-resource alignment anchor

Final INIT defines RES9 interaction-resource base 103.

The old-to-new graphics alignment therefore supports a reconstructed legacy interaction base of **51**. Because legacy wordcode begins at RES12:370, the corresponding reconstructed displacement is:

    370 - 51 = 319

These numbers are historical alignment reconstructions. They must not be described as values recovered from an old INIT file.

Using legacy base 51 and final base 103, **23/37** same-ordinal interaction programs resolve their configured base graphic to byte-identical old/new entries.

## RES12 wordcode is the same VM with an older opcode-05 encoding

Earlier documentation called RES12 an incompatible wordcode dialect because parsing it with the final opcode-05 arity caused ASCII/stat values to be mistaken for opcodes above 0x29.

That diagnosis is superseded.

The recovered legacy grammar is:

- same opcode domain 0x00..0x29;
- same variable-list rules for opcodes 00, 01 and 02;
- same operand arity for every fixed opcode **except 05**;
- final RES9 opcode 05 consumes one word: a static character-template ID;
- legacy RES12 opcode 05 consumes **141 words**: an inline compact character template.

With opcode 05 assigned arity 141:

    programs                         37 / 37 parse to EOF
    program words                    4,820
    decoded instructions             1,079
    invalid direct control targets       0
    timer slots used                    0, 1
    inline opcode-05 templates          16

Thus RES12 is not random corruption and not a different opcode dispatcher. It is a recoverable earlier generation of the same interaction/display VM format.

## Legacy opcode-05 compact template

The 141-word operand block has a deterministic field mapping into the later static character-template representation.

    words 0..15     16-word name, one ASCII byte value per word
    word 16         portrait/appearance
    word 17         sex
    word 18         race
    word 19         age
    words 20..22    three class codes
    word 23         alignment
    word 24         base armor class
    words 25..26    current/max HP
    words 27..29    three class levels
    words 30..32    three class progress/XP values; FFFF is absent
    word 33         Strength
    word 34         exceptional Strength
    words 35..39    Dexterity, Constitution, Intelligence, Wisdom, Charisma
    words 40..60    21 item slots
    words 61..140   80 words repacking exactly into the two 80-byte spell matrices

The legacy payload is **not** a verbatim 0x248-byte runtime character record. It is a compact word-oriented predecessor representation.

A deterministic expansion of those 141 words reproduces the final character-record prefix through +0x109:

- name and identity fields;
- HP/class progression;
- duplicated base/current ability bytes;
- 21 item words;
- both 80-byte spell matrices.

The final record's later runtime/effect-state region beginning at +0x10A is not serialized by the legacy opcode payload.

## Final CODE.2 static template table

The supplied CODE.2 stores ten static 0x248-byte character templates at file offset 0x8072C, stride 0x248.

Their ID order is:

    0  GLORIANNA
    1  VELIKA
    2  MISCHA
    3  VUKO
    4  TATIAN
    5  IRMGARDE
    6  ANSHELM
    7  RYSZARD
    8  VLADISLAV
    9  FHALKEN

All ten names occur in legacy RES12 opcode-05 payloads.

The corresponding expanded legacy prefix agrees with the final static template for most of the 266-byte prefix, with some templates fully identical and others containing real content revisions:

    FHALKEN    266 / 266 equal
    MISCHA     266 / 266 equal
    TATIAN     262 / 266
    ANSHELM    260 / 266
    IRMGARDE   253 / 266
    VELIKA     249 / 266
    GLORIANNA  239-240 / 266
    RYSZARD    239 / 266
    VLADISLAV  236 / 266
    VUKO       232 / 266

This proves a real producer/runtime format evolution:

    legacy RES12 script
        opcode 05 + 141-word inline compact character template

    later active RES9 script
        opcode 05 + one-word template ID
                   |
                   +--> centralized CODE.2 static 0x248-byte template table

## Interaction-program evolution

For same-ordinal programs 0..36, the initial text-base setup survives **37/37** unchanged.

Graphics sub-bases evolve in accordance with additions/reordering in the graphics bank. Later programs also contain more explicit sound/music setup than the legacy generation.

One particularly useful lineage event is IRMGARDE:

- legacy program 14 embeds the IRMGARDE character template;
- final active program 14 no longer executes opcode 05;
- final program 37, newly outside the 0..36 legacy range, applies template ID 5 twice;
- active programs 14 and 37 share the same interaction graphics/text/sound setup identity.

This is direct evidence of script/content restructuring between generations, not merely archive repacking.

## Chronology

The supplied CD-derived timestamps place RES11 and RES12 at:

    1994-03-04 11:28:46

Most active successor archives were rebuilt later:

    RES6    1994-03-09
    RES3    1994-03-09
    RES4    1994-03-10
    RES9    1994-03-10
    RES5    1994-03-13

RES8 and several middleware/support files also retain the March 4 timestamp, so date alone does not prove obsolescence. The chronology becomes significant when combined with the exact content lineage above.

A community-preserved CD directory listing labeled Ravenloft v1.1 independently reports the same RES sizes and March timestamps, including RES11/RES12. That source corroborates the period CD layout but does not provide hashes and therefore cannot establish byte identity by itself.

GOG distribution records state that the English release was updated to 1.2a in November 2015. A contemporary community patch post publishes 1.2a MD5 values for CODE.1 and CODE.2 that match the supplied executable pair. INFO.DAT must be treated separately because its selector/path bytes are installation-specific; a differing INFO.DAT hash is not evidence by itself of a mixed executable build.

The archive therefore records version provenance **per file/evidence layer** rather than flattening the supplied GOG image into a single historical mastering date.

## Historical classification

### Confirmed

- RES11 and RES12 are physically shipped on the supplied CD image.
- supplied CODE.1 and CODE.2 both open only RES0..RES10 through the indexed RES system.
- RES11 and RES12 contain large amounts of exact predecessor data for later active resource families.
- RES12 wordcode uses the same opcode set with an older 141-word opcode-05 template encoding.
- final CODE.2 centralizes those character templates and active RES9 replaces the inline structure with template IDs.

### Reconstructed / high confidence

- RES11 is a **legacy/pre-split composite asset archive** whose families later appear principally in RES3, RES4, RES5 and RES6.
- RES12 is a **legacy/pre-split interaction composite archive** whose graphics/VM lineage principally continues in RES9, while its shared 260-entry VOC bank chiefly continues in RES6.
- the March-4 composites represent an earlier resource generation retained on the mastered CD alongside later split archives.

### Intentionally unknown

- the exact historical CODE executable that selected all thirteen archive slots, if any;
- the exact DreamForge producer/editor version that generated RES11/RES12;
- whether the legacy generation corresponds exactly to a named retail patch level, internal build, mastering snapshot, or another pre-final milestone.

Those questions require an older matching executable, producer/editor, or documentary source. They are no longer blockers to understanding the archive lineage itself.

## Reproducibility

Run:

    python Resources/RES/analyze_legacy_archives.py /path/to/resource/root --code2 /path/to/CODE.2

The root must contain RES3, RES4, RES5, RES6, RES9, RES10, RES11 and RES12.

The analyzer validates the partitions, exact-entry correlations, major alignment anchors, XMIDI bank profiles, recovered RES12 VM grammar, control-flow targets, static CODE.2 template table, inline-template mapping, IRMGARDE program migration, and old/new interaction-base correspondence.
