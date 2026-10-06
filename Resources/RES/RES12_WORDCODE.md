# RES12 legacy interaction wordcode bank

Status: **the 37 RES12 raw programs are now structurally decoded as a legacy generation of the same 0x00..0x29 interaction/display VM used by active RES9. The only recovered fixed-arity grammar change is opcode 0x05: legacy RES12 serializes a 141-word compact character template inline, while active RES9 stores a one-word template ID into CODE.2's centralized static template table.**

## Container

RES12:370..406 contains 37 records with the same outer form as active RES9:

    uint16 word_count
    uint16 program_words[word_count]
    uint16 zero_bank[word_count]

Every second-bank word is zero.

Corpus totals:

    programs       37
    program words  4,820

The companion bank remains producer-side UNKNOWN because supplied CODE.2 does not read it.

## Why the old parser appeared to fail

Applying the active grammar verbatim previously produced apparent opcodes above 0x29 such as 0x41, 0x48, 0x4C and 0x55.

Those words occur inside legacy opcode-05 operands. Many are ASCII character-name bytes or character/stat/item data; they are not instructions.

## Recovered legacy grammar

The legacy generation uses the same opcode domain and the same operand rules as active RES9 except:

    active opcode 05:
        05 template_id
        fixed arity = 1 word

    legacy opcode 05:
        05 inline_character_template[141]
        fixed arity = 141 words

Using that rule:

    programs parsed                   37 / 37
    program words                      4,820
    decoded instructions               1,079
    invalid direct control targets         0
    timer slots used                      0, 1
    opcode-05 occurrences                 16

All direct targets checked with the active VM target rules land on decoded instruction starts.

RES12 is therefore not an incompatible opcode dispatcher. It is a related earlier serialization generation of the same interaction/display VM.

## The 141-word opcode-05 payload

The payload maps as:

    0..15      name words, low byte is ASCII
    16         portrait/appearance
    17         sex
    18         race
    19         age
    20..22     class codes
    23         alignment
    24         base armor class
    25..26     current/max HP
    27..29     class levels
    30..32     class progress/XP
    33         Strength
    34         exceptional Strength
    35..39     Dex/Con/Int/Wis/Cha
    40..60     item[21]
    61..140    two spell matrices, serialized as 80 words

This is a compact word-oriented template, not a verbatim 0x248-byte character record.

A deterministic expansion matches CODE.2's later static character-record layout through +0x109. The later runtime/effect state beginning at +0x10A is absent from the legacy payload.

## Template identities

CODE.2's ten static 0x248-byte templates occur in this ID order:

    0 GLORIANNA
    1 VELIKA
    2 MISCHA
    3 VUKO
    4 TATIAN
    5 IRMGARDE
    6 ANSHELM
    7 RYSZARD
    8 VLADISLAV
    9 FHALKEN

All ten identities are represented by legacy inline opcode-05 payloads.

Legacy occurrences:

    program  4  TATIAN
    program  5  FHALKEN
    program  6  FHALKEN
    program 10  MISCHA
    program 11  GLORIANNA
    program 12  VUKO
    program 13  VUKO
    program 14  IRMGARDE
    program 15  ANSHELM
    program 27  VLADISLAV   twice
    program 29  VELIKA
    program 30  RYSZARD
    program 33  GLORIANNA
    program 34  ANSHELM
    program 35  VELIKA

The expanded prefixes show both preservation and later content edits. FHALKEN and MISCHA are 266/266 byte-identical through the mapped prefix, while other templates differ in bounded fields.

## Program identity continuity

Same-ordinal legacy and active programs preserve their initial text-base operand for **37/37 programs**.

The graphics namespace was expanded/reordered. Exact graphics alignment identifies legacy interaction base 51 with final RES9 interaction base 103. Using those bases, 23/37 same-ordinal programs resolve their configured base graphic to byte-identical resources.

The old base 51 and inferred displacement 319 are reconstructed from archive alignment; they are not claimed as values recovered from an old INIT.

## IRMGARDE migration

Legacy program 14 embeds IRMGARDE.

In active RES9:

- program 14 no longer uses opcode 05;
- newly added program 37 uses template ID 5 twice;
- programs 14 and 37 share the same interaction resource/text/sound identity.

This records a concrete behavior/content restructuring between resource generations.

## Runtime-selection boundary

Certified CODE.2 raw wordcode loader 13F70 requests archive 9 only. No direct archive-12 raw-handle request exists in the certified CODE.2 object.

Certified CODE.1 also initializes only 11 indexed RES groups.

Therefore RES12 is historically related but inactive in the supplied executable pair.

See RES11_RES12_LINEAGE.md and analyze_legacy_archives.py for the complete cross-archive evidence.
