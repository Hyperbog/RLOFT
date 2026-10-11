# First user-owned GOG installation field test — Windows, October 11, 2026

**Branch:** `native-engine`  
**Installed-media evidence:** user-supplied screenshot of the **actual standalone Windows native bootstrap** after selecting their own installed GOG Ravenloft: Strahd's Possession folder.  
**Executable compiled source SHA:** `1f8287f61346c1143b3e9348342a3691a487df4a`.  
**Related automated evidence:** [VS2022 Windows build, 15/15 CTests](WINDOWS_VERIFIED_BUILD_20261011.md), [run #38133169095 attempt 2](https://github.com/Hyperbog/RLOFT/actions/runs/38133169095).

## Directly supported by the screenshot and user's report

- **The native Windows program opened successfully** on the user's Windows machine.
- After the user selected an original install directory, the application identified **`installed GOG game.gog CD image`** as the source type and displayed an actual `game.gog` media pathname. **The user's full personal game path is intentionally not reproduced in this source-control document.**
- The **minimum `INIT`/`RES0`/`T` media signature** reports **PASS**. The 18-name diagnostic inventory showed `[+]` for each candidate entry visible: `INIT`, `RES0`, `T`, `RES1`–`RES8`, `RES10`, `COLORS`, `LEVELS.DAT`, `OBJECTS.DAT`, `NPCS.DAT`, `SINTAB`, `CINE00.ANM`. These are presence signals, **not comprehensive asset decoding/integrity validation**.
- The application presented the original artwork for the **Advanced Dungeons & Dragons / Ravenloft: Strahd's Possession title and four-option CODE.2 startup menu**, rather than the synthetic diagnostic or RES0 cursor fallback. The caption explicitly reads **`Original CODE.2 STATIC MENU FRAME | NOT GAMEPLAY`**.
- The app indicated its CODE.2 static-menu reconstruction and displayed diagnostic indexed-frame/palette FNV64 fingerprints. Those fingerprints are source-test handles only; they have **not** been compared with a DOS runtime capture.
- The application continued to show **version/patch UNVERIFIED**; no specific GOG patch level or full original-game compatibility has been established.

## Accepted and unaccepted gates

| Gate | Field observation |
| --- | --- |
| Independent Windows x64 native bootstrap launches | **USER-OBSERVED PASS** |
| Select and read an actual GOG `game.gog` installation | **USER-OBSERVED PASS** |
| Locate minimum original-media signature | **USER-OBSERVED PASS** |
| Show 18 named source-resource presence indicators | **USER-OBSERVED PASS (presence only)** |
| Decode original-source indexed/palette menu composition | **USER-OBSERVED PASS** |
| Original installation unchanged before/after | **NOT INDEPENDENTLY VERIFIED**. Source code contains no intended writes; no before/after hash evidence submitted |
| Actual original menu input / transition to CODE.1 | **NOT IMPLEMENTED in this bootstrap** |
| Character creation, party import, normal-game world entry | **NOT REACHED** |
| Original game timing/DOS pixel parity | **NOT MEASURED** |
| Windows release code-signing, retail patch fingerprint | **NOT VERIFIED** |

The user-provided screenshot itself is **not committed**: it includes original copyrighted in-game imagery and an absolute local installation path. This document preserves only the necessary engineering observations.

## Next source-faithful integration target

The next milestone should **not** be another synthetic demo. In the separate `rloft_native` Windows game product, bind original menu selection/highlight and input to the inherited **`BootFlow`** contract, `StartupPresenter::menu_frame(index)`, and original CODE.2 transition evidence. The binary-traced CODE.2 startup jump-table index 2 returns status 10 through the supervisor to CODE.1 character creation; entries 0 and 1 retain their own CODE.2 behaviors and must not be invented. Keep original timing, loading/validation, profile-safe save rules, and patch detection distinct gates.

**Milestone accounting:** This field test closes the *installation-backed static-media graphics preview* portion of [M-WIN-01 #12](https://github.com/Hyperbog/RLOFT/issues/12); leave the issue open for independently verified no-write integrity, Windows GUI interaction/regression and original installation compatibility checklist. It **does not** close [M-ENGINE-01 #9](https://github.com/Hyperbog/RLOFT/issues/9).
