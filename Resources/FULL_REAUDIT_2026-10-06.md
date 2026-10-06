# Ravenloft resource workstream — full re-audit, 2026-10-06

Status: **RE-CERTIFIED FOR THE SUPPLIED RAVENLOFT RUNTIME / DATA SET, WITH TARGETED EXECUTABLE-FIDELITY ERRATA AND TOOLING HARDENING RECORDED BELOW.**

This document is a fresh adversarial audit of the completed `Resources/` workstream. It does not replace `FINAL_RESOURCE_AUDIT.md`; it records what remained correct, what required correction after direct binary re-check, and which apparent discrepancy was disproved by later cross-file evidence.

## 1. Audit standard

Controlling evidence remains:

1. immutable shipped binaries/resources;
2. direct CODE.1/CODE.2 disassembly and certified decompilation records;
3. reconstructed DAT/resource relationships;
4. committed validators and byte-exact corpus inventories;
5. period vendor documentation only for third-party/middleware nomenclature and role.

Historical/editor names are not invented. Unknown or alternate-build material remains explicitly bounded.

## 2. Overall verdict

The resource architecture remains correct and usable for the faithful initial decompilation/native-port track:

- all 33 supplied resource/support files remain accounted for;
- ANM outer grammar and all shipped image packet families remain valid;
- all 13 RES archives remain structurally reconstructed and byte-preservable;
- EH type classification remains valid;
- RES3 source-bank arithmetic remains valid;
- active RES9 wordcode selection and VM reconstruction remain valid;
- RES1 portrait/sentinel interpretation remains valid;
- INIT, COLORS, SINTAB, and T physical models remain valid;
- SAMPLE.AD remains correctly identified as Miles AIL YM3812/OPL2 GTL middleware;
- GM1.BNK / GM2.BNK remain correctly classified as ARIA General MIDI support banks with internal grammar deliberately deferred;
- INFO.DAT correctly proves the active indexed archive set as RES0..RES10; supplied RES11/RES12 are not promoted into that active set without executable evidence.

No supplied resource family requires reclassification or a new parser model.

Two executable-fidelity details do require correction/annotation before the resource workstream should be called audit-clean: signed runtime interpretation of selected RES words and the LZSS dictionary's partial initialization.

## 3. R-AUDIT-001 — aggregate resource verification / CI

**Class:** hardening, not semantic defect.

There is no single top-level resource command/workflow that checks canonical identities and invokes all permanent analyzers together.

Recommended closure:

- add `Resources/verify_resources.py` or equivalent;
- verify expected SHA-256 identities before semantic checks;
- run ANM, RES, type-7, wordcode, INIT, SINTAB, T, COLORS, and SAMPLE.AD validators;
- add a dedicated GitHub Actions workflow.

## 4. R-AUDIT-002 — WITHDRAWN: class-3 source identity

The initial re-audit temporarily treated `transient/moving effects` as an overstatement because `RES/TYPE7_SOURCE_BANKS.md` ends by saying class 3's higher gameplay identity remained under reconstruction.

That conclusion is withdrawn.

Later cross-file evidence closes the identity:

- class-3 scene emitter `2BFEC..2C22E` walks the runtime live-world-object array at `0x248EC`, using 0x16-byte records and count `0xC038`;
- it reads record word `+0x0E` as the type used to index the 12-entry INIT class-3 descriptor table at runtime pointer `0x22234`;
- LEVELS event opcode 3 calls constructor `08A80`, which creates the same 0x16-byte transient/moving-effect record family;
- `08A80` stores its effect type in record word `+0x0E`.

Therefore the higher-level documentation is correct:

```text
RES3 4158..4233   class 3 — transient/moving effects
```

The stale text is the final holdback paragraph in `RES/TYPE7_SOURCE_BANKS.md`. It should be updated to reflect the now-proved runtime identity.

## 5. R-AUDIT-003 — RES signed runtime interpretation

**Class:** executable-fidelity documentation/tooling correction. Supplied corpus interpretation is unchanged because observed values are positive.

Direct disassembly of the certified CODE.2 object confirms that some serialized 16-bit words currently presented as unsigned physical fields are sign-extended by the runtime.

### EH header-size word

In typed loader `2EB60`:

```text
2EB8E  MOVSX EAX, word [EBX+03]
2EB9F  MOVSX EAX, word [EBX+03]
```

The word at EH `+0x03` is therefore a 16-bit serialized field with **signed runtime interpretation** when CODE.2 allocates/reads the type header.

Documentation may retain a neutral physical `word` representation, but a faithful runtime model must not silently zero-extend it.

