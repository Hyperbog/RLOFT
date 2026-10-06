"""Original Ravenloft graphics used by the optional Strahd GUI theme.

Theme artwork is decoded from the user's certified RES0/COLORS files. Each
asset uses the palette bank that produces its intended UI appearance; assets
with flat source backgrounds expose those indices as alpha for compositing.
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
    def load(cls, source_dir: Path) -> "StrahdThemeAssets":
        source_dir = Path(source_dir)
        res0 = source_dir / "RES0"
        colors = source_dir / "COLORS"
        if not res0.is_file() or not colors.is_file():
            raise ThemeAssetError(
                "Strahd theme requires RES0 and COLORS in the selected source folder"
            )

        archive = res_assets.ResArchive(res0, 0)
        by_id = {resource.index: resource for resource in archive.resources()}
        palettes = {
            index: res_assets.palette_from_colors(colors, index)
            for index in (0, 12)
        }

        def image(
            resource_id: int,
            palette_index: int = 0,
            transparent_index: int | None = None,
        ) -> Image.Image:
            try:
                resource = by_id[resource_id]
            except KeyError as exc:
                raise ThemeAssetError(f"RES0:{resource_id} is missing") from exc

            raster, _ = res_assets.decode_type1(resource)
            if raster.count <= 0:
                raise ThemeAssetError(f"RES0:{resource_id} is an empty raster")

            height = raster.count * raster.element_height
            indexed = Image.frombytes(
                "P", (raster.width, height), raster.pixels
            )
            indexed.putpalette(palettes[palette_index])

            if transparent_index is None:
                return indexed.convert("RGB")

            rgba = indexed.convert("RGBA")
            alpha = Image.new("L", indexed.size, 255)
            mask = indexed.point(
                lambda value: 0 if value == transparent_index else 255,
                mode="L",
            )
            rgba.putalpha(mask)
            return rgba

        return cls(
            frame=image(16, 0),
            hud_frame=image(17, 0),
            menu_strip=image(71, 0),
            ornament=image(91, 0, transparent_index=254),
            parchment=image(77, 0),
            # RES0:99 is not authored for palette 0. Palette 12 gives the
            # coherent Ravenloft red/steel title treatment; index 160 is the
            # flat surrounding field and is transparent when composited.
            title_logo=image(99, 12, transparent_index=160),
        )
