from pathlib import Path
p=Path('Code1/Code1.cpp'); s=p.read_text()
needle='struct DirectoryBoundaries{'
assert needle in s
assert 'function_f750(' not in s
block='''// F750..F7B2: source reader used by the following resource parsers. Bit 0 selects an in-memory source; otherwise seek/read through the DOS wrappers and return the handle's resulting current position.\n[[nodiscard]]u32 function_f750(FileBoundaries&b,u32 source,u32 offset,u32 flags,u8*dst,u32 count)noexcept{\n if((flags&1u)!=0u){for(u32 i=0;i<count;++i)dst[i]=b.read_guest8?b.read_guest8(source+offset+i):0u;return offset+count;}\n (void)function_d85f(b,static_cast<u16>(source),static_cast<s32>(offset),0u);\n (void)function_d5ad(b,static_cast<u16>(source),dst,count);\n return static_cast<u32>(function_d85f(b,static_cast<u16>(source),0,1u));\n}\nstatic_assert(0xF7B3u-0xF750u==99u);\n\n'''
s=s.replace(needle,block+needle)
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=9666u;','constexpr u32 kVerifiedTranslatedBytes=9765u;')
assert 'kVerifiedTranslatedBytes=9765u' in s
p.write_text(s)
q=Path('Code1/PROGRESS.md'); t=q.read_text()
t=t.replace('13.72%25-red','13.87%25-red').replace('**Verified conversion:** 13.72%','**Verified conversion:** 13.87%').replace('**Remaining:** 86.28%','**Remaining:** 86.13%').replace('Current verified translated bytes: **9,666**.','Current verified translated bytes: **9,765**.').replace('Total integrated verified translated bytes: **9,666**.','Total integrated verified translated bytes: **9,765**.')
import re
t=re.sub(r'Raw coverage: \*\*[0-9.]+%\*\*, displayed as \*\*[0-9.]+%\*\*\.', 'Raw coverage: **13.8703%**, displayed as **13.87%**.', t)
inv='- Complete formatter driver/parser/materializer `EDE7..F741`: **2,395 bytes**.'
assert inv in t
t=t.replace(inv,inv+'\n- Resource/source reader helper `F750..F7B2`: **99 bytes**.')
q.write_text(t)
r=Path('Code1/STAGING_LEDGER.md'); u=r.read_text()
if 'F750..F7B2 integrated' not in u:u+='\n- `F750..F7B2 integrated`: 99-byte source reader directly disassembled; bit 0 selects the guest-memory copy path, otherwise the encoded D85F/D5AD/D85F seek-read-position sequence is retained.\n'
r.write_text(u)