### Type-1/type-2 raster descriptor words

At type-1 loader `2FD9C`:

```text
2FDD3  MOVSX EAX, word [EBP+02]   // height
2FDD8  MOVSX EAX, word [EBP+00]   // count
```

The loop paths likewise sign-extend `count` and `height` (`2FEC4`, `2FED7`, `2FF39`, `2FF4B`, etc.).

Current `analyze_res.py` uses `<HH` for both descriptor words. For exact runtime parity, the analyzer/port model should preserve their signed interpretation. This does not change any supplied positive descriptor result.

Recommended documentation form:

```text
+0x00 int16 count          // sign-extended by CODE.2
+0x02 int16 height         // sign-extended by CODE.2
```

For `type_header_size`, prefer wording such as `16-bit header-size word; sign-extended by CODE.2` if retaining physical-format neutrality.

Affected overview locations include `RES/FORMAT.md`, `FINAL_RESOURCE_AUDIT.md`, `RES/EXECUTABLE_ONLY_FORMATS.md` where applicable, and Technical Archive resource-format summaries.

## 6. R-AUDIT-004 — LZSS dictionary initialization

**Class:** executable-fidelity validator correction.

Current `RES/analyze_res.py` initializes all 4096 history bytes to `0xFE`:

```python
hist = bytearray([0xFE]) * 0x1000
hp = 0xFEE
```

Certified CODE.2 routine `37700` instead performs:

```text
3770B  EDI = dictionary base
37710  AL  = 0xFE
37712  ECX = 0x0FEE
37717  REP STOSB
37719  AX  = 0x0FEE
3771D  history cursor = AX
```

Thus only dictionary bytes `0x000..0xFED` are initialized to `0xFE` by this routine. The final 18 bytes `0xFEE..0xFFF` are not initialized here. This is also preserved by the CODE.2 decompilation continuity record as the decoder's partial `0xFEE`-byte dictionary initialization.

This does **not** invalidate the supplied compressed-family classifications or sizes. It does mean the current standalone Python decoder is not literally equivalent to CODE.2 for a stream that observes those final 18 history bytes before they acquire defined runtime contents.

Recommended closure is not to guess a replacement fill value. Instead:

1. model the original partial initialization explicitly;
2. determine/prove the initial or prior-call state of the remaining 18 bytes in the real runtime;
3. prove whether any supplied Ravenloft stream can read those locations before overwriting them;
4. only then claim exact decoder equivalence.

Until that check is complete, documentation should say **first 0xFEE dictionary bytes initialized to 0xFE; ring size 0x1000; initial cursor 0xFEE** rather than implying a 4096-byte fill.

## 7. R-AUDIT-005 — SINTAB validator coverage

**Class:** hardening.

The SINTAB reconstruction remains valid. `analyze_sintab.py` should nevertheless be strengthened so every certification claim is enforced rather than merely reported/inferred.

Recommended additions:

- hard-fail expected canonical SHA-256 in certification mode;
- explicitly validate the documented signed-magnitude sine sign-bit ranges in addition to magnitude checks;
- retain all existing block-size, selector-table, code-body, trailer, and EOF checks.

## 8. R-AUDIT-006 — T round-trip terminology

**Class:** documentation precision.

`analyze_t.py` begins reconstruction from `bytearray(data)` and rewrites the parsed active fields into that preserved carrier. Therefore it proves byte-exact **preservation/patch round-trip**, including inactive slack, but it is not an independent serializer that rebuilds the entire file from an abstract model.

The T format itself remains valid:

- 1,073 active offsets;
- uint16 length-prefixed records;
- XOR `0xDC` payload transform;
- 25 zero-length records;
- 276 bytes pre-payload slack;
- 30 inter-record gaps / 1,804 gap bytes;
- final record ending at EOF.

Recommended wording: `byte-exact preservation/patch round-trip` unless/until an independent serializer is implemented.

The optional CP437 `--dump` rendering is a display convenience, not part of the proved physical encoding.

## 9. R-AUDIT-007 — canonical identity enforcement is inconsistent

**Class:** hardening.

Validator behavior is inconsistent:

- some tools hard-fail source SHA mismatches;
- some calculate/report a `source_sha256_matches` boolean but still succeed;
- some only report calculated identity.

A certification-mode aggregate runner should hard-fail every canonical source/dependency mismatch before semantic validation.

## 10. R-AUDIT-008 — stale TYPE7_RENDERER remaining-work text

**Class:** documentation synchronization.

`RES/TYPE7_RENDERER.md` still lists work that later resource analysis completed, including:

