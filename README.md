# DOSDis — disassembler workstream of RLOFT

This branch owns the **multi-platform video game disassembly and recompilation workbench** and its independent-accuracy validation. Its source, build gates and original-comparison evidence remain under [`Tools/Disassembler/`](Tools/Disassembler/README.md); they are intentionally **not moved** to cross-project documentation.

The long-term scope covers systems from the NES era onward. **DOS, NES, and Sega Master System** are the first active platform targets. Current implemented decoding covers DOS/x86, the 151 documented NES 6502 opcodes and the supported SMS Z80 core/CB/ED families. Full instruction semantics, indexed Z80, mapper runtime states, lifting and round-trip proof remain pending. [Multi-platform architecture checkpoint](Tools/Disassembler/docs/MULTIPLATFORM_CORE_PLAN.md).

## Start here

- [DOSDis project README](Tools/Disassembler/README.md) — supported formats, .NET/WPF components, validation gates and usage
- [Current DOSDis checkpoint](Tools/Disassembler/docs/CURRENT_CHECKPOINT.md) and [continuous-improvement playbook](Tools/Disassembler/docs/CONTINUOUS_IMPROVEMENT_PLAYBOOK.md) — fastest evidence-backed resume path and accumulated defect-prevention lessons
- [Independent verification policy](Tools/Disassembler/docs/INDEPENDENT_VALIDATION_POLICY.md) and [G2 validation record](Tools/Disassembler/docs/G2_STRAHD_ORIGINAL_VALIDATION_RECORD_20261010.md)
- [Shared RLOFT project index](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/INDEX.md) — links to native game, executable reconstruction and other workstreams
- [Branch ownership](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/BRANCH_OWNERSHIP.md) and [shared CODE sync register](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/SHARED_SOURCE_REGISTER.md)

The locally mirrored original `Code1/` and `Code2/` reconstruction files are **references**, not the owner of DOSDis's compiler/decoder tests. The canonical branch now shares the original-byte-verified `F790` repair and accompanying `Code2/verify_f790_stack_local_preread.py` with the decompilation and native-game branches.

**Accuracy and build claims are scoped to an exact tested checkout.** Synthetic and bounded original-code comparisons do not certify complete control-flow discovery, game-runtime behavior or every 16-/32-bit instruction. Windows/.NET CI remains owner-authorized; housekeeping does not launch it.
