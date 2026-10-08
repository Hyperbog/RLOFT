# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 03 — Resource Archives, Cache Lifecycle, Resource Types, and Runtime Resolution

**Status:** PRIMARY RESOURCE-ARCHITECTURE RECONSTRUCTION  
**Documentation branch:** `code1-decompilation`  
**Pre-documentation baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`

---

## 1. Scope

This chapter documents the layer immediately above the protected-mode memory system reconstructed in Chapter 02:

- RES archive organization;
- archive opening and indexing;
- loaded-resource cache state;
- load/lock/unlock/evict/release behavior;
- normal typed `EH` resources;
- raw/non-`EH` resource access;
- decompression;
- resource-ID namespace translation;
- INIT-driven resource bases;
- RES3 scene-shape ownership;
- SINTAB renderer microcode coupling;
- inactive/legacy resource generations.

The immutable resource binaries and certified CODE.1/CODE.2 reconstructions remain controlling evidence. The existing `Resources/` workstream is treated as primary project evidence where its claims are executable-backed and byte-validated. Sources: [Resources README](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/README.md), [Resources final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md).

---

## 2. The shipped resource corpus

The supplied resource/support set contains:

```text
13 CINE*.ANM files
13 RES archives, RES0..RES12
 2 ARIA MIDI support banks, GM1.BNK / GM2.BNK
 5 standalone support files:
     COLORS
     INIT
     SAMPLE.AD
     SINTAB
     T
```

The RES archives alone contain:

```text
13 archives
99,663,006 bytes
12,562 entries
12,464 EH entries
21 zero-length entries
77 non-empty non-EH entries
```

Source: [Resources/RES/INVENTORY.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/INVENTORY.md).

The physical existence of thirteen RES files should not be confused with the active archive set used by the supplied executables. Certified project analysis shows the active indexed resource arrangement is the later RES0..RES10 generation; RES11 and RES12 preserve historically meaningful predecessor/composite content and are not ordinary active archive groups in the supplied CODE pair. Sources: [Resources final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md), [RES11/RES12 lineage](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/RES11_RES12_LINEAGE.md).

**Evidence class:** BYTE-PROVEN for corpus geometry; CROSS-CORROBORATED for active-versus-legacy role.

---

## 3. Outer RES archive format

Every supplied RES archive has the same outer structure:

```text
int32 entry_count
int32 entry_offsets[entry_count]
byte  entry_payloads[]
```

The offsets are absolute little-endian file positions.

Required invariants for the supplied corpus:

```text
entry_offsets[0] == 4 + 4 * entry_count
offsets are non-decreasing
entry i occupies [offset[i], offset[i+1])
last entry extends to EOF
duplicate adjacent offsets are valid zero-length entries
```

The 21 zero-length entries are therefore part of the format, not corruption to repair. The parser/round-trip tooling reproduces every archive byte-for-byte. Source: [Resources/RES/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/FORMAT.md).

This gives the first fundamental lookup operation:

```text
(archive_id, entry_id)
    |
    v
absolute entry byte range
```

Everything above the outer archive—typed decoding, raw VM loading, renderer interpretation—is layered over that pair.

---

## 4. CODE.1 exposes the cache architecture directly

CODE.1 contains one of the clearest reconstructions of the original resource-manager lifecycle. The canonical source models the relevant family at logical `0x8A04..0x95A7`. [Code1.cpp](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

The runtime creates one 16-byte group record per configured resource group.

### 4.1 Group record

The reconstructed layout is:

```text
group_record +0x00  archive/file handle
             +0x04  loaded-entry count
             +0x08  total entry count
             +0x0C  pointer to 9-byte cache-entry array