- mapping archive-3 IDs/source banks to NPCS/OBJECTS/level families;
- documenting the external rasterizer package loaded by renderer initialization.

Those items are now closed by `TYPE7_SOURCE_BANKS.md`, the related index ledgers, and `MISC/SINTAB/FORMAT.md`. The remaining-work section should be pruned so it does not imply already-completed dependencies remain open.

## 11. R-AUDIT-009 — ANM dispatcher-domain validator hardening

**Class:** hardening.

CODE.2 rejects ANM chunk IDs above 9 before dispatch. The shipped corpus contains only valid IDs, but `ANM/analyze_anm.py` currently accepts structurally valid arbitrary IDs unless another family-specific check happens to reject them.

Recommended certification check:

```text
0 <= chunk_id <= 9
```

This does not change any supplied ANM result.

## 12. R-AUDIT-010 — type-7 selector-domain validator hardening

**Class:** hardening.

The detailed corpus/docs establish the supplied type-7 raster selector domain as the 129 even values `0x0000..0x0100`, matching each SINTAB block's 129-entry table. `analyze_type7.py` currently enforces selector parity but does not itself reject an even selector outside that range.

Recommended check for each nonzero descriptor selector:

```text
0x0000 <= selector <= 0x0100
selector is even
```

The corpus conclusion remains valid; this merely moves a documented invariant into executable validation.

## 13. Cross-file integration results

### INFO.DAT -> active RES set — PASS

The proved indexed runtime archive set is RES0..RES10. INFO digits 11 and 12 are serialized/preserved but are not promoted into active RES11/RES12 selectors without an executable consumer. This agrees across the DAT analysis and Technical Archive.

### INIT -> resource namespace bases — PASS

The following relationships remain mutually consistent:

```text
15    RES0 raster base after 15 fonts
168   RES1 OBJECTS UI base
336   RES1 portrait base
0/2169/3316/4158  RES3 source-class bases
600   RES4 narrow 32-column texture-bank base
49    RES5 alternate XMIDI-bank offset
49/69/166/192/266 RES6 sampled-SFX boundaries
32    RES9 graphics/resource base
103   RES9 interaction-resource base
48    RES10 interaction voice/SFX base
414   RES9 wordcode displacement
```

### NPCS / OBJECTS / LEVELS / INIT -> RES3 — PASS

All four source classes close their exact RES3 partitions. Class 3 is now cross-file-proved as the transient/moving-effect live-record family; the lower-level source-bank document alone is stale on that point.

### SAVE / T / RES1 portraits — PASS

The portrait formula and possession offset remain consistent with SAVE status bit `0x0100` and the two 46-slot RES1 portrait banks. Terminal empty/raw entries remain preserved and inactive through established resource paths.

### RES9 wordcode -> INIT/T/audio/cinematics — PASS

`103 + 414 + program_id` still resolves program IDs 0..37 exactly to `RES9:517..554`. Wordcode text/audio/cinematic bases remain coherent with INIT and the shared T/audio subsystems.

### Type 7 <-> SINTAB — PASS

The type-7 selector/display-list model remains coupled exactly to the 129-entry SINTAB scale-rasterizer tables. External executable renderer microcode remains a real data dependency and must not be reduced to ordinary bitmap interpretation.

## 14. Superseded/withdrawn audit hypotheses

The following re-audit hypothesis is explicitly superseded:

- **WITHDRAWN:** class-3 `transient/moving effects` was thought to be an overclaim.
- **Reason withdrawn:** direct cross-file runtime evidence links the class-3 emitter, 0x16-byte live object array, `+0x0E` type field, INIT class-3 table, and `08A80` transient/moving-effect constructor.

Preserve this note so a future reviewer does not reintroduce the stale lower-level uncertainty.

## 15. Final re-certification decision

The original resource certification is **UPHELD** for the supplied Ravenloft runtime/data set.

The re-audit found no missing supplied resource family, no invalid outer grammar, no incorrect active archive routing, no broken RES3 namespace arithmetic, no incorrect wordcode bank selection, and no middleware misclassification.

Before calling the workstream completely audit-clean at instruction-level fidelity, perform the targeted maintenance above, especially:

1. annotate signed runtime interpretation for the affected RES words;
2. reconcile `analyze_res.py` with CODE.2's partial `0xFEE` dictionary initialization;
3. synchronize the stale class-3 and TYPE7_RENDERER documentation;
4. harden validators/identity checks and add aggregate CI.

