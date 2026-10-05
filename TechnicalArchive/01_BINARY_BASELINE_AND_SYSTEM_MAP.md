# Ravenloft: Strahd's Possession — Binary Baseline and System Map

Status: **PHASE I / BASELINE 01**  
Scope: immutable CODE baseline, evidence hierarchy, first top-level architectural partition, and external-source validation policy.

## 1. Purpose

This document freezes the first architectural baseline after completion of the instruction-accounted CODE.1 and CODE.2 reconstructions and their end-to-end semantic audits. It deliberately distinguishes facts proved by the shipped binaries from interpretations, period documentation, and community research.

The immutable Ravenloft binaries remain controlling evidence. External documentation is corroborative: it may explain a mechanism or supply historical terminology, but it cannot override contradictory machine-code or data evidence.

## 2. Evidence hierarchy

For architectural claims the working order is:

1. **Shipped Ravenloft bytes and encoded data** — controlling primary evidence.
2. **Certified instruction-accounted reconstruction and final audit records** — semantic representation of those bytes.
3. **Contemporary/official platform, compiler, extender, game manual, and publisher documentation** — authoritative context for mechanisms and intended user-visible behavior.
4. **Experimentally reproducible behavior of the matching game release** — supporting behavioral evidence.
5. **Community reverse engineering and historical research** — valuable leads and independent observations, accepted only after checking against stronger evidence where the project has the relevant bytes/code.
6. **Cross-title comparison** — useful for identifying DreamForge lineage, never proof of Ravenloft behavior by itself.

Community contributions are not excluded merely because they are unofficial. They are preserved with attribution and validation status.

## 3. Certified CODE.1 baseline

**Class: CONFIRMED**

- Original file: `CODE.1`
- File size: `277,787` bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`
- Executable object-1 extent: `70,402` bytes (`0x11302`)
- Object-1 file start: `0x2C724`
- Object-1 virtual base: `0x10000`
- Entry logical offset: `0xCC1C`
- Verified translated instruction bytes: `69,361`
- Proven non-code/data bytes: `1,041`
- Reconciled object bytes: `70,402 / 70,402`
- Sequential semantic audit: `70,402 / 70,402`
- Unresolved object-1 bytes: `0`

Canonical semantic source: `Code1/Code1.cpp`. Supporting records include `Code1/PROGRESS.md`, `Code1/FINAL_BINARY_AUDIT.md`, `Code1/RESIDUAL_LEDGER.md`, and the continuity/audit records.

The accounting distinction matters historically: 100% object reconciliation does **not** mean every object byte is executable instruction material. Embedded tables, padding, alignment, interrupt/data islands, and other directly proved non-code remain represented as non-code rather than being forced into C++ instruction semantics.

## 4. Certified CODE.2 baseline

**Class: CONFIRMED**

- Original file: `CODE.2`
- File size: `558,689` bytes (`0x88661`)
- SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Bound LE header: file `0x27BDC`
- Executable object-1 virtual base: `0x10000`
- Executable object-1 size: `239,698` bytes (`0x3A852`)
- Executable object-1 file start: `0x3AF24`
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- Entry logical offset: `0x353A0`
- Data object-2 virtual base: `0x50000`
- Data object-2 virtual size: `0x2DD10`
- Data object-2 file start: `0x75F24`
- Stored object-2 bytes: `0x1273D`
- Verified translated instruction bytes: `235,022`
- Proven non-code/data bytes: `4,676`
- Reconciled object bytes: `239,698 / 239,698`
- Semantic audit: `239,698 / 239,698`
- Unresolved object-1 bytes: `0`

Canonical semantic source: `Code2/Code2.cpp`. Certified `Code2.cpp` SHA-256: `b4ab60f12e0d5a3c986ef61ec3c2e5c6d68f0193a0e535ef35eb7af6f3aed4aa`; certified Git blob SHA: `c3f16122e79c3c464382e4511d9d932f6fcd694a`.

Authoritative certification record: `Code2/FINAL_CERTIFICATION.md`; detailed provenance remains in `Code2/FINAL_SEMANTIC_AUDIT.md`, `Code2/PROGRESS.md`, and the continuity records.

## 5. Executable/container model

### 5.1 Protected-mode executable evidence

**Class: CONFIRMED for encoded structure; RECONSTRUCTED for architectural role.**

CODE.2 contains a bound LE executable structure and separates executable object 1 from data object 2. Object 1 is based at virtual `0x10000`; object 2 is based at `0x50000`. The certified reconstruction also contains explicit DPMI, selector, DOS interrupt, callback/interrupt-frame, heap-growth, and DOS-memory mechanisms.

The architecture must therefore preserve two different ideas that modern source can easily blur:

- a logical/protected-mode address space used by the application/runtime; and
- DOS/selector/far-memory interfaces where segment/selector identity and register-level contracts are semantically observable.

Flattening every historical address into a native host pointer would erase behavior demonstrated during both audits.

### 5.2 CODE.1 and CODE.2 are separate evidence domains

**Class: CONFIRMED.**

The two CODE files are not documented as interchangeable modules or two names for one binary. Each has its own immutable identity, executable extent, entry point, address map, and reconstruction. During CODE.2 work, CODE.1 supplied numerous exact or near-exact routine signatures and compiler/runtime patterns, but every CODE.2 conclusion was re-established against CODE.2.

This distinction is retained in the archive even where later analysis proves shared implementation families.

## 6. First architectural partition

The following map is an **investigation architecture**, not a claim about DreamForge's original source-directory names.

```text
                 DOS / hardware / protected-mode environment
                               |
              +----------------+----------------+
              |                                 |
      extender/runtime boundary          hardware-facing paths
      DOS/DPMI/selectors/heap            VGA / PIT / input / vectors
              |                                 |
              +----------------+----------------+
                               |
                    CODE executable logic
                               |
       +-----------------------+-----------------------+
       |                       |                       |
   bootstrap / CRT       application control      resource & file I/O
   argv/environment      dispatch / lifecycle     loaders / streams
   termination                 |                       |
                               |
             +-----------------+------------------+
             |                 |                  |
       presentation       simulation/state     persistence/data
       VGA/render/UI      actors/events/etc.    saves / DAT / levels
             |                 |                  |
             +-----------------+------------------+
                               |
                     long-lived game state