```

This is directly visible in `function_8a04`, `function_8c40`, `function_8cd4`, and the cache routines. [Code1.cpp, resource-group initialization and shutdown](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

Initialization:

1. allocates `groups * 16` bytes;
2. constructs each group path;
3. opens the archive;
4. reads the first dword as entry count;
5. sets loaded count to zero;
6. allocates `entry_count * 9` cache bytes;
7. initializes every cache slot as unloaded.

The group table base is stored at DGROUP `0x2B08`; group count at DGROUP `0x2B04`. Source: [Code1.cpp `function_8a04`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

### 4.2 Archive path routing

Before opening resource groups, CODE.1 reads a 13-byte routing/configuration array and a variable-length path prefix. The thirteen bytes are converted from ASCII digits by subtracting `'0'`. Per group, the corresponding routing byte selects whether path construction uses the direct prefix or prepends an alternate base path before the resource prefix/group number. [Code1.cpp `function_8978` and `function_8a04`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

The exact historical source-level name of this routing file/table is not promoted here. What is machine-proven is that resource-group location is configurable per group rather than hard-wired to a single directory.

---

## 5. The 9-byte cache-entry format

Each CODE.1 resource entry has a 9-byte runtime cache record:

```text
+0x00  uint8 flags_and_type
+0x01  uint8 cleared/auxiliary byte
+0x02  uint8 state_class
+0x03  uint16 age_stamp
+0x05  uint32 object_pointer
```

The exact executable behavior permits stronger interpretation of several subfields.

### 5.1 `+0x00`: type nibble and lock bit

On successful load, the low nibble is replaced with the loaded resource type:

```text
flags_and_type =
    (old_flags & 0xF0)
  | (resource_type & 0x0F)
```

Bit `0x10` is the cache lock/pin bit:

```text
0x9440  lock    -> flags |= 0x10
0x9488  unlock  -> flags &= ~0x10
```

Release refuses to unload an entry while that bit is set. [Code1.cpp `function_91d8`, `function_9440`, `function_9488`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

The remaining high flag bits should not be assigned invented source names without additional evidence.

### 5.2 `+0x01`

The successful load path writes a 16-bit value at `+0` after constructing only the type/flag low byte, which clears byte `+1`. The semantic purpose of byte `+1` is not independently named by current evidence.

Safe classification:

```text
cache byte +1
    = explicitly cleared on load
    = historical source meaning UNKNOWN
```

### 5.3 `+0x02`: state/class byte

Initialization sets it to zero.

A successful normal load sets it to one. The public accessor `0x954C` returns the object pointer only if this byte is exactly one. Release also passes the byte to the type-specific release boundary. The eviction routine can select entries by a requested value of this byte. [Code1.cpp `function_8e3c`, `function_91d8`, `function_9378`, `function_954c`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

Thus `state_class` is safer than prematurely naming the field a simple Boolean.

### 5.4 `+0x03..+0x04`: age stamp

Every load or cache hit updates this word with:

```text
tick_7F50() >> 5
```

The timestamp is therefore intentionally coarse and 16-bit. [Code1.cpp `function_8e3c`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

The eviction path chooses the matching unlocked entry with the smallest 16-bit age value. [Code1.cpp `function_9378`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

This is an age-based eviction policy, but it should not be generalized into a modern “LRU cache” label without preserving the original 16-bit wrap and coarse-tick behavior.

### 5.5 `+0x05..+0x08`: loaded object pointer

The type-specific loader's returned guest pointer is stored here. Release clears it back to zero.

---

## 6. CODE.1 load path

The public CODE.1 accessor is `0x954C`.

Conceptually:

```text
get_resource(index, group)
    |
    v
8E3C ensure-loaded
    |
    v
if cache.state == 1
    return cache.object_pointer
else
    return 0
