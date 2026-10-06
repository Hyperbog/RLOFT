# LEVELS.DAT family C — loose/pickable item world records

Status: **family C is behaviorally identified as the loose/pickable item-instance family, with the same runtime records reused for dropped and thrown/moving items.** The name is based on direct CODE.2 consumers plus the supplied LEVELS.DAT/OBJECTS.DAT corpus.

## Identity and storage

Family C is counted by header word `0x258A4` and expands from archive 11-byte records to the 22-byte runtime array at DGROUP `0x17618`.

Archive compact form:

```text
+0x00  uint16 object_type_id
+0x02  int16  x_cell
+0x04  uint16 x_fraction
+0x06  int16  y_cell
+0x08  uint16 y_fraction
+0x0A  uint8  compact_flags
```

Loader `18878..18918` produces the common runtime position/type structure:

```text
+0x00  y fixed-point position
+0x04  x fixed-point position
+0x0C  cell index = y_cell * map_width + x_cell
+0x0E  OBJECTS.DAT type ID
+0x10  zeroed on legacy expansion
+0x14  runtime flags masked with 0xFE07
```

The legacy conversion does **not** import compact byte `+0x0A`. All 622 supplied archive family-C records store zero there, so current archive evidence is consistent with runtime-only movement/orientation state. Lossless tooling must still preserve the byte.

The loader also remaps compact type `0x54` to runtime type `0x58`. The supplied archive contains one such `0x54` family-C record. This is an executable-proved legacy migration rule; its historical design reason is not yet established.

## Direct behavioral proof

### Loose-item insertion

`2195C..21ADA` appends a record to family C using an OBJECTS.DAT type ID and world position. It computes the cell index, stores the position/type fields, initializes runtime flags, and increments `0x258A4`.

The family has an encoded runtime ceiling of 300 records (`0x12C`). When already full, the routine discards the oldest 22-byte record by shifting the remaining 299 records (`0x19B2` bytes) down before appending the new one. `23B38` uses the same 300-record policy.

### Pickup/removal

`21704` removes a selected family-C record. Before compaction it passes the record's type and a flag-derived value through `1A344`; it then memmoves the later 22-byte records down and decrements `0x258A4`.

The first-person selection path at `224FA..22566` proves the gameplay role. A rendered scene entry of class 2 resolves to a family-C index, calls `21704`, then obtains the corresponding OBJECTS.DAT name for the following UI/message path. This is direct evidence that family-C records are **pickable item instances**, not merely visual effects.

### Rendering

`2BDF4..2BFDC` walks family C, requires the record's cell to have current visibility bit `0x0040`, indexes OBJECTS.DAT with `type_id * 0x41`, and emits a scene/render entry whose source class is 2. The renderer therefore treats family C as its own world-object source, distinct from family B.

### Thrown/moving-item state

`23B38` creates an active family-C record with world position, object type, heading/direction fields, a motion/lifetime word, and byte `+0x15` bit `0x02` set. Wrapper `23D84` derives a heading from the current view/input state before calling this creator.

`2388C` advances those active records, updates fixed-point position and cell index, performs collision/contact handling, decrements the motion word, and can clear the active-motion flag. It contains item-type-specific handling for OBJECTS.DAT IDs including:

```text
0x57  AXE OF HURLING
0x58  ARROW OF SLAY UNDEAD
0x59  DAGGER OF THROWING
0x5A  JAVELIN OF LIGHTNING
```

The periodic update path `1BF4D..1BFA0` scans family C for the same active flag, calls `2388C`, compacts completed records when required, and decrements the family count.

Thus thrown/projectile behavior is not a separate serialized family: it is a runtime state layered onto the same loose-item records.

## Archive-wide corpus

The supplied 32-level archive contains:

```text
family-C records       622
levels with family C    26
maximum in one level    90
unique object type IDs 200
type-ID range        0..335
OBJECTS.DAT entries    336
compact flags           0 for all 622 records
```

Every family-C type ID is valid in supplied OBJECTS.DAT, and every stored cell coordinate is in map bounds.

The most common family-C types are recognizably loose inventory/ammunition objects:

```text
85  STONE
48  POTION OF EXTRA HEALING
37  ARROW
32  POTION OF HEALING
21  SLING BULLET
19  OIL OF FIERY BURNING
15  BAROVIAN COIN
```

Family C also reaches the full supplied OBJECTS.DAT namespace through ID 335. This contrasts with family B: 4,324 supplied family-B records use IDs only `0..174`, and their compact flag byte spans `0..7` rather than being uniformly zero.

## Safe semantic distinction

The current evidence supports these names:

- **family B:** placed object/item map records, with serialized placement/orientation flags and automap overlay behavior;
- **family C:** loose/pickable item world records, also used for runtime drops and thrown/moving items.

Do not collapse the two families merely because both use OBJECTS.DAT IDs and the common 22-byte world-record geometry. Their loaders, flag handling, consumers, rendering source classes, mutation paths, and type-ID distributions are materially different.
