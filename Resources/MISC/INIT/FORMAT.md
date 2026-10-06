# INIT partial reconstruction

Status: **the leading 0x24-byte table is directly consumed by CODE.2 as 18 little-endian 16-bit initialization values. Several entries are now tied to the RES interaction/text/audio system; the rest remain preserved with behavioral/global labels.**

The supplied `INIT` begins with:

```text
offset  value
+00      15
+02     168
+04     336
+06       0
+08    2169
+0A    3316
+0C    4158
+0E     600
+10      49
+12      49
+14      69
+16     166
+18     192
+1A     266
+1C      32
+1E     103
+20      48
+22     414
```

CODE.2 `16D04..17049` reads these words individually from file offsets `0x00,0x02,...,0x22` into engine globals.

## RES interaction/audio fields now resolved

```text
INIT +0x1C = 32  -> global 0x258C8
INIT +0x1E = 103 -> global 0x258CC
INIT +0x20 = 48  -> global 0x25614
INIT +0x22 = 414 -> global 0x7FCC
```

### `+0x1E`: archive-9 interaction resource base

Wordcode drawing paths use `0x258CC` as the base for typed archive-9 resources. The supplied value is 103, so the interaction resource region begins at `RES9:103`.

### `+0x22`: archive-9 wordcode displacement

Raw wordcode loader `13F70` seeks archive-9 entry:

```text
0x258CC + 0x7FCC + program_id
```

With the supplied INIT values:

```text
103 + 414 = 517
```

which exactly equals the first non-EH wordcode entry `RES9:517`. Thus `+0x22` is safely the displacement/count from the interaction resource base to the program bank in the supplied data.

### `+0x1C`: sound-effect base

Wordcode opcode `0x1C` adds its encoded sound ID to global `0x258C8` before direct playback through `27F24`. The supplied base is 32.

### `+0x20`: second sound/index base

A dialog/audio path adds program global `0x7FF8` and a local sound ID to `0x25614` before direct playback. The supplied base is 48. A stronger historical name is deferred until all consumers are reconciled.

## Safety boundary

The remaining leading words are proven initialization values because CODE.2 reads them explicitly, but they should not receive semantic resource-family names merely from numeric coincidence. The remainder of the 1,879-byte `INIT` file also requires separate structural reconstruction.
