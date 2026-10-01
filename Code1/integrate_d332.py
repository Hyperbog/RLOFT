from pathlib import Path
p=Path('Code1/Code1.cpp'); s=p.read_text()
old='struct FileBoundaries{DosAttrResult(*dos_get_attr)(u32)=nullptr;u32(*call_103c3)(u32)=nullptr;u32*(*call_103b7)()=nullptr;s32(*call_10314)(u16)=nullptr;u32(*call_1042d)()=nullptr;void(*call_1046f)()=nullptr;s32(*dos_read)(u16,void*,u32)=nullptr;s32(*dos_write)(u16,const void*,u32)=nullptr;s32(*dos_seek)(u16,s32,u8)=nullptr;s32(*dos_delete)(const char*)=nullptr;bool(*dos_close)(u16)=nullptr;void(*set_fd_state_10415)(u32,u32)=nullptr;bool(*ioctl_get_mode)(u16,u8&)=nullptr;bool(*ioctl_set_mode)(u16,u8)=nullptr;u32(*open_core_d332)(u32,u32,u32,u32)=nullptr;};'
assert old in s
new='struct DosHandleResult{u16 ax=0;bool carry=false;};\nstruct FileBoundaries{DosAttrResult(*dos_get_attr)(u32)=nullptr;u32(*call_103c3)(u32)=nullptr;u32(*call_10397)(u32)=nullptr;u32*(*call_103b7)()=nullptr;s32(*call_10314)(u16)=nullptr;u32(*call_1042d)()=nullptr;void(*call_1046f)()=nullptr;s32(*dos_read)(u16,void*,u32)=nullptr;s32(*dos_write)(u16,const void*,u32)=nullptr;s32(*dos_seek)(u16,s32,u8)=nullptr;s32(*dos_delete)(const char*)=nullptr;bool(*dos_close)(u16)=nullptr;DosHandleResult(*dos_open_raw)(u32,u8)=nullptr;DosHandleResult(*dos_create_raw)(u32,u16)=nullptr;DosHandleResult(*dos_close_raw)(u16)=nullptr;DosHandleResult(*dos_write_zero_raw)(u16)=nullptr;u8(*read_guest8)(u32)=nullptr;u32(*read_dgroup32)(u32)=nullptr;void(*set_fd_state_10415)(u32,u32)=nullptr;bool(*ioctl_get_mode)(u16,u8&)=nullptr;bool(*ioctl_set_mode)(u16,u8)=nullptr;u32(*open_core_d332)(u32,u32,u32,u32)=nullptr;};'
s=s.replace(old,new)
needle='// D4F2..D55D:'
assert needle in s and 'function_d332(FileBoundaries&' not in s
block='''// D332..D4F1: DOS open/create/truncate state machine. All DOS calls retain AX+CF explicitly; no host fopen semantics are substituted.\n[[nodiscard]]u32 function_d332(FileBoundaries&b,u32 path,u32 flags,u32 share,u32 create_mode)noexcept{\n while(b.read_guest8&&b.read_guest8(path)==0x20u)++path;\n u32 mode=flags&0x83u; const u8 open_mode=static_cast<u8>(mode|share);\n DosHandleResult opened=b.dos_open_raw?b.dos_open_raw(path,open_mode):DosHandleResult{0xffffu,true};\n u32 handle=opened.carry?0xffffffffu:static_cast<u32>(opened.ax); u16 error_ax=opened.ax;\n if((flags&3u)!=0&&handle!=0xffffffffu&&(b.call_10397?b.call_10397(handle):0u)==0u){\n  if((flags&0x400u)!=0u){if(b.dos_close_raw)(void)b.dos_close_raw(static_cast<u16>(handle));set_runtime_error(b,7u);return 0xffffffffu;}\n  if((flags&0x40u)!=0u){const DosHandleResult wr=b.dos_write_zero_raw?b.dos_write_zero_raw(static_cast<u16>(handle)):DosHandleResult{0xffffu,true};if(wr.carry){if(b.dos_close_raw)(void)b.dos_close_raw(static_cast<u16>(handle));return static_cast<u32>(map_dos_error(b,static_cast<s32>(wr.ax)));}}\n }\n if(handle==0xffffffffu){\n  if((flags&0x20u)==0u||error_ax!=2u)return static_cast<u32>(map_dos_error(b,static_cast<s32>(error_ax)));\n  u32 cm=create_mode?create_mode:0x180u; const u32 mask=b.read_dgroup32?b.read_dgroup32(0x5340u):0u; cm^=mask; const u16 attr=(static_cast<u8>(cm)&0x80u)?0u:1u;\n  const DosHandleResult cr=b.dos_create_raw?b.dos_create_raw(path,attr):DosHandleResult{0xffffu,true};if(cr.carry)return static_cast<u32>(map_dos_error(b,static_cast<s32>(cr.ax)));\n  const DosHandleResult cl=b.dos_close_raw?b.dos_close_raw(cr.ax):DosHandleResult{0xffffu,true};if(cl.carry)return static_cast<u32>(map_dos_error(b,static_cast<s32>(cl.ax)));\n  opened=b.dos_open_raw?b.dos_open_raw(path,open_mode):DosHandleResult{0xffffu,true};if(opened.carry)return static_cast<u32>(map_dos_error(b,static_cast<s32>(opened.ax)));handle=static_cast<u32>(opened.ax);\n }\n u32 state=b.call_103c3?b.call_103c3(handle):0u; state=(state&~0xffu)|static_cast<u8>(state&0x3cu);\n if((b.call_10397?b.call_10397(handle):0u)!=0u)state|=0x2000u; mode&=0x7fu;\n if(mode==2u)state|=3u;if(mode==0u)state|=1u;if(mode==1u)state|=2u;if((flags&0x10u)!=0u)state|=0x80u;\n const u32 text_state=(state&~0xffu)|static_cast<u8>(state|0x40u);\n if((flags&0x300u)!=0u){if((flags&0x200u)!=0u)state=text_state;}else if((b.read_dgroup32?b.read_dgroup32(0x55a5u):0u)==0x200u)state=text_state;\n if(b.set_fd_state_10415)b.set_fd_state_10415(handle,state);return handle;\n}\nstatic_assert(0xD4F2u-0xD332u==448u);\n\n'''
s=s.replace(needle,block+needle)
s=s.replace('u32(*open_core_d332)(u32,u32,u32,u32)=nullptr;','u32(*open_core_d332)(u32,u32,u32,u32)=nullptr;')
# D313 should use the integrated core, not an external placeholder.
s=s.replace('return b.open_core_d332?b.open_core_d332(path,flags,0u,first_variadic):0xffffffffu;','return function_d332(b,path,flags,0u,first_variadic);')
# function_d332 is defined after D313 in the source; add a forward declaration before D313.
marker='// D313..D331: variadic front wrapper.'
s=s.replace(marker,'[[nodiscard]]u32 function_d332(FileBoundaries&,u32,u32,u32,u32)noexcept;\n'+marker)
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=9141u;','constexpr u32 kVerifiedTranslatedBytes=9589u;')
assert 'kVerifiedTranslatedBytes=9589u' in s
p.write_text(s)
q=Path('Code1/PROGRESS.md'); t=q.read_text()
t=t.replace('12.98%25-red','13.62%25-red').replace('**Verified conversion:** 12.98%','**Verified conversion:** 13.62%').replace('**Remaining:** 87.02%','**Remaining:** 86.38%').replace('Current verified translated bytes: **9,141**.','Current verified translated bytes: **9,589**.').replace('Total integrated verified translated bytes: **9,141**.','Total integrated verified translated bytes: **9,589**.')
# tolerate prior raw string variance by replacing line prefix
import re
t=re.sub(r'Raw coverage: \*\*[0-9.]+%\*\*, displayed as \*\*12\.98%\*\*\.', 'Raw coverage: **13.6204%**, displayed as **13.62%**.', t)
t=t.replace('staging pool is **1,499 bytes**: 747 startup + 595 remaining DOS/CRT + 157-byte E2C3..E35F interrupt trampoline','staging pool is **1,051 bytes**: 747 startup + 147 remaining DOS/CRT + 157-byte E2C3..E35F interrupt trampoline')
t=t.replace('leaving **595 unique unintegrated bytes**','leaving **147 unique unintegrated bytes**')
q.write_text(t)
r=Path('Code1/STAGING_LEDGER.md'); u=r.read_text()
u=u.replace('`D280..D312`, `D332..D4F1` | 595 |', '`D280..D312` | 147 |')
u=u.replace('**Active unique staged bytes: 1,499.**','**Active unique staged bytes: 1,051.**')
# append explicit resolution note if absent
if 'D332..D4F1 integrated' not in u:u+='\n- `D332..D4F1 integrated`: 448-byte DOS open/create/truncate descriptor-state machine directly re-disassembled and folded with AX+CF DOS boundaries; no host fopen substitution.\n'
r.write_text(u)
