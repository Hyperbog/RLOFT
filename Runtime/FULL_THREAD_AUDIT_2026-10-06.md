# Runtime / Executable Thread Full Audit — 2026-10-06

Status: **AUDITED / CORE RUNTIME CERTIFIED.**

This document is the final consistency audit for the runtime/executable investigation covering `R.EXE`, CODE.1/CODE.2 handoff behavior, `CHARS`, status 10/12, finale/death-state semantics, optional installers/scripts/audio drivers, and the supplied `SOUND.CFG`.

The audit rechecked both repository documentation and the uploaded shipped binaries. Shipped bytes remain controlling evidence.

## 1. Controlling source identities

| File | Size | SHA-256 |
|---|---:|---|
| `CODE.1` | 277,787 | `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed` |
| `CODE.2` | 558,689 | `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314` |
| `R.EXE` | 12,368 | `a6d5874dc91fb068a3ed0a07290547c310ee7f4f50be4d8176c4788f251631b0` |
| `INSTALL.EXE` | 26,400 | `c14b38d43fe98ae7ba29afe4f70dea4a0bb1ac826d2276ca81637717305c8829` |
| `SINSTALL.EXE` | 381,660 | `dcb28a9a38c822ded4392f6ff0ae7213b0e5e9a76dc69aa56bd0d1d48fd1ba4a` |
| `LOADPATS.EXE` | 24,705 | `17f40351cf53e445dbb62a041ff90b18dd755f6e1f3a216354e14aaeaa50f8ea` |
| supplied `SOUND.CFG` | 59 | `b373f84879c0dfe524009526989a3b6562f518a9a64280b4ee282673cb0474cc` |

CODE.1 and CODE.2 hashes match the repository's certified decompilation baselines exactly.

## 2. R.EXE audit result — CONFIRMED

Direct re-disassembly of the shipped `R.EXE` main path reconfirms:

```text
initial status = 10
while status == 10:
    status = spawn/wait CODE.2
    if status == 10:
        status = spawn/wait CODE.1
    else if status == 12:
        delete CHARS
        status = 10
final delete CHARS
```

The main routine also directly calls DOS directory-creation wrappers for `WORK` and `AUTOMAPS` and manipulates the DOS4G environment state.

Certification:

- `R.EXE` is a 16-bit Borland DOS supervisor/launcher;
- it is not the protected-mode game image;
- its MZ-declared image size equals its physical file size, so there is no appended protected-mode payload;
- CODE.1 and CODE.2 are child processes;
- the embedded DOS/4GW 1.96 text is launcher/runtime environment material, not evidence that R.EXE itself contains the game engine.

No correction to the final supervisor model was required.

## 3. CODE object maps — CONFIRMED

Certified application mappings remain:

### CODE.1

```text
object 1 file start  0x2C724
object 1 extent      0x11302
object 1 vbase       0x10000
object 2 file start  0x3E724
object 2 vbase       0x30000
```

`CHARS` file `0x3E7B8` = object 2 `+0x94` = linear `0x30094`.

### CODE.2

```text
object 1 file start  0x3AF24
object 1 extent      0x3A852
object 1 vbase       0x10000
object 2 file start  0x75F24
object 2 vbase       0x50000
object 2 size        0x2DD10
```

`CHARS` strings:

```text
file 0x76634 -> object 2 +0x710  -> linear 0x50710
file 0x77064 -> object 2 +0x1140 -> linear 0x51140
```

The obsolete values `0x201DC`, `0x49858`, and `0x4A288` remain explicitly invalid.

## 4. CHARS lifecycle — CONFIRMED

Re-disassembly reconfirms the CODE.1 producer at `0x4EF0` and CODE.2 consumer at `0x16CD0`.

CODE.1:

```text
delete CHARS
create/reinitialize
close
reopen
transfer 0x920 bytes from party image
close
```

CODE.2:

```text
test CHARS existence/accessibility
if present:
    open
    transfer 0x920 bytes into runtime 0x218E0
    close
    post-load processing
else:
    alternate initialization
```