```

Source: [Code1.cpp `function_954c`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

### 6.1 Cache hit

If `entry +2` is nonzero, `0x8E3C` does not reload the object. It only refreshes the age stamp.

### 6.2 Cache miss

On an unloaded entry:

1. optional preload callback is invoked;
2. `0x8D0C` seeks to the archive entry;
3. the first five bytes of the typed header are read;
4. the signed 16-bit type-header size is inspected;
5. type-header bytes are separately allocated/read when nonzero;
6. the `EH` magic is checked;
7. type byte is dispatched through a seven-way loader table;
8. returned object pointer is stored;
9. state becomes loaded;
10. low type nibble is installed;
11. lock bit is cleared;
12. temporary type-header memory is freed;
13. group loaded-count increments;
14. age stamp is updated.

Sources: [Code1.cpp `function_8d0c` and `function_8e3c`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp), [CODE.1 final audit cache checkpoint](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

The final CODE.1 semantic audit specifically corrected the public accessors so they call the real loader rather than a generic release boundary. That correction is preservation-significant because it proves the intended on-demand cache semantics. [CODE.1 final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

---

## 7. Locking, eviction, and release

### 7.1 Lock

`0x9440` sets bit `0x10` when the entry is loaded.

### 7.2 Unlock

`0x9488` clears the lock bit.

### 7.3 Global unlock sweep

`0x94D0` scans every resource group and every entry, unlocking everything except one currently selected group-0 entry held in DGROUP `0x2B10`. [Code1.cpp](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

### 7.4 Release

`0x91D8` refuses to release:

- a locked entry;
- an unloaded entry.

Otherwise it:

1. extracts type from the low nibble;
2. invokes type-specific release using type/object/state;
3. clears state;
4. clears age;
5. clears object pointer;
6. decrements the group's loaded-entry count.

### 7.5 Eviction scan

`0x9378` scans all groups whose loaded count is nonzero and searches entries where:

```text
entry.state_class == requested_kind
AND lock bit is clear
```

It selects the numerically smallest age stamp and releases that entry.

The audit corrected these loops to preserve signed loop termination and original 16-bit age behavior. [CODE.1 final audit cache checkpoint](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

### 7.6 Full shutdown

`0x8B94` unlocks and releases all entries.

`0x8C40` then:

- closes every archive handle;
- clears count fields;
- frees every 9-byte cache array;
- frees the group table;
- clears the global group-table pointer.

[Code1.cpp `function_8b94` and `function_8c40`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

This is a complete original cache lifecycle, not merely a file-decoding utility.

---

## 8. Normal typed resources use the `EH` envelope

CODE.2's typed reader at `0x2EB60..0x2EC72` defines the common inner format:

```text
byte   magic[2]             // 'E','H'
uint8  resource_type
int16  type_header_size     // physical word, sign-extended at runtime
byte   type_header[type_header_size]
byte   payload[]
```

The signed interpretation matters. CODE.2 uses `MOVSX` for the header-size field. A faithful implementation must therefore preserve the original signed-width behavior rather than silently promoting the field to an unsigned host size. [Resources/RES/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/FORMAT.md), [Resources full re-audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FULL_REAUDIT_2026-10-06.md).

The type dispatcher at `0x2EDDA..0x2EEF7` supports types 1 through 7.

| Type | CODE.2 loader | Supplied entries | Runtime role |
|---:|---|---:|---|
| 1 | `2FD9C` | 1,591 | raster/image resource |
| 2 | `301A8` | 15 | font widths + type-1 raster |
| 3 | `286C4` | 172 | XMIDI/XDIR sequence |
| 4 | `28020` | 1,005 | Creative Voice File audio |
| 5 | `374E0` | 0 | executable-supported structured record |
| 6 | `37590` | 1,319 | first-person texture |
| 7 | `37590` | 8,362 | compiled scene shape/display list |

Source: [Resources/RES/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/FORMAT.md).

---

## 9. Type 1 — raster/image resource

Supplied type-1 resources use an 8-byte type header:

```text
+0x00 int16 count
+0x02 int16 height
+0x04 uint8 width_units
+0x05 uint8 field_5
+0x06 uint8 compression
+0x07 uint8 field_7
```

CODE.2 sign-extends both `count` and `height`.

Decoded bytes per element:

```text
height * width_units * 8
```

Source: [Resources/RES/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/FORMAT.md).

Compression selectors:

### Method 0 — raw

113 supplied resources.

### Method 1 — signed-control RLE

Executable-supported but absent from the supplied corpus:

```text
c = int8(next_byte)

if c >= 0:
    copy c + 1 literal bytes
else:
    repeat next byte 1 - c times
