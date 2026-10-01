from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes=8524u;' in s
old='bool(*ioctl_get_mode)(u16,u8&)=nullptr;bool(*ioctl_set_mode)(u16,u8)=nullptr;};'
new='bool(*ioctl_get_mode)(u16,u8&)=nullptr;bool(*ioctl_set_mode)(u16,u8)=nullptr;u32(*open_core_d332)(u32,u32,u32,u32)=nullptr;};'
assert old in s
s=s.replace(old,new,1)
needle='// D4F2..D55D: commit descriptor state with AL|40h;'
insert='// D313..D331: variadic front wrapper. The first variadic dword is forwarded as D332 arg4; arg3 is exactly zero.\n[[nodiscard]]u32 function_d313(FileBoundaries&b,u32 path,u32 flags,u32 first_variadic)noexcept{return b.open_core_d332?b.open_core_d332(path,flags,0u,first_variadic):0xffffffffu;}\nstatic_assert(0xD332u-0xD313u==31u);\n'
assert needle in s
s=s.replace(needle,insert+needle,1)
needle2='// D573..D5AC: AH=3Eh close.'
insert2='// D55E..D572: fixed wrapper calls D313(path,62h,mode), which forwards mode as D332 arg4.\n[[nodiscard]]u32 function_d55e(FileBoundaries&b,u32 path,u32 mode)noexcept{return function_d313(b,path,0x62u,mode);}\nstatic_assert(0xD573u-0xD55Eu==21u);\n'
assert needle2 in s
s=s.replace(needle2,insert2+needle2,1)
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=8524u;','constexpr u32 kVerifiedTranslatedBytes=8576u;',1)
p.write_text(s)

q=Path('Code1/PROGRESS.md')
t=q.read_text()
t=t.replace('verified%20conversion-12.10%25-red','verified%20conversion-12.18%25-red')
t=t.replace('**Verified conversion:** 12.10%  ','**Verified conversion:** 12.18%  ')
t=t.replace('**Remaining:** 87.90%  ','**Remaining:** 87.82%  ')
t=t.replace('Current verified translated bytes: **8,524**.','Current verified translated bytes: **8,576**.')
t=t.replace('Raw coverage: **12.1076%**, displayed conservatively as **12.10%**.','Raw coverage: **12.1815%**, displayed conservatively as **12.18%**.')
t=t.replace('- Descriptor/IOCTL setup `D4F2..D55D`: **108 bytes**.','- Open front/fixed wrappers `D313..D331` and `D55E..D572`: **52 bytes**.\n- Descriptor/IOCTL setup `D4F2..D55D`: **108 bytes**.')
t=t.replace('Total integrated verified translated bytes: **8,524**.','Total integrated verified translated bytes: **8,576**.')
t=t.replace('leaving **647 unique unintegrated bytes** in that staging span','leaving **595 unique unintegrated bytes** in that staging span')
t=t.replace('The unique unintegrated instruction-bounded staging pool is **2,116 bytes**: 747 startup + 647 remaining DOS/CRT + 722 E0D0..E3B2.','The unique unintegrated instruction-bounded staging pool is **2,064 bytes**: 747 startup + 595 remaining DOS/CRT + 722 E0D0..E3B2.')
q.write_text(t)
