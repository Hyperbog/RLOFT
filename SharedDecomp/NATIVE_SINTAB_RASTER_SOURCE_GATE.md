# Independent SINTAB / EH7 raster-plan source gate

This is the next safe renderer pipeline stage after the **4,234/4,234 original RES3 EH7 decode gate**.

## Newly implemented NativeEngine source

The independent NativeEngine implementation `native_sintab_raster.hpp/.cpp` parses **all 127 original SINTAB scale blocks** as original-data-owned 129-word patch-entry and resampling-step tables plus a bounded read-only generated x86 byte stream. It *validates* the six original instruction forms but **does not call the original x86**. It translates original EH7 entry/stop selectors into code instruction boundaries and provides a deliberately limited interpreter for the validated opcode subset:

- `8A 86` source byte read (32-bit displacement, original 128-byte source-column domain);
- `88 C4` copy AL to AH;
- `29 C8` wrapping 32-bit EAX minus ECX;
- `88 87` and `88 A7` AL/AH indexed stores at original 80-byte VGA-row offsets;
- `C3` terminator, not executed on the CPU.

The new `NativeIndexedVgaPlanes` has four separate 64-KiB planes; writes honor an explicit 4-bit VGA sequencer mask and can be inspected by original pixel/plane indices. `draw_native_eh7_span` preflights all read/write bounds and writes only after the entire descriptor succeeds, so a late corrupt input cannot partially modify the output. The patch-stop location is an **exclusive execution boundary**, not an in-memory executable-payload mutation.

**Limitations:** The original DOS `369D8` / `36B14` call setup, exact projected view, original texture/palette sourcing, animated frame selection and original regenerated VGA call grouping are not integrated. The new interpreter is a correct bounded host primitive for an explicitly supplied source, offset, ECX lookup word, destination, and mask—**not** a claim of faithful full-frame output.

The native source was committed in `native-engine` at `6521b089a362081d0ec06986d7467a236dee7699`. Native source-only CI was triggered but GitHub assigned **no runner** (runner_id=0; jobs failed before any step, including on retry). No successful current-C++ compile or 21/21 CTest result is being asserted yet.

## Source-grounded original SINTAB and RES3 offline verification

User-supplied original `SINTAB` and `RES3` were parsed independently on a local source-only Python implementation, without executing x86. Authentic SHA-256 identities:

```text
SINTAB 19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87
RES3   44955ebbed0a8ed8d874fae07b01710317044882ab3c7c95d9be54e883df9a36
```

| Independent original-file finding | Result |
|---|---:|
| All generated programs validated | 127/127 |
| SINTAB length-prefixed payload | 324,989 bytes |
| Strictly decoded generated x86 program body | 259,457 bytes |
| Source-read opcode instances | 11,587 |
| VGA destination-store opcode instances | 23,910 |
| RES3 EH7 source descriptors across referenced columns | 278,085 |
| Original descriptor-to-SINTAB resolutions at indices 0, 63, 126 | 834,255 |
| Nonempty selected code ranges across those three scales | 478,999 |
| Distinct original EH7 selector pairs | 7,854 |

The independently computed metadata and all **127 per-block** original SHA-256 values are in the local `rloft_original_sintab_safe_raster_reference.json`. The separate local source script `rloft_sintab_original_reference.py` generates that evidence without relying on the native program or shipping any original data bytes.

## Canonical reproducible cross-language acceptance check

`SharedDecomp/verify_native_sintab_raster.py` independently opens raw original `SINTAB` and `RES3`, parses every scale program and EH7 descriptor, and compares full compiled C++ audit output against all **127 scale block original payload hashes, code hashes, opcode counts, patch-boundary endpoints, 834,255 selector mappings, and the aggregate statistics**. Synthetic source-only unittest fixtures cover source corruption, missing hashes, unsupported frame parity claims, truncation and duplicate JSON keys. The original files must not be uploaded to CI.

Run after the NativeEngine source-only Linux compiler gate is restored:

```sh
rloft_native_sintab_raster_audit /private/original/install > /private/native-sintab.json
python3 SharedDecomp/verify_native_sintab_raster.py \
  --sintab /private/SINTAB --res3 /private/RES3 \
  --native-report /private/native-sintab.json \
  --out build/research/sintab-native-raster-diff.json
```

**Acceptance remains pending** until the actual committed new NativeEngine C++ source compiles, passes synthetic CTests and the resulting executable agrees with that independent original-file oracle. Windows/MSVC current-source demo, original DOS pixel/frame parity and a complete playable native engine are not certified. Windows release Actions remain manual-owner-controlled and DemoV1 gameplay remains unlinked.
