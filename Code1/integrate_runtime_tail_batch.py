from pathlib import Path
p=Path('Code1/Code1.cpp'); s=p.read_text()
needle='struct DirectoryBoundaries{'
assert needle in s and 'function_fe21(' not in s and 'function_fce9(' not in s and 'function_interrupt_stub_ffd4(' not in s
block=r'''// FCE9..FD2A: execute unprocessed six-byte runtime records in 55E0..55F1 by ascending priority. Equal priorities select the later record because the encoded JA only rejects strictly larger values.
struct RuntimeTailBoundaries{void(*call_guest_near)(u32)=nullptr;};
void function_fce9(Machine&m,RuntimeTailBoundaries&b)noexcept{
 for(;;){u32 best=0x55f2u;u8 priority=0xffu;for(u32 rec=0x55e0u;rec<0x55f2u;rec+=6u){const u8 state=*dgroup_ptr(m,dgroup(rec));const u8 pr=*dgroup_ptr(m,dgroup(rec+1u));if(state!=2u&&pr<=priority){best=rec;priority=pr;}}if(best==0x55f2u)return;const u32 fn=load_le32(dgroup_ptr(m,dgroup(best+2u)));if(fn&&b.call_guest_near)b.call_guest_near(fn);*dgroup_ptr(m,dgroup(best))=2u;}
}
static_assert(0xFD2Bu-0xFCE9u==66u);
// FD2B..FD6D: same six-byte record dispatcher for 55F2..55F7, selecting the greatest priority (again preferring the later record on equality).
void function_fd2b(Machine&m,RuntimeTailBoundaries&b)noexcept{
 for(;;){u32 best=0x55f8u;u8 priority=0u;for(u32 rec=0x55f2u;rec<0x55f8u;rec+=6u){const u8 state=*dgroup_ptr(m,dgroup(rec));const u8 pr=*dgroup_ptr(m,dgroup(rec+1u));if(state!=2u&&pr>=priority){best=rec;priority=pr;}}if(best==0x55f8u)return;const u32 fn=load_le32(dgroup_ptr(m,dgroup(best+2u)));if(fn&&b.call_guest_near)b.call_guest_near(fn);*dgroup_ptr(m,dgroup(best))=2u;}
}
static_assert(0xFD6Eu-0xFD2Bu==67u);

// FE21..FEFB: command-line token scanner. Arg1 is the mutable source cursor, arg2 is an optional argv array, arg3 receives the stopping cursor. Quoted tokens omit quotes; backslash only escapes quote/backslash in quoted mode and quote in unquoted mode.
[[nodiscard]]u32 function_fe21(u8*text,u8**argv,u8**end)noexcept{
 u32 argc=0;u8*p=text;
 for(;;){while(*p==0x20u||*p==0x09u)++p;if(*p==0){if(end)*end=p;return argc;}u8*start=nullptr;u8*out=nullptr;u8*after=nullptr;
  if(*p==0x22u){++p;start=p;out=p;for(;;){if(*p==0x22u||*p==0)break;if(*p==0x5cu&&(p[1]==0x22u||p[1]==0x5cu))++p;u8*next=p+1u;if(argv)*out++=*p;p=next;}}
  else{start=p;out=p;for(;;){if(*p==0||*p==0x20u||*p==0x09u)break;if(*p==0x5cu&&p[1]==0x22u)++p;u8*next=p+1u;if(argv)*out++=*p;p=next;}}
  after=p+1u;++argc;if(argv){argv[argc-1u]=start;if(*p==0){*out=0;if(end)*end=p;return argc;}after=p+1u;*out=0;p=after;}else{if(*p==0){if(end)*end=p;return argc;}p=after;}
 }
}
static_assert(0xFEFCu-0xFE21u==219u);

// FFD4..102D3: 256 three-byte interrupt veneers. Entry N is at FFD4+3*N and is exactly `INT N; RET`; the generic reconstruction preserves that table formula and dispatches the encoded vector through the machine interrupt boundary.
void function_interrupt_stub_ffd4(Machine&m,u8 vector)noexcept{interrupt(m,vector);}
[[nodiscard]]constexpr u32 interrupt_stub_address_ffd4(u8 vector)noexcept{return 0xffd4u+3u*static_cast<u32>(vector);}
static_assert(0x102D4u-0xFFD4u==256u*3u);

'''
s=s.replace(needle,block+needle)
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=10026u;','constexpr u32 kVerifiedTranslatedBytes=11146u;')
assert 'kVerifiedTranslatedBytes=11146u' in s
p.write_text(s)
q=Path('Code1/PROGRESS.md'); t=q.read_text()
t=t.replace('14.24%25-red','15.83%25-red').replace('**Verified conversion:** 14.24%','**Verified conversion:** 15.83%').replace('**Remaining:** 85.76%','**Remaining:** 84.17%').replace('Current verified translated bytes: **10,026**.','Current verified translated bytes: **11,146**.').replace('Total integrated verified translated bytes: **10,026**.','Total integrated verified translated bytes: **11,146**.')
import re
t=re.sub(r'Raw coverage: \*\*[0-9.]+%\*\*, displayed as \*\*[0-9.]+%\*\*\.', 'Raw coverage: **15.8320%**, displayed as **15.83%**.', t)
inv='- Resource image-size parser `F7B8..F8BC`: **261 bytes**.'
assert inv in t
t=t.replace(inv,inv+'\n- Runtime record dispatchers `FCE9..FD6D`: **133 bytes**.\n- Command-line token scanner `FE21..FEFB`: **219 bytes**.\n- Complete interrupt veneer table `FFD4..102D3`: **768 bytes**.')
q.write_text(t)
r=Path('Code1/STAGING_LEDGER.md'); u=r.read_text()
u+='\n- `FCE9..FD6D integrated`: 133 bytes of directly disassembled six-byte runtime-record dispatchers; lower range selects minimum priority, upper range maximum priority, both prefer later equal-priority records and mark each executed record state 2.\n- `FE21..FEFB integrated`: 219-byte directly disassembled command-line scanner, preserving whitespace, quote, backslash, in-place compaction, optional argv-output, and stopping-cursor behavior.\n- `FFD4..102D3 integrated`: complete 768-byte interrupt veneer table; all 256 entries are mechanically `INT imm8; RET` at `FFD4+3*vector`.\n'
r.write_text(u)
