# First verified independent native asset engine — Windows x64

**Date:** 2026-10-11. **Architecture:** replacement engine, *not* the DOS decomp/recomp demo.

## Exact, executed evidence

- [GitHub Actions run #38164976411](https://github.com/Hyperbog/RLOFT/actions/runs/38164976411), successful one-shot Windows job `114545540115`.
- Compiled the `native-engine` source at **`fd314b89e96db07c739176d25f1fc9be94ecaa19`** using VS2022/MSVC 19.44.35229.0 on the GitHub **windows-2022** x64 runner.
- Executed **9/9 independent native-engine CTests**, all passed: native framework, CPU indexed-frame oracle, disk mod mount, mod load/transaction, media-signature contract, transactional mod mount, real native asset-media synthetic format fixture, separate native Windows GUI target presence, renderer evidence contract.
- Built **`rloft_native.exe`**, independently inspected its executable bytes as Windows **PE AMD64 `0x8664`**, subsystem **Windows GUI (2)**. It is not the DemoV1 executable or a copy of the older bootstrap.
- The build script packaged its **asset-free** release ZIP, reopened it to verify four-file allowlist and its EXE SHA, wrote the sidecar, and uploaded the artifact.
- GitHub artifact [#11711054895](https://github.com/Hyperbog/RLOFT/actions/runs/38164976411/artifacts/11711054895) is the CI-provided outer download; the model independently downloaded and verified its bytes, the embedded distribution ZIP, and the executable.
- Owner authorization was for one build. A **single isolated, narrowly path-scoped Windows-only trigger branch** initiated the run because the connected GitHub app does not expose workflow_dispatch. Normal `native-engine` and default branches retain their manually dispatched Windows asset workflow; no standing push trigger was introduced there.

## Exact downloaded hashes

| Package | SHA-256 |
| --- | --- |
| GitHub outer artifact `RLOFT-NativeEngine-Windows-x64-ASSET-PREVIEW-20261011.zip` | `e5cb11fc0936481b5f34b2982c4bfac38348477c36e6aedd8977e6d63aec984c` |
| User distribution `RLOFT-NativeEngine-Windows-x64-ASSET-PREVIEW.zip` | `5ee2cdb58f3aa0ea10c715a2841b6be3eb0f411abb957132c4aa1150a857713c` |
| `rloft_native.exe` | `f74e46ed2c4184dd4fa6d577eb1d700095e36437e9898f8be1ccfe6d4ef15054` |

The inner distribution contains exactly **four** files: `rloft_native.exe`, `README-NATIVE-ASSETS.txt`, `SHA256SUMS.txt`, `build-provenance.json`. The embedded provenance records `source_commit=fd314b89...`, `engine_stage=NATIVE_ASSET_LOADING`, `gameplay_verified=false`, `original_assets_included=false`, `full_extraction_required=false`.

## Tested feature scope

The program is a newly implemented, **independent original-media loader**, not the source-traced DOS runtime. Its current C++ layer indexes original loose DOS files or a `game.gog` ISO9660 volume, reads original files on demand, handles original 2048-byte and raw 2352-byte sectors (user-data offset 16 or 24), resolves unambiguous DOS asset filenames in nested directories, validates bounded RES offset tables/entry counts, and decodes original 15-bank COLORS DAC6 palettes.

The GUI lets the owner select a source installation and inspect a 19-name asset inventory, RES archive entry counts, and the original COLORS palette #12. The CI resource corpus is **synthetic original-format** and contains no copyrighted game files. The old decomp demo's CINE00/menus/characters/engine code is **not linked** into this independent executable.

## Unaccepted gates and next test

- **Owner's actual GOG `game.gog` has NOT been tested by this newly built independent loader**. The previous bootstrap's successful real GOG static title/menu image does not automatically validate this native ISO/RES/COLORS parser. Select the same game directory in this new GUI, then inspect each `[BAD]` or unexpectedly missing resource.
- `--verify-assets "<original install dir>"` exits **0** for original INIT, RES0, T and COLORS validation, 3 for a present but incomplete required signature, and 2 for unreadable media.
- No real gameplay, world, native 3D rendering, party/saves, sound, cinematic playback or DOS runtime parity is claimed yet.
- No mandatory bulk extraction/repack: installed media is indexed and read directly, with a later optional derived cache only if needed.
- The artifact was compiled with expected MSVC warnings `D9025` because test targets deliberately override Release `/DNDEBUG` with `/UNDEBUG` to exercise assert-based fixtures. These were **warnings**, not compilation or CTest failures.
- **Do not call later documentation HEAD revisions compiled.** Only `fd314b89...` is covered by this Windows build.

**Outcome:** Windows native asset-engine source/build/test/distribution milestone **PASS**. Real original-media compatibility remains **pending user field-test**.
