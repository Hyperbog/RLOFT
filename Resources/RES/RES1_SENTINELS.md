# RES1 raw four-byte sentinel entries

Status: **preserved and runtime-unconsumed by the supplied CODE.2 resource paths; now structurally located as the final slot of two parallel 46-slot character-portrait namespaces.** Their exact historical producer/editor meaning remains `UNKNOWN`.

## Physical records

```text
RES1:380  empty
RES1:381  60 00 00 00
...
RES1:426  empty
RES1:427  60 00 00 00
```

`RES1:427` is the final archive entry.

## Portrait-namespace context

`INIT +0x04 = 336` is directly loaded to DGROUP `0x258E4`. Character rendering computes:

```text
resource = 336 + 23 * sex_index + face_index
if (status_word & 0x0100)
    resource += 46
```

This produces two parallel 46-slot RES1 namespaces:

```text
normal          RES1:336..381
status-0x0100   RES1:382..427
```

Both have exactly the same physical tail:

```text
local slot 0..43   typed type-1 portrait resources
local slot 44      zero-length entry
local slot 45      raw uint32 0x00000060
```

Thus `RES1:381` and `RES1:427` are not randomly placed anomalies: they occupy the same terminal local slot in two parallel portrait banks. This establishes producer-side **layout context**, not the semantic meaning of value `0x60`.

See `RES0_RES1_NAMESPACES.md` for the full INIT/character mapping.

## Why the normal loader cannot consume them

CODE.2 `2EB60` is the common typed-entry reader. It requires the normal `EH` envelope before dispatching types 1..7.

The marker entries are only four bytes long and begin `60 00`, so they fail that envelope test.

## Raw archive access audit

CODE.2 helper `2EC74(archive)` exposes a raw archive handle. A direct whole-object call scan finds exactly one call to `2EC74`:

```text
13F76: call 2EC74
```

That caller passes archive **9** and is the reconstructed interaction-wordcode loader `13F70`. No raw archive-1 request exists in the certified CODE.2 object.

Therefore the two markers are not reachable through either established runtime path:

- typed `EH` path: incompatible record form;
- raw path: archive 1 is never requested.

This proves inactivity in the supplied executable. It does not prove that no DreamForge editor, packer, or alternate executable used the values.

## Safe preservation rule

Model each as:

```text
physical role        terminal raw slot in a 46-slot RES1 portrait namespace
serialized value     uint32_le 0x00000060
runtime consumer     none established in supplied CODE.2
historical meaning   UNKNOWN
```

Round-trip tools must preserve the preceding empty entry, the raw four bytes, and all archive offsets exactly.
