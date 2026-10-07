# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 07 — Input, Cursor, HUD, Edge Menu, Inventory, and Modal UI Architecture

**Status:** PRIMARY INPUT/UI RECONSTRUCTION  
**Documentation branch:** `code1-decompilation`  
**Pre-documentation preservation baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`

---

## 1. Scope

This chapter documents the original interactive shell around the world/rendering systems reconstructed in Components 04–06:

- low-level keyboard interrupt handling;
- keyboard scan-code state, modifier state, and logical-key translation;
- DOS INT 33h mouse integration;
- the engine's 320×400 pointer coordinate domain;
- the queued pointer-event dispatcher;
- gameplay pointer hit regions and movement controls;
- the top-edge disappearing menu;
- cursor compositing across double-buffered pages;
- cursor-held item state;
- main gameplay HUD composition;
- party portraits, hand controls, and status presentation;
- full inventory mode;
- inventory/equipment hit testing;
- container/nested-container UI;
- party-character panel swapping;
- modal character/status panels;
- save-persistent UI state;
- faithful-port and remaster boundaries.

Combat mechanics, item mechanical effects, spell mechanics, and character-record semantics are referenced only where the UI consumes them. Their detailed gameplay behavior belongs to separate preservation components.

The immutable CODE.2 bytes remain controlling evidence. The existing certified `Code2.cpp` reconstruction supplies address-stable semantic translations, but misleading temporary local names such as `x/y` are not allowed to override the raw DOS mouse register flow. [Code2.cpp](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp) [CODE.2 final certification](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_CERTIFICATION.md)

## 2. UI coordinates are row/column oriented

One of the most important UI reconstruction details is the engine's coordinate ordering.

DOS mouse INT 33h function 3 returns:

```text
BX = buttons
CX = horizontal mouse coordinate
DX = vertical mouse coordinate
```

CODE.2 `33CB0` stores:

```text
0x2B7A8  buttons

0x2B7AC  CX / 2
           logical horizontal column

0x2B7AE  DX
           vertical row
```

The application initialization path then deliberately copies:

```text
0x9E31 <- state +6 = vertical row
0x9E33 <- state +4 = horizontal column
```

[Code2.cpp, `33C90..33DDD` and `1A0B0`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

Thus the safe UI terminology is:

```text
DGROUP 0x9E31  cursor_row     // vertical
DGROUP 0x9E33  cursor_column  // horizontal
DGROUP 0x9E35  mouse buttons
```

Some reconstructed helper parameters are named `x/y`, but their behavioral ordering is frequently:

```text
first coordinate  = row / vertical
second coordinate = column / horizontal
```

Native code should rename these at the compatibility boundary instead of perpetuating misleading Cartesian names.

## 3. Pointer range is 320 columns by 400 rows

Mouse initialization `1A0B0` calls:

```text
INT 33h function 8:
    vertical range 0 .. 0x18F = 399

INT 33h function 7:
    raw horizontal range 0 .. 0x27E = 638
```

The low-level read routine arithmetic-shifts the returned horizontal `CX` by one, yielding logical columns:

```text
0 .. 319
```

The set-position wrapper performs the inverse conversion:

```text
raw DOS CX = logical_column * 2
DOS DX     = logical_row
```

[Code2.cpp, `33C90..33D55`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

The resulting pointer domain is therefore:

```text
column  0..319
row     0..399
```

This independently agrees with the 320×400 VGA configuration documented in Component 04.

## 4. Mouse initialization

CODE.2 `1A0B0` probes mouse availability.

If present, it:

1. installs the 0..399 vertical range;
2. installs the doubled 0..638 DOS horizontal range;
3. positions the cursor at logical screen center:
   `column=160, row=200`;
4. clears pointer-control states;
5. clears mouse button state;
6. initializes the held-item state to empty;
7. initializes cursor page/backing state;
8. loads the ordinary cursor graphic;
9. installs the mouse callback.

Core state:

```text
0x85FC  mouse available/initialized