These are bounded corrections. They do not block continued faithful decompilation or architectural preservation, but the executable details must be carried into any native implementation rather than normalized away.

## 16. Intentional unknowns retained

The following remain legitimate evidence boundaries rather than audit defects:

- historical producer/editor purpose of the unread RES9 companion zero bank;
- literal producer meaning of the two inactive RES1 `0x00000060` records;
- exact provenance/build identity of RES11/RES12 alternate material;
- historical source/editor field names not recoverable from runtime behavior;
- semantic field names for uninstantiated resource type 5;
- higher-level producer semantics of unused ANM chunk type 4 beyond its executable control behavior;
- inner ARIA GM1/GM2 bank grammar pending specification or shipped-driver reconstruction.

Unknown remains UNKNOWN unless new primary evidence closes it.


## 17. Corrective implementation status

The corrective maintenance pass has now been integrated:

- **R-AUDIT-001:** implemented with `CANONICAL_MANIFEST.json`, `verify_resources.py`, and the resource-validator CI self-test workflow. Full identity/semantic certification accepts an external immutable corpus because original game assets are intentionally not stored in Git.
- **R-AUDIT-003:** implemented. RES documentation and `analyze_res.py` preserve signed runtime interpretation for the EH header-size word and type-1/type-2 `count`/`height` descriptor words.
- **R-AUDIT-004:** implementation corrected. The Python LZSS model initializes only `0x000..0xFED`; `0xFEE..0xFFF` remain explicit undefined sentinels until written. Any early read now fails certification. **Canonical-corpus post-fix execution remains required** because the RES binaries are not committed to this repository.
- **R-AUDIT-005:** implemented. SINTAB canonical identity, fixed serialized counts, signed-magnitude sine sign ranges, formula tolerances, patch-table first offsets, and strictly increasing scale-step sums are enforced.
- **R-AUDIT-006:** implemented in documentation. T exactness is described as preservation/patch round-trip rather than a from-scratch serializer.
- **R-AUDIT-007:** implemented centrally. The aggregate verifier hard-fails the canonical size/SHA-256 manifest before semantic analyzers run.
- **R-AUDIT-008:** implemented. The stale TYPE7_RENDERER remaining-work list now points to the already-closed source-bank and SINTAB work.
- **R-AUDIT-009:** implemented. ANM chunk IDs outside CODE.2 dispatcher domain `0..9` are rejected.
- **R-AUDIT-010:** implemented. Type-7 selectors must be even and inside `0x0000..0x0100`.

The withdrawn R-AUDIT-002 remains withdrawn; `TYPE7_SOURCE_BANKS.md` now records the proved transient/moving-effect identity.

That post-fix canonical-corpus run has now been completed against the original 33 supplied resource/support binaries. All 33 matched the canonical size/SHA-256 manifest exactly. The corrected partial-history LZSS model decoded all 11,174 shipped LZSS streams (1,478 type 1, 15 type 2, 1,319 type 6, 8,362 type 7) with zero reads from undefined dictionary bytes and zero trailing compressed bytes. R-AUDIT-004 is therefore fully closed for the supplied Ravenloft corpus.


### Certification-runner exit semantics

Post-commit verification found and closed one additional tooling edge: `analyze_res.py` historically reported semantic failures in its anomaly list but exited zero. It now exits nonzero for every unexpected anomaly. The only non-failing anomalies are the two exact documented malformed VOC records at `RES11:4877` and `RES12:430`; their archive/index, signature, declared length `0x11669`, and stored length `0x1669` must all match before they are treated as expected shipped anomalies. The CI self-test includes a synthetic malformed RES record and requires a nonzero analyzer exit.


## 18. Final post-fix canonical corpus run

The original `RES+OTHER.zip` and `ANM+OTHER.zip` source archives were re-supplied and checked directly after the corrective validator changes.

Results:

```text
canonical files checked                 33 / 33
size mismatches                         0
SHA-256 mismatches                      0
strict LZSS streams                 11,174
  type 1                            1,478
  type 2                               15
  type 6                            1,319
  type 7                            8,362
undefined-tail history reads             0
compressed-stream trailing-byte failures 0
```

The strict decoder models CODE.2 routine `37700` exactly at initialization time: dictionary bytes `0x000..0xFED` begin as `0xFE`, while `0xFEE..0xFFF` remain undefined until written. Every shipped compressed stream completes without observing an undefined byte.

This closes the last empirical qualifier introduced by the 2026-10-06 re-audit. The resource workstream is now **audit-clean for the supplied canonical Ravenloft corpus**, subject only to the intentionally bounded historical/vendor unknowns listed above.
