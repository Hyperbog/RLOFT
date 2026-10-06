# Ravenloft Demo Build v1

Goal: reproduce the original shipped behavior from boot through character creation while reading assets from a legally installed/owned copy of *Ravenloft: Strahd's Possession*.

## Asset policy

The demo does **not** bundle Ravenloft game data.

Normal use is:

1. locate an installed Ravenloft copy automatically;
2. identify the original media source;
3. read/extract only the files required by the native runtime;
4. preserve original resource IDs, palette indices, timing, and data semantics.

A pre-extracted resource folder remains supported for development and preservation testing, but it is not the intended end-user requirement.

## Installed-copy discovery

The native source layer currently supports:

- the GOG Ravenloft product registry entry (product ID `1432905053`) on Windows;
- `RLOFT_GAME_DIR` as an explicit override;
- common GOG Games / GOG Galaxy roots;
- common Steam roots;
- a directly supplied game installation directory;
- a directly supplied `game.gog` path;
- a loose directory containing Ravenloft `RES0`, `INIT`, and `T`.

Candidate installations are accepted only when Ravenloft-specific media anchors are present.

## game.gog

GOG's installed CD image can be consumed directly. The demo has a small read-only ISO-9660 reader and does not require 7-Zip or a manual extraction step.

Supported data-sector layouts:

- 2048-byte ISO sectors;
- 2352-byte raw Mode-1 sectors with 2048-byte user data;
- 2352-byte sectors whose ISO user data begins at offset 24.

ISO version suffixes such as `;1` are normalized during lookup. The indexer walks subdirectories so resource files do not need to be at the image root.

## Runtime-side files

For files that belong to the writable installed game rather than the CD resource image, lookup checks both:

- the installation root;
- the installation root's `RLOFT` directory.

This keeps the original distinction between installed runtime state and CD/resource media available to later compatibility work.

## Current boot milestone

The binary-proven transition implemented so far is:

```text
R.EXE supervisor
  -> CODE.2 startup
  -> CHARS-absent cold-start presentation
  -> CODE.2 four-entry startup menu
  -> menu index 2
  -> legacy result 10
  -> R.EXE launches CODE.1
  -> character-creation workflow
```

The next implementation unit is the native RES0/COLORS presentation renderer used by the cold-start sequence and startup menu.
