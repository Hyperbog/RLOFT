# CODE.1 `F2E9..F721` instruction map

Immutable authority: `CODE.1` SHA-256 `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`.

This is an instruction-derived control-flow ledger for the 1,081-byte materializer. It is evidence, not counted translation.

## Entry and argument state

- `F2E9..F30D`: save EBX/ESI/EDI/ES/EBP, allocate 14h locals. `ESI=[EBP+18h]` output scratch offset, `EAX=[EBP+1Ch]` address of the vararg cursor, `EBX=[EBP+20h]` format-state pointer. Local `-8=1`, state `+0Ch=0`, local selector `-4=DS`.
- `F30D..F378`: the shared-branch tree pre-reads an unsigned integer argument only for `X/o/u/x`. This point is easy to misread because `F32B` deliberately jumps into the `JE` at `F327`, reusing flags from `cmp dl,6Fh`; therefore `o` does reach `F33E`. Flag 20h consumes dword, 10h consumes zero-extended word, default consumes dword. The value is stored at local `-10h`. `and byte [state+14h],F9h` clears flag bits `02h` and `04h` while preserving bit `01h`. `d/i` do **not** pre-read here; they consume their signed argument later at `F4BA`. A specified precision changes pad byte `+16h` to space.
- `F378..F435`: conversion-character decision tree. Local radix starts at 10.

## Conversion families

- `F435..F45C`: `E/F/G/e/f/g` with flag 10h consumes one dword and calls `F206`, then prepares `{scratch,DS,-1}` for `F163`.
- `F45D..F476`: remaining floating conversions call `F2D2(output,vararg_cursor,state)`. The callback result is a register pair: EAX is an offset and DX is its selector. Both are preserved for the following selector-aware `F163` call. A C++ boundary that returns EAX alone is insufficient.
- `F477..F4B9`: octal uses the pre-read local `-10h`. Radix=8; alternate-form flag 01h emits a leading `0`; integer conversion uses `E098`. If precision is zero and the converted text starts with `0`, the first output byte is replaced by NUL. Length is then obtained through `F163`.
- `F4BA..F4FB`: signed decimal `d/i` consumes its argument here. Flag 20h consumes signed dword, flag 10h consumes sign-extended word, otherwise consumes signed dword. Conversion uses radix 10 and `E098`/`DFFB` exactly as encoded.
- `F4FC..F59B`: `s/S`. Flag 80h consumes an 8-byte far pointer `{offset,selector}`; flag 40h consumes a near offset; default consumes a near offset. Null pointer falls back to `[EBP+24h]`. `S` is not allowed to be normalized into the ordinary string-length path: with flag 20h it reads one selector-qualified word into EAX and advances EDI by 2; otherwise it reads one selector-qualified byte into EAX and advances EDI by 1, then jumps directly to `F695`. Non-`S` wide strings call `F18C`; byte strings call `F163`.
- `F59C..F5E3`: hexadecimal `X/x` and unsigned decimal `u`. Alternate hex with nonzero value writes prefix bytes `0`, conversion character, NUL at state `+17h..+19h`. Radix becomes 16 for hex. Unsigned conversion calls `E050`; `X` then calls `F722` to uppercase the generated digits.
- `F5E4..F660`: pointer `P/p`. Default width becomes 13 for far pointers (flag 80h) or 8 otherwise. `AND F9h` clears flag bits 02h/04h. The first consumed dword is retained at local `-14h`. Far form consumes another dword, masks it to its low 16 bits, converts that as four hex digits with `F1B3`, writes `:` at scratch+4, then converts the retained dword as eight hex digits at scratch+5. Near form converts the retained dword as eight hex digits. `P` uppercases via `F722`.
- `F661..F674`: `c`. Consume one dword, use its low byte, append NUL, clear local sign-processing flag, then enter one-character accounting.
- `F675..F694`: unknown/default conversion. Width is forced to zero, conversion byte itself is emitted followed by NUL, sign-processing is disabled, precision becomes one, and `AND F9h` clears flags 02h/04h.

## Common accounting tail

- `F695..F6D7`: when local `-8` is nonzero, inspect the first selector-qualified output byte. Leading `-` decrements EAX, is moved to state prefix `+17h`, and advances EDI; otherwise flags 02h/04h select space/plus prefixes. If EAX is greater than precision, precision becomes EAX; otherwise state `+0Ch` receives `precision-EAX`.
- `F6D8..F6E5`: `*` pad byte suppresses prefix and clears flags `02h/04h` via `AND F9h`.
- `F6E6..F6F9`: if precision is `FFFFFFFFh`, or EAX is smaller than precision, precision becomes EAX except for conversion `c`.
- `F6FA..F71A`: call `F163(state+17h,DS,-1)`, add state precision and state `+0Ch`, then subtract the sum from field width `state+4`.
- `F71B..F721`: return EDI through EAX and tail-jump to `F001` epilogue; DX is reloaded from the saved selector immediately before this tail.

## Register-width / memory-model constraints for the C++ lift

1. Far pointers are not host pointers. Preserve 32-bit offset plus 16-bit selector independently.
2. The vararg cursor is mutated in memory before each argument fetch (`add dword ptr [eax],4/8` then read from the new cursor minus width).
3. `F45D` requires the `F2D2` callback's EAX:DX pair. DX must not be discarded or widened into a flat address.
4. `F4FC` treats `{offset=0,selector=0}` as null for the far-pointer form; either nonzero component is accepted.
5. `F695` reads through ES:EDI. The selector chosen by the conversion path must survive until the common accounting block.
6. Width, precision, zero-count and prefix fields are mutated in place and observable by the caller; no libc-format abstraction is permitted.
7. `F32B -> F327` is a flags-sharing control-flow edge. A structured lift must preserve the fact that `o` reaches the pre-read path even though `F327` visually sits after `cmp dl,78h`.
8. `S` reaches `F695` with EAX loaded from the first selector-qualified byte/word and EDI already advanced. Replacing this with `strlen`/wide-`strlen` changes the encoded state and is not faithful.
