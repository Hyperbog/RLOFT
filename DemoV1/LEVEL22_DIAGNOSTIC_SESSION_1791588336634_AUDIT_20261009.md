# Level22 diagnostic session 1791588336634-20980 — user field report

**Scope:** original-game recreation, Level22 first. This incident is derived from the user's uploaded `1791588336634-20980.zip` session and companion `latest-session.txt`. Do not commit user-provided character/save bytes or absolute local machine paths to Git. **No gameplay DOS parity acceptance is implied.**

## Verified provenance and volume

- Manifest: `rloft-level22-diagnostic-v1`, `native-preview-not-original-DOS`.
- Actual executable revision: `6241a807baf53449256e5517687430283fdf2ca3` (Full Windows build #649, before the RES2 asset routing fix).
- Game media: original installed GOG `game.gog` CD image, not synthetic baked-in graphics.
- User ZIP contains **278 entries**, approximately **12,291,226 uncompressed bytes**.
- Session: **125** numbered JSONL events, **52** indexed native frame records, **4** F12-marked captures, **16** renderer `.tsv` captures, and multiple `.draws.tsv` ordered Type1 indexed operations.
- Modal/screens: 32 viewport, 9 party_panels, 7 top_edge_menu, 4 automap.
- The session recorded and the launcher packaged the ZIP **successfully**. The user uploaded the ZIP separately; it was not uploaded from the EXE itself.

## 2026-10-10 subsequent-build resource identity clarification

Original source `2E7F0→2F29C` proves the actor backgrounds and
inventory grids refer directly to **RES2** entries, NOT RES0+INIT.
The Build #649 screenshot's 320×1, 200-frame raster descriptors
were from the port's incorrect RES0 lookup; these are not proof that
the *original RES2 actor images* have that descriptor geometry.
The #651 field ZIP shows the fully drawn wrong RES0 artwork,
including an inappropriately placed movement/control pad.
[Current original-file verified resolution](LEVEL22_CODE2_RES2_BANK_ROUTING_FIELD_AUDIT_20261010.md).
This erratum does not change the archived source/user ZIP data.

## Crash — exact event and source branch

The terminal JSONL entries are:

```jsonl
{"seq":119,"event":"mouse.left.down","a":78,"b":371}
{"seq":120,"event":"screen.enter","a":22,"b":0}
{"seq":121,"event":"frame.capture","a":51,"b":22}
{"seq":122,"event":"mouse.left.up","a":78,"b":371}
{"seq":123,"event":"frame.capture","a":52,"b":22}
{"seq":124,"event":"mouse.left.down","a":68,"b":16}
{"seq":125,"event":"session.finish","a":52,"b":4}
```

The last completed framebuffer was `party_panels` frame **52**. The pointer-down happened at original VGA column **68**, row **16**. It is **exactly** the first actor's original encumbrance hit rectangle in `PartyPanelControlModel::modal_encumbrance_actor_at`: row **0x10..0x27** inclusive, local column **0x44..0x4D** inclusive. This excludes uncertainty between the portrait and encumbrance controls.

The native branch sets `inventory_encumbrance_actor`, and `PartyModalPresenter::draw_encumbrance_panel` immediately requests `assets_.type1(2,0x25)`. Full build #649's `RavenloftBootAssets::type1` lacks archive case 2 and throws the precise error displayed in the user's screenshot: `Demo v1 has no loaded type-1 archive for requested bank`.

**Root cause established:** original **RES2** was unhandled by the native boot asset router, despite existing on the immutable original game media. [Repair already committed](LEVEL22_RES2_ENCUMBRANCE_CRASH_20261009.md), Git `c2a4bfeb3f5e13b13e2343df75e1c4efd9a462ad`; the source now lazy-loads RES2. This repair is **unbuilt**, so the user has not yet retested it.

**Recorder observability gap:** there was **no** `error` event between `mouse.left.down` and `session.finish` because the exception from `party_modal_presenter.draw` escaped before the `present_traced` observer. A committed follow-up now catches/logs `party_modal.draw` exceptions immediately before rethrowing. The encumbrance click additionally emits `party.encumbrance.open` with actor index. Both keep the actual exception behavior unchanged.

## Inventory/party rendering — direct draw evidence

Party panel frames **10, 11, 12, 13, 14, 15, 16, 51, 52** show the same large black unused region and only partial original portrait/equipment artwork. The recorded `frame_0051.draws.tsv` (41 numbered operations + header and omitted counter) contains, among other calls:

```tsv
7	type1.frame	0	56	69	320	1	255	200
8	type1.frame	0	0	296	320	1	255	200
9	type1.frame	0	0	114	320	1	255	200
24	type1.frame	0	136	69	320	1	255	200
25	type1.frame	0	80	296	320	1	255	200
26	type1.frame	0	80	114	320	1	255	200
34	type1.frame	0	160	0	320	1	255	200
35	type1.frame	0	240	0	320	1	255	200
```

The native presenter is repeatedly selecting only **frame 0** from type-1 rasters with descriptor **200 frames × 320 columns × 1 row**, and drawing them at unusual per-actor offsets. This is **proven** by the source-native draw call log, as is the resulting incomplete background composition. **Resolved by independent original executable disassembly:** CODE.2 `2E510:2E4A4` iterates *all descriptor frames*, advancing the output row by the descriptor's frame height. These RES0 actor backgrounds were therefore missing 199 of the intended 200 source scanlines. `2975C` remains a single-frame path. [Faithful native repair, call-site audit and targeted regression](LEVEL22_SOURCE_MODAL_FULLFRAME_REPAIR_20261009.md). This is source-verified but unbuilt; complete DOS modal pixel parity and remaining source compositing semantics still need testing.

The two active party slots draw authentic RES1 portrait sprites and equipment. Their presence is evidence that archive reading works; their positions and incorrect panel backgrounds require separate original-DOS comparison.

## Automap — direct image evidence

Automap frames **19..22** capture a green original-cell grid with **an actual explored/visited shape**, rather than entirely empty map content. The automap screen records **324 `type1.mapped` draw operations** plus original border/menu art and a player-marker draw. Therefore, the incident does **not** establish total map-tile absence. The green rectangle, map viewport scale/orientation and external options art are visibly incomplete or uncertain, requiring an original DOS screenshot from the same state before changing their source behavior.

## Exploration — independent functional signal

The screenshot/session shows authentic sky/trees/NPC/ornate HUD/portraits. After menu/automap activity, exploration frames **25..46** record successive original fixed-point camera/world hash changes as the user moves, with a subsequent turn from facing **270** to **281**. This establishes native movement state changes and rendering in this session; it does **not** certify the original DOS movement timing or ground rendering parity.

## Remaining follow-ups

1. After the next manually authorized Windows Full build, re-test encumbrance **first-actor column x68..77, row16..39** and original RES2 container animations. Confirm the text/graphic is correct, not just that it no longer throws.
2. Review `party_panels` draw order/RES0 source sprite interpretation against original CODE.2 `0FF3C/1118C/10BBC/112EC`; black background is real and reproducible. Add a source-backed raster acceptance regression.
3. Compare captured automap with the original DOS game at same exploration state, geometry, visible-cell flags and zoom.
4. Ensure future failures of a modal draw include explicit `party_modal.draw` error event in the trace. Rebuild/tests required before calling this proven.
5. Preserve Level22-first delivery gate: no independent Level23 gameplay work.

**Current source status:** 180 existing registered CTests before this follow-up; prior Full #649 passed 179/179. New error instrumentation makes no change to gameplay acceptance or completion percentages. Build requires user's explicit authorization.

## 2026-10-09 original CODE.2 follow-up: strip-render root cause closed

The original graphics API's `2E510→2E4A4` whole-descriptor iteration is now proven from executable bytes and implemented for the party-modal background calls. `2975C` keeps its exact separate one-frame behavior. This is a source-code checkpoint, not a revised user diagnostic; all frame counts/data in the original session remain unchanged. [Full source audit and CTest](LEVEL22_SOURCE_MODAL_FULLFRAME_REPAIR_20261009.md). **181 registered**; current-head Full Windows and DOS acceptance pending.
