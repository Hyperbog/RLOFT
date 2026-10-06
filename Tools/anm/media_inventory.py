"""Certified media/source inventory for Ravenloft: Strahd's Possession.

The file identities and roles mirror Resources/FINAL_RESOURCE_AUDIT.md.
Discovery deliberately uses this table instead of filename heuristics.
"""
from __future__ import annotations

import hashlib
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class SourceSpec:
    name: str
    size: int
    sha256: str
    category: str
    role: str


def _spec(name: str, size: int, sha256: str, category: str, role: str) -> SourceSpec:
    return SourceSpec(name, size, sha256, category, role)


SOURCES = (
    _spec("CINE00.ANM",13781936,"6627ebb7e91cc62c56871ad5140a5df038fde198b079e94b010467433ce9eccb","cinematic","ANM cinematic"),
    _spec("CINE01.ANM",16009364,"47a1e618f687351ebdb3e9ed07961f0feb3a42969f4cf984e48884c548153362","cinematic","ANM cinematic"),
    _spec("CINE02.ANM",7498347,"62a06c3ecf78c3f7907279592cc042f652cbcafeb8694027ce72f5d6995d9199","cinematic","ANM cinematic"),
    _spec("CINE03.ANM",7498347,"7da29505c4aac4f3449eb8feed9f119f9f80101fb0c91ffdb46938956e3b6ce6","cinematic","ANM cinematic"),
    _spec("CINE04.ANM",7498347,"78f352452abfeb7c9f4f3edb34f30c4c44f30752b88e6e783a04d1a3c5d66c2f","cinematic","ANM cinematic"),
    _spec("CINE05.ANM",7498347,"6f991c789a1a99c2d182bae55426b6e4b00e5ceb210871874518afd0a5aefbe6","cinematic","ANM cinematic"),
    _spec("CINE06.ANM",7339821,"7149f7a9020a1d718970ce2fa610cfbfab76b5f89d95caae17b78e119d9e3925","cinematic","ANM cinematic"),
    _spec("CINE07.ANM",34569939,"70cefedf8d2cb68525e6ebdcb8bf9e4be07e4619d12610b537e2f3c9841685ef","cinematic","ANM cinematic"),
    _spec("CINE08.ANM",9255472,"cfbc8b5835906ceedcf74e8c202430cd0b5b4e4d4c7c00054d42b934def817f3","cinematic","ANM cinematic"),
    _spec("CINE09.ANM",5485203,"287c3752048c5545c78f51d09035635bcd5422849770482918179a0d89f9387c","cinematic","ANM cinematic"),
    _spec("CINE10.ANM",56273227,"e7d6404057d1ea472f0ee1ec870b9098fff9adb86020af08d2e531eae6961bfd","cinematic","ANM cinematic"),
    _spec("CINE11.ANM",57092507,"9ee3f32320e0ae975b496ec69d34691ff45cc9e48fbf54a5de4a3f2fb5ef067e","cinematic","ANM cinematic"),
    _spec("CINE12.ANM",1446666,"a39bdb4965ba27f9fd1b1f4a8fb2ac291a48da831070cdf226faab229b132291","cinematic","ANM cinematic"),
    _spec("RES0",2186991,"5e30584947c9627e31b5dd2573ad75c2d3cc9fe796ac669e0ca5d02d6db7e61d","resource","type-1 UI/raster graphics + type-2 fonts"),
    _spec("RES1",354310,"c13b8ae42f7d3b65c06d8413627f40a2a155a7404da44abfc158b38a4e5efd19","resource","type-1 object/UI graphics + portraits"),
    _spec("RES2",63736,"bd46022e716fb1014ce0d0be98fa2c8a1cad7970df6943b1b4d54d017581a8f8","resource","type-1 raster graphics"),
    _spec("RES3",5228638,"44955ebbed0a8ed8d874fae07b01710317044882ab3c7c95d9be54e883df9a36","resource","type-7 compiled scene shapes"),
    _spec("RES4",1975542,"d8989c50fa58e317ff33465e7baec16e970f40586417d790fd8a1b9005c60f46","resource","type-6 first-person textures"),
    _spec("RES5",1510960,"f943c933cfe593bb2912bc25e10b72f1ecafe10f684526f38489616cedbe91b6","resource","type-3 XMIDI/XDIR music"),
    _spec("RES6",6210899,"697aa0803b2219dcb094bfae769ffb0c60cd7d89f7a7ba06c12ed6a4553f736d","resource","type-4 Creative Voice File sampled audio"),
    _spec("RES7",211133,"78b46977d1d3232824392ec1e4852519a6d2e20277adb0e78a67c7605ee9c4e1","resource","type-1 raster graphics"),
    _spec("RES8",58772,"bfadd70c836260143f7e28b34128a0a16e7ca1743379a00176d2adfa525b28b4","resource","type-1 environment/backdrop graphics"),
    _spec("RES9",8061481,"c3021a8bb16bf1d8b9234ac47a670e4a41b8a1c078cba34be34f0f89a5135543","resource","type-1 interaction graphics + non-media wordcode"),
    _spec("RES10",46060354,"dc0c6b25a521f54d82bc564168859c74aa4b464413338d4b58d519b166e8e6f9","resource","type-4 interaction voice/SFX"),
    _spec("RES11",15589639,"5c231d0e117def4f465f9b8d24ea6a315f610919e5fdd0910b9c2446d435d903","resource","alternate type-3/4/6/7 media set"),
    _spec("RES12",12150551,"d03808be3f96ae48eab9f991433e8f7c4af989a6cf76ae9cb5a14f6964e40855","resource","alternate type-1/type-4 media + raw non-media records"),
    _spec("GM1.BNK",23907,"843f012e7980e0a58722aa5c7a6e88b652f2f8d404834638cd994ca0869df5a8","music-support","Sierra Semiconductor ARIA 512 KiB GM bank"),
    _spec("GM2.BNK",24273,"b4af656059a6afba5db2da972c3506ca08d899ca04c65389e0bec7aaf6536339","music-support","Sierra Semiconductor ARIA 1 MiB GM bank"),
    _spec("COLORS",11521,"e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5","graphics-support","15-bank VGA palette file"),
    _spec("INIT",1879,"cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276","mapping-support","resource namespaces, texture/shape/audio bases, environment profiles"),
    _spec("SAMPLE.AD",4662,"79ee9f4ad3134be906ce493072ce0c3b7169c9013d3c25947ac385aaf3f9633b","music-support","Miles AIL YM3812/OPL2 Global Timbre Library"),
    _spec("SINTAB",333757,"19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87","graphics-support","projection tables + type-7 scale rasterizer microcode"),
    _spec("T",133584,"a3f1e19e40f226c8cf150a43aacd9c378ee35a1294d2c9dd72870bfdf1e16315","cinematic-support","indexed text archive including ANM subtitles"),
)