```

Thus control `0x80` repeats 129 bytes. It must not be substituted with standard PackBits semantics.

### Method 2 — encoded error path

No supplied resource uses it.

### Method 3 — 4 KiB LZSS

1,478 supplied type-1 resources.

---

## 10. Shared LZSS decoder and its non-obvious initialization edge

CODE.2 routine `0x37700` implements the LZSS family used by:

- type-1 method 3;
- all supplied type-2 raster bodies;
- type 6;
- type 7.

Decoder characteristics:

```text
history size        4096 bytes
initial cursor      0xFEE
flags               LSB first
flag 1              literal
flag 0              2-byte backreference
offset              lo | ((hi & 0xF0) << 4)
length              (hi & 0x0F) + 3
```

The adversarial re-audit found a subtle but important edge:

```text
history 0x000..0xFED = initialized to 0xFE
history 0xFEE..0xFFF = not initialized by 37700
cursor begins at 0xFEE
```

The permanent validator now treats those final 18 bytes as undefined until written. All 11,174 shipped LZSS streams pass without reading undefined history and without trailing compressed bytes. Sources: [Resources final audit addendum](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md), [Resources postfix certification](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_POSTFIX_CERTIFICATION_2026-10-06.md).

A modern decoder that initializes the entire 4 KiB ring to `0xFE` will decode the supplied files identically, but it is **not** a literal reconstruction of the original routine. Preservation code should record that difference.

---

## 11. Type 2 — font

Type 2 has a 136-byte header:

```text
+0x00 uint8 glyph_widths[128]
+0x80 int16 count
+0x82 int16 height
+0x84 uint8 width_units
+0x85 uint8 field_5
+0x86 uint8 compression
+0x87 uint8 field_7
```

CODE.2 preserves/copies the 128 glyph-width bytes, then points the image loader at the type-1-compatible descriptor beginning at `+0x80`. All 15 supplied fonts use compression method 3. Source: [Resources/RES/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/FORMAT.md).

---

## 12. Types 3 and 4 — audio payload wrappers

### Type 3 — XMIDI/XDIR

```text
uint32 blob_size
byte blob[blob_size]
```

All 172 supplied records begin with `FORM ... XDIR`. The type-specific loader connects to the music subsystem. Source: [Resources/RES/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/FORMAT.md).

### Type 4 — Creative Voice File

Also:

```text
uint32 blob_size
byte blob[blob_size]
```

All 1,005 supplied resources carry the Creative Voice File signature.

Two byte-identical legacy records in RES11/RES12 contain a genuine shipped length anomaly:

```text
declared size   0x11669
physical blob   0x01669
difference      0x10000
```

The anomaly is preserved; tools must not “repair” the source bytes. Source: [Resources/RES/INVENTORY.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/INVENTORY.md).

---

## 13. Type 5 — executable-supported but absent

No supplied Ravenloft resource instantiates type 5.

The loader at `0x374E0` proves only the physical contract of at least 28 header bytes plus a variable tail. Its original historical field names remain unknown. [Resources/RES/EXECUTABLE_ONLY_FORMATS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/EXECUTABLE_ONLY_FORMATS.md).

This is a useful example of preservation discipline:

> executable support does not justify inventing content semantics when no shipped instance exists.

---

## 14. Types 6 and 7 share the loader but not the meaning

Both types use:

```text
uint16 decoded_size
byte compressed_data[]
```

The runtime allocation retains the two-byte decoded-size word followed by the decoded body. Both are decoded by `0x37590 -> 0x37700`. [Resources/RES/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/FORMAT.md).

### Type 6 — first-person texture

Decoded bodies are:

```text
4096 bytes = 32 columns * 128 texels
8192 bytes = 64 columns * 128 texels
```

The renderer addresses one 128-byte vertical source column at a time. Source: [Resources/RES/TYPE6_RENDERER.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE6_RENDERER.md).

### Type 7 — compiled scene shape

Decoded body:

```text
+0x00 uint16 source_width
+0x02 int16  x_min
+0x04 int16  x_max
+0x06 int16  y_min
+0x08 int16  y_max

then:
uint16 relative_column_pointer[x_max-x_min+1]

then:
zero-terminated lists of 6-byte draw descriptors
```

Each descriptor contains:

```text
int16 raster_stop_selector
int16 signed_adjustment
int16 raster_entry_selector
```

Source: [Resources/RES/TYPE7_RENDERER.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_RENDERER.md).

Type 7 is therefore not a conventional decoded bitmap. It is a compiled draw-list representation whose selectors address scale-dependent executable rasterizer fragments stored in SINTAB.

---

## 15. SINTAB makes the renderer partly data-driven executable code

SINTAB contains 127 scale blocks.

Each block contains:

```text
129-word table
129-word table
x86 rasterizer code
```

The type-7 selector fields span all 129 even selector values `0x0000..0x0100`.

CODE.2 uses the tables to select code offsets, temporarily patches a selected stop location with `RET`, runs the scale-specific micro-rasterizer, and restores the overwritten byte. [Resources final audit, renderer architecture](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md), [Resources/RES/TYPE7_RENDERER.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_RENDERER.md).

This is preservation-significant:

```text
type-7 resource data
      |
      v
display-list selectors
      |
      v
SINTAB scale block
      |
      v
external x86 rasterizer microcode
      |
      v