```

**Class: RECONSTRUCTED as a coarse partition.**

Evidence already demonstrates concrete families corresponding to runtime/bootstrap, DOS/CRT file I/O, memory allocation, VGA/image operations, text/font/UI, mouse/input, timer/event dispatch, actor/character processing, resource loading, and save/load behavior. The exact boundaries and dependencies between these families will be refined in the execution-flow and subsystem atlases.

## 7. Proven mechanism families already available for deeper mapping

### 7.1 Runtime/bootstrap and process boundary

**Class: CONFIRMED.**

CODE.1 contains reconstructed startup/recovery, argv/environment construction, command-line token scanning, runtime launch, runtime record dispatch, CRT error/descriptor/stack/selector helpers, interrupt veneers, heap allocation/growth, and termination paths. CODE.2 contains corresponding and expanded runtime/application mechanisms independently audited through its full object.

The archive will not label all of this "game engine" indiscriminately. Compiler/runtime/extender mechanisms are part of the shipped technology, but must remain distinguishable from DreamForge application logic.

### 7.2 DOS file/resource boundary

**Class: CONFIRMED.**

CODE.1 explicitly reconstructs find-first/find-next, DTA shuttles, open/create/truncate, descriptor/IOCTL setup, close, read/write/seek/delete, directory/utility operations, resource/source reading, image-size parsing, and image loading/relocation. These form a demonstrable file/resource boundary suitable for later linkage to RES and DAT consumers.

### 7.3 Memory and selector boundary

**Class: CONFIRMED.**

The certified code includes DOS memory allocation/free, DPMI services, selector-qualified transfers, guest heap/free-list logic, heap extent queries, raw arena acquisition, and heap extension. The final documentation must preserve register/carry contracts and selector identity wherever the bytes make them observable.

### 7.4 Timing/event machinery

**Class: CONFIRMED.**

CODE.1 contains PIT/timer setup, timer state management, registration/event machinery, interrupt paths, and 48 address-distinct dispatcher wrappers. CODE.2 independently contains a relocated timer/event family identified and reconciled during decompilation. The exact game-level scheduling semantics will be documented separately from the low-level timer mechanism.

### 7.5 Graphics/presentation

**Class: CONFIRMED at mechanism-family level.**

The reconstructions contain VGA/image helpers, palette operations, self-modifying scaled blitters, line/sprite graphics, text/font drawing, rectangle/frame operations, cursor and UI rendering, image/resource constructors, and repeated-object render loops. Self-modifying operands and partial-register behavior are historical implementation details and will remain explicit in the renderer specification.

### 7.6 Input/UI

**Class: CONFIRMED at mechanism-family level.**

Mouse-facing routines, event pumping, ordered input queues, editors/selection dispatchers, pointer-driven insertion, and numerous UI dispatch tables are reconstructed. Later documents will distinguish low-level input acquisition from menu/control-panel/gameplay interpretation.

### 7.7 Character/actor systems

**Class: CONFIRMED at mechanism-family level.**

CODE.1 contains extensive actor generation, class/order editing, bounded name input, progression/stat derivation, race/class minima, normalization, rendering, choices, and presentation paths. CODE.2 contains much larger application-state machinery whose relationships will be mapped from calls and shared data rather than inferred from CODE.1 names.

### 7.8 Persistence

**Class: CONFIRMED at mechanism-family level.**

CODE.2 audit/decompilation established save/load serializers and shared load tails, including exact short-write/close behavior. This will be cross-validated against `SAVE.DAT` and related state files before field semantics are promoted into the architectural specification.

## 8. Official and period documentation policy

External technical documentation is used to explain period mechanisms only after the binary establishes that the mechanism is present.

Priority sources include:

- Intel x86 architecture/instruction documentation for processor semantics.
- Microsoft/MS-DOS programmer documentation for DOS services and process/file interfaces.
- Rational Systems/Tenberry DOS/4G, DOS/4GW, DOS/16M and related extender documentation where it matches observed extender behavior.
- Watcom/Open Watcom compiler and runtime documentation/source for calling conventions and recognizable runtime/compiler idioms.
- Official Ravenloft manual/cluebook and other publisher-supplied documentation for intended player-visible behavior, file/export features, and release/version distinctions.

Example: Open Watcom's C/C++ documentation describes predefined calling-convention aliases such as `__cdecl` and `__watcall`; its `__cdecl` description specifies right-to-left stack argument pushing and caller cleanup. Such documentation is useful when interpreting a matching encoded call sequence, but the Ravenloft call site remains the proof of which convention/slot behavior that site actually uses.

## 9. Community research policy and first validated case: automaps

### 9.1 Source

Daniel D'Agostino, **"The Ravenloft Engine Trilogy's Automaps"**, Swords and Software, 10 March 2024.

Validation status: **PARTIALLY CORROBORATED / RETAIN AS COMMUNITY RESEARCH** pending direct linkage of every format claim to Ravenloft code/data.

### 9.2 Claims useful to the archive

The article reports that Ravenloft's automap can export the explored current map to `AUTOMAPS/AUTO###.MAP`; the exported representation is grid-resolution text using IBM/DOS character values for unexplored cells, walls, insets, floor, furniture, doors, secret doors, and trees. It also reports that notes are rendered into the exported map/footnotes and that the map can be interpreted as DOS/IBM-character-set ASCII art.

