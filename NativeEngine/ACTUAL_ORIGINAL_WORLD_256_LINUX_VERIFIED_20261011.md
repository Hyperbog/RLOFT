# Actual NativeEngine whole-world byte-preservation verification — Linux CI + local original files

Date: 2026-10-11. **CONTROLLING compiled Linux milestone; this is not native gameplay or current Windows/MSVC validation.**

## Exact source, build and regression evidence

- Native source at **`4676ebf50668480512861c9fd981d6e36402da82`**
- Linux GitHub Actions source-only [#38185185120](https://github.com/Hyperbog/RLOFT/actions/runs/38185185120): **16/16 CTests pass**, including `native_bank_system_tests`, `native_world_system_tests`, `native_world_payload_tests`, renderer contract fixture and all resource tests
- Build: CMake Debug, GCC, Ubuntu 24.04, original-media-free
- Original asset-free Linux binary SHA-256 from the final successful workflow:
  - `rloft_native_payload_audit`: `f2596a799ebf664511128e881a3b3670e70bf96faf0c935e539dc533ca1664a6`
  - `rloft_native_world_audit`: `50a84b719759dd505edc1bde8ec8cde0cab96778a83a437c94f612ada7d000be`
- The earlier Linux run failed because `verify_image()` in `native_bank_system.cpp` had no return from the successful EH2 font branch. GDB confirmed SIGILL at the closing brace. The fix explicitly `return false;` (non-inert) and does not loosen validation.

## Actual game-file differential verification, performed locally

GitHub-built Linux binaries were executed against copies of the *actual owner-provided*:

- `LEVELS.DAT` SHA-256 `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`
- `SINTAB` SHA-256 `19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87`

Only for satisfying `NativeMedia::open()`'s original-install **signature check**, the temporary *local* input directory also included empty synthetic `INIT`, `RES0` and `T` files. Neither audit under test loaded these three files' contents: each requested `LEVELS.DAT`, or `LEVELS.DAT` plus `SINTAB`, directly. Accordingly this verifies the genuine level/SINTAB **data decoders** and **not** a complete game install.

Native output:

- `rloft_native_payload_audit`: schema `rloft-native-world-payload-fingerprints/v1`, `decoded_section_roundtrip_equal=true`, 32/32 levels, 115,584 cells, 1,659 control records, 555 commands.
- `rloft_native_world_audit`: schema `rloft-native-world-system/v1`, 32 levels, 115,584 cells, 1,659 controls, 394 active event slots / 555 commands, 127 SINTAB scale blocks, 324,989 generated-code bytes.
- An **independent Python decoder** of original offset directories, control spans, script tables, three compact eleven-byte families, automaps and annotations compared all **32 × 8 = 256** SHA-256 digests to NativeEngine's *actual* compiled C++ output, not synthetic native-shaped JSON. Result: **256/256 EXACT MATCH**.
- Five section categories per level were additionally confirmed against the earlier independently generated actual-original research report (160 SHA entries). The other 96 entries are original compact-family source-byte spans, independently hashed in the new comparison.
- Complete original-file SHA-256 was additionally matched between the native tool and Python oracle. Section digests were not generated from one implementation and copied into the other.

Locally saved metadata-only JSON report: `rloft_native_original_sections_256_verified.json`. Original game payloads were neither distributed in any artifact nor committed to Git.

## What is and is not certified

**Verified:** The exact compiled Linux NativeEngine C++ implementation decodes and preserves all 32 original level records, has a byte-exact section round-trip for cells, 10-byte controls, full event slots, automap, annotations and all A/B/C families. The separate native world/SINTAB structural reader succeeds on original files.

**Not verified:** native GUI on current source under MSVC Windows; correctness of original DOS graphics execution, collision, SINTAB shading, playable first-person scene, dynamic AI/events, save/load, sound or user field acceptance. Windows Actions remain **manual only**, and source-only Linux CI neither checks out original media nor publishes a game/release artifact.

The next integrated workstream is persistent native level/actor state and first-person scene renderer semantics, with independent reference/host replay using the shared `code1-decompilation/ResearchInfra` and `SharedDecomp` evidence contract. Do not use `demo-v1` as the independent native engine's gameplay runtime.