VGA output
```

A modern renderer will almost certainly replace the self-modifying execution mechanism, but should retain a compatibility model that produces the same geometry/pixel result from the same type-7/SINTAB inputs.

---

## 16. INIT is the namespace bridge

The first 18 words of INIT provide runtime resource-base values. CODE.2 reads them individually rather than treating the header as an opaque blob. [Resources/MISC/INIT/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/FORMAT.md).

Important bases:

```text
INIT +00 = 15    RES0 raster/UI base
INIT +02 = 168   RES1 object/inventory UI base
INIT +04 = 336   RES1 portrait base

INIT +06 = 0
INIT +08 = 2169
INIT +0A = 3316
INIT +0C = 4158
                 four RES3 scene-shape source bases

INIT +0E = 600   RES4 secondary/narrow texture base
INIT +10 = 49    alternate RES5/XMIDI bank offset

INIT +12 = 49
INIT +14 = 69
INIT +16 = 166
INIT +18 = 192
INIT +1A = 266   RES6 sampled-SFX namespace boundaries

INIT +1C = 32    RES9 graphics/resource base
INIT +1E = 103   RES9 interaction-resource base
INIT +20 = 48    RES10 interaction voice/SFX base
INIT +22 = 414   RES9 wordcode displacement
```

Source: [Resources/MISC/INIT/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/FORMAT.md).

This means many IDs in DAT records are **bank-local**, not direct outer archive indices.

The runtime performs:

```text
absolute_resource_id
    = namespace_base
    + serialized_local_id
    + optional runtime frame/variant adjustment
```

That pattern is central to the engine.

---

## 17. RES3 is partitioned into four shape namespaces

All 4,234 RES3 entries are type 7.

INIT partitions them exactly:

```text
class 0 base 0       -> 0..2168
class 1 base 2169    -> 2169..3315
class 2 base 3316    -> 3316..4157
class 3 base 4158    -> 4158..4233
```

These ranges cover RES3 with no gaps or overlap.

Proved source families:

```text
class 0  NPC/creature scene shapes
class 1  placed object/item family
class 2  loose/pickable OBJECTS family
class 3  transient/moving effects
```

Sources: [Resources/RES/TYPE7_SOURCE_BANKS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_SOURCE_BANKS.md), [Resources/MISC/INIT/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/FORMAT.md).

For example, `NPCS.DAT +0x14` is a class-0 bank-local shape base. CODE.2 adds the class base and later requests archive 3 through generic resource access. [Resources/RES/TYPE7_NPC_INDEX.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_NPC_INDEX.md).

This explains why unrelated serialized databases can legitimately reuse local shape ID zero.

---

## 18. RES1 contains independently bounded UI namespaces

INIT base 168 plus `OBJECTS.DAT +0x2D` maps all 336 supplied objects to:

```text
RES1:168..335
```

Source: [INIT UI/resource bases](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/UI_RESOURCE_BASES.md).

Portraits begin at 336:

```text
portrait_id =
    336
  + face_index
  + sex * 23

if status & 0x0100:
    portrait_id += 46
```

Therefore:

```text
RES1 336..381   normal portrait bank
RES1 382..427   possessed portrait bank
```

The two terminal female slots in each bank are deliberately invalid/reserved in the supplied data: one zero-length entry followed by one raw `0x00000060` record. Their producer-side literal meaning remains unknown. Source: [INIT UI/resource bases](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/UI_RESOURCE_BASES.md).

---

## 19. Raw access exists beside normal typed access

Not every archive entry goes through the `EH` type loader.

CODE.2 helper `0x2EC74` exposes a raw archive handle.

A whole-object scan found exactly one direct call in the certified CODE.2 object:

```text
13F76 -> 2EC74(9)
```

This is the interaction/display wordcode loader at `0x13F70`. Sources: [Resources/RES/NON_EH.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/NON_EH.md), [RES1 sentinel audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/RES1_SENTINELS.md).

The active raw wordcode entry is:

```text
uint16 word_count
uint16 program_words[word_count]
uint16 companion_words[word_count]
```

CODE.2 reads only:

```text
word_count
program_words[]
```

It does not read the equal-length companion bank. All companion words are zero in the supplied corpus. Their historical editor/producer meaning remains UNKNOWN. [Resources/RES/NON_EH.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/NON_EH.md).

This is another reason a future modding API should distinguish:

```text
typed resource access
raw archive-entry access
```

rather than forcing every entry into one typed decoder.

---

## 20. Active RES9 wordcode indexing

INIT establishes:

```text
interaction base     = 103
wordcode displacement = 414
```

Therefore:

```text
RES9 entry
    = 103 + 414 + program_id
    = 517 + program_id
