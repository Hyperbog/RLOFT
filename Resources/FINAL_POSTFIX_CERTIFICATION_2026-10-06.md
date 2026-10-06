# Final post-fix resource certification — 2026-10-06

Status: **PASS — AUDIT-CLEAN FOR THE SUPPLIED CANONICAL RAVENLOFT RESOURCE CORPUS.**

This record closes the only empirical qualifier left by the 2026-10-06 full resource re-audit.

## Canonical source identity

The original source archives were re-supplied:

- `RES+OTHER.zip`
- `ANM+OTHER.zip`

Together they contain the 33 resource/support files tracked by `CANONICAL_MANIFEST.json`.

Direct verification result:

```text
files expected       33
files found          33
size mismatches       0
SHA-256 mismatches    0
```

Therefore the post-fix test used the exact canonical byte images documented throughout this repository.

## Corrected LZSS edge

CODE.2 routine `37700` initializes only dictionary bytes `0x000..0xFED` to `0xFE`, leaves `0xFEE..0xFFF` untouched by that routine, and starts the ring cursor at `0xFEE`.

The corrected validator represents those final 18 bytes as undefined until a literal/back-reference write defines them. Any read before definition is a hard certification failure.

Every shipped LZSS stream was re-run:

```text
type 1    1,478
type 2       15
type 6    1,319
type 7    8,362
          ------
total    11,174
```

Results:

```text
undefined-history reads             0
trailing compressed-byte failures   0
```

Thus no shipped Ravenloft compressed resource depends on unspecified initial contents in dictionary bytes `0xFEE..0xFFF`.

## Final decision

The earlier architectural/resource audit was not invalidated by the later decoder correction. The correction narrowed one implementation detail, and the canonical-corpus re-run proves that the supplied data remains fully compatible with the corrected CODE.2-faithful model.

The resource workstream is now **audit-clean for the supplied canonical build**.

Intentional unknowns remain bounded to historical/editor provenance, alternate-build material, unused executable branches, and third-party ARIA inner-bank semantics. None blocks the initial faithful decompilation/native-port track.