SOURCE_BY_NAME = {s.name: s for s in SOURCES}
ANM_NAMES = tuple(f"CINE{i:02d}.ANM" for i in range(13))
RES_NAMES = tuple(f"RES{i}" for i in range(13))


def sha256_file(path: Path) -> str:
    h = hashlib.sha256()
    with Path(path).open("rb") as fp:
        for block in iter(lambda: fp.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def validate_source_dir(root: Path, verify_hashes: bool = True) -> dict[str, object]:
    root = Path(root)
    present: list[str] = []
    missing: list[str] = []
    mismatched: list[dict[str, object]] = []
    for spec in SOURCES:
        path = root / spec.name
        if not path.is_file():
            missing.append(spec.name)
            continue
        present.append(spec.name)
        size = path.stat().st_size
        digest = sha256_file(path) if verify_hashes else None
        if size != spec.size or (verify_hashes and digest != spec.sha256):
            mismatched.append({
                "name": spec.name,
                "expected_size": spec.size,
                "actual_size": size,
                "expected_sha256": spec.sha256,
                "actual_sha256": digest,
            })
    return {
        "root": str(root),
        "expected": len(SOURCES),
        "present": present,
        "missing": missing,
        "mismatched": mismatched,
        "certified_complete": not missing and not mismatched,
    }
