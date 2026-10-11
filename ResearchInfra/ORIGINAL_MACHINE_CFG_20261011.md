# Original x86-32 LE CFG + source cross-reference milestone — 2026-10-11

Owner: \`ResearchInfra/\` on \`code1-decompilation\`. The other workstreams consume source-backed evidence and retain independent acceptance. The canonical DOSDis app, native-game engine, standalone native engine and asset extractor were **not** rewritten to use this prototype.

## Reproducible input and source of authority

Locally supplied owner-original \`CODE.1\` and \`CODE.2\`, **not** Github-generated data, were read without modifying them:

| Original | Full bytes | SHA-256 | Code object (LE virtual) | LE entry |
| --- | ---: | --- | ---: | --- |
| \`CODE.1\` | 277,787 | \`fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed\` | 70,402 | \`0xCC1C\` |
| \`CODE.2\` | 558,689 | \`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314\` | 239,698 | \`0x353A0\` |

\`original_cfg.py\` verifies the **complete original file hash, nested MZ/LE headers, object dimensions, object base addresses, entry address, LE code-page mapping** and each individual manifest window's SHA-256 before accepting any decoded instruction. It materializes the original code object from physical LE pages, not a guessed flat file offset. The independently identified GNU binutils \`objdump\` decodes each bounded original instruction window as **x86 32-bit**. Output must partition the exact input bytes with no gaps or overlaps. The artifact contains file identities, offsets, block edges, and observed service operands; it does **not** reproduce original code bytes or game media.

The separate C++ original LE loader's complete 12,430 relocation checks remain authoritative to relocated data state. The CFG witness tool reads **raw original code bytes** and is not itself a relocation validator.

## Locally executed coverage

\`ResearchInfra/original_witnesses.json\` now pins **15 source-selected nonoverlapping instruction windows**, including two CODE.1 slices and 13 CODE.2 runtime slices. They cover original startup \`353A0→35418\`, DOS/environment setup \`35418..3562E\`, command parsing, stream initializers, saved-SS initializer, original callback scheduling and the environment initializer \`3A2AD..3A389\`.

| Candidate decoder observation | Value |
| --- | ---: |
| Original code bytes with pinned span SHA | 1,939 |
| Linear-decoded instructions | 737 |
| Statically traversed/reachable-in-window instructions | 667 |
| Candidate basic blocks | 165 |
| Local graph edges | 715 |
| Out-of-window direct transfers | 19 |
| Original direct CALL sites (inside or outside windows) | 18 |
| Unresolved indirect CALL sites | 2 |
| INT 21h/31h instruction sites | 7 |
| Observed boundary/decode conflicts in selected windows | **0** |

**These numbers are not global percentages, complete game reconstruction, DOS execution, or instruction correctness certification.** The decoder linearly sweeps each source-selected span, then walks candidate direct branches. Inline data, alternate entrypoints, indirect call targets, real interrupt effects and DS/SS selector semantics still require independent analysis. The 70 linearly decoded but not traversed instructions are not described as executed code. An external jump/call is reported as an out-of-window transfer, not assumed to return into a window. All two indirect calls are deliberately left unresolved.

### Original startup evidence produced by the new CFG importer

The independently decoded original startup \`35418..3562E\` contains **167 statically reached candidate instructions**, 32 basic blocks, **five** DOS/DPMI interrupt sites, and direct calls:

- \`35623 → 395E2\` — original initializer dispatch, known EAX=FF contract.
- \`3562A → 39589\` — post-initializer launcher.

The original scheduler \`395E2..3962C\` has an indirect \`CALL EAX\` at \`395FF\`, and its companion termination scheduler \`3962D..3967B\` also contains an indirect \`CALL EAX\`. A source-only C++ direct call graph cannot resolve these dynamic callbacks by itself. Other independently decoded direct calls anchor environment \`3A2AD\` to \`37B42\`, \`30D30\` and \`37C45\`. These are precise *follow-up dependency candidates*, not proofs that host adapters behave as DOS.

## Cross-project link and ABI gap analysis

\`compare_call_evidence.py\` reads the fresh, SHA-authenticated original machine CFG and canonical \`index.py\` source-symbol graph. It produces an **18-site original direct CALL overlay** and labels direct-target overlaps or divergences as *observations*. A machine-only direct call may be delegated through a C++ \`Runtime...Boundary\` callback; treating such an edge difference as a functional mismatch would be incorrect. Explicit missing source IDs are preserved, not silently synthesized.

\`original_context.py\` combines bounded original CFG evidence, verified-on-read reconstructed source excerpts/callers/callees, the original-vs-C++ call overlay, and **review lanes for all six primary project branches**. These lanes are **triage suggestions**, and every one declares \`confirmed_direct_dependency=false\`. The \`DnD-Classics\` lane is read-only. CLI execution rebuilds the original CFG from authenticated files; it does not trust a previously serialized JSON file to assert media identity.

## Commands

Requires GNU binutils \`objdump\` and Python 3.10+. Commands from repository root. \`/private/original/install\` is a user-local folder containing the exact CODE.1/CODE.2 version. Never commit the originals or generated media-bearing fixtures.

\`\`\`sh
python3 ResearchInfra/index.py build --root . --output build/research/index.json
python3 ResearchInfra/original_cfg.py --original-root /private/original/install --source-index build/research/index.json --output build/research/original_cfg.json
python3 ResearchInfra/compare_call_evidence.py --original-root /private/original/install --index build/research/index.json --output build/research/assembly_vs_cpp_calls.json
python3 ResearchInfra/original_context.py --root . --original-root /private/original/install --source-index build/research/index.json --key code2:35418 --output build/research/startup_original_work_packet.json
python3 -m unittest discover -s ResearchInfra -p 'test_*.py' -v
\`\`\`

The positive original-file path above was locally run for the complete 15-window CFG using authenticated owner-supplied media; the later machine-to-reconstructed-source join is also covered by synthetic source tests. CI only has synthetic fixtures and source, **not user original media**.

## Verified automation checkpoint

[GitHub Actions run #38182572768](https://github.com/Hyperbog/RLOFT/actions/runs/38182572768) is **green** at \`0212d57bb58394ba0f318af994b32a8ac91bd943\`. Its logs report **18/18 ResearchInfra unit tests, 52/52 shared decomp/trace tests**, and a successful 1,560-function / 961-edge canonical source index build. This runner does **not** certify the original-file optional path. The original-file results above were executed locally with uploaded CODE.1 and CODE.2. Windows native Full/disassembler GUI builds were not dispatched.

## Next priority

Expand the manifest only after verifying original function boundaries, import actual DOSDis candidate xrefs as separately attributed evidence, resolve original \`395E2\`/ \`3962D\` indirect targets through CODE.2 initializer table/ABI, and connect the reconstructed environment initializer \`3A2AD\` to the canonical recomp runtime with exact selector/memory behavior. Cross-project delivery should remain bounded, with original file/version and tested Git SHA pinned.
