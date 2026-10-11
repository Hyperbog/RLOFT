# Watcom C 10.0a: authenticated owner archive and Windows-first compiler pilot

Canonical branch: code1-decompilation. Research date: 2026-10-11.
No gameplay source was changed. No Windows Full CI was dispatched.

## Actual archive verification

The owner-supplied wcc10.0a.tar.gz has exactly 9,247,235 bytes and SHA-256 920a9674d1b10f227ae65171c187212b06d7fc38b263313922615c4f5e9b24a6, matching the published decomp.me release.
Tar archive has 678 entries (one harmless relative root directory and 677 children) and 670 ordinary files. The safely extracted Windows 32-bit PE executable binnt/wcc386.exe hashes 8fee7d6ab3f2c06a0a874135e3898386898679e7dd53993fcc508e296364767d. Its OS/2 LX companion binb/wcc386.exe hashes c3666de94f6fa6800f452dae8acf45505ecdb62f0ade2cc27cc86c2d9e8e2b6b.

The prior archive provisioner initially rejected the genuine ./ directory entry. A fix now accepts precisely that root directory, checks relative canonical paths and duplicate names, and still rejects traversal paths, links and unsupported file types. The complete real archive extracted successfully after the fix.

The original game files were re-authenticated: CODE.1 SHA-256 fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed; CODE.2 SHA-256 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314.

## Why compiled Watcom output is still pending

The extracted historical Windows compiler launcher is a 32-bit PE executable. The companion compiler image is OS/2 LX. The current analysis container is Linux, with neither Wine/Wine32 nor an OS/2 execution environment installed, and its external download hosts cannot be resolved. Compiler archive authenticity does not imply compiler execution; no genuine Watcom OMF .obj or byte-exact compiler match has been measured yet.

## Four-profile Windows-only experiment runner

The new Code2/tools/run_watcom10_windows_probe.py verifies both extracted compiler executable hashes before invoking the Windows compiler. It compiles five source-only F0CC/F0C8/global-getter/callback hypotheses in Code2/tools/probes/watcom10_probe_candidates.c.

| Hypothesis | Watcom 10.0a options |
| --- | --- |
| 386 stack, no optimization | -bt=dos -3s -od -zq |
| 386 stack, time optimization | -bt=dos -3s -ot -zq |
| 386 register, time optimization | -bt=dos -3r -ot -zq |
| 486 stack, time optimization | -bt=dos -4s -ot -zq |

Options, compiler status, diagnostics, object hash, and available OMF inventory are written to a private output directory. This is not a linker or a compiler-match calculator. Unsupported OMF structures and compile errors remain explicit outcomes, not invented success. No original game binary or raw disassembly is included in the experiment source.

## Windows developer reproduction

Use Python 3 and the verified archive in a private directory. Output and extracted directory paths must not already exist.

```powershell
py -3 Code2/tools/provision_watcom10.py "C:\Downloads\wcc10.0a.tar.gz" --extract-to "C:\RLOFT-private\watcom10a"
py -3 Code2/tools/run_watcom10_windows_probe.py --watcom-root "C:\RLOFT-private\watcom10a" --output "C:\RLOFT-private\watcom-probe-01"
```

After execution, retain report.json and generated watcom_candidates.obj files or their omf_inventory.json files for independently verifying actual Watcom SEGDEF, PUBDEF, LEDATA, and FIXUPP32 behavior. The compiler-generated objects contain only newly authored probe hypotheses. No compiler executable, original CODE.2 data or game-owned disassembly should be uploaded publicly.

## Verification and acceptance

36 local tests passed: 30 existing original-source/tool tests, 2 new genuine-archive-layout/safety tests and 4 Windows-runner tests with controlled mock process execution. Real archive verification, safe extraction and actual compiler binary SHA checks passed. This is one compiler archive authenticated but ZERO Watcom-compiled object experiments executed. A real Windows run is required for further matching. Windows Full CI and native gameplay parity remain unrun and unaccepted.

References: https://github.com/decompme/compilers ; https://github.com/encounter/objdiff/issues/97