0x9E31  cursor row
0x9E33  cursor column
0x9E35  current button bits
0x9E37  cursor redraw/hide nesting depth
0x9E39  page associated with cursor backing image

0x8600  held OBJECTS.DAT item ID; -1 = empty
0x8602  held-item known/variant state
```

[Code2.cpp, `1A0B0..1A21D`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

## 5. Keyboard is interrupt-driven

CODE.2 installs IRQ vectors for keyboard/timer service and maintains keyboard state independently of the high-level UI loops.

The IRQ-9 handler `308F8` reads scan bytes from port `0x60`, performs the keyboard-controller acknowledgement sequence through port `0x61`, and distinguishes press/release state.

Core globals include:

```text
0xF588          ordered held/scan queue count
0x2B6C4[...]    ordered scan-byte queue

0xF572          current scan byte
0xF571          previous scan byte

0xF573          modifier bit mask
0xF574          one-shot key flag

0xF590[scan]    scan -> 32-bit logical-key translation
```

[Code2.cpp, `305A8..30D25`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

The input system therefore does not depend on BIOS `getch()`-style blocking reads for normal gameplay.

## 6. Modifier state

The keyboard ISR maintains these proved modifier bits:

```text
scan 0x2A  left Shift   -> modifier bit 0
scan 0x36  right Shift  -> modifier bit 1
scan 0x38  Alt          -> modifier bit 2
scan 0x1D  Ctrl         -> modifier bit 3
```

Helper `30ABC(mask)` exposes masked modifier state to gameplay/UI consumers.

This modifier state is separate from the scan queue and from the logical-key translation result.

## 7. Ordered scan queue supports held-key behavior

`307F0` inserts or moves a scan code to the tail of the ordered queue.

`30748` removes a scan code and refreshes current/previous scan state.

`308BC(scan)` tests whether a physical scan code is still held.

This is materially different from a one-event-per-key-down abstraction. Gameplay can ask both:

```text
what logical key is current?
which physical scan keys remain held?
```

That distinction is used by diagonal/multi-key movement.

## 8. Logical key translation

High-level routines do not switch directly on every raw scan byte.

The engine translates the active scan through:

```text
logical_key = uint32 table_F590[scan]
```

Two important consumers are:

```text
30AEC
    blocks/polls until a scan exists
    translates and consumes it

30B40
    pumps runtime
    translates current scan without consuming it
```

[Code2.cpp, Batch 71 keyboard runtime](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

A native port should preserve the distinction between:

- physical key identity;
- held-key state;
- translated logical command.

Later key rebinding can replace the translation layer without rewriting gameplay logic.

## 9. One-shot keyboard/toggle state

The ISR contains special one-shot/control behavior in addition to ordinary queue handling.

Examples include:

```text
scan 0x39
    sets one-shot byte 0xF574

30AC8()
    returns 0xF574 and clears it

scan 0x19
    sets word 0x1457C

Alt + scan 0x10
    sets a global request bit and clears the current scan
```

These are original encoded behaviors. User-facing key names should be assigned only where the translation/context is independently proved.

## 10. The main gameplay loop merges keyboard and pointer control

Main loop `1FC14..20293` repeatedly:

1. clears per-frame derived movement deltas;
2. polls mouse/pointer state through `1AE30`;
3. reads the current translated keyboard command through `30B40`;
4. updates shared movement command/direction state;
5. applies acceleration/decay;
6. runs simulation, world rendering, party/HUD refresh, event handling, and presentation;
7. services the edge-menu latch;
8. processes additional pointer/keyboard toggles.

[Code2.cpp, `1FC14..20294`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

Keyboard and mouse are therefore two producers feeding a shared command/state layer rather than two separate gameplay engines.

## 11. Keyboard movement domain

The main loop recognizes the extended logical codes:

```text
0x147
0x148
0x149

0x14B
0x14C
0x14D

