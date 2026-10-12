# CODE.2 2C3F1 bounded original scene draw-order checkpoint — 2026-10-12

**Owner:** `native-engine:NativeEngine/` (independent replacement engine).
**Scope:** original CODE.2 22-byte staged record ordering ONLY. This is not
original camera/raycast, scene-record generation, live NPC state, original
DOS video capture, or a playable game.

## Authenticated original-byte evidence

Original CODE.2 file identity, SHA-256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
LE object 1 is extracted at original file offset `0x3AF24`, size `0x3A852`,
SHA-256 `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
All logical addresses below are zero-based offsets within this object.

The original CODE.2 bytes were independently disassembled with
`objdump -D -b binary -m i386 -M intel`, and the selected
original object-byte windows SHA-256-check as follows:

| Original logical window | SHA-256 | Interpretation |
| --- | --- | --- |
| `2C3CA..2C47D` | `fc07c1620fa123b3ab8040bad2558bcb0a63471986fe2ae962122238b67e552f` | Outer loop and first-minimum scan |
| `2C7B0..2C7C5` | `62e5b167592360cdbfe71984bd2e3bbc30b79aea5d17280cfdbe712f037209e5` | Stamp record and mark processed |
| `2C294..2C3F0` | `450c9f79f4bc4cff39f3d120ec9279b9f652fb49347c5bda1d4de59f0c88ff63` | Upstream original scene-build context |

Concrete x86 contracts:

- `2C2AF` clears active scene array length `0x1130` bytes at
  `0x27148 + active_buffer*0x2260`; record stride is `0x16` = 22.
  Consequently one bank can contain at most **200** complete scene records.
- `2C3F1` initializes best projected-size word to **`0x7D00`**.
  `2C426..2C43F` compares `MOVZX WORD [record+2]` using `JGE`
  and updates the selected ordinal only for a **strictly lower** value.
  Equal depths retain the earlier record.
- `2C47B` resolves the selected record address from active buffer and
  selected ordinal, *then* dispatches type-dependent original render calls
  (these dispatches remain out of scope).
- `2C7B0..2C7B9` saves the outer draw ordinal as `WORD [record+0x0A]`.
  `2C7BD..2C7C1` then adds **`0x7D` to BYTE `[record+3]`**.
  This is neither a 16-bit high-word addition nor a record deletion.

## Native implementation and acceptance boundary

`native_code2_scene_order.hpp/.cpp` performs precisely this
bounded selection and byte mutation on **caller-supplied staged records**.
It rejects input count greater than 400, and rejects *initial* projected
size >= `0x7D00`: otherwise the original first-minimum search can leave
a stale selection index. A fail-closed exception is deliberate host
safety policy, **not a claim that the original DOS executable throws**.

`native_code2_scene_order_tests.cpp` independently computes expected
stable projected-size ordering and checks the source high-byte marks,
little-endian draw-ordinal writes, untouched fields, empty and all-tied
inputs, 400-record budget, and invalid upper-bound inputs.

**2026-10-12 corrective audit:** the original 0x1130 clear is half
of the 0x2260-byte buffer; the initial 200-record limit was incorrect.
This is now fixed and the 400-record path passes with AddressSanitizer and
UndefinedBehaviorSanitizer. No original media has been committed.

**Local standalone C++17 verification:** g++ `-Wall -Wextra -Wpedantic
-Werror -UNDEBUG` built and ran the source and test above successfully.
The full NativeEngine CMake suite and CI for this commit have **not yet
been independently checked**, so the previous known 29/29 Linux suite is
the latest accepted CI record. One new CTest is registered; 30/30 is
not claimed without its exact-source CI run.

**Next:** reconstruct the original `2B6D0..2C22E` actor/fixture/moving
scene builders, live projection `35798`, original type dispatch in
`2C47E..2C7B0` and the actual render call chain. Only then connect
this record order to the EH7/SINTAB compositor. Actual original DOS
frame comparisons remain mandatory. Keep native and canonical
recompilation branches separate.
