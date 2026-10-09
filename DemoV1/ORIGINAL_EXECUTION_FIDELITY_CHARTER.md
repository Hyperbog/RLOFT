# Ravenloft: Strahd's Possession — Original-Execution Fidelity Charter

**Project policy:** binding requirement from the project owner; takes precedence over demo convenience, implementation shortcuts, and modernization proposals.  
**Authority:** original shipped DOS executable behavior and original game assets, with provenance by game version.  
**Sequence:** **Phase I: 100% original execution and fidelity first. Phase II: separate modern port only after Phase I is accepted.**

## Phase I — preserve and reproduce the original game exactly

The primary objective is complete **original** *Ravenloft: Strahd's Possession* execution and behavior, from the first boot through character creation, all level traversal, events, encounters, inventory, spells, combat, effects, cinematics, credits/endgame, and original saved-game behavior. The aim is **100%** and there is no permission to silently redefine the target as a merely playable, improved, approximate or visually similar game.

1. **No gameplay redesign or intentional correction.** Preserve original rules, bugs, limitations, edge cases, overflow/wraparound, rounding, combat formulae, input handling, script order, object identity, procedural quirks, RNG, save and memory-layout behavior. Report contradictions between original versions explicitly instead of selecting a preferred modern behavior.
2. **Exact visual and audiovisual reference.** Compare indexed pixels, original palettes, resolution/aspect presentation, sprite art, clipping, text, font, animation frames, timing, transitions, music/SFX routing, sequencing, synchronization, game-clock and IRQ-dependent behavior against original DOS evidence. A visually acceptable approximation is not certification.
3. **Preserve content and execution provenance.** Original CODE.1/CODE.2/EXE/BAT/DLL bytes and DAT/RES/ANM/source media are the reference. For each reconstructed routine or workflow, identify original offsets, file/version/hashes where available, exact original preconditions and side effects, plus native and original-DOS replay evidence. Never fabricate level destinations, dialogue, assets, cinematics, triggers, drops, rewards, character state or alternate gameplay semantics.
4. **One continuous original game.** Its complete startup, party creation/load, level visits, event graph, original transitions, same-game cinematics, save/load and ending must work in the game's defined order with source-authentic persistent world/party/timer state.
5. **Do not conflate fidelity with compilation.** Reverse engineered source, generated C++, authored unit tests, CTest pass and a native EXE establish *different* evidence from actual original instruction execution or verified behavioral equivalence. A reconstructed C++ implementation does not literally run the original DOS machine instructions. Clearly label it **reconstruction/test harness** until the original full-runtime equivalence is demonstrated; never count it as a certified original execution merely because it is source-informed or automated tests pass.
6. **Preserve original releases.** Keep original assets and executable files immutable, and record differences between original release/patch/media versions. Do not patch the archival baseline to make the modern demo easier to implement.
7. **Gate every claimed completion.** An individual system can be called **100% original fidelity** only with original executable/source evidence, exact relevant state/control/pixel/timing tests, and verified end-to-end context. The entire original game is not certified until **every** system, level, event and beginning-to-ending path is proven; explicitly track unknowns, known mismatches and pending DOS comparisons.
8. **Correct course when fidelity diverges.** If an existing modern helper, approximation, protective recovery path, guessed value or convenience transition differs from original behavior, keep it isolated as provisional developer tooling and replace/disable it in the faithful baseline after verifying the original. Do not promote modern safety fixes as authentic game behavior.
9. **Development discipline.** Commit and push every code, evidence, test and continuity checkpoint to Git. Builds and GitHub Actions remain manual and user-authorized. Report exact build commit and tests; do not infer successful behavior from an unbuilt revision.

### Role of the existing DemoV1 native C++ work

The ongoing `demo-v1` Windows implementation, testbed, diagnostics, source-backed models and general level-state helper are **valuable research and verification infrastructure**, but they do not by themselves establish 100% **original execution**. In particular, the Part 9 `transition_to_level` API is a provisional bridge without evidence of original event exits, precise state lifecycle or DOS acceptance. Preserve the work and avoid inventing original-game behavior; do not confuse engineering readiness percentages (currently separately tracked) with original execution completion.

Where literal original instruction execution is required, use the preserved authentic original DOS binaries and an appropriate validated execution environment; where reconstructed source or native code is used, prove output equivalence instead of representing its instruction stream as original.

## Phase II — modern port, only after original certification

**Only after complete Phase I acceptance** create a **separate branch** for a modern port with newly written/adapted code, bug fixes, enhanced graphics or interface, accessibility and quality-of-life changes, extensibility, modding APIs/tools, and optional gameplay improvements.

- Maintain a separate immutable/reference original-fidelity baseline, not a moving mixture of original and modern behaviors.
- All improvements must be explicitly labeled modern changes with their own tests and acceptance criteria.
- Do not backport modern gameplay changes or altered assets/semantics into the original-fidelity branch.
- Original-preservation tooling may continue during Phase I for research, comparison and inspection. Such tooling is not permission to modernize original gameplay.

## Acceptance and reporting language

Report **(a)** original behavior proven, **(b)** reconstructed implementation done, **(c)** compiled/tested, **(d)** observed native-vs-original parity, and **(e)** remaining uncertainty separately. Percentage-based subsystem readiness remains an engineering diagnostic and **must not be presented as 100% original-execution completion** without original-runtime evidence.

This charter governs future Part 9 and later development and supersedes any earlier note that treats native demo feature completion as permission to modernize the original game before full fidelity.
