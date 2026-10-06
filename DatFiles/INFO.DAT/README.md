# INFO.DAT

Status: **reader located in certified CODE.2; fixed 13-byte configuration prefix and length-prefixed installation/resource path are proven.**

- Size: 17 bytes
- SHA-256: `4ffa6ac5d5b5afd103bd67d84d3a372b401b3b19407b0c68c44bb0dfd9e32c30`
- Exact bytes: `30 30 30 30 30 30 30 30 30 31 31 31 31 02 00 44 3A`.

CODE.2 `2E768..2E7EE` opens `INFO.DAT` and reads it as:

```text
+0x00  uint8 config_digits[13]
+0x0D  int16 path_length
+0x0F  uint8 path[path_length]
```

For the supplied file:

```text
config_digits = "0000000001111"
path_length   = 2
path          = "D:"
```

After reading the first 13 bytes, CODE.2 subtracts ASCII `'0'` from the configuration bytes in memory, proving that this prefix is a 13-element decimal-digit configuration vector rather than an arbitrary string. Individual digit meanings remain to be assigned from their consumers.

The trailing path has a direct cross-file use. LEVELS event opcode 11's numbered interactive-resource path reaches CODE.2 `139AC`, which copies the INFO.DAT path, appends `"\\"`, then appends a two-digit resource number. With this sample, numbered event 7 resolves to:

```text
D:\\07
```

Thus the old installation/path hypothesis is now proved: the length-prefixed tail is the resource/install path prefix used to locate numbered game resources.

Next work for INFO.DAT is to trace the 13 normalized configuration digits to their individual sound/video/input/install settings while preserving the exact byte representation for round-trip reconstruction.
