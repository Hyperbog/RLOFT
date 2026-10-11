RAVENLOFT: STRAHD'S POSSESSION — INDEPENDENT NATIVE ASSET ENGINE
=================================================================

STATUS: WINDOWS x64 ASSET-LOADING PREVIEW. NOT YET A PLAYABLE GAME.

This Windows application is a new replacement engine, conceptually closer
to OpenMW/KEX than to an original-source recompilation. It does NOT link
the decomp demo's game controllers, character creation, executable, or
source-traced cinematics.

It loads ORIGINAL USER-OWNED ASSETS DIRECTLY:
  * DOS loose-file installation with INIT, RES0 and T
  * GOG game.gog ISO9660 (2048-byte data sectors)
  * GOG raw CD sectors, 2352 bytes with data at offset 16 or 24
  * Original RES archive offset tables/entries
  * Original 15-bank COLORS palette and DAC6 channels
  * Inventory/status for RES0-RES8, RES10, LEVELS.DAT, OBJECTS.DAT,
    NPCS.DAT, SINTAB, CINE00.ANM and CINE12.ANM

No bulk unpacking, repacking or conversion is required to open a game
installation. The source game directory and game.gog are read-only.

TO TEST:
  1. Extract the ZIP in a folder separate from the original game.
  2. Run rloft_native.exe.
  3. Select the folder that contains your ORIGINAL game.gog or loose files.
  4. Inspect file sizes, RES entry counts, validation status and the
     original source COLORS palette 12.
  5. The engine also attempts to decode the original RES0 default
     cursor from INIT using its NEW independent EH type-1 raw/LZSS parser.
     If supported, an authentic palette-indexed cursor appears below the
     source COLORS palette. The cursor is a resource preview, NOT gameplay.
  6. NEW: choose RES0/RES1/.../RES10 in the image browser. Enter the
     original resource record number and press Go, or use Previous /
     Next image to find indexed EH type-1 images. The original palette
     is applied in memory. No resource bytes are exported or modified.
     Other EH resource types are reported as NOT an EH1 image, rather
     than guessed/decoded with the wrong codec.
  7. Record any BAD entries or unexpectedly missing resources. Cursor
     and optional RES image decode failures are reported separately
     from the 19 structural checks.

For a noninteractive status code on Windows, use the original-install path:
  rloft_native.exe --verify-assets "D:\GOG Games\Ravenloft - Strahd's Possession"
Exit 0: RES0 offset table, INIT, T and COLORS present/valid
Exit 3: source exists, but one of these primary validation checks failed
Exit 2: source cannot be opened

For the NEW stricter original raster verification:
  rloft_native.exe --verify-raster "D:\\GOG Games\\Ravenloft - Strahd's Possession"
Exit 0: required original assets and default RES0 cursor decode accepted
Exit 3: primary media signature incomplete
Exit 4: original cursor decoder/resource unsupported or invalid
Exit 2: original source cannot be opened

The independent EH1 decoder and --verify-raster option are part of a
NEW SOURCE REVISION. Earlier successfully built asset-preview executables
do NOT contain them; verify the compiled Git SHA before trying this mode.

IMPORTANT: The independent native game simulation, cinematics, world,
interaction, save system and DOS parity are separate future milestones.
The old decomp DemoV1 remains its OWN project. Its source/binary is NOT
bundled with or linked into the independent native engine.

ARCHITECTURE:
  original install (read only) -> native ISO/loose-file index -> native
  RES/palette decoders -> future render/audio/world/gameplay systems.

DECISION: Original assets are loaded lazily in place. A separate, versioned
cache of decoded resources can be introduced later if profiling shows it
helps. No rewriting of game.gog, no repacking original game files, and no
bulk unpack prerequisite for normal native execution.

This package contains no copyrighted original assets or private saves.
