# CODE.1 / CODE.2 Handoff and CHARS Evidence

Status: **ACTIVE — binary correlation checkpoint**

This note records only relationships established directly from the shipped CODE binaries and the certified repository state. Semantic names are withheld where the evidence does not yet prove them.

## Binary identity

Both shipped `CODE.1` (277,787 bytes) and `CODE.2` (558,689 bytes) are DOS executables containing the DOS/4G extender/bound protected-mode form. This is consistent with the already reconstructed CODE modules being the substantive protected-mode programs launched by `R.EXE`.

## CHARS occurrence map

Direct string extraction from the shipped binaries gives:

- `CODE.1`: `CHARS` at file offset `0x3E7B8`.
- `CODE.2`: `CHARS` at file offset `0x76634`.
- `CODE.2`: a second string containing `CHARS` at file offset `0x77062`.

The second CODE.2 occurrence is therefore independent evidence that `CHARS` is not merely a launcher-private filename. Both protected-mode modules contain references to it, with CODE.2 containing more than one occurrence.

## Nearby CODE.2 semantic neighborhood

The CODE.2 string region around the first `CHARS` occurrence also contains game-state/user-facing strings including:

- `SAVE.DAT`
- `SAVE`
- `GENERATING CHARACTERS`
- `CHARACTER BASICS`
- party-member selection text
- save-game failure/status text

This establishes a strong binary-level association between CODE.2's `CHARS` references and character/save-related subsystems. It does **not**, by itself, prove the exact on-disk format or whether every `CHARS` access is creation, import, export, or deletion.

## Cross-module lifecycle currently proven

The evidence now supports this minimum lifecycle:

1. `R.EXE` supervises execution of the protected-mode CODE programs and recognizes special child return states.
2. Both CODE programs contain a `CHARS` filename/reference.
3. CODE.2 contains `CHARS` in a region associated with character generation/save-game UI and data strings.
4. The launcher contains a special path which deletes `CHARS` when the CODE.2 return state is 12, then resumes the inter-module cycle using state 10.

Therefore `CHARS` is a **cross-program transient/state-transfer artifact**, not an unrelated installer file. The exact payload and direction of transfer remain under reconstruction.

## Important negative finding

The current repository-wide semantic source search does not expose an already-certified high-level label for launcher states 10 and 12. The canonical `Code1.cpp` and `Code2.cpp` are large reconstructed artifacts, but the launcher protocol was not previously documented as an explicit named interface. Accordingly, this runtime audit must recover the producer paths from binary/control-flow evidence rather than retroactively assigning names from intuition.

## Next proof targets

1. Resolve the protected-mode image layout sufficiently to translate `CHARS` data references into code xrefs.
2. Classify each CODE.1/CODE.2 `CHARS` xref as open/read/write/create/delete/existence-test.
3. Locate termination/runtime calls reached with values 10 and 12 and trace their predecessor control flow.
4. Correlate those predecessors with character-generation, game-entry, return-to-menu, load/save, or other known reconstructed subsystems.
5. Only after that correlation, assign semantic enum names to launcher states 10 and 12.

## Preservation rule

Do not replace the numeric protocol values with gameplay names until the corresponding CODE-side producer path is independently established. The launcher-side behavior and the CODE-side intent are separate claims and should remain separately cited.