`0x920 = 4 * 0x248`, proving a four-character party image.

Certification: `CHARS` is a transient module-boundary party-state serialization image, not an ordinary persistent save file.

## 5. Status 10 — CONFIRMED

CODE.1 routine `0x4978` returns 10 on the completed handoff condition. Its result reaches termination helper `0x5014` unmodified.

`0x5014` explicitly privileges status 10 and, when the two required state flags are present, invokes the CHARS producer before terminating with 10.

Canonical protocol name remains:

```text
STATUS_10 = CONTINUE_MODULE_CYCLE
```

The ordinary successful character-state path is:

```text
CODE.1 completed workflow
 -> CHARS 0x920 serialization
 -> status 10
 -> R.EXE
 -> CODE.2
 -> CHARS import
```

No correction required.

## 6. Status 12 — CONFIRMED, WITH DOCUMENTATION CORRECTION

CODE.2 helper `0x205B4` explicitly tests both 10 and 12 and bypasses ordinary cleanup for those transition values before process termination.

R.EXE semantics remain:

```text
STATUS_12 = RESTART_CODE_CYCLE_WITHOUT_CHARS
```

### Certified live cause 1: party wipe / no active party

The all-party check applies `status & 0xD040` to all four active-party records. Only when every slot is disabled does it enter `0xCBC8`, perform the terminal presentation, wait for acknowledgement, and request status 12.

### Certified live cause 2: five-artifact quest completion

The success gate checks inventory predicates for:

```text
0x74  LORD DHELT'S HOLY SYMBOL
0x5C  ANCIENT ELVEN CROWN
0x76  WERERAVEN FEATHER
0x8B  VISTANI TAROKKA CARD
0x77  PRICELESS FIRE RUBY
```

These IDs were rechecked directly against the supplied `OBJECTS.DAT`; all five names match exactly.

The success tail invokes numbered event 10 and then status 12.

### Correction: `0x23DF0` is not a third live cause

The standalone routine at `0x23DF0..0x23E1A` is real and contains the same event-10/status-12 tail, but the complete reachability audit found no direct branch, recognized jump-table entry, stored function pointer, or LE relocation targeting it.

Final classification:

```text
UNREFERENCED DUPLICATE FINALE ROUTINE
```

Older checkpoint text that counted three live status-12 producers was corrected during this audit.

## 7. Character geometry and finale predicate — CORRECTED

This audit found the most important documentation defect in the thread.

Certified active-party base:

```text
record base     0x218E0
record stride   0x248
status word     0x218F0 = +0x10
current HP      0x218FC = +0x1C
```

Some older Runtime checkpoints still used an obsolete base and therefore described these as `+0x18` and `+0x24`. Those labels were eight bytes too high and have been corrected in the current status/finale documents.

### Signedness correction

The event-10 scan executes:

```asm
movzx ebx,word [status]
cmp   ebx,4000h
jge   skip
```

Although `jge` is formally a signed 32-bit condition, `movzx` guarantees `EBX` is `0..65535`. Therefore the effective legacy predicate is:

```c
uint16_t status;
status < 0x4000u && (status & 0x0080u) == 0
```

The earlier reconstruction using `(int16_t)status < 0x4000` was wrong because it would make `0x8000` negative and qualifying.

### Certified status meanings

```text
0x0080  party-template / recruited-character provenance
0x4000  DEAD / zero-HP death state
0x8000  empty/unoccupied active-party slot
```

The `0x4000` meaning is independently closed by inverse transitions:

```text
lethal damage -> HP clamped to 0 -> set 0x4000
revival path  -> HP restored to 1 -> clear 0x4000
```

Finale result:

```text
CINE10.ANM
  at least one living original/non-recruited party character remains

CINE11.ANM
  no living original/non-recruited party character remains
```

## 8. SOUND.CFG / audio boundary — STRENGTHENED AND CORRECTED

The supplied sample is exactly 59 bytes. During this audit, CODE-side use was traced directly rather than inferred from strings.

### CODE.1