0x14F
0x150
0x151
```

These correspond numerically to the DOS extended keypad/navigation scan-code domain and route into the shared movement-state helper.

For diagonal movement, the routine also calls `308BC` to test neighboring held scan codes such as `0x47/0x49/0x48/0x50`.

A faithful implementation should preserve simultaneous-key behavior rather than reducing movement to mutually exclusive discrete keydown events.

## 12. Mouse callback feeds a queued UI-event layer

The mouse callback does not invoke every UI action directly.

Routine `22CE0` enqueues a six-byte pointer event:

```text
struct PointerEvent6 {
    int16 row;
    int16 column;
    int16 flags;
};
```

The queue has 20 physical slots and ring-buffer head/tail bytes:

```text
0xA63A  head
0xA63B  tail
0xA63C  PointerEvent6[20]
```

When the next tail would equal the head, a new event is dropped rather than overwriting the oldest event. [Code2.cpp, `22CE0..22E3E`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

This queue is an important original ordering/back-pressure behavior.

## 13. UI mode controls pointer-event routing

Runtime byte `0xD680` selects the active UI interaction domain accepted by the pointer queue.

The proved event drain `22D8C` routes:

```text
mode 0
    gameplay/world HUD
    -> 229CC

mode 1
    full inventory
    -> 12E70

mode 4
    automap
    -> 02DB0
```

Other modes are not accepted by `22CE0` into this normal queued dispatcher. [Code2.cpp, `22CE0..22E3E`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

The top-edge menu temporarily uses `D680=3` and handles its own direct mouse polling rather than normal mode-0 queued dispatch.

## 14. Event queue reset

`22D70`:

```text
clears one-shot pointer/UI state
clears a pointer-update guard
sets queue head = queue tail
```

This discards queued pointer events.

Modal transitions invoke this deliberately so stale clicks do not leak into the next screen.

## 15. Main gameplay hit map is data-driven

Gameplay pointer dispatcher `229CC` scans a table beginning at DGROUP `0xD684`.

Each hit region is four signed 16-bit values:

```text
min_row
min_column
max_row
max_column
```

A first field of `-1` terminates the table.

The supplied CODE.2 data contains 21 active regions followed by the terminator. [Code2.cpp, `229CC..22CE0`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

This means the gameplay HUD hit map is serialized executable data, not a set of arbitrary modern widgets.

## 16. Central first-person interaction region

Gameplay region 0 is:

```text
row     4 .. 259
column 32 .. 287
```

which is exactly:

```text
256 rows × 256 columns
```

The world interaction dispatcher handles this region specially:

- right-button/action path;
- held-item placement/drop/use behavior;
- first-person geometry/object hit testing;
- dynamic cell/object interaction;
- fallback pointer movement/control selection.

This provides a direct UI boundary for the first-person viewport.

## 17. Party/HUD regions repeat in four-character groups

Regions 1..16 are organized as:

```text
actor = (region - 1) / 4
lane  = (region - 1) & 3
```

giving four UI lanes for each of four party characters.

The dispatcher routes:

```text
lane 0
    hand/action slot 0 handler

lane 1
    spell-interface/character selection handler

lane 2
    hand/action slot 1 handler

lane 3
    character/inventory panel interaction
```

[Code2.cpp, `229CC`, `211D8`, `214B8`, `233A8`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

These lanes are part of the live HUD rather than the full-screen inventory screen.

## 18. Two live HUD hand slots map to character inventory slots 14 and 15

Hand/action handler `214B8(actor, hand, flags)` accesses:

```text
character item[14 + hand]
```

because the runtime addresses are:

```text
0x2193C = item[14]
0x2193E = item[15]
```

[Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

The HUD therefore manipulates the same canonical character inventory slots used by the full inventory interface.

## 19. Hand interaction distinguishes use from item swap

For a hand region:

- one pointer flag path performs the hand item's gameplay action, subject to actor status and item readiness;
- another pointer flag path swaps the hand item with the global held/cursor item.

The swap also transports the item-known/identified bit and clears per-hand readiness state as required. [Code2.cpp, `214B8..21702`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

UI item movement is therefore not merely graphic dragging; it mutates the authoritative character record and associated item-instance state.

## 20. Character-panel lane can enter full inventory

When the pointer uses the character-panel lane without a held-item insertion operation, `211D8` calls the full inventory loop `13118` for that active character.

With a held item, the same lane first attempts context-sensitive placement into hand/container state or the first free ordinary inventory slot. [Code2.cpp, `211D8..214B8`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

This is an important UI ownership relationship:

```text
live HUD character panel
    -> full inventory mode

