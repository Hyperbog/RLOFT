# RES9 wordcode program / NPC interaction index

Status: **all 38 active RES9 interaction/display programs now have executable-proved NPC identities. Programs `0..36` map directly to `NPCS.DAT` types `34..70`; program `37` is the explicit special mapping for NPC type `82`. Per-program graphics, text, sound, music, cinematic, and party-template setup values are decoded from the wordcode itself.**

## Executable mapping

The family-A/NPC interaction path around CODE.2 `31ADC..31B20` obtains the selected live NPC type from the family-A record and passes it through eligibility predicate `3AD18`.

For an accepted interaction:

```text
if npc_type == 0x52:
    program_id = 0x25
else:
    program_id = npc_type - 0x22

16524(program_id)
```

Thus:

```text
NPC types 0x22..0x46 / 34..70 -> programs 0..36
NPC type  0x52       / 82     -> program 37
```

This is a direct runtime mapping, not a name/content correlation. Fixed caller sites elsewhere in CODE.2 independently invoke several of the same program IDs for their corresponding story interactions.

## Per-program setup fields

The table below decodes the first program setup values where present:

```text
resource base   opcode 0x17 -> archive-9 typed-resource sub-base
text base       opcode 0x19 -> T indexed-text base
sound base      opcode 0x27 -> voice/sound index sub-base
music IDs       opcode 0x1D -> music-sequence IDs used by program
cinematic IDs   opcode 0x24 -> ANM/numbered cinematic invocation
party template  opcode 0x05 -> static party-character template ID
```

A dash means that opcode/setup is absent in the program.

| Prog | NPC | NPCS.DAT name | RES9 gfx base | T text base | Sound base | Music ID(s) | Cinematic | Party template(s) |
|---:|---:|---|---:|---:|---:|---|---|---|
| 0 | 34 | MAN IN WOODS | 7 | 239 | 0 | 26 | — | — |
| 1 | 35 | BRIGAND | 17 | 256 | 8 | 31 | — | — |
| 2 | 36 | VISTANI | 27 | 262 | 11 | 28 | — | — |
| 3 | 37 | BRAY | 38 | 284 | 22 | 27 | — | — |
| 4 | 38 | TATYANA | 46 | 302 | 30 | 32 | — | 4 |
| 5 | 39 | PRISONER CHAINED | 58 | 312 | 35 | 32 | — | 9 |
| 6 | 40 | PRISONER UNCHAINED | 58 | 312 | 35 | 32 | — | 9 |
| 7 | 41 | PRIEST KNEELING | 236 | 330 | 0 | 33 | — | — |
| 8 | 42 | PRIEST STANDING | 68 | 334 | 124 | 33 | — | — |
| 9 | 43 | GHOST | 76 | 343 | 44 | 30 | — | — |
| 10 | 44 | CLERIC | 102 | 358 | 55 | 33 | — | 2 |
| 11 | 45 | ELVEN MAGE | 113 | 371 | 62 | 29 | — | 0 |
| 12 | 46 | GRAVEROBBER OUT | 122 | 387 | 68 | 26 | — | 3 |
| 13 | 47 | GRAVEROBBER TAVERN | 122 | 387 | 68 | 26 | — | 3 |
| 14 | 48 | ADVENTURER | 134 | 396 | 73 | 32 | — | — |
| 15 | 49 | STRAHDS PRISONER | 144 | 410 | 80 | 31 | — | 6 |
| 16 | 50 | BURGOMASTER | 152 | 420 | 85 | 28 | 6 | — |
| 17 | 51 | WERERAVEN | 160 | 441 | 93 | 27 | — | — |
| 18 | 52 | PYOOR | 183 | 449 | 97 | 27 | — | — |
| 19 | 53 | ELVEN WARRIOR | 197 | 464 | 105 | 29 | — | — |
| 20 | 54 | MEMBER OF KEEPERS | 210 | 475 | 110 | 27 | — | — |
| 21 | 55 | WEREWOLF | 218 | 487 | 116 | 30 | — | — |
| 22 | 56 | MAD OLD WOMAN | 247 | 502 | 128 | 27 | — | — |
| 23 | 57 | MAGIC MIRROR WHOLE | 256 | 511 | 132 | 30 | — | — |
| 24 | 58 | MAGIC MIRROR BROKE | 198 | 513 | — | — | — | — |
| 25 | 59 | STATUE OF MAIDEN 1 | 296 | 516 | 135 | 29 | — | — |
| 26 | 60 | STATUE OF MAIDEN 2 | 296 | 516 | 135 | 29 | — | — |
| 27 | 61 | STRAHD DISGUISED | 323 | 527 | 140 | 28 | — | 8, 8 |
| 28 | 62 | FINAL STRAHD | 332 | 544 | 148 | — | — | — |
| 29 | 63 | DANDY WOMAN | 375 | 551 | 153 | 32 | — | 1 |
| 30 | 64 | MAGE | 385 | 563 | 159 | 26 | — | 7 |
| 31 | 65 | OLD PALADIN | 400 | 574 | 164 | 33 | — | — |
| 32 | 66 | VISTANI INSIDE | 27 | 262 | 11 | 28 | 1 | — |
| 33 | 67 | ELVEN MAGE IN BAR | 113 | 371 | 62 | 29 | — | 0 |
| 34 | 68 | STRAHDS PRIS OUT | 144 | 410 | 80 | 31 | — | 6 |
| 35 | 69 | DANDY WOMAN IN BAR | 375 | 551 | 153 | 32 | — | 1 |
| 36 | 70 | MAN IN WOOD IN BAR | 7 | 239 | 0 | 26 | — | — |
| 37 | 82 | ADVENTURER | 134 | 396 | 73 | 32 | — | 5, 5 |

