# INIT format reconstruction

Status: **the supplied 1,879-byte `INIT` file is structurally reconstructed end-to-end and its fixed header is substantially behaviorally mapped.** CODE.2 consumes an 18-word fixed header, two variable-count 7-byte scene-descriptor tables, and two environment-profile backdrop/panorama tables. `analyze_init.py` parses every byte and reserializes the file byte-identically.

## Top-level layout

```text
0x0000  uint16 init_word[18]                         // 0x24 bytes
0x0024  uint16 class1_descriptor_count = 175
0x0026  scene_descriptor class1[175]                // 7 bytes each
0x04EF  uint16 class3_descriptor_count = 12
0x04F1  scene_descriptor class3[12]                 // 7 bytes each
0x0545  uint16 environment_profile_count = 2
0x0547  environment_profile profiles[2]             // variable length
0x0757  EOF
```

No bytes remain outside those structures in the supplied file.

## Fixed 0x24-byte initialization table

The supplied words are:

```text
offset  value   CODE.2 destination / proved role
+00      15     0x258DC  RES0 type-1/raster graphics base
+02     168     0x258D8  RES1 OBJECTS inventory/UI graphics base
+04     336     0x258E4  RES1 character portrait base
+06       0     0x27100  RES3 source-class-0 base
+08    2169     0x27102  RES3 source-class-1 base
+0A    3316     0x27104  RES3 source-class-2 base
+0C    4158     0x27106  RES3 source-class-3 base
+0E     600     0x258F8  RES4 secondary/narrow texture-bank base
+10      49     -> music subsystem  alternate RES5/XMIDI bank offset
+12      49     0x258D0  RES6 sampled-SFX namespace base/boundary
+14      69     0x258C0  RES6 sampled-SFX namespace base/boundary
+16     166     0x258BC  RES6 sampled-SFX namespace base/boundary
+18     192     0x258D4  RES6 sampled-SFX namespace base/boundary
+1A     266     0x258C8  RES6 end / latent direct-SFX namespace base
+1C      32     0x258EC  archive-9 graphics/resource base
+1E     103     0x258CC  archive-9 interaction-resource base
+20      48     0x25614  archive-10 interaction voice/SFX base
+22     414     0x7FCC   archive-9 wordcode-bank displacement
```

CODE.2 `16D04..17049` reads these words individually. The destination mapping above follows the actual instruction sequence; it supersedes the earlier mistaken note that assigned `+0x1C` to `0x258C8`. The immutable object proves:

```text
16F70..16F9D  read INIT +0x1A -> DGROUP 0x258C8
16FA2..16FCA  read INIT +0x1C -> DGROUP 0x258EC
16FCF..16FF2  read INIT +0x1E -> DGROUP 0x258CC
```

### `+0x00 = 15`: RES0 raster base

RES0 contains exactly 15 type-2 fonts at entries `0..14` followed by 126 type-1 raster resources at `15..140`. Graphics paths add local image IDs to `0x258DC` before archive-0 drawing. Thus `15` is the exact first raster-resource index.

### `+0x02 = 168`: RES1 OBJECTS UI base

Inventory/equipment paths read `OBJECTS.DAT +0x2D`, add global `0x258D8`, and load archive 1. All 336 supplied objects store local UI indexes `0..167`, resolving exactly to `RES1:168..335`.

### `+0x04 = 336`: RES1 portrait base

Character presentation computes:

```text
portrait_id = 336 + 23 * sex_index + int8(face_index)
if (status_word & 0x0100)
    portrait_id += 46
```

The two resulting 46-slot ranges are `RES1:336..381` and `RES1:382..427`. Each consists of 44 typed portrait resources, one empty entry, and a terminal raw `0x00000060` record. `T[27]/T[28]` prove the selector at character `+0x13` is MALE/FEMALE; `T[15..20]` prove `+0x14` is the six-value race selector. See `Resources/RES/RES0_RES1_NAMESPACES.md`.

### `+0x06..+0x0C`: complete RES3 scene-shape partition

The four bases are indexed by source class through common helper `2B2F4`:

