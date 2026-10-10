# Original F790 item-description type dispatch (2026-10-10)

**Status:** source-verified category routing, source-only model. The actual Windows presenter still uses a provisional item-description helper and is NOT connected to the original F790 sequence.

## Source proof

Original supplied CODE.2 SHA-256: 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314; object-1 physical base 0x3AF24. The new binary verifier passed six distinct original instruction windows at logical F7E4, F812, F8AF, F8D4, FB3D, FD58 and read the exact ten DWORD branch destinations in the original CS table at F768..F78F.

F790's nonpending/held-item branch copies the original item name from the 0x41-byte object record to buffer 759C (F7E4..F811), optionally appends enchantment based on record byte +0x37 and original known argument (F812..F882), then reads item category byte +0x2F (F8B4..F8BE). At F8BF..F8D3 it subtracts two in an 8-bit AL register, branches with an **unsigned** comparison to the default, or jumps via a ten-entry CS table:

| Item category | Exact original destination | Table slots |
| --- | --- | --- |
| 2, 3, 4 | FA85 | 0–2 |
| 5, 6, 7, 8 | FB3D | 3–6 |
| 9, 10, 11 | F8D4 | 7–9 |
| 0, 1 or 12–255 | FB3D default | Outside table |

These are *code addresses*, not speculative modern item-class names. The original branches format different suffix strings and consult additional item fields and text resources; the complete bytes of those strings and branches have NOT been recast in this checkpoint.

The current native `InventoryItemDescriptionModel::describe` produces only name, optional enchantment and generic weight. It is therefore NOT yet evidence-backed as a complete F790 message builder for the categories above. Connecting its string to the authenticated F66C ribbon would give visually sourced artwork over incorrect game text. That must be resolved before live wiring.

## Verified new models and tests

New header-only `OriginalF790ItemCategoryRoute` reproduces the original **unsigned 8-bit** subtraction and destination selection without asserting anything about branch-specific text semantics. The C++17 regression checked **all 256 possible category bytes** and every original jump-table position; compiled and passed with strict `-Wall -Wextra -Werror -pedantic`. The binary verifier passed all six exact CODE.2 windows and the ten-branch table against the immutable original. CMake registers a C++ routing CTest and a Python source/binary guard that rejects premature Win32 hookup while the simplified description remains.

**192 CTests registered when Python is available.** Last completed full Windows build remains #651 with 182/182 at an earlier revision. The new regressions have not been run as a full native Windows build; original-DOS parity, readiness and gameplay acceptance remain unchanged. Original art and the live renderer were not modified.

**Next:** reconstruct the three original F790 text-builder branches completely from CODE.2 and associated TEXT/object resources, including known/variant and formatting rules, then map the 17 original F790 caller events and connect the existing shared RNG. Do not advance independent Level23 gameplay or redesign the game.
