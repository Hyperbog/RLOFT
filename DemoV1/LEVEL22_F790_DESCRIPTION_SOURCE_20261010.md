# Original CODE.2 F790 item-description construction, source fields (2026-10-10)

**Status:** exact source-field arithmetic and branch composition modeled, original string/resource resolver still required. Not live-wired to Win32 or gameplay; original binary and resource files remain immutable.

Original user-provided CODE.2 has 558,689 bytes, SHA-256 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314. Original OBJECTS.DAT has 336 records of 0x41 bytes, SHA-256 f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0. The new offline verifier passed **17 exact original CODE.2 instruction windows**, the ten original F768 CS jump entries, and all 336 source item name/record fixtures. Category routes are the original 2–4→FA85, 5–8/default→FB3D, 9–11→F8D4.

## Source-proved branch arithmetic

At F7E4–F883, F790 copies the original name from OBJECTS.DAT +0 into buffer 759C, conditionally appends the original DS string at 0x610 and decimal object byte +0x37 when nonzero AND known, then always appends original DS string at 0x614. All category text then follows.

**Categories 2–4, FA85:** append decrypted T[0x40], DS string 0x618, decimal negation of the **signed** byte +0x34, and DS string 0x614. Original FADD uses MOVSX before NEG; interpreting that record byte as unsigned is wrong.

**Categories 9–11, F8D4:** append T[0x3E], DS 0x618, decimal (byte+0x38 + byte+0x3A), DS 0x61C, decimal (byte+0x38 × byte+0x39 + byte+0x3A); then append DS 0x620, decimal (byte+0x3B + byte+0x3D), DS 0x61C, decimal (byte+0x3B × byte+0x3C + byte+0x3D). These are original byte-wise range calculations; the actual display strings come from original resources.

**Categories 5–8 and default, FB3D:** append T[0x3D], decimal integral weight in half-unit scale from +0x30, add the recursive F5F8(signed byte+0x31) result only for category 1, and truncate the combined half-unit total to **16 bits** at FB90 before dividing by two. When the low bit is odd, append original DS 0x624 (the half-unit suffix). For category 1, lookup byte [0x7332 + signed subtype×10]; omit remaining suffix when 0x7F, otherwise append DS 0x614, T[0x41], DS 0x618 and that unsigned lookup decimal. Non-category-1 follows the alternate gate: if byte+0x31 is nonzero OR item ID==0x4A, and the held known flag is nonzero, append original dynamically pointed text from DS 0x6D98 followed by decimal byte [0x67EC + signed subtype].

The helper 2E6F0 resolves decimal conversion via the original signed/base-10 routine 373BF; 2E70C uses the adjacent decimal routine 37483. The new model reproduces their reachable numeric values with signed/unsigned bounds explicitly instead of approximating with modern item categories.

## Exact resource boundaries and verification

New C++17 header `OriginalF790ItemDescriptionSource` operates on an authentic raw 0x41-byte object record. Its injected resolvers provide original DS string contents (addresses 0x610/614/618/61C/620/624/6D98), decoded original T records 0x3D/3E/40/41 (the original 1C368 reader XORs byte contents with 0xDC), original F5F8 nested weight, and original 7332/67EC dynamic lookup tables. **Those resources are not fabricated**; their actual runtime values are a separate dependency before attaching this model to native pixels.

A strict local C++17 regression passed category branches, signed byte -2 and -128, positive and half-unit weight, the 16-bit truncation, category-1 0x7F sentinel, known flag, special object ID 0x4A, and exact resource callback order. A Python verifier checks original binary windows plus optional original OBJECTS.DAT. The native `InventoryItemDescriptionModel::describe` and live `PartyModalPresenter` were deliberately left unchanged. The model is isolated and **not proof of DOS message-text parity**.

**194 registered CTests when Python is available**, up from 192. Latest actually completed Windows Full #651 passed 182/182 at an earlier revision. Current source is not Windows-built or user-accepted; no Level22 completion, original DOS parity or readiness percentage increase. Manual Windows builds require user permission.

## Follow-on authentic resource closure

Original CODE.2 LE data object 2 is physically at 0x75F24; original DS literals and pointer 6D98→598 and TEXT indexes 3D/3E/40/41 were independently recovered. Initial ten-byte container descriptors at 7330, F5F8 recursion and 67EC variant table are available through a source-verified parser. The new OriginalF790ResolvedDescription binds all immutable string labels and numeric branches with an **injected current nested-container weight**, rather than replaying stale initialized slot arrays. [Source identity, exact texts, tests and acceptance gates](LEVEL22_F790_AUTHENTIC_DATA_T_RESOURCES_20261010.md). Live event/raster integration remains deferred.

## Next source gates

Resolve the original DS string storage/pointer initialization and decrypted T lookup for F790, plus the F5F8 nested-weight recursion and 0x7332/0x67EC tables from original startup/game-state data. Verify all original 17 F790 event-call argument contexts, wire a single event-owned rendering sequence with `ExplorationRuntime::original_random_`, then request a manual Windows build and original-DOS indexed-pixel/time comparison. No independent Level23 work or modernization.