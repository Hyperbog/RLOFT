# Part 24 — retail LE palette fixups and DOSBox-X port provenance handoff

**Owning branch:** `Recomp`. **Status:** original static LE fixup
audit **PASS**; independent original DOSBox-X port-write replay **NOT CAPTURED**.
Do not equate a generated event stream or a saved VGA register snapshot with an
independent trace of retail instructions.

## Authenticated complete CODE.2 LE fixup table

[`tools/audit_original_le_palette_fixups.py`](tools/audit_original_le_palette_fixups.py)
parses the actual LE module inside SHA-pinned original `CODE.2`:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Bound module origin `0x25D24`; nested LE header `0x27BDC`;
page table `+0x235`; record table `+0x371`; terminating
record-table boundary `+0x1329E`.

All **78 logical pages** and **9,813 relocation records** parse
without leftovers. The original only uses source type `07h`
(internal 32-bit offset fixup), with target flags `00h` and `10h`:

| Original fixup form | Records |
| --- | ---: |
| 07/00 targeting object 1 | 273 |
| 07/10 targeting object 1 | 521 |
| 07/00 targeting object 2 | 5,079 |
| 07/10 targeting object 2 | 3,940 |
| **Total** | **9,813** |

This is a *full table decode* using page boundaries and variable-width
target records, not a substring search for pointers. Among the 794
internal references to executable object 1, **none targets**
`0x29248` (partial DAC submit), `0x323BC` (palette range copy),
or `0x324F8` (triplet rotate). These exact functions therefore have
no ordinary loader-created pointer represented by an internal fixup
directly to their entry points in this image.

The relocation targets for known object-2 palette staging buffers are:
`0xC054`: **76**, `0xC355`: **5**, and `0xF9D0`: **8**.
These **89** relocated data-buffer references independently support
the earlier static palette caller/source-buffer classification.

**Limits:** This does not eliminate runtime-generated indirect calls,
relocation to an *interior* target, or code using a pointer to a containing
structure. Nor does it prove those three functions execute in retail
gameplay. No LE object/entry mapping is interchangeable with absolute
DOS CPU EIP without loader mapping.

## Independent original DOS DAC replay interface

[`tools/verify_dosboxx_palette_port_trace.py`](tools/verify_dosboxx_palette_port_trace.py)
accepts a separately recorded original **byte-level I/O stream** and a
matching DOSBox-X `2026.10.01 (SDL2)` original `.sav`.

Required schema: `rloft-dosboxx-original-dac-port-v1`; provenance
includes:
`origin=independent_retail_dosboxx_io_hook`,
`program_sha256=<original CODE.2 SHA>`,
`dosbox_version=DOSBox-X 2026.10.01 (SDL2)`,
`saved_state_sha256=<entire .sav SHA>`,
nonempty `capture_id`, and
`flush_before_snapshot=true`.
`events` is a strictly sequential array of
`{ "seq": 0, "port": 968, "value": 0 }` objects, optionally including
`object1_eip` (verified object-relative code offset, not a raw
host process instruction address). Port 968 is `0x3C8`; port 969
is `0x3C9`. **Every byte of original `REP OUTSB` must generate one
separate port-969 event**, in actual I/O order. The events must include
an index write and cover **all 768** RGB6 channel positions after
index auto-increment/wraparound, with six-bit channel constraints.

Example, after an *independent original DOSBox-X I/O hook* records
the port stream and a matching frozen original `.sav` exists:

```sh
python3 Recomp/tools/audit_original_le_palette_fixups.py \
  --code2 /private/CODE.2 --report /private/retail-le-fixups.json

python3 Recomp/tools/verify_dosboxx_palette_port_trace.py \
  --trace /private/original-dosboxx-io.json \
  --state /private/original-same-freeze.sav \
  --report /private/original-dac-replay-receipt.json
```

The independent hook must be part of the executing DOSBox-X I/O
write path (e.g., instrument its byte-write dispatch or VGA DAC
handlers in a version-pinned source build). A CPU `LOG` listing,
window screenshot, VGA save-state, or native C++ replay alone does
**not** provide a complete original ordered byte-event stream.
The debugger documentation supports instruction logging but does not
establish a built-in, automatically synchronized `3C8/3C9` byte
capture interface. An instrumented build must also record the
original-program image hash, identity of the capture build, normalized
CODE.2 object/EIP mapping where known, and exact freeze/capture ordering.

A replay that equals the independent RGB6 DAC is reported as
**`DAC_RGB6_EQUAL_DECLARED_CAPTURE_UNATTESTED`**, not certified
original caller order. Provenance declarations are not externally
attested; trace capture version and synchronization still require
independent review. The validator deliberately rejects missing
channels, data before index initialization, noncontiguous records,
invalid RGB6 values, mismatched saved-state hash, and differing output.

## Verification and acceptance boundary

- Retail LE parser: passed on the actual original `CODE.2`
  (9,813 of 9,813 fixups).
- Trace replay: passed complete synthetic 768-component data and
  CLI integration against a **synthetic** version-signature state.
  Positive synthetic acceptance is never DOS provenance.
- Regression: a deliberately **767-component** stream now fails with
  `Incomplete palette`, correcting the previous defective negative test.
- Authentic source workflow: [saved-world Linux #39387745097](https://github.com/Hyperbog/RLOFT/actions/runs/39387745097)
  **passed** the original-media fixup test and synthetic replay self-test.
- No original DOSBox-X byte-port-event capture was supplied or generated.
  The two prior `.sav` snapshots prove final hardware palette values,
  **not** CPU instruction or port-write history.
- Original retail 246–253 bank-3 caller sequence and synchronized
  DOS/native pixel parity remain **NOT VERIFIED**. The native
  saved-world bank3 slice is still explicitly **provisional**.

**Next practical gate:** obtain and attest the original DOSBox-X host
I/O hook trace and same-frozen-state `.sav` *without* deriving the
events from the native renderer, the original COLORS file, or the
saved VGA DAC. Run this validator, inspect actual indexed port-write
segments and original CODE.2 instruction ancestry, then decide
whether the provisional native slice can be replaced by a source-backed
runtime palette update path.
