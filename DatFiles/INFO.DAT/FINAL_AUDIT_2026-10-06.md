# INFO.DAT final reconstruction audit — 2026-10-06

Status: **ARCHIVAL-COMPLETE FOR THE SUPPLIED RAVENLOFT BUILD, WITH ACTIVE RES0..RES10 ROUTING PROVED, HISTORICAL SELECTOR SLOTS 11..12 BOUNDED, AND BYTE-EXACT SEMANTIC DECODE→ENCODE VERIFIED.**

## Controlling source identity

```text
INFO.DAT   17 bytes
SHA-256    4ffa6ac5d5b5afd103bd67d84d3a372b401b3b19407b0c68c44bb0dfd9e32c30
bytes      30 30 30 30 30 30 30 30 30 31 31 31 31 02 00 44 3A
```

Certified consumers:

```text
CODE.2 INFO reader              2E768..2E7EE
CODE.2 indexed resource opener  2E7F0..2E9C3
CODE.1 INFO reader              function 8978
CODE.1 indexed resource opener  function 8A04
```

## Final physical layout

```text
+0x00  char  resource_location_digits[13]
+0x0D  int16 resource_path_length
+0x0F  byte  resource_path[resource_path_length]
```

Supplied values:

```text
digits = "0000000001111"
length = 2
path   = "D:"
```

The path length is read with signed extension. The supplied file has an exact extent of `15 + 2 = 17` bytes.

## Selector semantics

Both certified game modules read all thirteen ASCII decimal bytes and normalize them by subtracting `'0'`.

The active indexed RES bootstrap passes:

```text
base name = "RES"
count     = 11
```

Therefore selectors 0..10 are proved RES0..RES10 location controls.

For active index `i`:

```text
selector[i] == 0  -> RESi
selector[i] != 0  -> <resource_path>\RESi
```

The supplied file routes:

```text
RES0..RES8   -> local/current path
RES9         -> D:\RES9
RES10        -> D:\RES10
```

The executable tests zero versus nonzero after decimal normalization; it does not require the active digit to equal one specifically.

## Historical selectors 11 and 12

Positions 11 and 12 are structurally real and are always read/normalized, but neither certified executable initializes more than eleven indexed RES groups.

The complete certified CODE.2 object contains:

```text
direct selector-base references F054 = 2
direct selector-11 address F05F      = 0
direct selector-12 address F060      = 0
```

No independent active consumer for slots 11/12 has been proved.

The shipped CD does contain RES11 and RES12, but the completed resource lineage audit identifies them as legacy/pre-split composite generations whose content principally migrated into the later active RES3/4/5/6/9 families. Their presence supports a historical thirteen-slot lineage without proving active selector-11/12 semantics in the supplied executable pair.

Therefore slots 11 and 12 remain:

> **preserved historical selector positions with no active consumer proved in the certified build**

They must not be silently relabeled as active RES11/RES12 controls.

## Shared resource-path semantics

The trailing path is not exclusive to indexed RES loading.

LEVELS opcode-11 numbered interactive-resource handling independently constructs:

```text
resource_path + "\" + two_decimal_digits
```

For the supplied path:

```text
resource 07 -> D:\07
```

This independently proves the field as a shared installation/resource path prefix.

## Semantic serializer closure

The INFO parser now treats the normalized 13-value selector tuple as semantic state rather than merely echoing retained ASCII bytes.

Serialization performs:

```text
selector value 0..9 -> ASCII '0'..'9'
signed int16 path length
exact resource_path bytes
```

Validation against the supplied file:

```text
semantic decode -> encode  PASS byte-for-byte
rebuilt SHA-256            4ffa6ac5d5b5afd103bd67d84d3a372b401b3b19407b0c68c44bb0dfd9e32c30
```

## Bounded remaining question

Only historical provenance remains open: the exact older executable/editor generation, if any, that actively used selector positions 11 and 12.

That question does not affect the supplied-build format, active routing behavior, preservation model, or semantic serializer.

## Final decision

For the supplied STRAHD'S POSSESSION build, `INFO.DAT` has no unresolved structural or active-runtime semantic region.

The INFO reconstruction is therefore **archival-complete to the supplied evidence boundary**.
