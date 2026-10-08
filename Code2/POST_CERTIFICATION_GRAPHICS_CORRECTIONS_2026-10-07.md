# CODE.2 post-certification graphics semantic corrections — 2026-10-07

## Scope and verified binary identity

This is a narrowly scoped correction to the reconstructed C++ semantics, **not** a change to the shipped game or to object-1 byte coverage. Immutable original `CODE.2`: 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; object-1 starts at physical file `0x3AF24`. The uploaded original binary was independently hashed and its relevant bytes disassembled with `objdump -D -b binary -m i386 -M intel`.

The prior 2026-10-05 certified reconstruction is retained in Git history as blob `c3f16122e79c3c464382e4511d9d932f6fcd694a`. A direct full-content comparison to the demo-branch version `87f6638921c53a32bde59735515fbc175e2c96ee` found **exactly two change regions**. Both edits below are supported by the original instructions. The corrected source blob has now been synchronized to both `code1-decompilation` and `demo-v1`: `87f6638921c53a32bde59735515fbc175e2c96ee`. The original historical certification identity and its verification result remain true **only of the original certified snapshot**.

## Correction 1 — automap graphic placement, logical `0x0139C..0x01556`

Original physical bytes `CODE.2[0x3C2C0..0x3C47A]`. In `function_0139c`, the previous C++ source called both `function_00b6c` and `function_00cb0` with constant last parameters `6,4` for every visible cell. This discarded encoded per-column and per-row draw-position increments.

Original instruction witnesses:

- `013B2: lea ebx,[edi*8]`; `013BD: lea eax,[ebx+6]`; `013C0..013C6: neg ebx; add eax,ebx` produce the initial horizontal drawing origin of `6`.
- `0143E..01448`: `inc edi; add [esp+0x10],8` advances the first drawing pass by **8 per source column**, including clipped columns.
- `01406..01412` creates the first row's drawing coordinate, and `01436..0143C`: `inc ebx; add esi,5` advances it by **5 per source row**, including clipped rows.
- The second pass independently rebuilds the horizontal/vertical coordinate state at `01454..014B2` and increments row/column coordinates at `014D6..014E8`.

The source now passes `draw0=6+8*(col_index-original_col)` and `draw1=4+5*(row_index-original_row)` (with the original loop ordering and 32-bit wrapping arithmetic) to both original draw helpers. Rendering only within cell bounds must not skip coordinate advancement.

## Correction 2 — five-slot graphics wrapper, logical `0x2975C..0x29789`

Original physical bytes `CODE.2[0x64680..0x646AD]`. The previous C++ wrapper declared only four parameters (`a1,a2,key,set`) and passed `a1,a2,key,image` to `2A380`, conflating the third coordinate and resource key.

Original instruction witnesses:

- `2975C` / `29760`: two successive `push [esp+0x14]` consume original argument slots **5, then 4** because the first push moves ESP. The call at `29764` invokes `2F29C(key,set)`.
- `29770`: push the resulting image; `29771`, `29775`, `29779`: three successive `push [esp+0x10]` select original slots **3, 2, 1**. Call `2977D` is `2A380(frame,y,x,image)`.
- `29785`: tail jump to `2E758` after stack cleanup; the existing return behavior is preserved.

The corrected C++ signature is `function_2975c(Application57Boundary&, frame, y, x, key, set)`. Neither change alters original function boundaries or serialized file formats.

## Verification and certification boundary

1. Compared the complete texts of the two 1.3-MB reconstructions: two differences only, confined to the routines above.
2. Verified the supplied `CODE.2` binary SHA-256 and disassembled both physical byte ranges directly from that file.
3. Reused the exact previously existing demo Git blob on the historical main branch; GitHub confirmed matching post-update blob SHA on both branches.
4. The prior full-object byte-accounting figure (239,698/239,698) and past strict C++17 certification remain **historical audit results**. No post-synchronization strict full-file C++17 compilation, runtime frame comparison, or fresh DOS/port integration test is claimed here. Those are independent future verification gates.

For any future semantic change: preserve the previous source commit, cite the original instruction range, reproduce the assembly-to-source check and re-run `g++ -std=c++17 -Wall -Wextra -pedantic -fsyntax-only Code2/Code2.cpp`. Do not confuse post-certification corrections with unaccounted original bytes.
