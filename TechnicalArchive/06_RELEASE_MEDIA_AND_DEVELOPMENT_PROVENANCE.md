# Release media, patch layering, and development-residue provenance

Status: **ACTIVE HISTORICAL PRESERVATION RECORD**

This document tracks evidence that belongs to the shipped/distributed media history but is not reducible to the runtime decompilation.

## Preservation principle

Do not assign one version label to every file merely because those files occur in one modern distribution image.

Record provenance per artifact:

- byte identity and hash;
- filesystem timestamp where preserved;
- physical source image/disc;
- executable behavior;
- patch/distributor metadata;
- community CD listings only as corroboration.

## Supplied preservation image

The project corpus was supplied from the Ravenloft CD filesystem carried by the GOG game.gog image.

This source includes RES0..RES12 and STATUS.ME.

The project records exact hashes for supplied files independently of any external version label.

## Resource-generation chronology

The supplied CD-derived resource timestamps include:

    RES11  1994-03-04 11:28:46
    RES12  1994-03-04 11:28:46
    RES8   1994-03-04 11:28:46

    RES10  1994-03-08
    RES3   1994-03-09
    RES6   1994-03-09
    RES0   1994-03-10
    RES1   1994-03-10
    RES2   1994-03-10
    RES4   1994-03-10
    RES9   1994-03-10
    RES5   1994-03-13

Timestamp ordering by itself is not enough to classify an archive: active RES8 is also March 4.

The decisive historical evidence is the byte-level lineage documented in Resources/RES/RES11_RES12_LINEAGE.md. RES11 and RES12 are older/pre-split composite generations whose contents substantially survive in later active split archives.

## Community-preserved v1.1 CD listing

A WinWorld forum preservation post labels a Ravenloft: Strahd's Possession disc as v1.1 and reproduces a DOS directory listing.

Relevant corroboration:

- RES0..RES12 sizes agree with the canonical corpus;
- RES11 and RES12 are listed at 1994-03-04 3:28p;
- the later split archives carry March 8-13 dates.

Source:
https://forum.winworldpc.com/discussion/9842/ibm-pc-compatible-game-installation-file-listing-screenshot-from-cd-media-not-floppy/p19

Evidence class: **COMMUNITY-RESEARCH / media-listing corroboration**.

Constraint: the listing supplies sizes/timestamps, not cryptographic hashes. It therefore corroborates the historical CD layout but does not by itself prove byte identity or establish that every supplied GOG file is literally from that labeled v1.1 disc.

## GOG 1.2a update

GOG distribution release notes record:

    Patch 1.2a — 03 November 2015
    English release updated to 1.2a

Source:
https://www.gogdb.org/product/1432905053/releasenotes

This is distributor-history evidence for the modern GOG package.

A contemporary GOG forum post published the following MD5 values for a 1.2a patch set:

    CODE.1  b6916a96f85c65f318663413ac1269f6
    CODE.2  ef764911c84b9c55bfcee1e5300399fd

The project's supplied CODE.1 and CODE.2 match those two values exactly.

Source:
https://www.gog.com/forum/dungeons_dragons_ravenloft_series/strahds_possession_need_help_with_bugsglitches/page1

Evidence class: **COMMUNITY-RESEARCH / independently checked executable identity**.

The same forum post gives an INFO.DAT hash that does not match the supplied INFO.DAT. That mismatch must not be used to infer a mixed executable build because INFO.DAT contains installation-dependent location/path bytes. Its hash can legitimately change with installation layout.

Safe conclusion:

- supplied CODE.1/CODE.2 are byte-identical to the community-recorded 1.2a pair;
- GOG records an English 1.2a update;
- resource sizes/timestamps match a preserved earlier retail-CD listing;
- resources may simply have persisted unchanged across executable patches;
- assign version/build claims per artifact rather than declaring the entire image a single immutable 1994 build snapshot.

## STATUS.ME development residue

The CD-shipped STATUS.ME is 1,029 bytes, SHA-256:

    8f76f831bf7ab218784c1f2bfdee2ce8d7f6ec5ee926688f1fae5ba1adc585db

It begins:

    @MULTI-EDIT VERSION 7.0    O:\BCOOLEY\6WORLDS\

and preserves an editor-state reference to:

    O:\BCOOLEY\6WORLDS\PLAGUE.BAT

plus INSTALL?.NFO and other workspace/status material.

Independent Multi-Edit 7.0 research documents STATUS.ME as the editor's saved status/workspace file when the relevant restore mode is selected.

Source:
https://www.forum.old-dos.ru/viewtopic.php?p=7536

This makes STATUS.ME valuable **development/mastering residue**, not runtime game data.

Do not infer:

- a real-person identity from BCOOLEY;
- that 6WORLDS was definitely Ravenloft's project codename;
- that PLAGUE.BAT was necessarily part of Ravenloft's source/build pipeline.

Those strings are preserved facts; their higher historical meaning still requires independent corroboration.

## RES11/RES12 and the thirteen-slot layout

INFO.DAT stores thirteen resource-location selector bytes.

Both certified CODE.1 and CODE.2 read all thirteen but initialize only eleven indexed RES groups. The supplied CD nevertheless contains RES11 and RES12, and byte-level analysis proves those archives are legacy predecessors of later active resource families.

This combination is consistent with a resource/install format that retained capacity/history from an earlier thirteen-archive generation.

That historical interpretation is **RECONSTRUCTED / HIGH CONFIDENCE**, not proof that a specific retail executable opened all thirteen. The exact older consumer remains an explicit preservation target.

## Future primary-evidence targets

Highest-value missing historical artifacts now include:

1. an earlier CODE.1/CODE.2 pair from a period retail build, especially the executable corresponding to the March resource generation;
2. DreamForge resource compiler/editor binaries or source;
3. period patch archives with authenticated file manifests;
4. installer versions whose archive-selection behavior can be tied to a specific disc revision.

An older matching executable could convert the remaining RES11/RES12 consumer question from reconstruction to direct proof.