held item + same panel
    -> placement logic
```

rather than every click always opening inventory.

## 21. Spell-interface selection is grouped by party pair

Handler `233A8` computes:

```text
group = actor / 2
```

and stores the selected character in:

```text
DGROUP 0xD8A8[group]
```

The two serialized dwords at `0xD8A8` are independently reconstructed as `spell_ui_selected_character[2]`. [SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

Thus each left/right party pair has its own current spell-interface character selection.

## 22. Movement-control hit regions are also table-driven

Pointer movement helper `1AC38` consumes two rectangle tables.

The first table contains seven records:

```text
min_row, min_column, max_row, max_column,
movement_index, direction
```

with supplied records:

```text
0  292 149 324 171   3 -1
1  309 126 341 148   1 -1
2  309 172 341 194   1 +1
3  325 149 357 171   0 -1
4  342 126 373 148   2 -1
5  342 172 373 194   2 +1
6  358 149 389 171   0 +1
```

These values are decoded directly from CODE.2 object-2 data and consumed at runtime `0xA6DC`.

They describe the compact central movement/control area in the lower portion of the 320×400 UI.

## 23. Second movement/control table

The second table contains eight records at runtime `0xA730`:

```text
0  row 4..88      col 17..116    command 4  dir -1
1  row 4..88      col 117..202   command 0  dir -1
2  row 4..88      col 203..287   command 4  dir +1

3  row 88..174    col 17..116    command 1  dir -1
4  row 88..174    col 203..287   command 1  dir +1

5  row 175..259   col 17..116    command 2  dir -1
6  row 175..259   col 117..202   command 0  dir +1
7  row 175..259   col 203..287   command 2  dir +1
```

For these eight regions, `1AC38` also derives analog-like movement gain from cursor distance to encoded control centers/edges before storing command state. [Code2.cpp, `1AC38..1AE30`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

Pointer movement is therefore rectangle- and distance-driven rather than a simple eight-button D-pad.

## 24. Pointer polling has two gameplay control modes

`1AE30` branches on signed word `0xCF48`.

One path reads relative/raw INT 33h motion and converts it into bounded movement/look deltas.

The other path uses button-down state and `1AC38` to drive the screen-region movement controls.

A toggle routine `1A220` flips the low byte of `0xCF48` and stores/restores cursor position around the transition. [Code2.cpp, `1A220` and `1AE30`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

This is a real original control-mode switch. A later free-mouselook implementation should be layered above it, not mistaken for the original input model.

## 25. The disappearing menu is a TOP-edge menu

Mouse/event routine `19D18` sets menu latch `0xA6C2` when:

```text
cursor_row == 0
```

not when the horizontal coordinate is zero.

The menu routine `080AC` remains active while:

```text
cursor_row < 0x40
```

and captures/draws region:

```text
row     0 .. 63
column  0 .. 319
```

The eight menu choices are selected by:

```text
choice = cursor_column / 0x28
       = cursor_column / 40
