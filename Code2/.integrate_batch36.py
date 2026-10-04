from pathlib import Path
import base64,gzip
root=Path('Code2')
code_p=root/'Code2.cpp'; payload_p=root/'.batch36_fragment.cpp.gz.b64'
ledger_p=root/'RESIDUAL_LEDGER.md'; progress_p=root/'PROGRESS.md'; add_p=root/'CODE2_CONTINUITY_ADDENDUM.md'
code=code_p.read_text()
assert 'Batch 36:' not in code
frag=gzip.decompress(base64.b64decode(payload_p.read_text().strip())).decode().rstrip()+"\n\n"
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 114822u;','constexpr u32 kVerifiedTranslatedBytes = 118294u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 2246u;','constexpr u32 kVerifiedNonCodeBytes = 2270u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 111780u +
              ((0x1AE30u-0x1A220u)-46u));'''
new='''static_assert(kVerifiedTranslatedBytes == 114822u +
              (0x1AF85u-0x1AE30u) + (0x1B1C2u-0x1AF90u) +
              (0x1B27Cu-0x1B1C4u) + (0x1B392u-0x1B27Cu) +
              (0x1B3F5u-0x1B394u) + (0x1B4F8u-0x1B3F8u) +
              (0x1B6EBu-0x1B4F8u) + (0x1B75Eu-0x1B6ECu) +
              (0x1BBD5u-0x1B760u));'''
assert old in code
code=code.replace(old,new,1)
marker='} // namespace code2'
pos=code.rfind(marker); assert pos>=0
code=code[:pos]+frag+code[pos:]
code_p.write_text(code)

ledger=ledger_p.read_text()
ranges='''1AF85..1AF8F | padding | 11
1B1C2..1B1C3 | alignment | 2
1B392..1B393 | alignment | 2
1B3F5..1B3F7 | alignment | 3
1B6EB..1B6EB | alignment | 1
1B75E..1B75F | alignment | 2
1BBD5..1BBD7 | alignment | 3
'''
end='<!-- RESIDUAL-RANGES-END -->'
assert end in ledger and '1AF85..1AF8F' not in ledger
ledger=ledger.replace(end,ranges+end,1)
ledger=ledger.replace('**Total proven non-code/data:** **2,246 bytes**.','**Total proven non-code/data:** **2,270 bytes**.',1)
ledger += '''\n\nBatch 36 evidence: `1AF85..1AF8F` is eleven zero bytes bounded by the directly reconstructed `1AE30` return and the `1AF90` entry; a full object-1 direct relative control-flow target scan found no incoming target to the range. `1B1C2..1B1C3`, `1B392..1B393`, and `1B75E..1B75F` decode as `MOV EAX,EAX`; `1B3F5..1B3F7` and `1BBD5..1BBD7` decode as `LEA EAX,[EAX+0]`; `1B6EB` is `NOP`. All are bounded compiler separators with no incoming direct relative call/jump target. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=progress.replace('object%20accounting-48.84%25-red','object%20accounting-50.30%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **114,822**','**Verified translated instruction bytes:** **118,294**',1)
progress=progress.replace('**Proven non-code/data bytes:** **2,246**','**Proven non-code/data bytes:** **2,270**',1)
progress=progress.replace('**Reconciled object bytes:** **117,068 / 239,698 = 48.8398% (48.84%)**','**Reconciled object bytes:** **120,564 / 239,698 = 50.2983% (50.30%)**',1)
progress=progress.replace('**Unresolved object bytes:** **122,630**','**Unresolved object bytes:** **119,134**',1)
progress_p.write_text(progress)

add=add_p.read_text().rstrip()+'''\n\n\n## CODE.2 Batch 36 checkpoint\n\nBatch 36 added **3,472 translated bytes** plus **24 proven non-code/data bytes**. Cumulative accounting is **118,294 translated + 2,270 non-code = 120,564 / 239,698 = 50.2983% (50.30%)**; unresolved is **119,134 bytes**.\n\nThe sequential frontier now reaches `1BBD7`; the next routine begins at `1BBD8`. Preserve `1AE30`'s disabled-path zeroing, INT 33h coordinate capture, right-button `C04C` eight-step repeat, signed `C6E8` clamp, and tail jump to `1F820`. `1AF90` builds its eight-byte direction priority map from signed high-word deltas and returns either the matching rank, the incoming fallback, or a random index when rank four is requested. `1B1C4` rotates the 1C0h orientation field modulo six and only rerolls packed coordinates on wrap to zero. `1B4F8` preserves the descriptor-controlled orientation divisor and the encoded low-word stack alias used to derive the tile index from the high words of the two new fixed-point coordinates. `1B6EC` returns true when any active actor has status bit 20h or when every active actor meets the descriptor/status qualification. `1B760` preserves the repeated `1B6EC` calls, signed comparisons after x86 two's-complement ABS, the `10h/20h` record-state transitions, type-`09h` and type-`21h` exceptions, signed steering-table remainder, and the exact early-return paths that skip the final RNG reseed. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_p.write_text(add)
