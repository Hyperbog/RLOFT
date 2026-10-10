# CODE.2 post-certification F790 stack-local text pre-read correction — 2026-10-10

## Source and branch ownership

**Workstream A (decompilation only):** `code1-decompilation`, canonical `Code2/Code2.cpp`. This is a narrowly bounded correction to an already byte-accounted and semantically audited reconstruction, **not** a change to any gameplay host, graphics, DOS image, or `demo-v1` source. It was identified by independently comparing the current canonical `function_0f790` with the immutable original executable during native-to-decomp feedback review.

Immutable original `CODE.2`: **558,689 bytes**, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; executable object 1 physically at `0x3AF24`, logical length `0x3A852`.

## Exact original control flow

```asm
F794  sub esp,10h              ; reserve 16 bytes of stack locals
F79B  ...                      ; restore saved VGA background via 2932C
F7B4  cmp byte ptr [6DACh],0
F7BB  je F7CC                  ; pending feedback false
F7BD  push 759Ch
F7C2  call F66C                ; pending feedback bypasses text pre-read
F7C7  jmp FD79
F7CC  push 3Fh                 ; source T record 63
F7CE  lea eax,[esp+4]         ; pointer into original stack local area
F7D2  push eax
F7D3  call 1C368              ; indexed file seek/read/XOR-decode
F7D8  add esp,8
F7DB  cmp ebx,-1              ; only NOW checks empty item hand
F7DE  je FD7C
```

The full 15-byte original instruction witness is `6A 3F 8D 44 24 04 50 E8 90 CB 00 00 83 C4 08` at logical `F7CC..F7DA`. The five-byte `E8` at `F7D3` resolves to `1C368`. The entry's stack-local allocation is `83 EC 10` at `F794`. The original `1C368` routine takes a real destination address and T index, accesses the source text-file handle at DGROUP `0x22224`, performs indexed seeks/reads, stores a trailing NUL, and XOR-decodes data with `0xDC`.

Independently decoding the original `T` archive in `DnD-Classics/MASTER` confirms **`T[0x3F]` is exactly `"D"`**, one encoded byte plus the terminating zero written by `1C368`. The stack-local data does not appear to be used to construct the final description string: subsequent F790 display text is built at DGROUP `759C`. Nonetheless the source **always performs the text-file read when `6DAC==0`**, including when `item==-1`. Omitting it changes file I/O sequencing, read-buffer writes, and any stream state that crosses this call.

## Corrected canonical source

The previous `function_0f790` started with background restoration, checked `6DAC`, and then immediately returned for item `-1`; it contained no `T[0x3F]` read. The correction adds:

- `Application21Boundary::call_1c368_local(u8* local_buffer, u32 text_index)`: an explicitly **stack-local** boundary for the original `1C368` call. Passing an invented DGROUP address to the old `call_1c368(u32 guest_offset,u32 index)` would incorrectly alias guest global memory, so the boundary is separate.
- `u8 text_3f_stack_local[0x10u];` in `function_0f790`, matching the encoded 16-byte allocation (uninitialized as in original stack storage).
- A conditional boundary invocation for `T[0x3F]` **after** the pending-message early exit but **before** the empty-hand return.

The new callback is optional, like the existing opaque CODE.2 external boundaries; full source-equivalent file sequencing requires a future faithful hookup of the same original indexed text reader, not a synthetic substitute. No original `T` asset is copied into the repository.

## Verification and remaining gates

- Re-extracted the original 239,698-byte CODE.2 object and regenerated `F790` and `1C368` with `objdump -D -b binary -m i386 -M intel`.
- **PASS** immutable CODE.2 SHA-256, F790 local allocation, full pending branch, original 15-byte pre-read witness, rel32 target to `1C368`, empty-hand branch and `1C368` terminator bytes.
- **PASS** decoding original `T[0x3F]` as a single `D` from the authenticated DnD-Classics archive blob.
- **PASS** a strict `g++ -std=c++17 -Wall -Wextra -Werror -pedantic` isolated callback order smoke test, including pending/no-pending and empty-item branches.
- **Open verification gate:** a fresh **whole-file** `g++ -std=c++17 -Wall -Wextra -pedantic -fsyntax-only Code2/Code2.cpp` has not yet run against this corrected 1.3 MB revision; this must be performed when the branch is available to the compiler. Do not attribute historical October 5 certification to the corrected file. Real original-DOS stream-state equivalence is likewise not accepted.
- A permanent self-check `Code2/verify_f790_stack_local_preread.py` validates source order by default and original CODE.2/T bytes when supplied. Run the command from the repository root as documented in the script.

**Object accounting stays exactly 235,022 translated code bytes + 4,676 proven non-code/data = 239,698/239,698; zero unaccounted bytes. No certification percentage increase.**

## Cross-workstream handoff — Native Engine `demo-v1`

The decompilation reveals an original side effect previously absent from source-only F790 handling: **a non-pending empty-hand F790 call still performs the T[3F] indexed stream read before returning**. Pending `6DAC` calls skip this pre-read; non-empty ordinary calls also perform it. The native stream/text subsystem should consider that source observable call ordering during strict parity work, but **do not perform F790 on 70-Hz UI repaint** or introduce a new RNG seed. A separate native conversation owns any implementation or behavior verification. No changes to `demo-v1` were made here.

Source code evidence: commit `972dee94221ccdd7e759aa4709591cf8c65ae543` (plus this documentation checkpoint). Canonical source path: `Code2/Code2.cpp`. Test: `Code2/verify_f790_stack_local_preread.py`.


## Additional original address-identity repair (2026-10-10)

The earlier `Application21Boundary` source named the resource-reader boundary `call_2c368`, despite all five original F790 reader E8 instructions (`F7D3`, `F91C`, `FA9D`, `FB5A`, `FC78`) targeting **`1C368`**. The original spell-scribe read at `116E2` also calls `1C368`, and canonical CODE.2 already implements `function_1c368`. A source-only rename corrected all **28 occurrences** of the stale boundary symbol to `call_1c368`, including the local-stack form; full before/after source comparison confirmed there were no other changes. Current canonical source Git blob: `5b0b4dd7b6a91bfcc99068970c3152f0f62ce54e`. All 21 original F790 direct E8 targets were checked; strict whole-file C++17 compilation of this updated source remains pending. These corrections belong to the decompilation branch; `demo-v1` was not modified.
