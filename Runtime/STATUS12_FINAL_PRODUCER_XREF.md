# CODE.2 standalone finale routine `0x23DF0..0x23E1A`

Status: **CORRECTED / BINARY-PROVEN UNREFERENCED IN SHIPPED STATIC APPLICATION CONTROL FLOW.**

This document supersedes the earlier revision that left upstream reachability unresolved.

## 1. Exact routine boundary

Object-relative `0x23DEF` is padding. The executable entry is `0x23DF0`:

```asm
23DF0  mov byte [BF38],0
23DF7  call 1F1C8
23DFC  call 1EA78
23E01  call 28168
23E06  push 0Ah
23E08  call 13EF8
23E0D  add esp,4
23E10  push 0Ch
23E12  call 205B4
23E17  add esp,4
23E1A  ret
```

The preceding routine returns at `0x23DE4`; `0x23DE5..0x23DEF` are padding/alignment. `0x23E1C` begins a separate predicate.

## 2. Body semantics

The routine performs the same semantic tail as the live five-artifact finale path:

```text
clear BF38
cleanup/presentation teardown
numbered event 10
    -> CINE10 or CINE11
controlled status 12
    -> R.EXE deletes CHARS
    -> restart CODE cycle
```

The body is real and fully executable. The question resolved here is whether the shipped application reaches it.

## 3. Direct and stored-reference audit

The complete CODE.2 object-1 application image was checked for incoming static references to `0x23DF0`.

Results:

- no `CALL rel32` target;
- no `JMP rel32` target;
- no short-jump or short-conditional target;
- no recognized compiler jump-table entry;
- no stored aligned application dword containing `0x00023DF0`.

The apparent byte sequence `F0 3D 02 00` at object offset `0x8BE8` is the displacement operand of the instruction at `0x8BE7` calling `0x2C9DC`; it is not a function pointer.

## 4. LE fixup audit

The CODE.2 LE header is at file `0x27BDC`; the fixup-page and fixup-record tables are header-relative `0x235` and `0x371` respectively.

The complete relocation stream contains no relocation target for object-1 offset `0x23DF0`. Therefore the loader does not construct an ordinary relocated callback/function pointer to this entry.

## 5. Indirect application-control audit

Known application switch/dispatch tables and relevant indirect-call machinery were audited. No gameplay/application construction path was found that can supply `0x23DF0` as a callback target.

This does not mathematically exclude every conceivable self-modifying or runtime-generated jump. It establishes the preservation-relevant static fact: **there is no shipped binary evidence for an incoming application edge.**

## 6. Final classification

```text
UNREFERENCED DUPLICATE FINALE ROUTINE
    0x23DF0
    -> event 10
    -> CINE10/CINE11
    -> status 12
```

The live five-artifact path at `0x26BB9..0x26BDB` independently inlines the same semantic sequence and remains the certified quest-finale producer.

Do not manufacture a third gameplay transition for `0x23DF0` in a native port. Preserve it in the evidence/archive layer as compiled residual code unless future dynamic evidence proves a caller.
