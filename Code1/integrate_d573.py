from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
old='s32(*dos_seek)(u16,s32,u8)=nullptr;s32(*dos_delete)(const char*)=nullptr;};'
new='s32(*dos_seek)(u16,s32,u8)=nullptr;s32(*dos_delete)(const char*)=nullptr;DosMemResult(*dos_close)(u16)=nullptr;void(*set_fd_state_10415)(u32,u32)=nullptr;};'
assert old in s
s=s.replace(old,new,1)
needle='[[nodiscard]]s32 function_d5ad(FileBoundaries&b,u16 handle,u8*dst,u32 count)noexcept{'
insert='// D573..D5AC: AH=3Eh close. RCL/ROR materializes CF in EAX sign; success clears descriptor state, failure stores runtime errno 4.\n[[nodiscard]]u32 function_d573(FileBoundaries&b,u32 h)noexcept{const DosMemResult r=b.dos_close?b.dos_close(static_cast<u16>(h)):DosMemResult{true,1,0};if(!r.carry){if(b.set_fd_state_10415)b.set_fd_state_10415(h,0);return 0;}set_runtime_error(b,4);return 0xffffffffu;}\nstatic_assert(0xD5ADu-0xD573u==58u);\n'
assert needle in s
s=s.replace(needle,insert+needle,1)
assert 'constexpr u32 kVerifiedTranslatedBytes=8291u;' in s
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=8291u;','constexpr u32 kVerifiedTranslatedBytes=8349u;',1)
p.write_text(s)

q=Path('Code1/PROGRESS.md')
t=q.read_text()
t=t.replace('verified%20conversion-11.77%25-red','verified%20conversion-11.85%25-red')
t=t.replace('**Verified conversion:** 11.77%  ','**Verified conversion:** 11.85%  ')
t=t.replace('**Remaining:** 88.23%  ','**Remaining:** 88.15%  ')
t=t.replace('Current verified translated bytes: **8,291**.','Current verified translated bytes: **8,349**.')
t=t.replace('Raw coverage: **11.7766%**, displayed conservatively as **11.77%**.','Raw coverage: **11.858%**, displayed conservatively as **11.85%**.')
t=t.replace('- File read/write/seek/delete `D5AD..D8CD`: **801 bytes**.','- Close wrapper `D573..D5AC`: **58 bytes**.\n- File read/write/seek/delete `D5AD..D8CD`: **801 bytes**.')
t=t.replace('Total integrated verified translated bytes: **8,291**.','Total integrated verified translated bytes: **8,349**.')
t=t.replace('`DosCrtNext.staging.cpp` covers **880 functional instruction bytes** in `D23A..D5AC`','`DosCrtNext.staging.cpp` originally covered **880 functional instruction bytes** in `D23A..D5AC`; `D573..D5AC` is now integrated, leaving **822 unique unintegrated bytes** in that staging span')
t=t.replace('The unique unintegrated instruction-bounded staging pool is **2,349 bytes**: 747 startup + 880 DOS/CRT + 722 E0D0..E3B2.','The unique unintegrated instruction-bounded staging pool is **2,291 bytes**: 747 startup + 822 remaining DOS/CRT + 722 E0D0..E3B2.')
q.write_text(t)