```

Active program IDs `0..37` map exactly to:

```text
RES9:517..554
```

The supplied active VM contains:

```text
38 programs
11,396 program words
4,237 decoded instructions
42 opcodes (0x00..0x29)
0 parse errors
0 invalid direct control-flow targets
```

Source: [Resources/RES/WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/WORDCODE_VM.md).

The event/script VM will receive its own later chapter; here the important point is that it is part of resource resolution and bypasses the normal typed loader.

---

## 21. RES11 and RES12 are preserved historical generations

RES11 and RES12 are not garbage or disposable duplicates.

Project analysis proves that they retain an earlier/pre-split resource generation whose content later appears distributed among active RES3/4/5/6/9/10 families. RES12 also preserves a legacy version of the interaction VM where opcode `0x05` embeds a 141-word character template instead of the active one-word template ID. [Resources/RES/RES11_RES12_LINEAGE.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/RES11_RES12_LINEAGE.md), [Resources/RES/RES12_WORDCODE.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/RES12_WORDCODE.md).

Safe classification:

```text
RES11 / RES12
    = historically meaningful predecessor-generation content
    != normal active archive groups in supplied CODE.1/CODE.2
```

The exact earlier executable/editor/build that used them remains unknown.

---

## 22. Standalone support resources

Several important original formats sit outside RES.

### T

`T` is a 1,073-record indexed text archive whose record payload is XORed with `0xDC`. Twenty-five entries are legitimately zero-length. Unindexed residue/slack must remain preserved rather than interpreted as hidden dialogue. [Resources final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md).

### COLORS

```text
uint8 palette_count = 15
15 * 256 * RGB triples
```

All components are in VGA DAC range `0..63`. [Resources final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md).

### INIT

INIT is both initialization data and a cross-file namespace map. Its resource bases are not optional metadata; they determine how DAT-local IDs resolve into RES entries.

### SINTAB

SINTAB is renderer microcode and lookup data, not a normal static trigonometry-only table despite its filename.

### SAMPLE.AD

Identified as a Miles AIL 2.x YM3812/OPL2 Global Timbre Library with 233 directory entries and 14-byte timbres. [Resources final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md).

### GM1.BNK / GM2.BNK

These are ARIA General MIDI support banks used with the ARIA driver path. Their inner vendor grammar remains a third-party-runtime preservation question rather than DreamForge-native game logic. [Resources final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md).

---

## 23. ANM cinematics are a separate streamed resource family

The thirteen `CINE00.ANM..CINE12.ANM` files are not RES entries.

Their packet dispatcher supports image, delta, palette/timing, subtitle, music, and sound-effect commands. The format is byte-round-trippable across all supplied files. [Resources/ANM/README.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/ANM/README.md), [Resources/ANM/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/ANM/FORMAT.md).

ANM is therefore part of the broader resource system but has its own stream grammar and runtime player rather than using the indexed RES cache.

A later cinematic/audio chapter should document the chunk-state machine in full.

---

## 24. Cross-file dependency model

The current resource architecture can be represented as:

```text
                    INIT
                     |
     +---------------+----------------+
     |               |                |
     v               v                v
resource bases   scene bases      environment profiles
     |               |                |
     v               v                v
 RES0/1/5/6/9/10   RES3           RES8
     ^               ^
     |               |
OBJECTS/SAVE       LEVELS/NPCS/OBJECTS
     |
     v
bank-local IDs
     |
     v
absolute RES entry IDs
     |
     v
typed/raw loaders
     |
     +--> raster/font
     +--> music
     +--> sampled SFX
     +--> texture
     +--> compiled shape
     +--> interaction wordcode
```

Renderer-specific type 7 continues:

```text
RES3 type 7
    |
    v
compiled column lists
    |
    v
SINTAB selectors/microcode
    |
    v
VGA rasterization
```

This dependency graph is executable-backed; it is not a reconstruction of DreamForge's original source-tree filenames.

---

## 25. Native-port implementation requirements

The future faithful C++ port should preserve the original file model even if its in-memory architecture is cleaner.

A suitable implementation split would be conceptually:

```text
LegacyArchiveReader
    |
    +-- ResArchive
    +-- TextArchive
    +-- InitDatabase
    +-- PaletteBank
    +-- AnimationStream
    |
    v
LegacyResourceResolver
    |
    +-- namespace-base translation
    +-- raw entry access
    +-- typed EH dispatch
    |
    v
