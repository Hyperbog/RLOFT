# Cross-game CODE.2 CHARS and resource handoff — Phase 6

## Scope and provenance

This unit reconstructs instruction-verified segments of the **retail**
Menzoberranzan and Stone Prophet CODE.2 resource-initialization functions.
Original filenames and executable offsets are from the owner-provided retail
files under `DnD-Classics/MASTER` (both CODE.2 originals independently
SHA-256 matched their inventories in earlier phases). All addresses below are
LE code-object-relative. We independently checked **39 out of 39** new
`CALL rel32` / branch / store witnesses against the original CODE.2 bytes
retrieved directly from that branch on November 13, 2026.

The source modules **do not execute** the original x86 machine code. They
translate only the verified control flow and delegate all still-unconverted
callees to `OriginalResourceStartupPort`. This is not yet a playable Menzo or
Stone Prophet game and not a verified gameplay implementation.

## Confirmed CHARS branches

| Title | Previous Phase 4 result | Source covered here | First unconverted next stage |
|---|---|---|---|
| Menzo | `0x23977` | `0x23977..0x23A20` | `0x204DC` at `0x23A22` |
| Stone | `0x2D4B5` absent / `0x2D4E6` present | through `0x2D54D` | conditional `0x46600` or `0x4191C` |

- **Menzo:** performs two resource-buffer preparation passes, calls original
  `0x37248("CHARS", 0)`, preserves its full handle, loads the `"t"` setup
  value, then selects `0x1DF34` when CHARS is absent or
  `0x1B68C -> 0x23790` when present. No title-agnostic party record
  interpretation is assumed.
- **Stone Prophet:** Phase 4's `0x43338("CHARS", 0)` return is now
  preserved as a **32-bit handle**, not collapsed to a boolean. When
  missing, `0x3C914(3)` and the resource-buffer setup execute. When
  present, `0x3C914(0x83)` executes instead. The shared tail writes
  `0x24E54 = 1`, calls the original configuration and setup functions,
  and conditionally calls three further functions depending on the
  return of `0x24F14`. Its next UI action is `0x46600` if CHARS was
  absent or `0x4191C` when present; neither is implemented here.

## Native media boundary

`OriginalNativeMediaSession` takes a **game identity**, an injected original
resource reader, and a separate transient CHARS reader. Its `res(n)` method
uses the existing `load_shared_original_res`, validates the original
per-title RES0 count (Strahd **141**, Menzo **175**, Stone **174**) and
caches original archive bytes. They can be passed to the engine's existing
`ResArchive` parser. Neither raw copyrighted game data nor a second resource
format implementation is embedded into the source. The `chars()` method
reads on demand, without caching absence, because CODE.1 can create CHARS
between calls. It never applies the Strahd-specific `0x920`-byte party codec
to the other games: their CHARS records require independent verification.

The existing Strahd `RavenloftBootAssets` uses the same shared RES loader
from Phase 5. This new native session is now part of the **same engine
library**, but it is **not yet invoked by Menzo/Stone native gameplay**.

## Verification and limits

- Executable-derived checks: Menzo **19/19** and Stone **20/20** passed.
- Native source regression added for both present/absent CHARS paths,
  secondary Stone initialization, media caching, and wrong-title RES0
  rejection. It is registered as a CTest in `Enhanced/CMakeLists.txt`.
- Compilation of the new test harness encountered two diagnostic failures
  before test execution: a braced initializer-list deduction error, then
  `-Werror=range-loop-construct` on a copied element. The implementation
  was corrected to use typed `std::array` iteration by `const auto&`, but
  **the corrected source was not recompiled** under the user's two-retry
  limit. Therefore local compilation and complete Windows CI remain
  **unverified**; do not claim a passing CTest or a playable game.

## Next bounded implementation

Translate Menzo `0x204DC` and Stone conditional `0x46600/0x4191C`
from the original machine code. Determine actual Menzo/Stone CHARS record
sizes/fields from their writers and readers before enabling party decoding.
Only then wire each title's character handoff and first interactive scene
into the one native Windows player.
