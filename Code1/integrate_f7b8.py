from pathlib import Path
p=Path('Code1/Code1.cpp'); s=p.read_text()
# F750 is already guest-address-safe; require that invariant before extending its parser chain.
assert 'if((flags&1u)!=0u){for(u32 i=0;i<count;++i)dst[i]=b.read_guest8?b.read_guest8(source+offset+i):0u;return offset+count;}' in s
oldfb='void(*dta_post_find)(u32)=nullptr;};'
newfb='void(*dta_post_find)(u32)=nullptr;s32(*compare_guest_string)(const char*,u32)=nullptr;};'
assert oldfb in s
s=s.replace(oldfb,newfb)
needle='struct DirectoryBoundaries{'
assert needle in s and 'function_f7b8(' not in s
block='''// F7B8..F8BC: determine required resource image size. The two-byte signature is read at the header-supplied offset and compared with the DGROUP string at 038Ch; each 24-byte record contributes its first dword.\n[[nodiscard]]u32 function_f7b8(FileBoundaries&b,u32 source,u32 flags)noexcept{\n u32 h=source;if((flags&1u)==0u){h=function_d313(b,source,0x200u,0u);if(h==0xffffffffu)return 0u;}\n u8 offbuf[4]{}; (void)function_f750(b,h,0x3cu,flags,offbuf,4u); const u32 header=load_le32(offbuf);\n char sig[4]={' ',' ',0,0}; (void)function_f750(b,h,header,flags,reinterpret_cast<u8*>(sig),2u);\n const s32 mismatch=b.compare_guest_string?b.compare_guest_string(sig,0x38cu):1;\n if(mismatch!=0){(void)function_d573(b,h);return 0u;}\n u8 hdr[0xacu]{}; (void)function_f750(b,h,header,flags,hdr,0xacu);\n u32 pos=header+load_le32(hdr+0x40u); const u32 records=load_le32(hdr+0x44u); u32 total=0u;\n for(u32 i=0;i<records;++i){u8 rec[0x18u]{};pos=function_f750(b,h,pos,flags,rec,0x18u);total+=load_le32(rec);}\n if((flags&1u)==0u)(void)function_d573(b,h);return total+0x0fu;\n}\nstatic_assert(0xF8BDu-0xF7B8u==261u);\n\n'''
s=s.replace(needle,block+needle)
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=9765u;','constexpr u32 kVerifiedTranslatedBytes=10026u;')
assert 'kVerifiedTranslatedBytes=10026u' in s
p.write_text(s)
q=Path('Code1/PROGRESS.md'); t=q.read_text()
t=t.replace('13.87%25-red','14.24%25-red').replace('**Verified conversion:** 13.87%','**Verified conversion:** 14.24%').replace('**Remaining:** 86.13%','**Remaining:** 85.76%').replace('Current verified translated bytes: **9,765**.','Current verified translated bytes: **10,026**.').replace('Total integrated verified translated bytes: **9,765**.','Total integrated verified translated bytes: **10,026**.')
import re
t=re.sub(r'Raw coverage: \*\*[0-9.]+%\*\*, displayed as \*\*[0-9.]+%\*\*\.', 'Raw coverage: **14.2411%**, displayed as **14.24%**.', t)
inv='- Resource/source reader helper `F750..F7B2`: **99 bytes**.'
assert inv in t
t=t.replace(inv,inv+'\n- Resource image-size parser `F7B8..F8BC`: **261 bytes**.')
q.write_text(t)
r=Path('Code1/STAGING_LEDGER.md'); u=r.read_text()
if 'F7B8..F8BC integrated' not in u:u+='\n- `F7B8..F8BC integrated`: 261-byte resource image-size parser directly disassembled; preserves header offset 3Ch, two-byte signature check against DGROUP+038Ch, 0ACh header read, 18h-byte record walk, and encoded close behavior.\n'
r.write_text(u)