LegacyResourceDecoder
    |
    +-- type 1 raster
    +-- type 2 font
    +-- type 3 XMIDI
    +-- type 4 VOC
    +-- type 5 compatibility
    +-- type 6 texture
    +-- type 7 compiled shape
    |
    v
FaithfulRuntimeResourceCache
```

The names above are suggested modern implementation names, not recovered original symbols.

### 25.1 Preserve stable IDs

Resource identity should remain:

```text
(archive_id, entry_id)
```

with namespace translations represented explicitly rather than flattening all resources into one generated global numbering scheme.

### 25.2 Preserve raw access

The port must allow raw entry reads for resource families such as RES9 wordcode.

### 25.3 Preserve source bytes

Unknown fields, zero-length entries, companion arrays, inactive markers, and legacy archives should remain byte-preservable even when the runtime ignores them.

### 25.4 Decouple cache policy from resource identity

The original age/lock cache can be reproduced for strict testing. A modern engine may eventually use a different memory policy, but that change belongs to the remaster/modernization layer—not the historical format model.

### 25.5 Keep decompression compatibility tests

Every decoder should be validated against the original corpus and, where possible, byte-for-byte original decoded output.

---

## 26. Modding implications

A safe modding model should expose:

```text
archive ID
entry ID
resource type
namespace/local ID where applicable
decoded logical representation
original serialized bytes
```

It should not silently:

- reorder archive entries;
- collapse duplicate offsets;
- repair invalid/reserved entries;
- rewrite unknown companion arrays;
- convert local IDs to unrelated global IDs without preserving the original mapping;
- replace type-7 compiled shapes with ordinary bitmaps inside the legacy compatibility layer;
- normalize the two malformed legacy VOC length fields.

Modern asset overrides can exist above this layer later.

---

## 27. Historical unknowns retained

The resource architecture is functionally closed for the supplied build, but several historical questions remain:

1. producer/editor purpose of the unused RES9 companion word arrays;
2. original meaning of RES1 terminal `0x00000060` markers;
3. original source names for type-1 bytes `+0x05/+0x07`;
4. source-level field semantics of unsupported-in-corpus type 5;
5. exact older executable/editor that produced or consumed RES11/RES12;
6. higher-level authoring meaning of unused ANM chunk type 4;
7. exact ARIA GM1/GM2 bank grammar.

These remain candidates for later historical archaeology and must not be filled with speculative labels.

---

## 28. Source ledger

### Primary project sources

- [Resources/README.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/README.md)
- [Resources/FINAL_RESOURCE_AUDIT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md)
- [Resources/FULL_REAUDIT_2026-10-06.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FULL_REAUDIT_2026-10-06.md)
- [Resources/RES/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/FORMAT.md)
- [Resources/RES/INVENTORY.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/INVENTORY.md)
- [Resources/RES/NON_EH.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/NON_EH.md)
- [Resources/RES/TYPE6_RENDERER.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE6_RENDERER.md)
- [Resources/RES/TYPE7_RENDERER.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_RENDERER.md)
- [Resources/RES/TYPE7_SOURCE_BANKS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_SOURCE_BANKS.md)
- [Resources/RES/WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/WORDCODE_VM.md)
- [Resources/MISC/INIT/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/FORMAT.md)
- [Resources/MISC/INIT/UI_RESOURCE_BASES.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/UI_RESOURCE_BASES.md)
- [Resources/ANM/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/ANM/FORMAT.md)
- [Code1/Code1.cpp](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)
- [Code1/FINAL_BINARY_AUDIT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md)
- [Code2/CODE2_CONTINUITY_ADDENDUM.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md)

---

## 29. Architectural checkpoint

The original resource path is now documented as:

```text
serialized archive/support files
       |
       v
archive index / direct stream
       |
       v
namespace translation from INIT/DAT state
       |
       v
typed EH loader OR raw-entry loader
       |
       v
decompression / type-specific materialization
       |
       v
runtime cache entry
       |
       +-- type
       +-- state
       +-- age
       +-- lock
       +-- loaded object pointer
       |
       v
renderer / UI / audio / VM / world consumers
```

The next preservation chapter should document the **graphics pipeline itself**: VGA mode/state, planar framebuffer organization, draw/page-flip architecture, type-1 and type-6 raster paths, type-7/SINTAB shape rendering, palettes, clipping, and the original 320×200/320×400 coordinate spaces.
