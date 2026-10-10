# Original CODE.2 F5F8 Bag of Holding weight exception — 2026-10-10

**Authority:** original CODE.2 SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, executable object-1 file base `0x3AF24`. This checkpoint corrects a demonstrated native gameplay mismatch, not an aesthetic choice.

Original `F5FE–F603` compares its container subtype with `0x14` (decimal 20) and branches directly to `F657`. There, it reads the object database pointer at `0x255D8`, then loads an unsigned weight byte at base+`0x1480`, and returns. With original object stride `0x41`, `0x1480/0x41 = 80`, so the byte is OBJECTS.DAT item `0x50` (Bag of Holding), field `+0x30`. For subtype20 **no contents are traversed**, whether empty or full.

All other subtypes continue through `F605–F655`: the original ten-byte descriptor at `0x7330+10*subtype`, count at +0, signed-WORD contents pointer at +5, `-1` empty sentinel, record weight +0x30 and recursive category-1 items via signed subtype +0x31. Four original machine instruction windows at `F5FE`, `F614`, `F637`, `F657` matched the immutable CODE.2 bytes. Eight E8-rel32 direct callers of F5F8 were independently enumerated at `0F63C 0FB84 0FDC3 1157C 115B6 11E37 11E83 11EC6` (direct-call inventory only, not exhaustive runtime edges).

The preexisting native `InventoryRuntimeState::container_weight_half_units` walked container items indiscriminately, including descriptor20, so a Bag of Holding could incorrectly contribute the weight of its contents to recursive item burden. The correction is deliberately narrow:

`if (index == 0x14U) return objects.at(0x50U).weight_half_units;`

The early return is before container traversal and cycle handling; the original source takes its special branch before those operations. Other subtypes and existing failure protections are unchanged.

New `original_f5f8_special_weight_tests` constructs synthetic OBJECTS.DAT with object #80 weight4, other item #164 weight9, and a subtype20 containing eight heavy items; expects 4 independently of the contents. It also verifies an ordinary parent containing a magic bag and an ordinary item, and a separate regular container. The real original object #80 has weight0; the synthetic weight4 is intentional to prove the actual source-address rule, not coincidental zero. A Python source/binary check pins the native branch and the four original instruction windows.

**Status:** C++ gameplay fix and CTest are committed; new test is registered but full native compilation and Windows execution have NOT occurred. Last completed Windows Full #651 passed 182/182 at an older revision. Registered CTests now 199 with Python; no DOS parity, engineering readiness or Level22 completion percentage change. A manually authorized Windows build and real DOS comparison remain required.

The earlier source-only F790 resource resolver still needs authentic current nested weights and exact F790 event routing across the 17 original callsites before the Windows party modal can replace its provisional ribbon. No independent Level23 work or modernization.