```text
class 0     RES3 0..2168       2169 entries
class 1     RES3 2169..3315    1147 entries
class 2     RES3 3316..4157     842 entries
class 3     RES3 4158..4233      76 entries
                                   ----
                                   4234
```

Proved source families:

```text
class 0  LEVELS family A / NPCS.DAT NPC-creature scene shapes
class 1  LEVELS family B / first INIT scene-descriptor family
class 2  LEVELS family C loose/pickable OBJECTS family
class 3  transient/moving-effect descriptor family
```

### `+0x0E = 600`: RES4 type-6 texture split

CODE.2 adds `0x258F8` to local selectors before archive-4 loads. The supplied RES4 archive contains exactly:

```text
RES4:0..599    600 type-6 resources decoding to 8192 bytes = 64 x 128 texels
RES4:600..666   67 type-6 resources decoding to 4096 bytes = 32 x 128 texels
```

Thus `600` is the exact base of the secondary/narrow 32-column texture namespace.

### `+0x10 = 49`: alternate RES5/XMIDI bank offset

During initialization this value is passed to `288F0` and retained by the music subsystem. Music loaders `28C54` and `28E58` add it to the sequence ID for specific driver modes before loading archive 5. RES5 contains 98 type-3/XDIR resources, so `49` divides it exactly into two 49-entry banks. A driver-facing source name is intentionally not invented.

### `+0x12..+0x1A`: RES6 sampled-SFX namespace boundaries

Initialization stores these values as:

```text
+12  49  -> 0x258D0
+14  69  -> 0x258C0
+16 166  -> 0x258BC
+18 192  -> 0x258D4
+1A 266  -> 0x258C8
```

Consumers add local sound IDs and route them through direct/queued sampled-audio playback (`27F24`, `28110`, `28164`). The sound subsystem is configured for archive 6, whose supplied corpus is exactly 266 type-4/VOC resources (`0..265`). These values therefore define SFX namespace bases/boundaries inside that archive; `266` equals the archive count and is one-past-end for this supplied set.

The executable-supported wordcode opcode `0x1C` adds its operand to `0x258C8`; no supplied active wordcode program uses that opcode. Therefore `+0x1A=266` is preserved as a latent direct-SFX namespace base/end boundary rather than assumed to identify an extant RES6 resource.

### `+0x1C = 32`: archive-9 graphics/resource base

Global `0x258EC` is used with archive 9 by UI/display paths such as `1FB1E`, `2CF24`, and `2D1AD`. The supplied RES9 entries before the wordcode bank are type-1 graphics, so this is safely an archive-9 graphics/resource base. A narrower historical subsystem name remains open.

### `+0x1E = 103` and `+0x22 = 414`: active interaction/wordcode bases

Wordcode graphics use `0x258CC` as the archive-9 interaction-resource base. Raw wordcode loader `13F70` seeks:

```text
0x258CC + 0x7FCC + program_id
= 103 + 414 + program_id
= RES9:517 + program_id
```

This proves active program IDs `0..37` are exactly `RES9:517..554`.

### `+0x20 = 48`: archive-10 interaction voice/SFX base

Dialog/audio paths compute `48 + program_sound_subbase + local_sound_id`, temporarily select archive 10 via `280DC(10)`, and play through `27F24`. RES10 contains 219 type-4/VOC resources. Thus this word is the archive-10 interaction voice/sampled-audio base.

## Scene-descriptor tables

After the fixed header CODE.2 continues reading from the same open stream.

### Source-class-1 table

```text
INIT +0x0024  uint16 descriptor_count = 175
INIT +0x0026  descriptor[175]          // 7 bytes each
```

### Source-class-3 table

```text
INIT +0x04EF  uint16 descriptor_count = 12
INIT +0x04F1  descriptor[12]           // 7 bytes each
```

Shared physical descriptor:

```text
+0x00 uint16 local_shape_base
+0x02 uint16 spatial_extent_or_fixed_point_field
+0x04 uint8  shape_stride
+0x05 uint8  variant_count
+0x06 uint8  scene_influence_step
```

