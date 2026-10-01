# CODE.1 `F2E9..F721` instruction map

Immutable authority: `CODE.1` SHA-256 `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`.

This is an instruction-derived control-flow ledger for the 1,081-byte materializer. It is evidence, not counted translation.

## Entry and argument state

- `F2E9..F30D`: save EBX/ESI/EDI/ES/EBP, allocate 14h locals. `ESI=[EBP+18h]` output scratch offset, `EAX=[EBP+1Ch]` address of the vararg cursor, `EBX=[EBP+20h]` format-state pointer. Local `-8=1`, state `+0Ch=0`, local selector `-4=DS`.
- `F30D..F378`: pre-read integer argument for `X,d,i,o,u,x` according to length flags. Flag 20h consumes dword; 10h consumes zero-extended word; default consumes dword. Stored at local `-10h`. Flags 01h/02h are cleared by `and byte [state+14h],F9h`. A specified precision changes pad byte `+16h` to space.
- `F378..F435`: conversion-character decision tree. Local radix starts at 10.

## Conversion families

- `F435..F45C`: `E/F/G/e/f/g` with flag 10h uses the `F206` fixed-point helper, then prepares selector/length arguments for `F163`.
- `F45D..F476`: remaining floating conversions call `F2D2(output,vararg_cursor,state)`. Returned EAX is an offset and DX is its selector; both are preserved for the following selector-aware length call.
- `F477..F4B9`: octal. Radix=8; alternate-form flag 01h emits a leading `0`; integer conversion uses `E098`. If precision is zero and the converted text is exactly `0`, the first output byte is replaced by NUL. Length is then measured with `F163`.
- `F4BA..F4FB`: signed decimal `d/i`. Flag 20h consumes signed dword, flag 10h consumes sign-extended word, otherwise consumes signed dword. Conversion uses radix 10 and `E098`/`DFFB` exactly as encoded.
- `F4FC..F59B`: `s/S`. Flag 80h consumes an 8-byte far pointer `{offset,selector}`; flag 40h consumes a near offset; default consumes a near offset. Null pointer falls back to `[EBP+24h]`. `S` with flag 20h reads a selector-qualified word and advances by 2; otherwise it reads a selector-qualified byte and advances by 1. Non-`S` wide strings use `F18C`; byte strings use `F163`.
- `F59C..F5E3`: hexadecimal `X/x` and unsigned decimal `u`. Alternate hex with nonzero value writes prefix bytes `0`, conversion character, NUL at state `+17h..+19h`. Radix becomes 16 for hex. Unsigned conversion calls `E050`; `X` then calls `F722` to uppercase the generated digits.
- `F5E4..F660`: pointer `P/p`. Default width becomes 13 for far pointers (flag 80h) or 8 otherwise. Flags 01h/02h are cleared. The first consumed dword is retained at local `-14h`. Far form consumes another argument, converts the low 16 bits as four hex digits with `F1B3`, writes `:` at scratch+4, then converts the retained dword as eight hex digits at scratch+5. Near form converts the retained dword as eight hex digits. `P` uppercases via `F722`.
- `F661..F674`: `c`. Consume one dword, use its low byte, append NUL, clear local sign-processing flag, then enter one-character accounting.
- `F675..F694`: unknown/default conversion. Width is forced to zero, conversion byte itself is emitted followed by NUL, sign-processing is disabled, precision becomes one, flags 01h/02h are cleared.

## Common accounting tail

- `F695..F6D7`: when sign-processing is active, inspect the first selector-qualified output byte. Leading `-` is removed from measured text and copied to state prefix `+17h`; otherwise flags 02h/04h select space/plus prefixes. Precision is raised to measured length when needed, or zero-fill count `+0Ch` is set to `precision-length`.
- `F6D8..F6E5`: `*` pad byte suppresses prefix and clears flags 01h/02h.
- `F6E6..F6F9`: precision is replaced by measured length when unspecified or shorter, except conversion `c`.
- `F6FA..F71A`: call `F163(state+17h,DS,-1)` to measure prefix, add precision and zero-fill count, subtract total from field width `state+4`.
- `F71B..F721`: return EDI through EAX and tail-jump to `F001` epilogue.

## Register-width / memory-model constraints for the C++ lift

1. Far pointers are not host pointers. Preserve 32-bit offset plus 16-bit selector independently.
2. The vararg cursor is mutated in memory before each argument fetch (`add dword ptr [eax],4/8` then read from the new cursor minus width).
3. `F45D` returns an offset in EAX and selector in DX; DX must not be widened by inventing a flat address.
4. `F4FC` treats `{offset=0,selector=0}` as null for the far-pointer form; either nonzero component is accepted.
5. `F695` reads the first output byte through ES:EDI, so the selector selected by the conversion path must survive until common accounting.
6. Width, precision and prefix fields are mutated in place and are observable by the caller; no libc-format abstraction is permitted.
