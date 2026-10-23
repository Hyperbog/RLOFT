# Part 28 — Original 1F1C8 active-exterior frontier and 29838 six-argument ABI

**Status:** Strict source-backed Level 22 exterior **prefix accepted**, full `1F1C8` exterior raster and full `1F8FC` scene compositor **NOT accepted**. Branch: `Recomp`; original game bytes in `DnD-Classics/MASTER`. No original proprietary assets embedded.

## Original Level 22 path, proved against original-media Windows CI

Previously accepted retained source:
- Actual `181B4` level-22 loading, `17A68` actor postload, `334C4→32954` ray preparation.
- Exact ten `2FD9C` type-1 decode order, independent decoded bytes and cache reuse.
- `1F494/1EA90` four-panel source HUD, 12,966 aggregate changed indexed VGA pixels; four independent panel regions: 1,933 / 1,774 / 5,990 / 6,189.

Retained Level 22 scene profile immediately after original level load:
```text
art_mode=4 retained_mode=3 facing=270
exterior_flags C026=1 C027=1 C028=1
active_world_cell [258A8]=1
active_kind [258B0]=58; companion_kind [258B1]=155
draw_page=1
```

Actual `function_1f1c8` entry executes:
1. Original `291B0` output-page selection.
2. Original `1EDAC` animated source lookup remapping.
3. Real retained `334C4 → 32954` Level 22 ray geometry.
4. Real `34998` source transforms and exactly **four original self-modifying-code immediate callbacks** (`34A11,34A27,34A3D,34A54`). These are validated callbacks only; the original `34A74` executable patch/raster has not been integrated.
5. Since the **actual** `C028=1` branch is taken, it calls **`314B0` active environment**, not `33E28` sky fill. The latter source callback is connected for other scene profiles but not used in this Level 22 witness.
6. Real `314B0` executes its original 32-sprite random prepass through compiled `29838` and reaches the *first* genuine archived sprite request. Probe intentionally stops before `2F29C` archive materialization / `2A478` pixel raster; it never fabricates art or claims the full exterior was drawn.

Original-media Windows acceptance: [#39882404011](https://github.com/Hyperbog/RLOFT/actions/runs/39882404011) **PASS**, including the exact HUD and source closure gates:
```text
CODE2_ORIGINAL_LIVE_BOOT=1F1C8_EXTERIOR_SOURCE_PREFIX original_source=1F1C8_1EDAC_334C4_32954_34998_314B0_29838_OR_2986C active_world_cell=1 actor_calls=1 resource_requests=1 resource_id=50 resource_slot=8 34998_patch_callbacks=4 changed_indexed_pixels=0 retained_guest=YES frontier=314B0_RESOURCE_SPRITE_RASTER_PENDING full_1F8FC=NOT_ENABLED
```
The zero pixel delta is intentional: the only current Level22 sky/environment path is `314B0`, and the **first original RES8:50** sprite materialization and raster are not connected yet. `33DE0/33E28` are authentically implemented, but claiming a drawn sky from them in this profile would be wrong.

## Independent authoritative machine-code ABI correction

Uploaded retail `/mnt/data/CODE.2` is 558,689 bytes. In original LE object's object-1 page, PC offsets equal file offsets minus `0x3AF24`. Independent x86 disassembly with `objdump -D -b binary -mi386 -M intel` gave:

```asm
29838: push DWORD PTR [esp+0x14]   ; arg5 = archive slot
2983C: push DWORD PTR [esp+0x14]   ; arg4 = resource ID
29840: call 0x2F29C
29845: add esp,8
2984C: movzx edx,BYTE PTR [esp+0x18] ; arg6 = palette delta
29851: push edx
29852: push eax
29853: push DWORD PTR [esp+0x14]  ; arg3 = column
29857: push DWORD PTR [esp+0x14]  ; arg2 = row
2985B: push DWORD PTR [esp+0x14]  ; arg1 = frame
2985F: call 0x2A478
```

Calling x86 site `0x31528..0x31558` pushes SIX args (0,4,32,random resource,8,mode) and cleans **0x18 bytes**. It confirms that `29838` must accept (frame,row,column,resource_id,archive_slot,palette_delta), not the earlier faulty five-argument function which asked for `RES50:32` instead of `RES8:50`. Source `314B0` now passes six arguments and uses `palette_delta=mode` for modes 0..8, clamped to 8 for modes 9/10, mirroring original register/stack behavior.

Implemented in [`61d92b42`](https://github.com/Hyperbog/RLOFT/commit/61d92b429d36140d6acd2d492973d66f9c5a8455) and verified by a dedicated fixture [`7c795d3c`](https://github.com/Hyperbog/RLOFT/commit/7c795d3ccc3fc9000493b5922268a459c0e82c83).

Mandatory Windows CI now requires `resource_id=50 resource_slot=8` as an original-media runtime witness, **not** a value forced into the renderer.

## Scope boundaries

- Accepted: compiled real `1F1C8` source prefix through `314B0` and corrected compiled `29838` argument lookup; no standalone cloned gameplay guest.
- Not accepted: original `RES8:50` image materialization and `2A478` subtractive planar draw, full `314B0` multi-sprite environment, `34A74` oblique sprites, `34DE8` polygon raster, `1BDAC` moving actors, `2C294` sorted actor/wall compositor, `274A4` HUD compass, `1F120` ambient audio, full mode-four `1F8FC`, `1A5B0` consecutive frame presentation and DOS screenshot parity.
- Existing `1A5B0`, `30070`, RES0 compositor callbacks are already implemented but not certified as one complete frame, and `181B4`'s real mode-change `1F8E0` remains intentionally disconnected.

**Next engineering action:** Connect `2F29C(50,8)` and record the exact RES8 type-1 IMAGE layout on the *same* retained guest; validate full decoded bytes independently and execute `2A478` through the existing source VGA `Graphics2Boundary`. Then continue `314B0` sprite loop and enumerate each added decode in the exact resource ledger. Keep `1F8FC` disabled until `34A74/34DE8/2C294/1A5B0` source paths are all integrated and independently pixel-validated.
