"""Automatic palette routing for Ravenloft indexed graphics.

The game does not attach a palette number to each RES raster.  CODE.2 keeps a
runtime palette mode and uses fixed palette banks for several modal screens.
This module records the resource-to-palette relationships that are directly
proved by the reconstructed call paths and uses the shipped level-table mode
for context-dependent gameplay artwork.

Automatic mode intentionally emits one convenient preview image per resource.
For world-context assets whose original palette varies by level, bank 4 is the
canonical preview because it is the most common mode in the shipped 32-level
table; the other proved runtime banks are retained in the manifest as valid
alternatives.  Canonical .indices data is unaffected by this choice.
"""
from __future__ import annotations

from dataclasses import dataclass


PALETTE_MODE_AUTOMATIC = "Automatic (recommended)"
PALETTE_MODE_MANUAL = "Manual override"
PALETTE_MODES = (PALETTE_MODE_AUTOMATIC, PALETTE_MODE_MANUAL)

# CODE.2 DGROUP 0x81AC: first byte of each 3-byte level display record.
LEVEL_PALETTE_BANKS = (
    4, 4, 4, 2, 2, 3, 3, 2,
    4, 2, 3, 3, 2, 2, 2, 4,
    4, 4, 3, 4, 3, 3, 4, 3,
    3, 4, 4, 3, 2, 0, 3, 4,
)
WORLD_RUNTIME_BANKS = tuple(sorted(set(LEVEL_PALETTE_BANKS)))
WORLD_CANONICAL_BANK = 4


@dataclass(frozen=True)
class PaletteDecision:
    bank: int
    source: str
    confidence: str
    alternatives: tuple[int, ...] = ()


def _decision(
    bank: int,
    source: str,
    confidence: str = "exact",
    alternatives: tuple[int, ...] = (),
) -> PaletteDecision:
    return PaletteDecision(
        bank=bank,
        source=source,
        confidence=confidence,
        alternatives=tuple(value for value in alternatives if value != bank),
    )


def _res0_palette(index: int) -> PaletteDecision | None:
    # Main in-game frame resources are selected directly from C034, the same
    # value passed to the COLORS loader.  These formulas are encoded in
    # CODE.2 functions 1F8FC, 1EA04, 1F494, 22F40 and 23024.
    if 16 <= index <= 30:
        return _decision(
            (index - 16) // 3,
            "CODE.2 RES0 world-mode triplet (base+1..3 + mode*3)",
        )
    for start, label in (
        (31, "base+0x10+mode"),
        (36, "base+0x15+mode"),
        (41, "base+0x1A+mode"),
        (46, "base+0x1F+mode"),
        (51, "base+0x24+mode"),
    ):
        if start <= index <= start + 4:
            return _decision(
                index - start,
                f"CODE.2 RES0 world-mode family ({label})",
            )

    # Character/inventory chooser: function 05AA4 passes palette 10 for
    # resources 63/64 and palette 11 for 65/66; function 0518C also draws the
    # corresponding mode decoration at entries 89/90.
    if index in (63, 64, 89):
        return _decision(10, "CODE.2 character/inventory chooser palette")
    if index in (65, 66, 90):
        return _decision(11, "CODE.2 character/inventory chooser palette")

    # Main/menu presentation path.  Function 19464 reloads COLORS bank 12
    # around the 192CC/1942C resources.  RES0:100 is independently used by the
    # extractor skin with bank 12.
    if index == 62 or 97 <= index <= 105:
        return _decision(12, "CODE.2 menu/modal palette")

    # Function 0EC1C uses bank 13 while displaying RES0:116/117.  RES0:77 is
    # independently reconstructed as the title-screen red texture with bank 13.
    if index in (77, 116, 117):
        return _decision(13, "CODE.2/title-screen special palette")

    return None


def resolve_palette(archive: int, index: int, resource_type: int) -> PaletteDecision:
    """Choose the best automatic display palette for one decoded RES resource.

    Exact mappings are preferred.  Assets whose shipped appearance is
    legitimately level-dependent use a canonical world preview bank and list
    the other observed runtime banks as alternatives.
    """
    if archive == 0:
        fixed = _res0_palette(index)
        if fixed is not None:
            return fixed
        return _decision(
            WORLD_CANONICAL_BANK,
            "RES0 context-dependent/base UI preview",
            "contextual",
            tuple(bank for bank in WORLD_RUNTIME_BANKS if bank != WORLD_CANONICAL_BANK),
        )

    if archive == 8:
        # INIT environment profile 0 resolves from base 8 and appears in
        # shipped levels using world palette modes 3 and 4.  Profile 1 resolves
        # from base 45 and is used by level 22, whose mode is 4.
        if 8 <= index <= 44:
            return _decision(
                4,
                "INIT environment profile 0 / shipped level palette modes",
                "contextual",
                (3,),
            )
        if 45 <= index <= 71:
            return _decision(
                4,
                "INIT environment profile 1 / level 22 palette mode",
                "exact",
            )

    if archive in (1, 2, 3, 4, 7, 8, 9, 11, 12):
        return _decision(
            WORLD_CANONICAL_BANK,
            "level-dependent gameplay palette; canonical shipped preview",
            "contextual",
            tuple(bank for bank in WORLD_RUNTIME_BANKS if bank != WORLD_CANONICAL_BANK),
        )

    return _decision(0, "base COLORS palette fallback", "fallback")
