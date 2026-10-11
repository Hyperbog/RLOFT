# Historical Watcom OMF pilot — 2026-10-11

**Branch:** `code1-decompilation`. No native game code or `demo-v1` gameplay behavior is modified.

## What was implemented

The offline `Code2/tools/inspect_watcom_omf.py` inventories the OMF/OMF32 format used by historical DOS compilers. It validates THEADR, LNAMES, SEGDEF, PUBDEF, EXTDEF, LEDATA, FIXUPP/FIXUPP32, MODEND, record checksums, symbol indexes, enumerated-data bounds, thread references and relocation-site width. Its output is metadata-only JSON: module hash, segment/symbol names, fixup location and target descriptions, and explicitly unparsed record features.

**It does not apply linker fixups or claim an original-code match.** GRPDEF, LIDATA, COMDAT, and less common records need additional proof before linked code can be reconstructed. The importer has been tested with synthetic OMF32 fixtures, **not yet against a real Watcom output object**.

`Code2/tools/provision_watcom10.py` independently verifies a local Watcom C 10.0a release archive before optional extraction, using its published 9,247,235-byte size and SHA-256:

`920a9674d1b10f227ae65171c187212b06d7fc38b263313922615c4f5e9b24a6`

Official download: https://github.com/decompme/compilers/releases/download/compilers/wcc10.0a.tar.gz

The extractor rejects traversal paths, symlinks, special entries, filename collisions, oversized archives, and existing output destinations. It never downloads or executes compiler binaries, and may reject a genuine archive containing unsupported layout/features. No unverified compiler is silently trusted.

## Commands for the Windows developer workstation

```powershell
python -m unittest discover -s Code2/tools -p "test_*.py"
python Code2/tools/provision_watcom10.py "C:\Downloads\wcc10.0a.tar.gz"
python Code2/tools/provision_watcom10.py "C:\Downloads\wcc10.0a.tar.gz" --extract-to "$env:TEMP\rloft-watcom-10.0a"
python Code2/tools/inspect_watcom_omf.py "C:\Private\first_watcom_probe.obj" --output "$env:TEMP\rloft-omf-inventory.json"
```

These are project development tools, not a command-line edition of the native Windows desktop game.

## Independent tests and source identity

**30 local unit tests passed** with the uploaded exact original `CODE.2`: 11 earlier function-packet/ELF controls, 12 new OMF fixture tests, and 7 new archive extraction tests. The authentic `CODE.2` source is 558,689 bytes with SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; executable object-1 is 239,698 bytes and hashes `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

**Watcom 10.0a has not been downloaded or executed in this container:** outbound download DNS is unavailable and no Watcom or DOS runtime is installed. The external published archive's metadata was checked, but its bytes have not been verified here. No OMF comparison has been run against actual historical compiler output and **no compiler-exact Watcom match is claimed**.

## Next acceptance gate

Provide an independently downloaded official `wcc10.0a.tar.gz` in a private Windows workspace, validate its checksum, identify and hash `wcc386`, and compile the five original-source probe hypotheses. Inspect the first generated OMF object with this tool. Only after validating its actual SEGDEF/PUBDEF/LEDATA/FIXUPP methods should we implement relocation-aware byte matching against the original LE executable. Compiler-exact matches, source-semantic fidelity, integrated Windows engine behavior and original DOS parity remain separate measures.

References: [IBM/OS2 OMF specification](https://komh.github.io/os2books/os2tk45/omfref.htm); [objdiff OMF support request](https://github.com/encounter/objdiff/issues/97); [decomp.me compiler catalogue](https://github.com/decompme/compilers/blob/main/values.yaml).

No Windows Full CI or gameplay build was dispatched.
