from pathlib import Path

cpp_path=Path('Code1/Code1.cpp')
audit_path=Path('Code1/FINAL_BINARY_AUDIT.md')
cpp=cpp_path.read_text()

old="""[[nodiscard]]u32 function_f163(FarReadBoundary&b,u32 offset,u16 selector,u32 limit)noexcept{u32 n=0;while(n!=limit){if(!b.read8||b.read8(selector,offset)==0)break;++offset;++n;}return n;}\n[[nodiscard]]u32 function_f18c(FarReadBoundary&b,u32 offset,u16 selector,u32 limit)noexcept{u32 n=0;while(n!=limit){if(!b.read16||b.read16(selector,offset)==0)break;offset+=2;++n;}return n;}"""
new="""[[nodiscard]]u32 function_f163(FarReadBoundary&b,u32 offset,u16 selector,u32 limit)noexcept{u32 n=0;for(;;){const u8 v=b.read8?b.read8(selector,offset):0u;offset+=1u;if(v==0u||n==limit)return n;++n;}}\n[[nodiscard]]u32 function_f18c(FarReadBoundary&b,u32 offset,u16 selector,u32 limit)noexcept{u32 n=0;for(;;){const u16 v=b.read16?b.read16(selector,offset):0u;offset+=2u;if(v==0u||n==limit)return n;++n;}}"""
assert old in cpp
cpp=cpp.replace(old,new,1)

old_loop="for(u32 i=0;i<s.precision;++i){fixed&=0x0000ffffu;fixed*=10u;*end++=static_cast<char>(static_cast<u8>(fixed>>16)+static_cast<u8>('0'));}"
new_loop="for(u32 i=0;static_cast<s32>(i)<static_cast<s32>(s.precision);++i){fixed&=0x0000ffffu;fixed*=10u;*end++=static_cast<char>(static_cast<u8>(fixed>>16)+static_cast<u8>('0'));}"
assert old_loop in cpp
cpp=cpp.replace(old_loop,new_loop,1)

old_shift="const u32 data_off=(load_le32(map)<<page_shift)+data_pages_off;"
new_shift="const u32 data_off=(load_le32(map)<<(page_shift&31u))+data_pages_off;"
assert old_shift in cpp
cpp=cpp.replace(old_shift,new_shift,1)

cpp_path.write_text(cpp)

audit=audit_path.read_text()
entry="""

## Semantic audit checkpoint 10l — `EDE7..FC97`

Fresh direct disassembly of immutable `CODE.1` reconciled the complete formatter driver/parser/materializer and helper block `EDE7..F741`, the source reader and resource-size parser `F750..F8BC`, the resource image loader/relocator `F8C4..FC96`, and the LE-relocated one-byte `FC97` RET callback. The original file and extracted-object SHA-256 values were reverified before this pass.

Three semantic corrections were required. `F163` and `F18C` are not ordinary bounded strlen loops: the machine loads the current byte/word first, advances its local source pointer, and only then compares the already-counted length against the limit. Consequently a non-NUL string incurs one look-ahead memory read at `limit` (including when the limit is zero). The prior C++ prechecked `n != limit` and omitted that architecturally observable read. Both helpers now preserve the encoded read-before-limit-test ordering while returning the same count.

`F206`'s 16.16 fixed-point fractional-digit loop uses `CMP EAX,[precision]` / signed `JL`. The previous C++ used an unsigned `i < precision` loop, which diverged if a parsed precision wrapped into the signed-negative range. The loop now compares the modulo-32-bit counter and precision as signed 32-bit values exactly as encoded. The surrounding sign handling, decimal-point generation, fractional multiply/truncate sequence, and carry/rounding propagation were rechecked unchanged.

In `F8C4`, the page-data offset uses `SHL EAX,CL`; x86 masks the shift count to five bits. The previous C++ shifted by the raw header byte and could invoke host undefined behavior for counts of 32 or more. The reconstructed expression now applies `page_shift & 31` before the 32-bit shift. The object/page loops, 16-byte alignment gate, virtual-size clipping, fixup validation, relocation patch addresses, and cleanup paths were otherwise rechecked directly.

`EDE7..F006` retains the encoded `%n` near/far and word/dword store variants, two-byte `s/S` source stepping, prefix/zero/field padding loops, and the shared `F001` epilogue relationship. `F007..F162`, `F1B3..F2E8`, `F2E9..F721`, `F722..F741`, `F750..F7B2`, and `F7B8..F8BC` were otherwise instruction-reconciled without further semantic change. The known non-code/alignment islands `F742..F74F`, `F7B3..F7B6`, `F7B7`, `F8BD..F8C0`, and `F8C1..F8C3` remain unchanged. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `FC97`; the next code begins at `FC98`.
"""
assert 'Semantic audit checkpoint 10l' not in audit
audit_path.write_text(audit+entry)
print('applied semantic audit checkpoint 10l')