```

[Code2.cpp, `19D18`, `080AC..0869F`, and INT33 state flow](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

Therefore the original UI is exactly an **appearing/disappearing top-edge menu: 64 pixels high, eight 40-pixel-wide columns across the 320-pixel screen width**.

This confirms the behavior described from the original game and corrects any reconstruction that would place the menu on the left edge.

## 26. Top-edge menu labels are serialized executable strings

DGROUP table `0x60F0` contains eight pointers.

Following those pointers in the supplied CODE.2 object-2 data gives, in menu order:

```text
0  rest
1  pray
2  memorize
3  automap
4  load
5  save
6  quit
7  options
```

These labels are direct shipped executable data, not inferred from the called functions.

## 27. Menu presentation states

For each of the eight choices, helper `app12_draw_menu_row` draws one of three image states:

```text
sprite 0  normal
sprite 1  current hover/selection
sprite 2  pressed
```

and draws the corresponding string from the eight-pointer menu label table.

The resource graphic is resolved relative to:

```text
RES0 UI base + 0x49
```

The menu therefore shares the RES0 raster UI namespace documented in the resource archive. [INIT UI bases](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/UI_RESOURCE_BASES.md)

## 28. Menu opens by screen capture, not by permanent layout replacement

When activated, `080AC`:

1. pauses/changes gameplay timing through the timing-gate stack;
2. synchronizes cursor/page state;
3. saves the current UI mode;
4. switches to menu mode 3;
5. captures the top 64×320 region;
6. draws menu buttons over that capture;
7. restores the captured region on exit;
8. restores the prior UI mode and timing state.

[Code2.cpp, `080AC..0869F`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

The menu is therefore an overlay over the live screen rather than a separate permanent top toolbar.

## 29. Menu hover and press polling are deliberately stateful

While the pointer remains in the top strip, the routine:

- tracks the current 40-pixel column;
- redraws old and new columns when selection changes;
- polls the mouse twice around press acceptance;
- uses pressed sprite state before dispatch.

The second poll is encoded even after the first poll has already reported a press. [Code2.cpp, `080AC..0869F`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

A faithful UI test should preserve press/release sequencing, not only final command selection.


## 30. Exact top-edge menu command mapping

Because the executable string table and dispatch switch use the same eight-index order, the complete menu map is:

```text
column 0   rest
column 1   pray
column 2   memorize
column 3   automap
column 4   load
column 5   save
column 6   quit
column 7   options
```

Each column is 40 logical pixels wide.

The command bodies are also distinct:

### 30.1 REST

Menu action 0 calls the dedicated rest/camp path at `0461C`, then explicitly waits for mouse-button release before returning to hover tracking.

### 30.2 PRAY and MEMORIZE

Actions 1 and 2 temporarily save:

```text
held item ID    0x8600
held item state 0x8602
```

then clear the held cursor item before entering the corresponding spell-preparation interface.

After the interface returns, the original held item/state is restored. If the sub-interface changed presentation state, the top-strip backing capture is regenerated before the menu resumes.

Thus spell-preparation screens intentionally detach the cursor-held object while active rather than allowing an inventory item graphic to remain attached to the pointer.

### 30.3 AUTOMAP

Action 3 calls the automap entry path with selector 0 and returns to a full redraw of the edge menu afterward.

### 30.4 LOAD

Action 4 enters the load interface and can show a modal message using the menu-associated error/status text pointer before recapturing the top strip.

### 30.5 SAVE

Action 5 first writes the **current live level snapshot** through:

```text
171FC(current_level)
```

before entering the broader save-game UI.

This directly links the menu SAVE action to the standalone `LEVnnn.DAT` persistence model documented in Component 05.

### 30.6 QUIT

Action 6 calls a confirmation/modal path using the quit text pointer. On affirmative return it invokes the program/session transition function; on cancellation it redraws the menu over a refreshed capture.

### 30.7 OPTIONS

Action 7 enters the options/control path, then refreshes/rebuilds the underlying main presentation before restoring the top menu.

[Code2.cpp, `080AC..0869F`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

## 31. Held-item state is gameplay state, not a visual-only cursor property

The global cursor-held item is:

```text
0x8600  int16 OBJECTS.DAT object ID
        -1 = no held item