The same article notes an important cross-title distinction: global map save/restore is present in Menzoberranzan and Stone Prophet but **not** Strahd's Possession. That claim must not be generalized into Ravenloft architecture without binary confirmation.

### 9.3 What currently checks out

The community account is internally strengthened by its comparison with the official manuals: the article reports that the Strahd's Possession/Menzoberranzan manuals say exported maps may be viewed in a text editor and that printing requires the IBM character set. That intended-user behavior is consistent with the reported CP437-style byte representation rather than an arbitrary binary image format.

The article's broader gameplay description — smooth movement coexisting with optional step/grid movement while the automap represents explored space as cells with finer-resolution overlays — is a useful behavioral hypothesis for the later world/automap subsystem specification. It will not be marked CONFIRMED until the corresponding Ravenloft code/data paths are traced.

### 9.4 What is not yet promoted to binary fact

The exact semantic table of exported character bytes, note-placement implementation, map-export filename construction, and relationship between internal map state and export cells remain **RECONSTRUCTED/COMMUNITY** rather than CONFIRMED in this archive until located in the certified code and/or matching data files.

This is the model for community contributions generally: preserve attribution, record the claim, test it, and promote only the portion supported by Ravenloft evidence.

## 10. Version sensitivity

**Class: RECONSTRUCTED / requires release inventory.**

Community walkthrough research identifies its tested copy as the GOG CD-ROM version 1.2A and reports differences/bugs across distributions. Therefore future behavior tests must record the exact executable/data hashes and release source. A behavior observed in another distribution must not silently override the immutable CODE.1/CODE.2 evidence archived here.

A later provenance document should inventory known Ravenloft releases and hashes before cross-version claims are made.

## 11. Immediate unknowns deliberately retained

- The historical DreamForge source-level names and original module boundaries are unknown unless embedded evidence or surviving documentation is recovered.
- The precise original build toolchain/version for all application code is not asserted merely from recognizable Watcom/runtime evidence.
- The high-level reason for the shipped CODE.1/CODE.2 division is not yet stated as fact; architecture must derive it from startup/control/data relationships.
- Exact ownership boundaries between renderer, world state, automap, UI, and level data remain under reconstruction.
- DAT and RES semantics are not promoted merely from filenames. They will be established from byte structure plus producer/consumer code.
- Cross-title "same engine" descriptions remain comparative/community terminology until a later DreamForge lineage study quantifies shared code/data architecture.

## 12. Next evidence unit

`TechnicalArchive/02_EXECUTION_LIFECYCLE_AND_CODE_RELATIONSHIP.md`

The next unit will trace the executable lifecycle from entry/bootstrap through application dispatch and termination, then begin proving the operational relationship between CODE.1 and CODE.2. It will use original logical addresses and certified reconstruction paths rather than assigning speculative source-module names.