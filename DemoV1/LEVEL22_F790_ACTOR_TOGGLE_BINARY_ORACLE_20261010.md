# CODE.2 11AD8 actor-pane mode / 11B63 F790 executable oracle — 2026-10-10

**Branch:** `demo-v1`. **Scope:** faithful native reimplementation, source verification only. This is not a Windows UI integration or DOS parity claim.

## Authoritative evidence

The immutable original user-supplied `CODE.2` has 558,689 bytes and SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; executable object 1 begins at physical `0x3AF24`. `tools/verify_original_f790_actor_toggle.py` independently passed **13 original instruction windows and eight E8-rel32 targets** against those bytes. Only small necessary instruction signatures are committed, never the original game executable.

## Original input-flag lineage (additional original-byte verification)

The raw flag bit is not a UI-specific invention. Original CODE.2 `22CE0` takes the caller's input flags, and at `22D49..22D53` stores their **low 16 bits** to the input-event ring entry `A640+6*index`. At `22DC1`, the consumer **sign-extends** that stored WORD to `ECX`; at `22E1D..22E20` it pushes `ECX` as the third argument of `12E70` only under modal state `D680=1`. The input/actor procedure at `12F8A` forwards that argument to `11AD8`. The new verifier independently asserts these three upstream instruction windows and the `22E20→12E70` E8 target, in addition to the prior actor toggle checks.

This establishes **bit-2 preservation along the original ring-buffer route**, not its external mouse-button/key meaning. That origin still requires separate caller and DOS event testing. In particular, the ring stores a truncated WORD and the consumer sign-extends it; it does not preserve arbitrary 32-bit input flag values unchanged.

The exact source behavior is **not** a generic repaint:

- `12F76..12F8F` accepts the inclusive actor-local rectangle row `0x45..0x60`, column `0x39..0x4D`, and passes the original input flags plus actor index to `11AD8`.
- `11AE9` tests flag bit `0x02`. Without it, `11B30` toggles the low bit of selected actor's byte at `6F90 + 4*actor`; `11B4B` redraws that actor through `11268`.
- With bit `0x02`, `11AF0` loads the selected actor's **whole DWORD** at `6F90 + 4*actor`, `11B02` toggles bit zero of its low WORD **once**, and `11B0E..11B2E` writes the resulting DWORD to **all four** actor contexts and redraws each via `11268`. The original `EB DE` loop jumps back to `11B0E`, never to the XOR.
- After either redraw path, `11B53..11B63` passes the sign-extended global held WORD `8600` and zero-extended known BYTE `8602` to **one original F790 call**, then tests bit `0x02` again.
- The bit-2 follow-on is **not** finished with the redraw: `11B72..11BB7` reads held/known, conditionally changes `BF44` for held IDs `0xA7..0xC9`, calls `1A5B0` at `11BA8` using `C050`, and flips `C050` bit zero. Faithful live integration must preserve those subsequent effects too; do not substitute a guessed mouse modifier or omit that branch.

The original `F790` pending-text and shared RNG ownership remain governed by the previously audited source. Screen repaints at the host's 70-Hz cadence must never be treated as source `11B63` events.

## Native checkpoint and automated gates

Existing `PartyPanelControlModel::modal_toggle_actor_at` already reflects the source inclusive rectangle; existing `OriginalF790CallerCatalog` correctly classifies `11B63` as global-held argument provenance. The Windows host currently exposes only an individual-party toggle path, without independently proved DOS input-bit-2 forwarding or the source-owned F790 renderer. It **must not** be called fully integrated.

Added `DemoV1/tools/verify_original_f790_actor_toggle.py`, registered `original_f790_actor_toggle_binary_contract_tests` in CMake (source-only by default), and added both modes to the on-demand `run_f790_source_feedback_loop.py` orchestrator. The binary check passed locally:

```sh
python DemoV1/tools/verify_original_f790_actor_toggle.py --original-code2 /path/to/CODE.2 --binary-only
```

The CTest registry is now **211** when Python is available, but the new test is not Windows-built or Windows-tested. Last previously accepted Windows Full was #651 (182/182 on older code). Runs #652 and #653 did not pass full acceptance; #654 was already user-authorized on an older source snapshot and was still running at this checkpoint. No additional build was dispatched. No DOS comparison, Level22 completion, readiness-percentage increase, or Level23 independent implementation is asserted.

**Next:** identify the actual `12E70` input-flag producer and faithfully connect the full `11AD8` branch, including post-F790 side effects, one event and one shared game RNG; then run original-DOS indexed-image, timing and interaction comparison.
