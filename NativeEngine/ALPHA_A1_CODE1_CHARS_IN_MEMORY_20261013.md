# Native A1 — original CODE.1 character creation and CHARS memory transfer

**Branch:** `native-engine`  
**Source revision audited:** `064e0bf191d4fb6d71d1d1e2164b3494831fca26`  
**Date:** 2026-10-13  
**Status:** Implementation checkpoint, **NOT a passing current-source build or playable Alpha claim**.

## Integrated in the independent native Windows startup window

1. The existing original CODE.1 `CharacterCreationPresenter`, palette remapping, statistics tables/model, and 0x920-byte `CharacterTransfer` serializer now compile in `Shared/StartupPresentation`. DemoV1 links the same implementation; its only retained local transfer source is the `RavenloftBootAssets::source()` compatibility adapter.
2. After the original CINE00/CINE12 → RES0 logos/title → CODE.2 supervisor menu, selecting the character creation option enters the source 70-frame CODE.1 opening and original indexed character cards. Actor, sex, race, class, option, portraits and second portrait page selection use the shared source `CharacterCreationFlow`; source portrait mouseover switches raw versus luminance mapped pixels.
3. Original `CharacterStatsModel` drives roll, reroll, selected-stat editor and source-backed status screens. Final accept/reject, existing actor keep/delete, and name input are connected. Names accept only original CODE.1 uppercase A–Z, 0–9 and space, at most 15 bytes, and must fit the actual RES0 font width strictly below 0x30. Cursor blink is host timed.
4. After **two accepted actors**, choosing the source actor-choice completion control requires retained original stats for both, builds the exact `0x920` CHARS structure using original CODE.1 family/race equipment and OBJECTS.DAT conflict table, and **retains the transfer bytes in memory only**. The shared `BootFlow` transitions from CODE.1 status 10 to CODE.2 startup; native does **not** proceed to gameplay and never writes CHARS to original game data.
5. Synthetic original-format native regression creates and serializes two explicitly test-only actors using the shared CHARS code and checks both 0x248-byte actor record boundaries and total 0x920 bytes.
6. Real original-media startup audit was extended to render all 70 CODE.1 opening frames, initial actor and sex screens, and parse all 90 original starter-equipment rows. Windows workflow checks original source file SHA256 before/after, plus assertions for 70 frames, 90 rows and zero source writes.
7. Dual-consumer Linux CI now builds and requests boot, creation, character statistics, transfer and palette regression targets. These changes **require new passing run logs for acceptance**.

## Independent original-byte observations from supplied files

- `CODE.1`: **277,787 bytes** (`0x43D1B`), with CODE.1 DGROUP beginning at file `0x3E724`.
- `OBJECTS.DAT`: **21,844 bytes**, count **336**, exactly `4 + 336*65` bytes.
- Original starter table: **15 class-family × 6 race = 90 rows**, **14 words each**, **1,260 words** total, beginning at DGROUP `0x0988`.
- **309 nonempty** source starter-item references, with **zero** references outside the 336 original OBJECTS entries. This confirms the table decode bounds; it is not yet DOS runtime equivalence.

## Acceptance status and unimplemented gates

The source files, GitHub commit and relocation hashes were independently inspected; lexical delimiter/string balance checks passed on modified C++ sources. **No current-HEAD MSVC/Linux compiled CTest result, physical Windows GUI interaction proof, real-source visual snapshot parity, or DOS RNG/timing parity has been retrieved**. Prior 38/38 Windows tests at [run #38346464375](https://github.com/Hyperbog/RLOFT/actions/runs/38346464375) belong to older source and do not accept this checkpoint.

The native executable still launches a developer media inspector, with the source startup as a separate window; it is **not yet the canonical game boot entrypoint**. The created in-memory CHARS image is not imported into a native CODE.2 party/world state and does not survive closing the startup window. Native world exploration remains a separate read-only GPU preview. Original controller transition/dissolve animations, music, VOC playback, full live state scheduling, actor gameplay, interactions, save/load and real first-level session remain open.

**Required next order:** confirm exact-source CI build and original-media audit; rectify concrete failures; make normal native game startup the default; import in-memory source CHARS into a real game/party session; load level 22 through normal CODE.2 initialization; implement one authentic interaction, inventory/event encounter and writable **separate profile** save before using the term first playable Alpha. Beta and Final remain later milestones.
