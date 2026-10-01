from pathlib import Path
p=Path('Code1/Code1.cpp');s=p.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes=12125u;' in s
assert 'function_102d4' not in s
s=s.replace('u16 ds=0,es=0,gs=0;bool interrupt_enable=false;','u16 cs=0,ds=0,es=0,ss=0,fs=0,gs=0;bool interrupt_enable=false;')
marker='struct FileBoundaries{DosAttrResult(*dos_get_attr)'
pos=s.index(marker)
end=s.index(';\n// D23A..D27C',pos)+2
block=r'''

// 102D4..104DD: directly disassembled CRT error/descriptor/stack/selector helpers.
// 102D4 and 102EC conditionally forward the encoded error word; the second argument is only a gate.
[[nodiscard]]s32 function_102ec(FileBoundaries&b,u32 error,u32 gate)noexcept{return gate?(b.call_10314?b.call_10314(static_cast<u16>(error)):-1):static_cast<s32>(error);}
[[nodiscard]]s32 function_102d4(FileBoundaries&b,u32 error,u32 gate)noexcept{return gate?function_102ec(b,error,gate):static_cast<s32>(error);}
static_assert(0x10303u-0x102D4u==47u);
// 10303..10313: set runtime errno 9 and return -1.
[[nodiscard]]s32 function_10303(FileBoundaries&b)noexcept{if(b.call_103b7)if(u32*p=b.call_103b7())*p=9u;return -1;}
static_assert(0x10314u-0x10303u==17u);
// 10314..10383: DOS/runtime error mapper. 8-bit errors use the encoded 20-entry signed table at 532C after DOS-version remapping; >=100h errors use bits 8..15 directly. The original error byte is also stored at 8E6C.
[[nodiscard]]s32 function_10314(Machine&m,u32 error)noexcept{
 const u8 original=static_cast<u8>(error);store_le32(dgroup_ptr(m,dgroup(0x8e6cu)),static_cast<u32>(original));u32 mapped=0;
 if(error<0x100u){u8 e=original;if(*dgroup_ptr(m,dgroup(0x5326u))>=3u){if(e==0x50u)e=0x0eu;else if(e>=0x22u)e=0x13u;else if(e>=0x20u)e=5u;}if(e>0x13u)e=0x13u;mapped=static_cast<u32>(static_cast<s32>(static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(0x532cu+e)))));}
 else mapped=(error>>8)&0xffu;store_le32(dgroup_ptr(m,dgroup(0x8e68u)),mapped);return -1;
}
static_assert(0x10384u-0x10314u==112u);
// 10384..10396: carry-sensitive DOS result reducer: success returns zero, failure maps low AX.
[[nodiscard]]s32 function_10384(Machine&m,bool carry,u32 eax)noexcept{return carry?function_10314(m,eax&0xffffu):0;}
static_assert(0x10397u-0x10384u==19u);
struct RuntimeIoctlBoundary{struct Result{u32 edx=0;bool carry=false;};Result(*ioctl_get)(u16)=nullptr;};
// 10397..103B6: INT 21h AX=4400h device query; the RCL/ROR pair materializes CF into EDX bit 31, and SETNE tests DL bit 7 exactly as encoded.
[[nodiscard]]u32 function_10397(RuntimeIoctlBoundary&b,u16 handle)noexcept{const auto r=b.ioctl_get?b.ioctl_get(handle):RuntimeIoctlBoundary::Result{};u32 edx=r.edx;edx=(edx<<1)|(r.carry?1u:0u);edx=(edx>>1)|(edx<<31);return (static_cast<u8>(edx)&0x80u)?1u:0u;}
static_assert(0x103B7u-0x10397u==32u);
[[nodiscard]]constexpr u32 function_103b7()noexcept{return 0x8e68u;}[[nodiscard]]constexpr u32 function_103bd()noexcept{return 0x8e6cu;}
static_assert(0x103C3u-0x103B7u==12u);
// 103C3..10414: fetch descriptor state from table 5398, lazily tagging handles 0..5 with 40h and 20h when the encoded device query succeeds.
[[nodiscard]]u32 function_103c3(Machine&m,RuntimeIoctlBoundary&b,u32 h)noexcept{const u32 n=load_le32(dgroup_ptr(m,dgroup(0x5344u)));if(h>=n)return 0u;const u32 table=load_le32(dgroup_ptr(m,dgroup(0x5398u)));if(h<=5u){u8*entry=dgroup_ptr(m,dgroup(table+h*4u));if((entry[1]&0x40u)==0){entry[1]|=0x40u;if(function_10397(b,static_cast<u16>(h)))entry[1]|=0x20u;}}return load_le32(dgroup_ptr(m,dgroup(table+h*4u)));}
static_assert(0x10415u-0x103C3u==82u);
// 10415..1042C: store descriptor state after forcing DH bit 6.
void function_10415(Machine&m,u32 h,u32 state)noexcept{const u32 table=load_le32(dgroup_ptr(m,dgroup(0x5398u)));state|=0x00400000u;store_le32(dgroup_ptr(m,dgroup(table+h*4u)),state);}
static_assert(0x1042Du-0x10415u==24u);
// 1042D..10435: return ESP minus the runtime stack-base global at 5300.
[[nodiscard]]u32 function_1042d(Machine&m)noexcept{return m.cpu.esp-load_le32(dgroup_ptr(m,dgroup(0x5300u)));}
static_assert(0x10436u-0x1042Du==9u);
// 10436..10437 are two zero bytes and are data/alignment, not translated code.
// 10438..1043E: capture SS at 55B0.
void function_10438(Machine&m)noexcept{store_le16(dgroup_ptr(m,dgroup(0x55b0u)),m.cpu.ss);}
static_assert(0x1043Fu-0x10438u==7u);
struct StackGuardBoundary{void(*fatal_ce8e)(u32,u32)=nullptr;};
// 1043F..1044E exchanges EAX with the caller stack argument, calls 10452, restores EAX from that slot, then RET 4. 1044F is the encoded RET 4 alternate entry.
void function_10452(Machine&m,StackGuardBoundary&b,u32 probe)noexcept{bool ok=false;if(probe<m.cpu.esp){const u32 distance=m.cpu.esp-probe;if(distance<=load_le32(dgroup_ptr(m,dgroup(0x5300u))))ok=true;}if(!ok&&m.cpu.ss==load_le16(dgroup_ptr(m,dgroup(0x55b0u)))){if(b.fatal_ce8e)b.fatal_ce8e(0x55b2u,1u);}}
void function_1043f(Machine&m,StackGuardBoundary&b,u32 argument)noexcept{const u32 incoming_eax=m.cpu.eax;m.cpu.eax=argument;function_10452(m,b,incoming_eax);m.cpu.eax=incoming_eax;}
void function_1044f()noexcept{}
static_assert(0x10452u-0x1043Fu==19u);static_assert(0x1047Eu-0x10452u==44u);
// 1047E..104B0: bounded copy followed by zero padding; returns the original destination.
[[nodiscard]]u8*function_1047e(u8*dst,const u8*src,u32 count)noexcept{u8*out=dst;while(count&&*src){--count;*dst++=*src++;}while(count){--count;*dst++=0;}return out;}
static_assert(0x104B1u-0x1047Eu==51u);
// 104B1..104DD: capture ES,CS,SS,DS,FS,GS in the exact encoded field order.
void function_104b1(Machine&m,u8*out)noexcept{store_le16(out+0,m.cpu.es);store_le16(out+2,m.cpu.cs);store_le16(out+4,m.cpu.ss);store_le16(out+6,m.cpu.ds);store_le16(out+8,m.cpu.fs);store_le16(out+10,m.cpu.gs);}
static_assert(0x104DEu-0x104B1u==45u);
// 104DE..104DF are two zero bytes and are data/alignment, not translated code.
'''
s=s[:end]+block+s[end:]
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=12125u;','constexpr u32 kVerifiedTranslatedBytes=12645u;')
p.write_text(s)
q=Path('Code1/PROGRESS.md');t=q.read_text();t=t.replace('17.22%25-red','17.96%25-red').replace('**Verified conversion:** 17.22%','**Verified conversion:** 17.96%').replace('**Remaining:** 82.78%','**Remaining:** 82.04%').replace('**12,125**','**12,645**').replace('**17.2225%**, displayed as **17.22%**','**17.9611%**, displayed as **17.96%**').replace('- Complete interrupt veneer table `FFD4..102D3`: **768 bytes**.','- Complete interrupt veneer table `FFD4..102D3`: **768 bytes**.\n- CRT error/descriptor/stack/selector helpers `102D4..104DD` excluding four zero alignment bytes: **520 bytes**.').replace('Total integrated verified translated bytes: **12,125**.','Total integrated verified translated bytes: **12,645**.')
q.write_text(t)
r=Path('Code1/STAGING_LEDGER.md');u=r.read_text();u+='\n- `102D4..104DD integrated`: 520 functional bytes directly disassembled; four zero bytes at 10436..10437 and 104DE..104DF excluded. Error mapping, descriptor-table state, stack guard, bounded copy/pad, and selector capture are explicit.\n';r.write_text(u)