0x8602  uint8 known/identified state
```

[SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

These fields are written to and restored from `SAVE.DAT`.

Therefore the cursor may visually display an item, but the authoritative held-item pair belongs to persistent game state.

## 32. Cursor item graphics come from OBJECTS.DAT and RES1

When a held item is installed, the cursor path indexes:

```text
OBJECTS.DAT[object_id] +0x2D
```

to obtain the object's local UI graphic selector.

INIT supplies:

```text
res1_object_ui_base = 168
```

and the supplied object-UI namespace is exactly:

```text
RES1:168..335
```

[INIT UI bases](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/UI_RESOURCE_BASES.md) [RES0/RES1 namespaces](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/RES0_RES1_NAMESPACES.md)

Legacy serialized object IDs and local graphic indexes must remain separate from the resolved RES1 handle.

## 33. Software cursor has a 32×16 backing footprint

The cursor/page compositor consistently treats the cursor rectangle as:

```text
width   0x20 = 32
height  0x10 = 16
```

in the underlying raster copy operations.

For each draw page, the system stores background under the cursor in a dedicated backing region:

```text
0x9E3A + page * 0x400
```

and restores that backing before moving/removing/redrawing the cursor. [Code2.cpp, `1A288..1AB6C`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

The pointer is therefore a software-composited sprite, not the DOS mouse driver's default hardware/text cursor.

## 34. Cursor hide/show is nesting-aware

`0x9E37` is a redraw/hide nesting depth.

If cursor drawing is requested while the depth is already nonzero, the engine increments depth rather than blindly redrawing.

Corresponding remove/restore helpers decrement the depth and only perform the actual backing-buffer operation when the count reaches the encoded boundary.

This allows nested drawing operations to suppress cursor compositing without losing the prior visibility state. [Code2.cpp, `1A288`, `1AACC`, `1AB20`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

A native implementation should preserve this as a scoped/nested cursor-compositing contract rather than one boolean.

## 35. Page copying is cursor-aware

Routine `1A8C8` copies a rectangle from the opposite page while preserving the software cursor.

It first tests whether the update rectangle overlaps:

```text
cursor row/column
+ 32×16 cursor footprint
```

If there is no overlap, the page rectangle is copied directly.

If there is overlap, the routine:

1. temporarily removes the cursor;
2. copies the page rectangle;
3. synchronizes the cursor backing region;
4. redraws the cursor;
5. restores page/depth state.

[Code2.cpp, `1A8C8..1A9F8`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

This is a historical dependency between the UI pointer and the double-buffered VGA page system from Component 04.

## 36. Held-item changes are synchronized across pages

`1A5B0` and `1A724` update the held item/variant while preserving the cursor image on both presentation pages.

They can:

- update the global object ID;
- reload the cursor item graphic;
- update the companion known/variant byte;
- remove the current cursor;
- refresh or copy the specified page/rectangle;
- synchronize the opposite page's saved cursor background;
- redraw the cursor.

A faithful port can implement this more cleanly internally, but regression tests must validate the externally visible result across page flips.

## 37. Main gameplay frame composition

`1F8FC` is the high-level main gameplay presentation reset/redraw.

It:

1. selects the current draw page;
2. draws two large mode-dependent RES0 frame/background graphics;
3. refreshes optional saved side strips;
4. marks all four party HUD slots dirty;
5. redraws party panels;
6. refreshes world/presentation state;
7. draws mode-specific overlays where active;
8. reinstalls the current held-item cursor;
9. toggles the page state;
10. clears queued pointer input and returns to gameplay UI mode 0.

[Code2.cpp, `1F8FC..1FB80`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

The HUD is therefore part of the same page-composited presentation as the first-person renderer.

## 38. Four party HUD positions are computed, not independently hard-coded

`1F494` computes each party panel's horizontal placement using:

```text
position =
    (slot % 2) * 0x35
  + (slot / 2) * 0xD3
```

for slots 0..3.

This yields:

```text
slot 0    0
slot 1   53
slot 2  211
slot 3  264
```

The layout creates two party members on the left side and two on the right, leaving the central gameplay/control area between them.

## 39. HUD portrait identity uses the same character face namespace

For an active party slot, `1F494` resolves:

```text
portrait =
    RES1 character-face base
  + sex * 23
  + face_index

if status_word & 0x0100:
    portrait += 46
