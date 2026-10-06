# Ravenloft Technical Preservation Archive — Source Register

Status: **ACTIVE**

This register records external evidence used by the Technical Preservation Archive. Inclusion does not make an external claim true. Ravenloft's shipped bytes and certified reconstruction remain controlling evidence.

## Evidence-source classes

- **PRIMARY-GAME** — shipped Ravenloft binary/data/manual or publisher-supplied material.
- **PRIMARY-PLATFORM** — official/contemporary CPU, DOS, extender, compiler/toolchain documentation.
- **COMMUNITY-RESEARCH** — independent reverse engineering, experimentation, historical research, walkthrough analysis.
- **CROSS-TITLE** — evidence from another game; comparative only unless independently established in Ravenloft.

Each community claim should eventually carry a validation state: `UNTESTED`, `PARTIALLY CORROBORATED`, `CONFIRMED AGAINST RAVENLOFT`, or `CONTRADICTED/RELEASE-SPECIFIC`.

## SR-001 — Open Watcom C/C++ Programmer's Guide

Class: **PRIMARY-PLATFORM / preserved successor documentation**

URL: https://open-watcom.github.io/open-watcom-v2-wikidocs/cpguide.html

Use: DOS/4GW application model and toolchain context.

Relevant documented fact: the guide contains a dedicated section on creating 32-bit DOS/4GW applications with the Watcom toolchain.

Constraint: successor Open Watcom documentation explains Watcom/DOS4GW mechanisms but is not proof of Ravenloft's exact compiler version/options.

## SR-002 — Open Watcom C/C++ User's Guide 1.9

Class: **PRIMARY-PLATFORM / preserved successor documentation**

URL: https://open-watcom.github.io/open-watcom-1.9/cguide.html

Use: 32-bit calling conventions, stack/register argument contracts, memory/segment layout, far/near controls.

Relevant documented facts include stack-based argument passing, register-preservation contracts, 32-bit memory layout, and auxiliary pragma support for near/far calls and custom calling information.

Constraint: use only to explain machine-code patterns independently observed in Ravenloft.

## SR-003 — Open Watcom C/C++ Programmer's Guide 1.9, DOS/4GW section

Class: **PRIMARY-PLATFORM / preserved successor documentation**

URL: https://open-watcom.github.io/open-watcom-1.9/pguide.html

Use: DOS/4GW historical/platform behavior.

Relevant documented fact: DOS/4GW is described as a Tenberry Software DOS extender, a subset of DOS/4G customized for Watcom 32-bit applications.

Constraint: archive should seek contemporary Rational Systems/Tenberry manuals where available and use those preferentially for period-specific details.

## SR-004 — Swords and Software, "The Ravenloft Engine Trilogy's Automaps"

Author: Daniel D'Agostino
Date: 2024-03-10
Class: **COMMUNITY-RESEARCH**

URL: https://www.swordsandsoftware.com/ravenloft-automaps.php

Current validation: **PARTIALLY CORROBORATED**

Claims queued for Ravenloft verification:

- automap explored space is represented on a cell grid while finer-resolution entities are overlaid in-game;
- current automap can be exported under `AUTOMAPS` as an `AUTO###.MAP`-style file;
- export is intended as IBM/DOS character-set text art;
- proposed glyph semantics: unexplored `20`, wall `DB`, bottom/left/right/top inset `DC/DD/DE/DF`, floor `FA`, furniture `FE`, secret door `B2`, vertical door `BA`, horizontal door `CD`, tree `42`;
- player notes are represented in exported/printed maps with map markers and footnotes;
- global automap save/restore is reported for Menzoberranzan and Stone Prophet but not Strahd's Possession.

Corroboration reported by the source: the Ravenloft/Menzoberranzan manuals describe saved maps as viewable in a text editor; printing documentation refers to the IBM character set. This supports export intent but does not independently prove every glyph semantic.

Required project verification: locate export/print producer, recover glyph table from Ravenloft bytes, trace filename construction, note rendering and map-cell source, and prove absence/presence of global-save behavior in this Ravenloft release.

## Planned primary-source additions

The register should expand with exact editions/pages/sections as used:

- Intel 80386/80486/x86 architecture manuals;
- Microsoft MS-DOS Programmer's Reference / technical references for observed INT 21h contracts;
- Rational Systems/Tenberry DOS/4G/DOS/4GW documentation;
- official Ravenloft: Strahd's Possession manual and any version/readme/update documentation;
- executable-format documentation appropriate to the bound LE structure;
- hardware references for VGA, PIT, keyboard/mouse/interrupt mechanisms when those subsystem documents are written.

## Citation rule inside archive documents

For external sources, record source ID plus exact section/page where available. For Ravenloft binary claims, cite logical/file addresses and canonical reconstruction/audit artifacts. Community claims retain author/source attribution even after independent confirmation so the historical contribution is not erased.


## SR-005 — shipped CD `STATUS.ME`

Class: **PRIMARY-GAME / shipped development-mastering residue**

Source: original Ravenloft CD filesystem as supplied through the GOG `game.gog` CD image.

Preserved file identity:

- bytes: `1,029`
- SHA-256: `8f76f831bf7ab218784c1f2bfdee2ce8d7f6ec5ee926688f1fae5ba1adc585db`

The file begins with the literal header:

```text
@MULTI-EDIT VERSION 7.0    O:\BCOOLEY\6WORLDS\
```

and contains Multi-Edit workspace/status fields including an open-file reference to:

```text
O:\BCOOLEY\6WORLDS\PLAGUE.BAT
```

plus a file-history/filter entry `INSTALL?.NFO`.

Independent period/reconstructed Multi-Edit 7.0 documentation identifies `STATUS.ME` as the editor's saved status/workspace file. Accordingly this file is classified as **third-party development/editor residue accidentally or intentionally carried into the mastered CD filesystem**, not as a Ravenloft runtime data format.

Preservation value:

- proves part of a development/mastering workstation path survived onto the shipped CD;
- provides historical tooling/environment clues;
- may help future source-tree archaeology.

Constraints:

- `BCOOLEY` is a directory name; do not identify a specific person without independent evidence;
- `6WORLDS` and `PLAGUE.BAT` are preserved strings, but are **not** by themselves proof of Ravenloft codenames, project names, or runtime components;
- the file should not be included in the active resource/runtime dependency graph unless executable evidence is found.
