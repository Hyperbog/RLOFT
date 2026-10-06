"""Bundled Strahd's Possession theme assets.

These PNGs were extracted from the supplied Ravenloft RES0 using COLORS palette
bank 0. The source archives are not needed at runtime for the optional GUI skin.
"""
from __future__ import annotations

import hashlib
import json
from dataclasses import dataclass
from pathlib import Path

from PIL import Image

ASSET_DIR = Path(__file__).resolve().parent / "assets" / "strahd"
MANIFEST_PATH = ASSET_DIR / "manifest.json"


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
    def load(cls, verify: bool = True) -> "StrahdThemeAssets":
        try:
            manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError) as exc:
            raise ThemeAssetError(f"cannot read Strahd theme manifest: {exc}") from exc

        def open_asset(key: str) -> Image.Image:
            record = manifest["assets"][key]
            path = ASSET_DIR / record["file"]
            try:
                payload = path.read_bytes()
            except OSError as exc:
                raise ThemeAssetError(f"missing bundled theme asset: {path.name}") from exc
            if verify:
                actual = hashlib.sha256(payload).hexdigest()
                expected = record["sha256"]
                if actual != expected:
                    raise ThemeAssetError(
                        f"{path.name}: SHA-256 mismatch ({actual} != {expected})"
                    )
            with Image.open(path) as image:
                return image.convert("RGB")

        return cls(
            frame=open_asset("frame_main"),
            hud_frame=open_asset("frame_hud"),
            menu_strip=open_asset("menu_strip"),
            ornament=open_asset("ornament_frame"),
            parchment=open_asset("parchment_texture"),
            title_logo=open_asset("title_logo"),
        )
