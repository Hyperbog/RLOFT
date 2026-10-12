# Shared optional REA/Ghidra analysis for all RLOFT projects

**Owner:** `code1-decompilation:ResearchInfra/`. **Consumers:** CODE.1,
CODE.2, Recomp, DemoV1, NativeEngine, DOSDis, asset extraction, DAT/RES
research and other authorized projects. This is a cross-project *analysis*
capability, **not** a shared game runtime dependency. The originals branch
`DnD-Classics` remains read-only.

Upstream: [morluto/rea](https://github.com/morluto/rea), MIT-licensed.
Audited released package: **rea-agents 6.4.0** (2026-10-11). Its
[documentation](https://github.com/morluto/rea/blob/main/README.md)
describes local CLI + MCP workflows backed by a separately installed
Hopper/Ghidra/IDA provider. We use **read-only static Ghidra** for x86.
The bridge `ResearchInfra/rea_bridge.py` has no external Python dependencies.

## Integration versus installation

The **integration is repository-ready**: a deterministic CLI adapter,
original-file identity check, private LE code exporter, SHA-bound analysis
plans and optional local REA invocation. The CLI can operate on targets in
any checked-out owning branch, or even an external project, by running the
one copy in the canonical checkout with an absolute `--target` path.

**REA itself is not bundled or globally installed** by this change.
The analyst chooses if/where to install Node, REA, Ghidra and/or register
the MCP server. The bridge makes no network calls, executes no original
game program, and never silently downloads packages.

REA upstream specifies Node.js 22.19+ (or 24.11+/26+). Static native
analysis also needs supported Ghidra (currently 12.1.x) and an appropriate
JDK (21 verified). To install on a compatible workstation, separately
and only when wanted:

```sh
npm install --global rea-agents@6.4.0
rea doctor --provider ghidra --json
rea providers --json
```

To configure an agent for *interactive MCP* usage, **review the proposed
configuration and consent to changes** before invoking
`npx rea-agents@6.4.0 setup`. Standalone `rea` CLI use does not require
any MCP registration. REA is optional for projects; no build must fail
because an analyst does not have Ghidra.

## Workflow A — general project-owned binaries

Run against a locally accessible artifact on any branch or another owned
project, without duplicating or merging `ResearchInfra` into it.
From the canonical repository root:

```sh
python3 ResearchInfra/rea_bridge.py plan \
  --project disassembler-foundation \
  --target /absolute/path/to/owned/native/module.exe \
  --format auto \
  --out .rloft-private/rea/dosdis.plan.json
```

Then, **only on a workstation where the appropriate REA native provider
is installed**:

```sh
python3 ResearchInfra/rea_bridge.py run \
  --plan .rloft-private/rea/dosdis.plan.json \
  --out .rloft-private/rea/dosdis.inspect.json
```

Both commands keep output in the ignored
`.rloft-private/` directory. `plan` does not run anything: it hashes
the source, selects the exact format, records the project, and outputs a
candidate read-only `rea inspect --provider ghidra --json` command.
`run` refuses a stale/modified plan or changed source and does not
invoke `npx`, download REA, run target application code, or use
software not explicitly installed. It stores potentially sensitive
decompiler content **locally**; its console status omits the analysis.

For targets stored in a *separate checkout*, use the absolute binary path
above and leave the analysis plan in this **canonical ignored checkout**.
An analyst may instead pass `--checkout /absolute/other/checkout` with
an output below that checkout's `.rloft-private/`, **only** if its
`.gitignore` already explicitly excludes that directory.
`--project external-owned-project` permits other authorized games or
applications without adding their proprietary files to this repository.

The `auto` format delegates binary classification to REA. For raw x86
blobs, explicitly specify
`--format raw-binary --architecture x86 --entry-point 0x10000`.
The entry value is the address at which **file offset zero is loaded**,
not an arbitrary code address.

## Workflow B — authenticated CODE.1/CODE.2 LE code

REA's DOS MZ importer **explicitly rejects LE/LX/NE** and its Windows
experimental Ghidra native P0 currently admits **PE** rather than DOS
or raw x86. Do **not** point its DOS importer at the DOS/4GW
protected-mode LE container. The [upstream LE/DOS limitation](https://github.com/morluto/rea/blob/main/docs/ghidra-dos.md)
and [raw format contract](https://github.com/morluto/rea/blob/main/docs/raw-binary.md)
are binding.

The repository's existing `ResearchInfra/original_cfg.py` authenticates
**both the complete original file hash and exact LE code object mapping**
before extraction. The REA bridge uses this existing parser rather than
introducing a second unaudited LE decoder. Example CODE.1 free-list
investigation, from the canonical repo root:

```sh
python3 ResearchInfra/rea_bridge.py prepare-le \
  --module code1 \
  --original /private/game/CODE.1 \
  --start 0xDB40 --length 0x350 \
  --output-dir .rloft-private/rea/code1
```

This produces **locally ignored** raw machine-code bytes and a plan
`code1-db40-350.plan.json`. The matching
`code1-db40-350.bin` spans original CODE.1 object offsets
`0xDB40..0xDE90`; REA maps raw file offset zero at **0x1DB40**.
This mapping preserves CODE.1's original object offset plus
the LE object's declared base `0x10000`; it is **not** real-mode
`segment:offset`. To inspect original `DB5B`, REA's mapped
address is `0x1DB5B` when the underlying provider recognizes it.

```sh
python3 ResearchInfra/rea_bridge.py run \
  --plan .rloft-private/rea/code1/code1-db40-350.plan.json \
  --out .rloft-private/rea/code1/db5b.inspect.json
```

Windows Ghidra P0 **will not** accept this raw x86 target. Run the
actual Ghidra provider on a supported Linux/macOS host and preserve
the resulting CLI output for independent review. This command only
verifies static import and inventory; deeper disassembly/decompilation
is a follow-up `rea function`, `rea instructions`, `rea decompile`,
`rea xrefs`, or `rea trace` task with exact mapped addresses.

### Evidence limits

- Extraction uses the **unaltered** original code-object image; LE
  relocation fixups and guest DGROUP state are **not applied** to the raw
  slice. Machine-code bytes alone are not an authenticated loaded image.
- A raw slice may cut across a control-flow target; run separate,
  overlapping windows with fully recorded original hashes to cross-check.
- The Ghidra processor/language profile is an *analysis choice*, not proof
  of the original Watcom compiler or DOS/4GW calling conventions.
- REA pseudocode is secondary evidence. Revalidate corrections with
  original bytes, CODE.1/CODE.2 source CTests, and, when available,
  **independently captured original-DOS traces**. Neither an REA success
  nor a CTest success changes `Recomp/ACCEPTANCE_GATES.json` automatically.
- The private `.bin` is a derived copy of proprietary game bytes:
  **never commit, attach to public issues, or upload to GitHub Actions**.
  The bridge produces no original-byte analysis artifacts in CI.

## Project usage and ownership

| Owner / workstream | Use REA for | What REA cannot certify |
| --- | --- | --- |
| `code1-decompilation` | CODE.1/CODE.2 authentic static x86 audits, LE window cross-references | DOS/4GW memory parity, full game boot |
| `disassembler-foundation` | Independent DOSDis candidate instruction/CFG comparison | That DOSDis is semantically identical to original execution |
| `demo-v1` | Compare own native binaries with bounded original x86 evidence | Original AI/graphics/movement fidelity |
| `native-engine` | Inspect locally built native modules, locate regressions | Original game runtime/gameplay acceptance |
| `extractor-wpf-fluent` | Inspect authored extraction tools (managed/native as provider permits) | Correctness of proprietary resources |
| `DnD-Classics` | **Read-only owner-local** analysis of preserved installed media | Permission to modify originals or distribute their bytes |
| `external-owned-project` | Inspect separately authorized binaries through same adapter | An assertion of rights or any RLOFT acceptance |

Use `--project` to set the ownership context and retain the original
binary's SHA256; **never merge all owning branches merely to share REA**.
Run the shared Python adapter from the canonical checkout; use a private
workspace for each analysis. In the project's status report, distinguish
`REA_STATIC_ANALYSIS`, `AUTHENTICATED_ORIGINAL_BYTE_CHECK`,
`RECONSTRUCTED_SOURCE_TEST`, and `ORIGINAL_DOS_OBSERVATION`.

## Verification

```sh
python3 -m unittest discover -s ResearchInfra -p 'test_rea_bridge.py' -v
python3 -m unittest discover -s ResearchInfra -p 'test_*.py' -v
```

CI executes only standard-library asset-free regression tests.
`test_rea_bridge.py` checks source SHA tampering, private-path
confinement, owner routing, exact raw address interpretation and
the explicit read-only invocation contract, with **mocked native REA**.
CI does not claim a real Ghidra engine invocation or original-game
decompilation.
