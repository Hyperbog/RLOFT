"""Original Ravenloft graphics used by the optional Strahd GUI theme.

Artwork is decoded directly from the selected certified RES0/COLORS files.
Nothing is approximated or redrawn.
"""
from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from PIL import Image

import res_assets


class ThemeAssetError(RuntimeError):
    pass


@dataclass(frozen=True)
class StrahdThemeAssets:
    frame: Image.Image
    hud_frame: Image.Image
    menu_strip: Image.Image
    ornament: Image.Image
    parchment: Image.Image
    title_logo: Image.Image

    @classmethod
    def load(cls, source_dir: Path, palette_index: int = 0) -> "StrahdThemeAssets":
        source_dir = Path(source_dir)
        res0 = source_dir / "RES0"
        colors = source_dir / "COLORS"
        if not res0.is_file() or not colors.is_file():
            raise ThemeAssetError("Strahd theme requires RES0 and COLORS in the selected source folder")

        archive = res_assets.ResArchive(res0, 0)
        palette = res_assets.palette_from_colors(colors, palette_index)
        by_id = {resource.index: resource for resource in archive.resources()}

        def image(resource_id: int) -> Image.Image:
            try:
                resource = by_id[resource_id]
            except KeyError as exc:
                raise ThemeAssetError(f"RES0:{resource_id} is missing") from exc
            raster, _ = res_assets.decode_type1(resource)
            height = raster.count * raster.element_height
            result = Image.frombytes("P", (raster.width, height), raster.pixels)
            result.putpalette(palette)
            return result.convert("RGB")

        return cls(
            frame=image(16),
            hud_frame=image(17),
            menu_strip=image(71),
            ornament=image(91),
            parchment=image(77),
            title_logo=image(99),
        )