## Resource sharing is intentional

The mapping exposes repeated interaction identities across alternate NPC definitions:

```text
VISTANI / VISTANI INSIDE             programs 2 / 32
ELVEN MAGE / ELVEN MAGE IN BAR       programs 11 / 33
STRAHDS PRISONER / STRAHDS PRIS OUT  programs 15 / 34
DANDY WOMAN / DANDY WOMAN IN BAR     programs 29 / 35
MAN IN WOODS / MAN IN WOOD IN BAR    programs 0 / 36
```

These pairs deliberately reuse the same graphics, text, sound, and music bases while retaining distinct NPCS.DAT type IDs and separate wordcode program records. A port/editor should not deduplicate the program IDs merely because their setup bases match.

PRISONER CHAINED/UNCHAINED (`5/6`) and the two STATUE OF MAIDEN definitions (`25/26`) likewise share major resource bases while representing different runtime/story state.

## Direct link to `T`

The opcode-`0x19` values above point into the 1,073-entry `T` indexed text archive. Decoding the corresponding records produces coherent dialogue/message groups for the mapped NPC identities. This validates the cross-file chain independently of the numeric caller mapping:

```text
NPCS.DAT type
    -> RES9 program ID
    -> wordcode text base
    -> T indexed text records
```

The documentation intentionally records indexes rather than reproducing dialogue text.

## Party-template operations

Opcode `0x05` occurs in programs associated with recruitable/party-relevant interactions. Active template IDs are `0..9`.

The executable proves the operation applies a fixed party-character template to live party state. RES12 now closes the historical encoding relationship: its legacy opcode 0x05 stores a 141-word compact character template inline, while active RES9 stores the corresponding one-word template ID. `RES12_WORDCODE.md` and `RES11_RES12_LINEAGE.md` preserve the full proof.

Do not derive template names solely from NPC display names: program 37, for example, shares the ADVENTURER interaction graphics/text base with program 14 but applies a different template.

## Cinematic links

Only two active programs invoke opcode `0x24`:

```text
program 16 / BURGOMASTER      cinematic 6
program 32 / VISTANI INSIDE   cinematic 1
```

The cinematic operation is already tied to the reconstructed ANM subsystem in `Resources/ANM/FORMAT.md`.

## Safe implementation boundary

A faithful interaction index may expose the exact mappings above while keeping these layers separate:

- NPC type ID (`NPCS.DAT` identity);
- wordcode program ID (`RES9:517 + id`);
- archive-9 interaction graphics sub-base;
- `T` text-record base;
- sound/index sub-base;
- music/cinematic IDs;
- party-template IDs.

Do not flatten shared resource bases into shared NPC identity or shared gameplay state.

## Legacy RES12 program generation

RES12:370..406 preserves programs corresponding to active IDs 0..36.

Recovered comparison:

- same VM opcode domain 0x00..0x29;
- same grammar except opcode 0x05;
- legacy opcode 0x05 = 141-word inline compact character template;
- active opcode 0x05 = one-word static template ID;
- same-ordinal text base preserved 37/37;
- legacy graphics interaction base aligns at 51 versus final INIT base 103;
- using those bases, 23/37 same-ordinal program base graphics are byte-identical.

The final executable template ID order is:

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

Legacy program 14 embeds IRMGARDE. In the active generation program 14 no longer adds a template; new program 37 uses template ID 5 twice while sharing program 14's interaction identity. This is preserved as a concrete script/content migration between generations.
