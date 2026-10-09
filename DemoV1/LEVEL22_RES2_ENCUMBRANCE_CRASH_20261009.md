# Level22 inventory/encumbrance crash — missing original RES2 bank

**User report:** Build #649 screenshot: `Demo v1 has no loaded type-1 archive for requested bank`, after clicking the inventory's encumbrance/stat control. Screenshot shows built revision `6241a807baf5`, the correct #649 build source. This is an application source omission, not evidence of stale EXE or missing original game files.

## Exact call chain

1. Original CODE.2 **12988** loads the encumbrance panel from the original **RES2:0x25**, at framebuffer row `0x73` and the chosen 80-pixel actor column. The source-backed native `PartyModalPresenter::draw_encumbrance_panel` calls `assets_.type1(2,0x25)`.
2. `PartyModalPresenter::draw_open_container` also requests original **RES2** type-1 frames for container-opening transitions and panel art (`0x14 + subtype * 2 + phase - 1`, and the original container graphic selector). These controls have the same missing-bank defect.
3. In build #649, `RavenloftBootAssets::type1` had cases **0,1,7,8** only. The original RES2 requests fell directly into the default error `Demo v1 has no loaded type-1 archive for requested bank`; outer Win32 exception reporting displayed it as an unrecoverable dialog. This is a precisely reproduced source-level cause.
4. The **original supplied Strahd RES2 exists** on `DnD-Classics` under `MASTER/Ravenloft - Strahd's Possession/RLOFT/RES2` and corresponding CD-Extracted/RLOFT. Certified size **63,736 bytes**, SHA-256 `bd46022e716fb1014ce0d0be98fa2c8a1cad7970df6943b1b4d54d017581a8f8` (original preservation work `Resources/RES/RES2/README.md`). No file is to be generated or substituted.

## Repair

`RavenloftBootAssets` now has **lazy cached `res2_`**, loaded only when a real RES2-dependent original UI action requests a type-1 entry. `type1(2,index)` resolves `source_.read("RES2")` and uses the already-established `ResArchive::decode_type1`, supporting both original loose files and the GOG disc image through the same trusted `GameSource`. It does not preload RES2 at normal game startup, invent placeholder graphics, map RES2 requests to RES0, or alter the DOS original files.

`original_res2_asset_bank_tests` reconstructs a minimal synthetic EH type-1 archive fixture at the real encumbrance entry `0x25`. It proves construction is successful before RES2 exists (lazy behavior), verifies the original bank-2 routing and 8-pixel type-1 payload, checks cache reuse, rejects the wrong RES2 index and still rejects unsupported banks. The fixture is test-only; it does not copy original asset bytes into our executable or repo.

**Status:** source patch and test committed, but **not yet Windows-built**. Currently **180 registered CTests**, compared to **179/179 passing** in prior Full Windows build #649. No automatic build triggered. Re-test the real Level22 encumbrance and container UI only after explicit next build authorization. Exact DOS appearance and original menu timing remain separate acceptance requirements.

## Where diagnostic ZIPs are saved

The new `Diagnose-Level22.cmd` launcher uses `%TEMP%\\RLOFT-Demo-Diagnostics`. On game exit it writes `<timestamp>-<process-id>.zip` next to the timestamped source capture folder and prints its path. `latest-session.txt` in the parent directory points to that folder. If ZIP creation failed, compress the timestamped folder manually and upload it to this conversation. Nothing is automatically uploaded to ChatGPT. A crash before `--dev-trace` initializes could leave no folder, but this encumbrance error is entered after the normal exploration setup and typically after recorder creation; check the folder.

**Priority:** fix all user-observed Level22 defects and finish the entire playable source-faithful Level22 up through its original mist sequence; no independent Level23 development.
