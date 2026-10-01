from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes=8349u;' in s
needle='struct FileBoundaries{'
insert='struct DosAttrResult{u16 ax=0;u16 cx=0;bool carry=false;};\n'
assert needle in s and insert not in s
s=s.replace(needle,insert+needle,1)
old='struct FileBoundaries{u32(*call_103c3)(u32)=nullptr;'
new='struct FileBoundaries{DosAttrResult(*dos_get_attr)(u32)=nullptr;u32(*call_103c3)(u32)=nullptr;'
assert old in s
s=s.replace(old,new,1)
needle2='static void set_runtime_error(FileBoundaries&b,u32 v)noexcept{'
func='// D23A..D27C: INT 21h/AH=43h attribute query. Caller flag bit 1 plus returned CX bit 0 synthesizes DOS error 6; 102D4/102EC/10314 reduce CF to 0 on success or mapped -1 on failure.\n[[nodiscard]]s32 function_d23a(FileBoundaries&b,u32 path,u32 flags)noexcept{DosAttrResult r=b.dos_get_attr?b.dos_get_attr(path):DosAttrResult{1,0,true};if(!r.carry&&(flags&2u)&&(r.cx&1u)){r.ax=0x0600u;r.carry=true;}if(!r.carry)return 0;return b.call_10314?b.call_10314(r.ax):-1;}\nstatic_assert(0xD27Du-0xD23Au==67u);\n'
# function needs call_10314 member, so insert after FileBoundaries definition instead
end='void(*set_fd_state_10415)(u32,u32)=nullptr;};\n'
assert end in s
s=s.replace(end,end+func,1)
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=8349u;','constexpr u32 kVerifiedTranslatedBytes=8416u;',1)
p.write_text(s)
q=Path('Code1/PROGRESS.md')
t=q.read_text()
t=t.replace('verified%20conversion-11.85%25-red','verified%20conversion-11.95%25-red')
t=t.replace('**Verified conversion:** 11.85%  ','**Verified conversion:** 11.95%  ')
t=t.replace('**Remaining:** 88.15%  ','**Remaining:** 88.05%  ')
t=t.replace('Current verified translated bytes: **8,349**.','Current verified translated bytes: **8,416**.')
t=t.replace('Raw coverage: **11.8589%**, displayed conservatively as **11.85%**.','Raw coverage: **11.9542%**, displayed conservatively as **11.95%**.')
t=t.replace('- Close wrapper `D573..D5AC`: **58 bytes**.','- Attribute/status wrapper `D23A..D27C`: **67 bytes**.\n- Close wrapper `D573..D5AC`: **58 bytes**.')
t=t.replace('Total integrated verified translated bytes: **8,349**.','Total integrated verified translated bytes: **8,416**.')
t=t.replace('leaving **822 unique unintegrated bytes** in that staging span','leaving **755 unique unintegrated bytes** in that staging span')
t=t.replace('The unique unintegrated instruction-bounded staging pool is **2,291 bytes**: 747 startup + 822 remaining DOS/CRT + 722 E0D0..E3B2.','The unique unintegrated instruction-bounded staging pool is **2,224 bytes**: 747 startup + 755 remaining DOS/CRT + 722 E0D0..E3B2.')
q.write_text(t)
