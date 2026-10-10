# Windows Full #655 — RES4 forensic probe build accepted (2026-10-10)

**Verified CI outcome:** [GitHub Actions run #655](https://github.com/Hyperbog/RLOFT/actions/runs/38082247830), ID `38082247830`, completed **success** at 2026-10-10 20:11:57 UTC.

| Evidence | Exact value |
| --- | --- |
| Actual checkout compiled | `80cd269bb60cc2bf73ba984cab5caacd2b461a61` |
| Native Windows compilation | PASS |
| Canonical CODE.2 strict C++17 syntax | PASS |
| Full Windows CTest | **212/212 PASS (100%)** |
| Published ZIP artifact | `Ravenloft-Demo-v1-Windows-x64` |
| Artifact ID | `11680458704` |
| Artifact ZIP size | 824,112 bytes |
| Artifact ZIP SHA-256 | `633d3be030e114465698ec4829d5c12c75b765efbded64e2d99c920349f100d4` |
| Checked-in `ravenloft_demo_v1.exe` SHA-256 | `0184787edaf74b4aa7a76bdc46f7d47e846a56e6a7dbd9063051deb3ade8339b` |
| Windows build workflow after trigger | Original manual-only blob `7c8e5720f38c0690d01457ea3daf556124725ef2`, temporary marker deleted |
| Linux runner | Deliberately skipped |

## What the successful build covers

The new `OriginalGroundRes4ProbeSnapshot` and its Windows `rloft_res4_ground_probe.exe` were compiled. Existing `original_ground_res4_audit_tests` synthetic regression was run in this **212-test** checkout: even when the original selected archive entry fails with `RES entry is not an EH type-1 raster`, the utility preserves the source `INIT` base, signed level selector, selected raw header and original candidate inventory **before returning an error**. This addresses the user report `RLOFT-Level22-RES4-Ground-24033.txt`, whose 65-byte old output contained no inventory.

No actual original user `RES4` or `INIT` file was available inside Windows CI, so this does **not** establish that Level 22's selected resource decodes or that its display matches original DOS pixels. It is a successful diagnostic build, not native floor graphical acceptance.

## Important subsequent change after checkout

An independent source investigation committed an original on-disk **EH6 compressed-block → runtime type-1** loader path in `DemoV1/src/original_ground_res4_audit.cpp` **after** this build checked out source. Original `2ECB4` and `2F29C` behavior means source runtime-type-1 acceptance is **not** identical to a hard on-disk EH1 requirement. This is a promising explanation for the user's selection error, but the EH6 code change is **NOT built or covered by #655**, and its actual installed-media selection has not been validated by this run. Check its subsequent regression and Windows CI separately; never use #655 as proof of the newer EH6 code.

## Immediate manual source-data gate

From the **#655** Windows ZIP on the run page, extract a fresh copy of `rloft_res4_ground_probe.exe` and `Probe-Level22-Ground.cmd`, keeping them in the same folder. Run `Probe-Level22-Ground.cmd` (no demo EXE required) against the same original GOG/Steam media. Upload the **new** `%TEMP%\\RLOFT-Level22-RES4-Ground-xxxxx.txt` file. It should include selected entry type and a candidate list even if it returns an error. Its raw data will distinguish the selected original record and confirm the exact path to the EH6 ground-code correction.

All original-game assets remain unchanged. Native DOS pixel/timing and full Level22 acceptance stay open. Do not raise subsystem readiness percentages from CI-only success. Manual CI authorization rules remain in effect.