At `0x9652`, CODE.1 opens pathname data offset `0x2EC` (`sound.cfg`), requests a `0x3B` transfer into its audio configuration block, then closes the file.

### CODE.2

At `0x2F37F`, CODE.2 independently opens pathname data offset `0x143C` (`sound.cfg`), requests the same `0x3B` transfer, then closes it.

The supplied sample contains:

```text
+0x16  a32mt32.dll
+0x24  a32sbpdg.dll
```

CODE.2 then consumes two separate numeric/driver-name groups, consistent with the installer's Music and Digital Sound Effects distinction.

Correction: earlier text called the uploaded file a "genuine generated" sample. The audit now records only what is proven: it is the **supplied** 59-byte sample. SINSTALL is strongly associated with selecting this configuration, but its exact byte writer has not yet been reconstructed.

## 9. Optional executables and drivers — DEFER DECISION CONFIRMED

### File roles rechecked

- `INSTALL.EXE`: MZ installer/layout provenance.
- `SINSTALL.EXE`: DOS/4G Ravenloft sound installer/configurator.
- `LOADPATS.EXE`: LZEXE 0.91-compressed UltraSound/patch utility.
- `PATCHES.INI`: explicitly an UltraSound LOADPATS table for AIL 32.
- 18 A32/GUS DLLs: 32-bit LX-format Miles/Gravis hardware backends.

No evidence places missing DreamForge gameplay logic inside those files.

### Important batch-script correction

`RLOFT.BAT` and `SOUND.BAT` contain comments describing failure of `LOADPATS`, but the supplied scripts never actually execute `LOADPATS.EXE` before their `IF ERRORLEVEL 1` test.

Therefore the exact historical LOADPATS activation path is **not certified from these batch files**. This is an archival/hardware-preservation question, not a blocker for the initial native reconstruction.

### Final priority

```text
INITIAL FAITHFUL RECONSTRUCTION
    CODE-side audio contract
    59-byte SOUND.CFG semantics needed by CODE
    game-visible music/sound triggering and status behavior

END-OF-PROJECT / OPTIONAL
    full INSTALL.EXE decompilation
    full SINSTALL.EXE decompilation
    exact LOADPATS activation and patch-loader internals
    A32/GUS driver source reconstruction
    original ISA/DMA/IRQ behavior
```

## 10. Audit corrections applied

The audit resulted in substantive repository corrections:

1. corrected CINE10/CINE11 `int16_t` signedness error;
2. corrected status offset `+0x18 -> +0x10`;
3. corrected current-HP offset `+0x24 -> +0x1C`;
4. retired `0x23DF0` as a supposed third live status-12 trigger;
5. updated older status-12 ownership documentation to the two-live-cause taxonomy;
6. directly certified both CODE modules' 59-byte `sound.cfg` readers;
7. removed unsupported provenance wording for the supplied `SOUND.CFG`;
8. documented that the supplied BAT files do not actually invoke `LOADPATS.EXE`;
9. refreshed `Runtime/README.md` from Phase-1 inventory status to the certified current architecture.

## 11. Remaining open items

No remaining item blocks the core runtime architecture or initial faithful gameplay decompilation.

Bounded optional questions are:

- semantic names for every numeric `SOUND.CFG` field;
- any CODE-visible audio completion/timing edge not yet reconstructed;
- the exact SINSTALL writer routine;
- exact historical LOADPATS invocation path;
- installer/hardware-driver internals for preservation/emulation;
- low-level R.EXE prerequisite/environment details that do not alter the already certified module state machine.

## 12. Final certification

The runtime-control reconstruction is internally consistent after the corrections above.

Canonical architecture:

```text
R.EXE supervisor
    -> CODE.2
       -> status 10 -> CODE.1
       -> status 12 -> delete CHARS and restart
    -> CODE.1
       -> CHARS 0x920 transfer + status 10 -> CODE.2
```

The initial faithful native reconstruction can proceed without reverse engineering INSTALL/SINSTALL/LOADPATS/A32/GUS internals. Those binaries should be retained for later historical preservation unless a specific unresolved CODE-facing dependency emerges.
