# Part 24 — indirect palette reference audit and DOS DAC provenance boundary

**Owning branch:** `Recomp` · **Retail image:** CODE.2 SHA256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
**Evidence grade:** retail original physical bytes + bounded x86 interpretation, **not** an independently recorded DOS runtime call sequence.

## Authentic original palette output

The compiled original object-1 `0x29234` routine pushes byte count
`0x300` (768) and starting DAC index zero, then calls `0x29CA0`.
The latter writes the starting index using `OUT DX,AL` with
`DX=0x3C8`, then transfers indexed RGB6 components using
`REP OUTSB` with `DX=0x3C9`; it limits each retrace-controlled
chunk to `0xC0` (192) bytes. Relevant original instruction addresses:
`0x29CB6` (index write), `0x29CCF` (full chunks),
`0x29CE0` (remainder). These are original object-1-relative
addresses; an actual DOS protected-mode CPU EIP must be normalized
using the loaded LE object's runtime base before comparisons.

The [direct call census](tools/trace_original_palette_write_chain.py)
identifies 21 original callers of the full `0x29234` palette submitter
from the guest palette buffers `0xC054` (16), `0xC355` (2) and
`0xF9D0` (3), and 23 direct near-call candidates to the full-bank
`COLORS` file reader `0x325B0`.

## New bounded indirect-reference census

The new [static indirect-reference candidate audit](tools/audit_original_palette_indirect_candidates.py)
refuses changed-original SHA and tests exact raw `E8 rel32`,
`E9 rel32`, `EB rel8` and 32-bit literal-address encodings in
the authenticated original executable's object-1 file suffix.

| Function | E8 relative calls | E9/EB jumps | Exact 32-bit pointer markers |
| --- | ---: | ---: | ---: |
| `0x29248` partial DAC wrapper | 0 | 0 | 0 |
| `0x323BC` palette-range copy | 0 | 0 | 0 |
| `0x324F8` triplet rotation | 0 | 0 | 0 |

**Important:** these are *raw encoding* absence results, **not** a closed
control-flow graph. LE fixup-mediated targets, indirect branch
targets calculated at runtime, pointers encoded in other LE objects,
and code/data overlap have **not** been excluded.

Additional verified original call-site arguments:
- `0x05481` and `0x05A63` call `0x32468` to blend 6 colors
  starting at index `0xEC=236`: indices **236..241**, not 246..253.
- `0x0ECBF` calls `0x32404` to fill 111 colors starting at
  `0x90=144`: **144..254**. This touches indices 246..253 but is
  a color fill/transition, **not** source-bank-3 provenance.
- `0x324DE` invokes a separate full 256-color fill/clear.

The preserved independent two-save hardware DAC hash
`0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812`
and the 8 retail `COLORS` bank-3 entries remain a *value-level
reconciliation*, not traced retail gameplay instructions.

## Runtime evidence still required

Neither original `CODE.2` bytes nor a DOSBox-X `.sav` VGA
snapshot contains a historical sequence of CPU `OUT/OUTSB`
events. A current 256-entry DAC snapshot proves *final hardware
values*, not the sequence that wrote them. The previous two
screenshot/save-state pairs were not at identical skeleton animation
phases; do not infer DOS/native frame parity from their differences.

For a legitimate dynamic provenance certificate, obtain an
**independent original retail DOS trace** recording, in order:
(1) object-relative original instruction EIP at `0x29CB6`,
`0x29CCF`, `0x29CE0` (or any unexpected alternate VGA writer);
(2) each `OUT 0x3C8` index and `OUT 0x3C9` RGB6 byte, including
each byte of `REP OUTSB` and interrupt/frame boundary;
(3) palette-buffer bytes before writes, branch ancestry or call-stack
context, DOS memory/LE runtime-base mapping, and final CRTC-visible
page and saved VGA DAC bytes at a synchronized freeze.

A trace without independently verified byte source, runtime mapping,
or synchronization cannot certify original-DOS caller order.
Do not manufacture matching original/native JSONL from the native
reconstruction or infer instruction history from the palette hash.

## Validation and constraints

```sh
python3 Recomp/tools/audit_original_palette_indirect_candidates.py --self-test
python3 Recomp/tools/audit_original_palette_indirect_candidates.py \
  --code2 /private/retail/CODE.2 \
  --report /private/palette-indirect-census.json
```

Both commands passed locally against the exact original user-supplied
`CODE.2`. The owning saved-world CI
[`recomp-saved-world-linux.yml`](../.github/workflows/recomp-saved-world-linux.yml)
now checks the census against read-only retail media and retains scalar
verification markers only; the private report is deleted.

**Outstanding:** LE fixup-aware/indirect CFG resolution,
actual retail runtime DAC events and bank-3 source caller,
same-phase screenshot/VRAM parity, and acceptance gate R04.
