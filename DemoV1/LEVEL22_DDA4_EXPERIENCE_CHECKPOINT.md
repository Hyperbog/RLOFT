# CODE.2 DDA4 XP source checkpoint (2026-10-08)

Status: source-only, no live XP mutation or Windows build.

Original CODE.2 SHA-256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

At logical DDA4 (virtual disassembly address 1DDA4), NPC definition
WORD +23 is shared across eligible party members. The original DDAF
status test uses WORD +10 & F240 == 0. DDD0 calculates the rounded
party share (award + N/2)/N, minimum one; zero qualifying members
causes an unsupported divide-by-zero. DE27 counts up to three
class tracks based on signed class bytes +16 and +17, with the first
track always counted. DE80 rounds a second time for each class track.
DEC2 writes DWORD class experience at +26 + 4*track with 32-bit wrap.

The new read-only CombatXpDistributionModel projects this arithmetic
from a PartyImage without mutating it. It does not run original
level threshold scans, level-up services or derived attack-target
recomputation. Registered C++ tests cover rounding, class tracks,
status masks and overflow; source remains unbuilt on current Windows HEAD.

Twenty-five new original opcode signatures were checked, bringing
the verifier to 318 instruction anchors plus one Strength data anchor.
A standalone equivalent C++17 model passed local Debug and Release
assertion tests, which are not full-repository build evidence.

Original type-9 E284 death ordering: four item drops, BF44 story write,
28110 presentation, three no-match tracker comparisons, corpse bit,
then DDA4 award service. The supplied Level 22 tracker is (-1,-1,-1).

Overall five-gate demo acceptance stays 20%. GitHub Actions remain
manual-only while private.
