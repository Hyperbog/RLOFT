# RES1 raw four-byte sentinel entries

Status: **preserved but inactive/unconsumed in the supplied CODE.2 resource paths.** Their exact historical producer-side meaning remains unknown.

## Physical records

`RES1` contains two non-empty entries that do not begin with the normal `EH` resource envelope:

```text
RES1:381  60 00 00 00
RES1:427  60 00 00 00
```

Each is immediately preceded by a zero-length entry:

```text
RES1:380  empty
RES1:381  60 00 00 00
...
RES1:426  empty
RES1:427  60 00 00 00
```

`RES1:427` is the final archive entry.

The remaining non-empty RES1 entries are ordinary typed resources, overwhelmingly type-1 graphics. This makes the two four-byte records intentional archive anomalies/markers rather than evidence that RES1 is generally a mixed raw-data archive.

## Why the normal loader cannot consume them

CODE.2 `2EB60` is the common typed-entry reader. It reads the five-byte envelope and requires the first word to equal `"EH"` before dispatching resource types 1..7.

These entries are only four bytes long and begin with `60 00`, not `45 48`. Passing either through the normal typed path would therefore fail the envelope test and enter the resource-error path rather than produce a valid object.

## Raw archive access audit

CODE.2 helper `2EC74(archive)` exposes a raw archive handle. A direct call-target scan of the complete certified CODE.2 object finds exactly one call to `2EC74`:

```text
13F76: call 2EC74
```

That caller pushes archive number **9** and is the already reconstructed interaction-wordcode loader `13F70`.

There is no direct CODE.2 raw-handle request for archive 1.

Therefore the two RES1 raw entries are not reachable through either of the executable's established resource paths:

- typed `EH` loader: incompatible envelope;
- raw archive loader: archive 1 is never requested.

This is negative evidence about the supplied executable, not proof that no other DreamForge tool/build ever used the records.

## Safe preservation rule

Do not delete, rewrite, convert, or reinterpret the values as graphics merely because they occur inside RES1.

For this Ravenloft executable they should be modeled as:

```text
raw 4-byte archive records
serialized value = 0x00000060
runtime consumer = not established / inactive in supplied CODE.2
historical producer-side meaning = UNKNOWN
```

Round-trip tools must preserve the preceding empty entries and the four raw bytes exactly.
