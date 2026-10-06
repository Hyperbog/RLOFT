# LEVELS.DAT dynamic 10-byte cell records

Status: **second runtime record-group animation state, passability endpoint, and automap class relationship are instruction-proven from certified CODE.2; archive-wide initialization pattern is independently measured.**

This document specializes the 10-byte record discussion in `SEMANTICS.md` and the dynamic automap classes in `AUTOMAP.md`.

## Record-group split

The legacy archive stores one width-indexed 10-byte-record collection. The certified loader splits it into two runtime pointer groups:

```text
DGROUP 0xCB3C  runtime group 1
DGROUP 0xCD3C  runtime group 2
```

Legacy records whose byte `+0 == 1` are diverted into runtime group 2; all others remain in group 1.

The standalone format serializes both groups separately, with byte `0xCF50` between them.

## Group-2 animation fields

CODE.2 `20B40..20C0B` proves the following runtime interpretation for group-2 records:

```text
record +0x06  uint8 position/state, stable endpoints 0 and 8
record +0x07  int8  animation delta, normally -1, 0, or +1
DGROUP 0xCF50 uint8 active-animation count
```

When `0xCF50 != 0`, the update loop scans every group-2 record. If byte `+7` is nonzero it performs:

```text
record[6] += record[7]
```

and stops that record's animation when byte `+6` reaches either endpoint `0` or `8`. At an endpoint it decrements the active count and clears byte `+7`.

When no animations remain, the cleanup pass forces every group-2 byte `+6` that is not exactly `8` to `0`. Thus the persistent stationary states are explicitly normalized to **0 or 8**.

## Interaction/toggle routine `20C0C..20CF2`

The action path first requires several eligibility tests, including record-byte-1 bit `0x04` set, bit `0x40` clear, no current animation (`+7 == 0`), and collision/placement checks.

It then inspects `record[1] & 0x03`.

### Low-two-bit mode 1

This mode toggles between the two endpoints:

```text
if position == 0:
    delta = +1                 # 0 -> 8
else:
    position = 8
    delta = -1                 # 8 -> 0
```

The routine increments `0xCF50` and calls the associated update/feedback path.

### Low-two-bit mode 2

This mode sets:

```text
delta = -1
```

then increments `0xCF50` and calls the same associated path. The executable therefore gives mode 2 an openward-only animation direction; its higher gameplay identity remains unresolved.

## Open versus closed proved by collision behavior

The passability distinction is independently closed by the cell-traversal path around `0B164..0B18D`.

For a cell carrying flag `0x0080`, CODE.2 calls `1E930(cell_index)`. That helper searches runtime group 2 and returns:

```text
matching_record[6] << 12
```

or `0x8000` when no group-2 record exists.

The traversal path treats return value **zero** as nonblocking and any nonzero value as blocking. Therefore:

```text
record[6] == 0  -> open / passable endpoint
record[6] == 8  -> closed / blocking endpoint
record[6] 1..7  -> in motion and still blocking in this traversal path
```

This is executable behavior, not an automap-label inference.

## Automap classes `0x02` and `0x04`

Routine `01558` uses the same `1E930` result for explored special cells:

```text
result == 0x8000 -> cache class 0x04
otherwise        -> cache class 0x02
```

For cells backed by a real group-2 dynamic record, this means:

```text
class 0x04 -> fully closed endpoint (record[6] == 8)
class 0x02 -> open endpoint or transitional position 0..7
```

A missing group-2 record also produces `0x8000`, so class `0x04` must not be defined globally as "closed door" without that qualification.

The connected-map resolver turns aligned class-2 and class-4 cells into two internal horizontal/vertical door-result pairs. The final CP437 exporter maps both pairs to the same horizontal (`0xCD`) or vertical (`0xBA`) door glyph, so the exported text automap intentionally does not preserve this dynamic open/closed distinction.

## Archive-wide initialization evidence

Across all 32 supplied `LEVELS.DAT` records, the legacy split yields:

```text
runtime-group-1 candidates : 1,335
runtime-group-2 candidates :   324
```

For the 324 group-2 candidates:

```text
byte +6 position:
    8 : 323 records
    0 :   1 record

byte +7 delta:
    0 : 324 records

byte +1 low two bits:
    1 : 323 records
    0 :   1 record

full byte +1:
    0x01 : 224
    0x05 :  99
    0x00 :   1
```

Thus **323/324** shipped group-2 records begin stationary at the closed/blocking endpoint, and all 323 use low-two-bit mode 1. This strongly corroborates the dynamic barrier/door interpretation while remaining a data-set observation rather than a format constraint.

The supplied `LEV022.DAT` contains no runtime-group-2 records and has `0xCF50 == 0`.

## Safe implementation boundary

A faithful implementation may expose group-2 fields as:

```text
position_0_to_8
animation_delta
```

and may describe endpoint 0 as open/passable and endpoint 8 as closed/blocking.

It should preserve the complete byte `+1` and the other unresolved bytes. In particular, bit `0x04`, bit `0x40`, and low-two-bit mode 2 have proved control-flow roles but should not receive stronger editor-facing names until their remaining callers are closed.