```

[RES1 portraits](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/RES1_PORTRAITS.md)

Thus the HUD, character/inventory panels, and serialized character identity all converge on the same normal/possessed portrait banks.

## 40. HUD hit-point bar is calculated from live character state

For a living/non-death-state character, the HUD derives a bounded bar against a maximum width of:

```text
0x50 = 80
```

from current/effective HP and maximum HP.

The bar changes raster color when its computed length is at or below:

```text
0x14 = 20
```

[Code2.cpp, `1F494..1F7B0`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

The UI therefore consumes the live character record directly rather than storing an independent HUD health percentage.

## 41. HUD status overlays are state-derived

The same party-panel routine overlays additional framing/graphics when character status/effect fields are nonzero.

Examples include:

- primary/timed effect overlay;
- disease/poison/paralysis-class status masking;
- possession portrait-bank substitution;
- temporary numeric/status presentation.

The exact character status/effect catalog is documented in the SAVE/party reconstruction; the UI should consume those canonical flags rather than duplicate them.

## 42. Each party member has two live hand widgets

`1F494` calls:

```text
1EA90(slot, 0, ...)
1EA90(slot, 1, ...)
```

for every active character.

The hand renderer clears a 32×16 hand rectangle, derives action/availability state, draws the equipped hand item's UI graphic or empty-hand fallback, optionally draws selection/status overlays, and presents action-ready countdown information. [Code2.cpp, `1EA90..1EDAC`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

These widgets correspond to character inventory slots:

```text
14  hand A
15  hand B
```

[Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

## 43. Full inventory mode is a separate queued-input UI mode

Entering the main inventory loop `13118`:

- pushes/changes gameplay timing state;
- draws all four inventory/character panels through `112EC`;
- operates while `D680 == 1`;
- reads translated keyboard state;
- drains the same 20-entry pointer event queue through the inventory dispatcher;
- returns when the mode changes.

[Code2.cpp, `112EC`, `13118`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

Inventory is not an unrelated second program; it is another mode of the same presentation/input engine.

## 44. Inventory displays four character panels horizontally

Inventory pointer dispatcher `12E70` receives the queued pair in row/column order and computes:

```text
character_slot = column / 0x50
               = column / 80
```

The local column within the panel is then reduced by:

```text
slot * 80
```

Since the logical screen width is 320, the four party inventory panels divide the screen exactly into four 80-pixel horizontal bands:

```text
slot 0  columns   0..79
slot 1  columns  80..159
slot 2  columns 160..239
slot 3  columns 240..319
```

[Code2.cpp, `12E70..1309F`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

This is the inverse of the more conventional “one character per horizontal row” layout.

## 45. Ordinary inventory grid is 3×4 per character

Character item slots `0..11` are drawn using:

```text
vertical row coordinate:
    0x129 + (slot / 4) * 0x22

panel-local horizontal coordinate:
    5 + (slot % 4) * 0x12
```

so the ordinary inventory area is:

```text
3 vertical groups
×
4 horizontal positions
=
12 ordinary slots
```

[Code2.cpp, `1118C`, `11FB0`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

The corresponding serialized character fields are `item[0..11]`.

## 46. Inventory hit testing uses explicit geometry

`12E70` divides each active character panel into separately handled regions.

Proved examples include:

```text
row < 0x10
    party-character panel swapping path

row 0x129..0x18C,
panel-column 5..0x4A
    ordinary 12-slot inventory grid

row 0x6E..0x123,
panel-column 2..0x4D
    equipment/hand region

row 0xDB..0x11F,
panel-column 5..0x4D
    active nested-container grid when present

other compact regions
    character statistics/status modal controls
    panel mode toggles
```

[Code2.cpp, `12E70..1309F`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

The exact rectangles are original UI behavior and can be regression-tested independently of artwork.

## 47. Equipment placement is type-directed

When the cursor holds an item, `12180` inspects the item's `OBJECTS.DAT +0x2F` type/category and determines its equipment target.

Certified routing includes:

```text
object category 3
    -> character item slot 17

category 4
    -> slot 16, subject to character/equipment gates

category 5
    -> slot 12 or 13 depending panel-side click

category 6
    -> slot 18

category 7
    -> slot 20

category 8
    -> slot 16

default category:
    one side -> slot 14 or slot 19 depending item predicate
    other side -> slot 15