Executable evidence establishes base+variant¥ÍÑÉ¥‘”Í•±•Ñ¥½¸°µ½‘Õ±¼µÙ…É¥…¹Ñ}½Õ¹Ñ€ÕÁ‘…Ñ•Ì°…¹Í•¹”µ¥¹™±Õ•¹”¡…¹‘±¥¹œ¸Q¡”€¬ÁàÀÉ€¡¥ÍÑ½É¥…°Í½ÕÉ”¹…µ”É•µ…¥¹Ì½¹Í•ÉÙ…Ñ¥Ù”¸()½ÉÁÕÌ±½ÍÕÉ”è()Ñ•áĞ)±…ÍÌ€Äµ…à¡±½…±}Í¡…Á•}‰…Í”€¬Í¡…Á•}ÍÑÉ¥‘”€¨Ù…É¥…¹Ñ}½Õ¹Ğ¤€ô€ÄÄĞÜ)±…ÍÌ€Ìµ…à¡±½…±}Í¡…Á•}‰…Í”€¬Í¡…Á•}ÍÑÉ¥‘”€¨Ù…É¥…¹Ñ}½Õ¹Ğ¤€ô€€€ÜØ)€()Q¡•Í”•á…Ñ±äµ…Ñ Ñ¡”½ÉÉ•ÍÁ½¹‘¥¹œILÌ‰…¹¬Í¥é•Ì¸((ŒŒ¹Ù¥É½¹µ•¹ĞµÁÉ½™¥±”Ñ…‰±•Ì…Ğ€¬ÁàÀÔĞÕ€()I½ÕÑ¥¹”€ÄØääÁ€½¹ÍÕµ•ÌÑ¡”É•µ…¥¹‘•È…Ìè()Ñ•áĞ)Õ¥¹ĞÄØÁÉ½™¥±•}½Õ¹Ğ)É•Á•…ĞÁÉ½™¥±•}½Õ¹Ğè(€€€Õ¥¹ĞÄØÉ•Ìá}‰…Í”(€€€Õ¥¹ĞÄØÍÑ½É•‘}É•½É‘}½Õ¹Ğ(€€€Á±…•µ•¹ĞÁ±…•µ•¹ÑmÍÑ½É•‘}É•½É‘}½Õ¹Ñt()Á±…•µ•¹Ğè(€€€¥¹ĞÄØÉ•Í½ÕÉ•}Í•±•Ñ½È(€€€¥¹ĞÄØ…é¥µÕÑ¡}‘•É••Ì(€€€¥¹ĞÄØ‘É…İ}İ¥‘Ñ¡}Á¥á•±Ì(€€€¥¹ĞÄØÍÉ••¹}ä)€()MÕÁÁ±¥•ÁÉ½™¥±•Ìè()Ñ•áĞ)ÁÉ½™¥±”€ÀèÉ•Ìá}‰…Í”ôà°€ÍÑ½É•‘}É•½É‘}½Õ¹ĞôÌà)ÁÉ½™¥±”€ÄèÉ•Ìá}‰…Í”ôĞÔ°ÍÑ½É•‘}É•½É‘}½Õ¹ĞôÈÜ)€()… ±¥ÍĞ•¹‘Ì¥¸½¹”…±°µé•É¼Á±…•µ•¹ĞÉ•½É¸I•¹‘•É•È€ÌÄÑÁ€İ…±­ÌÕ¹Ñ¥°É•Í½ÕÉ•}Í•±•Ñ½È€ôô€Á€°İ¡¥±”Ñ¡”±½…‘•ÈÉ•Ñ…¥¹ÌÑ¡”Í•É¥…±¥é•½Õ¹Ğ™½È…±±½…Ñ¥½¸½É•…Í¥é”¸()1Y1L¹P¡•…‘•È€¬ÁàÈäÁ€Í•±•ÑÌÁÉ½™¥±”€À½È€Ä‘¥É•Ñ±äì€Áá€Ñ…­•ÌÑ¡”…±Ñ•É¹…Ñ”½‘•™…Õ±ĞÁ…Ñ ¸€ÌÄÑÁ€É•Í½±Ù•ÌÉ•Í½ÕÉ•ÌÑ¡É½Õ …É¡¥Ù”€à°ÁÉ½©•ÑÌ…é¥µÕÑ¡}‘•É••Í€……¥¹ÍĞÑ¡”ÕÉÉ•¹ĞÙ¥•Ü…¹±”İ¥Ñ ƒ
ÄÌØÀİÉ…ÁÁ¥¹œ°ÕÍ•Ì‘É…İ}İ¥‘Ñ¡}Á¥á•±Í€™½È•¹Ñ•É¥¹œ½±¥ÁÁ¥¹œ°…¹Á…ÍÍ•ÌÍÉ••¹}å€‘¥É•Ñ±äÑ¼Ñ¡”‘É…ÜÁ…Ñ ¸()M•”9Y%I=959Q}AI=%1L¹µ‘€™½ÈÑ¡”‘•Ñ…¥±•É•¹‘•É•È•Ù¥‘•¹”¸((ŒŒMÑÉÕÑÕÉ…°±½ÍÕÉ”…¹É•ÁÉ½‘Õ¥‰¥±¥Ñä()…¹…±åé•}¥¹¥Ğ¹Áå€Ù…±¥‘…Ñ•ÌÍÑÉÕÑÕÉ…°Á…ÉÍ¥¹œ½É½Õ¹µÑÉ¥À™½È…±°€Ä°àÜä‰åÑ•Ì¸…¹…±åé•}É•Í½ÕÉ•}‰…Í•Ì¹Áå€¥¹‘•Á•¹‘•¹Ñ±äÙ…±¥‘…Ñ•ÌÑ¡”™¥á•µİ½ÉÉ•Í½ÕÉ”¹…µ•ÍÁ…”É•±…Ñ¥½¹Í¡¥ÁÌ……¥¹ÍĞÑ¡”¥µµÕÑ…‰±”ILÀ¼Ä¼Ğ¼Ô¼Ø¼ä¼ÄÀ°=	)QL¹P°P°…¹MY¹P½ÉÁÕÌ¸((ŒŒM…™”¥µÁ±•µ•¹Ñ…Ñ¥½¸‰½Õ¹‘…Éä()™…¥Ñ¡™Õ°É•…‘•È½É•Á…­•Èµ…ä•áÁ½Í”Ñ¡”ÁÉ½Ù•É•Í½ÕÉ”‰…Í•Ì…¹Í•¹”½•¹Ù¥É½¹µ•¹ĞÍÑÉÕÑÕÉ•Ìİ¡¥±”ÁÉ•Í•ÉÙ¥¹œ•Ù•ÉäÍ•É¥…±¥é•Ù…±Õ”•á…Ñ±ä¸%¸Á…ÉÑ¥Õ±…Èè((´‘¼¹½ĞÉ•İÉ¥Ñ”±½…°=	)QL°9A°Á½ÉÑÉ…¥Ğ°½ÈÍ•¹”µÍ¡…Á”¥¹‘•á•ÌÑ¼…‰Í½±ÕÑ”…É¡¥Ù”%Ì½¸‘¥Í¬ì(´ÁÉ•Í•ÉÙ”é•É¼µ±•¹Ñ …É¡¥Ù”Í±½ÑÌÑ¡…Ğ…É”Á…ÉĞ½˜™¥á•¹…µ•ÍÁ…•Ìì(´ÁÉ•Í•ÉÙ”Ñ¡”É…ÜILÄÁ½ÉÑÉ…¥Ğµ‰…¹¬Ñ•Éµ¥¹…°µ…É­•ÉÌ•Ù•¸Ñ¡½Õ Ñ¡¥Ì=¸È‘½•Ì¹½Ğ½¹ÍÕµ”Ñ¡•´ì(´­••À¡¥ÍÑ½É¥…°½•‘¥Ñ½È¹…µ•Ì½¹Í•ÉÙ…Ñ¥Ù”İ¡•É”½¹±äÉÕ¹Ñ¥µ”¹…µ•ÍÁ…”‰•¡…Ù¥½È¥ÌÁÉ½Ù•¸(