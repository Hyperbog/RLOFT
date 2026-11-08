# Part 21: Original Level 5 world-quads, RES3 EH7 and DOS parity handoff

Validated branch: recomp, source commit 2cbf5747410a2dbe5ef6af43c34c87069eb09e2a.
Passing checks:
- Saved Level 5: https://github.com/Hyperbog/RLOFT/actions/runs/39164949396
- Independent Level 11 RES4: https://github.com/Hyperbog/RLOFT/actions/runs/39164949379
- Windows MSVC: https://github.com/Hyperbog/RLOFT/actions/runs/39164949386
- Linux and original-source-link regression: both green at that same commit.

## Verified actual original source

1. Source 07938 deserializes the exact saved SAVE.DAT into the authenticated CODE.2 initialized-data guest; source 181B4 loads standalone LEV005.DAT. Source 334C4 / 32954 supplies 256 ray records at the saved camera x=0x65960, y=0x6EB68, heading=252 degrees.
2. Source 16D04 and 16810/168C8/1686C/16934/16990 initialize the original INIT, NPCS.DAT and OBJECTS.DAT descriptor banks: 83 NPC definitions, 336 object definitions, 175 class-1 and 12 class-3 descriptors. The original virtual pointers resolve into allocated guest memory.
3. Compiled 1F1C8 → 34998 → 34A74 → 342E4/34954 renders original RES4:634 and RES4:635 sky and ground. Source 335DC/3263C/338C8 renders wall/depth; the original 291B0 / 2ABF4 active VGA draw page is 1.
4. NEW: Compiled 1E8C8 traverses 61 original saved-level scene records, 25 with source-eligible quad flags. Compiled 34DE8 → 34BB0 → 33E6C projected one source-visible world quad, ran 29 source scanlines, and transferred 76 changed bytes into the actual VGA aperture. The original CODE.2 retail clip defaults FDAC=0x120 and FDB0=0x20 are copied only for original save-based execution, not synthetic Level 11 fixtures. This chain selects RES4 textures (original 34BB0 requests slot 4); it must not be mislabeled RES3 billboard rendering.
5. NEW: Source NPC record type 61 maps through authentic NPCS.DAT definition offset +0x14 to RES3 shape 1829. The canonical 2F29C → 2ECB4 → 37590 → 37700 source chain decompressed this real type-7 EH7 shape to a 4,219-byte body and passed read/seek-free cache reuse.
6. Compiled original 2C294 scene ordering and 2B6D0/2BBA0/2BDF4/2BFEC projection are connected, with original 21BF0 RES3 shape callbacks and source RNG/timing helpers. At the *actual saved Level 5 camera*, no original primary actor cells were marked ray-visible (0 of 90). Consequently zero source sprite projection requests and zero 36CEC pixels occurred. Decoding one metadata-selected EH7 shape is NOT source sprite raster parity.
7. Compiled 1EA04 → 2F29C → 2939C → 2A380 still draws 12 RES0 HUD images, changing 657 visible indexed pixels. The final draw order in the native diagnostic is source sky/ground, RES4 world quad, wall/depth, original RES3 projection/ordering (zero visible sprites in this scene), then RES0 HUD.

The latest saved-world source frame SHA256 is:
d4831707916391c12290afc488318e65b9325d6e178ebb35fce4c794e9cbb8b7

This 128,000-byte 320×400 indexed file and 768-byte RGB6 palette are destroyed by public CI cleanup rather than distributing original game art. Hashes identify NATIVE reconstructed output ONLY, never DOS execution.

Authenticated resource identities: RES3 SHA256 44955ebbed0a8ed8d874fae07b01710317044882ab3c7c95d9be54e883df9a36; NPCS.DAT f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b; OBJECTS.DAT f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0.

## Local private reproduction

The existing executable rloft_original_code2_world_frame accepts:

--save-world SAVE.DAT LEV005.DAT SINTAB RES4 COLORS CODE.2 INIT RES0 NPCS.DAT OBJECTS.DAT RES3 /private/save5-output

It writes a native indexed /private/save5-output.idx, unrounded RGB6 /private/save5-output.pal, and preview /private/save5-output.bmp. This generated BMP has NOT been visually inspected within this conversation; no raw renderer screenshot was uploaded. Visual inspection is therefore outstanding despite structural frame and write tests passing.

## Independent original DOS capture remains missing

An original DOSBox-X / real-DOS execution at the same save position, heading, animation/frame phase and VGA mode is required. Observe original code/executable and emulator hashes, VGA CRTC displayed start/page, four physical VRAM planes (including correct read-map selection), horizontal/vertical panning, and all 256 DAC RGB6 triplets. Produce a verified row-major 320×400 source frame (128,000 bytes) and 768-byte palette. Do not pass off a host PNG screenshot or native frame dump as byte-exact independent DOS data.

The existing strict validator Recomp/tools/compare_dos_indexed_frames.py compares real DOS and native indexed frame/palette bytes without allowing altered pixels. A debugger MEMDUMPBIN on ordinary guest RAM alone cannot establish VGA planar/DAC provenance, and no original DOS framebuffer was acquired in this run. Raw original media and frames must not enter public GitHub CI artifacts.

## Next engineering boundary

- Authenticate source CODE.2 36CEC sprite draw function and implement/compile its EH7 per-column raster and depth/occlusion behavior. It is presently an external callback without reconstructed source execution.
- Exercise it at another owner-original camera with source-visible RES3 records. Do not set ray-visible marks artificially or fabricate sprite overlays to pass a test.
- Privately inspect the complete native BMP and obtain independent original-DOS indexed/DAC capture from the same real camera and frame. Only then make an exact parity assertion.
- Continuous original CODE.1 → CODE.2 game boot and R04 remain separate unaccepted milestones.

Acceptance: original save/NPCS/OBJECTS source initialization, source world quads, source RES3 EH7 decode/cache and RES0 HUD pass. Visible RES3 sprite raster, native visual inspection, independent DOS parity and continuous game boot are NOT accepted.
