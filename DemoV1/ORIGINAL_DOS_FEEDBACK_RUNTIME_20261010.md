# Original DOS feedback runtime — native source-backed adapters, 2026-10-10

**Authentic source:** uploaded Strahd CODE.2, 558,689 bytes, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Object-1 starts at physical file `0x3AF24`. Byte-based
`verify_original_dos_feedback_runtime.py` independently validates
complete source spans `30AEC..30B18` and `311E8..31217`,
the original `2F868..2F87B` callback prelude, **16 opcode windows**,
and two exact calls `30AF5→2F868`, `2F827→311E8`.
Verified locally against the authenticated original file.
No original game binary is included in Git.

## Original input polling

`30AEC` checks live BYTE `F572` before invoking any draw service.
While zero, it repeatedly calls `2F868`, which conditionally calls
the installed DWORD pointer `F0C8` (when nonzero), then rechecks
`F572`. When nonzero it indexes DWORD array `F590`
using unsigned `BYTE[F572]` as a DWORD element index, returns
the result and **then** clears the original pending byte.
The routine does **not** contain a bounds check for that index.

`OriginalDosFeedbackRuntimeModel::poll_30aec` models those exact
observed accesses against mutable captured `OriginalDosInputState`.
The injected callback can change `F572` and the indexed DWORD
between polls. There is deliberately **no synthetic timeout**
or invented key mapping. The adapter rejects an unavailable draw
service when a wait is necessary and rejects an insufficiently
captured `F590` table *without clearing the pending byte*.
Those exceptions are host capture guards, **not original DOS
error semantics**.

## Original mutating DOS text output

`311E8` scans caller-owned bytes through the first NUL and changes
each embedded `'$'` to `'*'` in place. It then writes a
temporary `'$'` into the original terminating byte,
invokes DOS `INT 21h / AH=09` on the **same mutable bytes**,
and writes NUL back to that terminal byte. Internal
substitutions are permanent. The zero-length string still
invokes the DOS output service. Uncaptured backing without NUL
is rejected before mutation; this host safety guard must not
be mistaken for original DOS memory behavior.

`OriginalDosFeedbackRuntimeModel::print_311e8` accepts a
caller-owned mutable byte buffer and an injected DOS output
observer. Its tests cover multiple dollars, empty strings,
trailing captured bytes, final NUL restoration, exact callback
order, missing terminator rejection and original backing identity.

## Verification, fidelity gate, and M-PLAY-01

The standalone native C++17 test compiled and **passed locally**
under GCC 14 with `-Wall -Wextra -Wpedantic -Werror`.
The optional original-file Python oracle passed locally.
CMake registers a native CTest
`original_dos_feedback_runtime_tests` and an independent
**source-only** Python CTest
`original_dos_feedback_binary_contract_tests`.
A source-only green result does **not** prove the CODE.2 file was
available to CI.

The adapters are compiled into `rloft_boot_flow` but **not wired**
to the Windows normal-play input event pump or DOS text presenter.
Original CODE.1/CODE.2 calling integration, the installed F0C8
callback target, DOS interrupt timing, ES/DS aliases, and actual
player-visible output remain open. Consequently M-PLAY-01,
the current-revision Windows Full build and matched original-DOS
playthrough are **not accepted**. No native renderer changes,
invented callback target, CI run, or readiness-score increase
are asserted by this bounded implementation.

## Original CODE.2 keymap captured from installed assets

The original `CODE.2` is the controlling asset. The DGROUP `F590`
keymap occupies **89 little-endian DWORD entries**, original file byte
range `0x854B4..0x85617`, directly followed by the independent
`F6F4` RNG seed DWORD. Original 356-byte table SHA-256:
`fb179b786f755ff0efb0fbdc639ca738fcb6f74a05719103cdd08490b1fe024f`.
The original `30AEC` loads its index from unsigned BYTE `F572`,
then loads `F590[index]` as a DWORD, without bounds checking.

The native `capture_initial_f590_from_code2` loads these 89 DWORDs
from the **caller-supplied installed CODE.2 bytes** using the pinned
original layout, without hardcoding a translated keyboard map. It also
captures the initial `F572` byte. The loader rejects different
file sizes; it does **not** cryptographically authenticate every byte
of the supplied executable. The accompanying optional original-file
Python verifier authenticates the full CODE.2 SHA and complete table.
The native source-only test exercises all 89 decoded entries, byte
order, initial key state, and unsupported file-size rejection.

This is a real native installation-media decoding seam, **not wired**
to Win32 keyboard scan input yet. Original 307xx hardware scan/IRQ
semantics, live keyboard layout, DOS selector handling and modded
CODE.2 variants must be resolved before normal-play event routing.