```

[Code2.cpp, `12180..1246F`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

Cross-correlated with the certified character record:

```text
12  ring A
13  ring B
14  hand A
15  hand B
16  torso/body
17  head
18  accessory
19  quiver
20  bracer/gauntlet
```

[Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

The equipment UI therefore uses item-category logic rather than accepting arbitrary drag-and-drop into every visible slot.

## 48. Empty-cursor equipment clicks use a descriptor table

When no item is held, `12180` scans a selected table at runtime pointer `0x12C00`.

It contains ten 24-byte hit descriptors:

```text
+0x00  int32 inventory slot index
+0x04  uint32 accepted pointer flags
+0x08  int32 minimum row
+0x0C  int32 minimum panel-column
+0x10  int32 maximum row
+0x14  int32 maximum panel-column
```

The first matching occupied slot whose flags accept the click becomes the target.

Two descriptor-table variants are selected according to character/panel state.

This is another example of the original UI's strongly data-driven hit geometry.

## 49. Inventory swap carries the known/identified bit

Character inventory object IDs are paired with:

```text
character +0x12B
    item_known_bits
```

where bit `i` corresponds to `item[i]`.

Inventory swap `117A0`:

1. reads the current slot's object ID;
2. extracts its known bit;
3. reads cursor-held object ID and cursor-known byte;
4. writes cursor item into the character slot;
5. writes its known state to the corresponding item-known bit;
6. moves the old slot item and old known state to the cursor.

[Code2.cpp, `117A0..11AD8`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp) [Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

The identified/unknown property belongs to the item instance as it moves through UI containers.

## 50. Temporary hand-effect object IDs cannot be moved normally

The inventory swap path rejects current slot values in:

```text
0x7E..0x86
```

These IDs correspond to the temporary hand-effect objects already reconstructed in the character record.

They are runtime effect placeholders, not ordinary draggable inventory items.

A generic modern inventory implementation that treats every non-`-1` word as a movable object would be behaviorally wrong.

## 51. Nested containers are real UI state

Container-capable items use:

```text
OBJECTS.DAT +0x31
    signed container-state index
```

selecting a runtime descriptor at:

```text
0x7330 + index * 10
```

The descriptor includes:

```text
slot_count
UI columns
capacity
UI offset
container subtype
contents pointer
per-slot state bits
```

[SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

The full inventory screen can open these container grids inside a character panel.

## 52. Open-container panel context is persistent SAVE state

Per-party UI context begins at runtime `0x6FA0`, four bytes per party slot.

Container panel functions `10FAC` and `110B4` set/clear these bytes while opening and closing nested inventory presentation.

SAVE serialization later writes four 4-byte party container-UI contexts. [SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

Therefore some UI navigation state survives a save/load and is part of the legacy compatibility contract.

## 53. Nested-container placement enforces subtype and capacity rules

`11C3C` handles clicks inside an open container grid.

Before swapping, it validates:

- current panel mode;
- container subtype;
- held-item category;
- prohibition against recursively placing certain container types into incompatible/same classes;
- target grid bounds;
- slot count;
- aggregate burden/capacity.

[Code2.cpp, `11C3C..11FAE`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)

Container UI policy is therefore enforced by the original engine at interaction time, not merely by visual slot filtering.

## 54. Ordinary inventory can auto-place into an open container

When swapping into a normal inventory/equipment slot, if both the existing slot and cursor-held object are present and the existing object is a compatible container, `117A0` can call `11404` to attempt nested insertion.

If successful:

- the held item goes into the first compatible free nested slot;
- the cursor becomes empty;
- burden/presentation is updated;
- the outer inventory slot continues to hold the container.

This context-sensitive behavior should be represented as an explicit legacy interaction rule in the faithful port.

## 55. Inventory can drop the held item into the world

While inventory mode `13118` is active, translated logical key `0x44` checks the cursor-held item.

If an item is held, it calls the loose-world-item constructor with the player's current coordinates, then clears/updates the UI through the existing cursor/world pathways.

This links:

```text
inventory cursor-held item
    -> family-C loose item world record
```

from Component 05.

Inventory is therefore directly connected to world-state creation rather than storing dropped items in a separate UI queue.
