from pathlib import Path
import re

cpp=Path('Code1/Code1.cpp')
progress=Path('Code1/PROGRESS.md')
ledger=Path('Code1/STAGING_LEDGER.md')
continuity=Path('Code1/DECOMPILATION_CONTINUITY.md')
s=cpp.read_text()
anchor='constexpr u32 kVerifiedTranslatedBytes=16261u;'
assert anchor in s
assert 'function_8e3c(' not in s

block=r'''

// Batch 12: deferred asset dispatcher, startup/runtime machine adapters, and exact VGA wrappers.
struct AssetDispatchBoundary12{
 u8(*read8)(u32)=nullptr;u16(*read16)(u32)=nullptr;u32(*read32)(u32)=nullptr;
 void(*write8)(u32,u8)=nullptr;void(*write16)(u32,u16)=nullptr;void(*write32)(u32,u32)=nullptr;
 void(*range_error)(s32,s32)=nullptr;void(*pre_load)(s32,s32)=nullptr;
 void(*prepare_8d0c)(u32,u32,s32,u8*)=nullptr;
 u32(*load_767c)(u32,u32)=nullptr;u32(*load_79c8)(u32,u32)=nullptr;u32(*load_b51c)(u32,u32)=nullptr;
 u32(*load_c5b0)(u32,u32)=nullptr;u32(*load_e0d0)(u32,u32)=nullptr;u32(*load_e190)(u32,u32)=nullptr;u32(*load_e208)(u32,u32)=nullptr;
 void(*free_7240)(u32)=nullptr;u32(*ticks_7f50)()=nullptr;u32 group_table=0;
};
static u8 ad12_r8(AssetDispatchBoundary12&b,u32 a)noexcept{return b.read8?b.read8(a):0u;}
static u32 ad12_r32(AssetDispatchBoundary12&b,u32 a)noexcept{return b.read32?b.read32(a):0u;}
static void ad12_w8(AssetDispatchBoundary12&b,u32 a,u8 v)noexcept{if(b.write8)b.write8(a,v);}
static void ad12_w16(AssetDispatchBoundary12&b,u32 a,u16 v)noexcept{if(b.write16)b.write16(a,v);}
static void ad12_w32(AssetDispatchBoundary12&b,u32 a,u32 v)noexcept{if(b.write32)b.write32(a,v);}
// 8E3C..91BB: seven-way 9-byte asset-record loader. 8E20..8E3B and 91BC..91D7 are address tables, not code.
void function_8e3c(AssetDispatchBoundary12&b,s32 item,s32 group)noexcept{
 const u32 gr=b.group_table+static_cast<u32>(group)*16u;
 if(item<0||item>=static_cast<s32>(ad12_r32(b,gr+8u))){if(b.range_error)b.range_error(item,group);}
 const u32 entries=ad12_r32(b,gr+12u),entry=entries+static_cast<u32>(item)*9u;
 if(ad12_r8(b,entry+2u)==0u){
  if(b.pre_load)b.pre_load(item,group);
  u8 local[0x2f]{};if(b.prepare_8d0c)b.prepare_8d0c(static_cast<u32>(item),ad12_r32(b,gr),group,local);
  const u8 type=static_cast<u8>(local[2]-1u);u32 result=0;
  switch(type){case 0:if(b.load_767c)result=b.load_767c(reinterpret_cast<u32>(local),ad12_r32(b,gr));break;case 1:if(b.load_79c8)result=b.load_79c8(reinterpret_cast<u32>(local),ad12_r32(b,gr));break;case 2:if(b.load_b51c)result=b.load_b51c(reinterpret_cast<u32>(local),ad12_r32(b,gr));break;case 3:if(b.load_c5b0)result=b.load_c5b0(reinterpret_cast<u32>(local),ad12_r32(b,gr));break;case 4:if(b.load_e0d0)result=b.load_e0d0(reinterpret_cast<u32>(local),ad12_r32(b,gr));break;case 5:if(b.load_e190)result=b.load_e190(reinterpret_cast<u32>(local),ad12_r32(b,gr));break;case 6:if(b.load_e208)result=b.load_e208(reinterpret_cast<u32>(local),ad12_r32(b,gr));break;default:if(b.range_error)b.range_error(type,group);break;}
  ad12_w32(b,entry+5u,result);ad12_w8(b,entry+2u,1u);const u16 old=b.read16?b.read16(entry):0u;ad12_w16(b,entry,static_cast<u16>((old&0xfff0u)|(local[2]&0x0fu)));ad12_w8(b,entry,static_cast<u8>(ad12_r8(b,entry)&0xefu));
  const u32 scratch=static_cast<u32>(local[5])|(static_cast<u32>(local[6])<<8)|(static_cast<u32>(local[7])<<16)|(static_cast<u32>(local[8])<<24);if(scratch&&b.free_7240)b.free_7240(scratch);ad12_w32(b,gr+4u,ad12_r32(b,gr+4u)+1u);
 }
 const u16 stamp=static_cast<u16>((b.ticks_7f50?b.ticks_7f50():0u)>>5);ad12_w16(b,entry+3u,stamp);
}
static_assert(0x91BCu-0x8E3Cu==896u);

struct StartupBoundary12{
 u8* dgroup=nullptr;u16 ds=0;
 u8(*read8)(u16,u32)=nullptr;u16(*read16)(u16,u32)=nullptr;u32(*read32)(u16,u32)=nullptr;
 void(*write16)(u16,u32,u16)=nullptr;void(*runtime_call)(u32)=nullptr;void(*int33)(u32&,u32&,u32&,u32&)=nullptr;u32(*call3)(u32,u32,u32,u32)=nullptr;
};
struct StartupRegs12{u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0,esp=0;u16 ds=0,es=0;};
static u16 st12_r16(const u8*p)noexcept{return load_le16(p);}static u32 st12_r32(const u8*p)noexcept{return load_le32(p);}static void st12_w16(u8*p,u16 v)noexcept{store_le16(p,v);}static void st12_w32(u8*p,u32 v)noexcept{store_le32(p,v);}
// CD97..CEBF: PSP command tail, environment copy/scan, runtime launch, and common exit continuation.
void function_cd97(StartupBoundary12&b,StartupRegs12&r)noexcept{if(!b.dgroup)return;b.ds=r.ds;b.dgroup[0x531e]=static_cast<u8>(r.eax);r.es=static_cast<u16>(r.ebx);if(b.write16)b.write16(r.es,0xcc90,r.ds);st12_w32(b.dgroup+0x5320,r.esi);st12_w16(b.dgroup+0x5324,static_cast<u16>(r.ecx));const u32 saved_esi=r.esi;r.es=st12_r16(b.dgroup+0x52fc);r.edx=0x8e9bu;r.edx=(r.edx&~0xffu)|((r.edx&0xffu)&0xf0u);r.ecx=b.read8?b.read8(r.es,r.edi-1u):0u;if(b.read8){while(r.ecx){const u8 c=b.read8(r.es,r.edi++);--r.ecx;if(c!=0x20u)break;}}r.esi=r.edi-1u;r.edi=r.edx;const u16 psp=r.es;r.es=r.ds;if(r.ecx){++r.ecx;while(r.ecx--){const u8 v=b.read8?b.read8(psp,r.esi):0u;b.dgroup[r.edi++]=v;++r.esi;}}b.dgroup[r.edi++]=0;b.dgroup[r.edi++]=0;r.esi=saved_esi;--r.edi;const u16 env=st12_r16(b.dgroup+0x5324);r.ebp=0;for(;;){const u32 v=b.read32?b.read32(env,r.esi):0u;if((v|0x20202020u)==0x37386f6eu&&b.read8&&b.read8(env,r.esi+4u)=='=')++r.ebp;while(b.read8&&b.read8(env,r.esi)!=0u)++r.esi;++r.esi;if(!b.read8||b.read8(env,r.esi)==0u)break;}r.esi+=2u;while(b.read8){const u8 v=b.read8(env,r.esi++);b.dgroup[r.edi++]=v;if(v==0u)break;}st12_w16(b.dgroup+0x531c,static_cast<u16>(r.ebp));st12_w32(b.dgroup+0x5300,r.edi);st12_w32(b.dgroup+0x52ec,r.esp);u32 n=0x8e8cu-0x55f8u;if(b.dgroup[0x531e]==1u&&n>0x1000u)n=0x1000u;for(u32 i=0;i<n;++i)b.dgroup[0x55f8u+i]=0;u32 aligned=0x8e9bu;aligned=(aligned&~0xffu)|((aligned&0xffu)&0xf0u);st12_w32(b.dgroup+0x52f4,aligned);st12_w32(b.dgroup+0x52f8,r.esi);if(b.runtime_call)b.runtime_call(0xfce8u);r.ebp=0;if(b.runtime_call)b.runtime_call(0xfc98u);}
void function_ce8b(StartupBoundary12&b,u32)noexcept{if(b.runtime_call)b.runtime_call(0xfd2bu);}
static_assert(0xCEC0u-0xCD97u==297u);
[[nodiscard]]u16 function_cec0(u16 saved_ds)noexcept{return saved_ds;}static_assert(0xCEC8u-0xCEC0u==8u);
struct AdapterArgs12{u32 esi,edi,edx,ebx;};
[[nodiscard]]u32 function_cec8(StartupBoundary12&b,const AdapterArgs12&a)noexcept{if(b.runtime_call)b.runtime_call(0xfefcu);return b.read32?b.read32(b.ds,a.edx):0u;}static_assert(0xCEE9u-0xCEC8u==33u);
struct Cef0Locals12{u32 regs[10]{};u8 far_cell[12]{};};
void function_cef0(StartupBoundary12&b,u32 caller,Cef0Locals12&l,u32 regs_guest,u32 far_guest)noexcept{if(b.call3)(void)b.call3(0x84a0u,far_guest,0u,12u);l.regs[0]=0x300u;l.regs[1]=0x33u;l.regs[2]=0u;l.regs[5]=caller;st12_w16(l.far_cell,b.ds);AdapterArgs12 a{0x31u,regs_guest,regs_guest,far_guest};(void)function_cec8(b,a);}static_assert(0xCF41u-0xCEF0u==81u);
[[nodiscard]]bool function_cf44(StartupBoundary12&b,Cef0Locals12&l,u32 rg,u32 fg)noexcept{if(!b.dgroup)return false;if(b.call3)(void)b.call3(0x84a0u,0x8a28u,0u,0x32u);st12_w32(b.dgroup+0x8a44,0x53c1u);st12_w16(b.dgroup+0x8a4a,static_cast<u16>(st12_r32(b.dgroup+0x30a8)>>16));st12_w32(b.dgroup+0x8a3c,0);function_cef0(b,0x8a28u,l,rg,fg);return st12_r32(b.dgroup+0x8a44)==1u;}static_assert(0xCF98u-0xCF44u==84u);
void function_cf98(StartupBoundary12&b,Cef0Locals12&l,u32 rg,u32 fg)noexcept{if(!b.dgroup)return;u32 eax=0,ebx=0,ecx=0,edx=0;if(b.int33)b.int33(eax,ebx,ecx,edx);st12_w16(b.dgroup+0x8a5a,static_cast<u16>(eax));st12_w16(b.dgroup+0x8a5c,static_cast<u16>(ebx));st12_w16(b.dgroup+0x8a5e,0);if(static_cast<u16>(eax)!=0u){const u32 p=b.call3?b.call3(0x6f10u,0x40u,0x30a8u,0x30acu):0u;st12_w32(b.dgroup+0x30a4,p);if(p&&function_cf44(b,l,rg,fg))st12_w16(b.dgroup+0x8a5e,b.dgroup[p]);if(st12_r16(b.dgroup+0x8a5e)==0u&&b.call3)(void)b.call3(0x6f74u,st12_r16(b.dgroup+0x30ac),0u,0u);}else{if(b.call3)(void)b.call3(0x61d0u,0x350u,0u,0u);if(b.runtime_call)b.runtime_call(0x8424u);}}
static_assert(0xD08Cu-0xCF98u==244u);

struct Int24Frame12{u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0,esp=0;u16 ds=0,es=0,ss=0;};
struct Int24Boundary12{u16(*restore_ds)()=nullptr;u8(*runtime_mode)()=nullptr;u32(*runtime_stack_top)()=nullptr;void(*set_runtime_stack_top)(u32)=nullptr;u32(*far_callback)(u32,u32,u32,u32,u16)=nullptr;};
// E2C3..E35F: machine-level INT 24h trampoline; saved register and stack images remain explicit.
void function_e2c3(Int24Frame12&f,Int24Boundary12&b)noexcept{const Int24Frame12 saved=f;const u32 old_top=b.runtime_stack_top?b.runtime_stack_top():0u;f.ds=b.restore_ds?b.restore_ds():f.ds;f.es=f.ds;if(b.set_runtime_stack_top)b.set_runtime_stack_top(0x8a68u);const u32 linear=((saved.ebp&0xffffu)<<4)+((saved.esi)&0xffffu);u16 sel=0x34u;const u8 mode=b.runtime_mode?b.runtime_mode():0u;if(mode==1u)sel=f.ds;const u32 r=b.far_callback?b.far_callback(saved.eax&0xffffu,saved.edi&0xffffu,linear,sel,sel):0u;f=saved;f.eax=r;if(b.set_runtime_stack_top)b.set_runtime_stack_top(old_top);}static_assert(0xE360u-0xE2C3u==157u);

struct RegImage12{u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0;u32 carry_mask=0;};
struct FullRegImage12{u32 eax=0,ebx=0,ecx=0,edx=0,ebp=0,esi=0,edi=0;u16 ds=0,es=0,fs=0,gs=0,flags=0;};
struct InterruptAdapterBoundary12{void(*invoke)(u8,RegImage12&,u16&,u16&,bool&)=nullptr;void(*invoke_full)(u8,FullRegImage12&)=nullptr;};
// FEFC..FF56: compact register-image INT veneer adapter; vector index selects FFD4+3*n.
void function_fefc(InterruptAdapterBoundary12&b,u8 vector,RegImage12&r,u16&es,u16&ds)noexcept{bool carry=false;if(b.invoke)b.invoke(vector,r,es,ds,carry);r.carry_mask=carry?0xffffffffu:0u;}
// FF57..FFD3: full register/segment/flags image adapter used by the runtime interrupt bridge.
void function_ff57(InterruptAdapterBoundary12&b,u8 vector,FullRegImage12&r)noexcept{if(b.invoke_full)b.invoke_full(vector,r);}
static_assert(0xFFD4u-0xFEFCu==216u);

struct VgaBoundary12{void(*call_86a4)(u32,u32,u32)=nullptr;void(*call_c7a0)(u32,u32,u32,u32,u32)=nullptr;void(*call_c8c4)(u32,u32,u32,u32,u32)=nullptr;void(*call_cacc)(u32,u32,u32,u32,u32,u32,u32)=nullptr;void(*call_c9d4)(u32,u32,u32,u32,u32,u32,u32)=nullptr;void(*call_af08)()=nullptr;void(*call_af64)(u32)=nullptr;void(*call_af80)(u32)=nullptr;void(*int10)(u16&,u16&,u16&,u16&)=nullptr;u8(*in8)(u16)=nullptr;void(*out8)(u16,u8)=nullptr;void(*out16)(u16,u16)=nullptr;u32(*map_aea8)(u32,u32,u8&)=nullptr;u8(*read8)(u32)=nullptr;void(*write8)(u32,u8)=nullptr;};
// 9BE0..9C43: choose equal-axis helper or one of two five-argument span helpers.
void function_9be0(VgaBoundary12&b,u32 mode,u32 a,u32 c,u32 d,u32 e,u32 f)noexcept{if(a==c){if(b.call_86a4)b.call_86a4(d,f,e);return;}if(mode){if(b.call_c7a0)b.call_c7a0(a,c,d,e,f);}else if(b.call_c8c4)b.call_c8c4(a,c,d,e,f);}static_assert(0x9C44u-0x9BE0u==100u);
void function_9c44(VgaBoundary12&b,u32 mode,u32 a,u32 c,u32 d,u32 e,u32 f,u32 g,u32 h)noexcept{if(mode){if(b.call_cacc)b.call_cacc(a,c,d,e,f,g,h);}else if(b.call_c9d4)b.call_c9d4(a,c,d,e,f,g,h);}static_assert(0x9C98u-0x9C44u==84u);
// 9CA0..9DF3: initialize planar VGA state and report mode geometry through the caller record.
[[nodiscard]]u32 function_9ca0(Machine&m,VgaBoundary12&b,u32 mode,u32 out)noexcept{store_le32(dgroup_ptr(m,dgroup(0x2b29u)),mode&0x80u);mode&=0x7fu;store_le32(dgroup_ptr(m,dgroup(0x2b2du)),mode);store_le32(dgroup_ptr(m,dgroup(0x2b3du)),0xc8u);store_le32(dgroup_ptr(m,dgroup(0x2b41u)),0x50u);store_le32(dgroup_ptr(m,dgroup(0x2b49u)),0xa0000u);store_le32(dgroup_ptr(m,dgroup(0x2b31u)),0u);*dgroup_ptr(m,dgroup(0x2b28u))=0;store_le32(dgroup_ptr(m,dgroup(0x2b4du)),0x400u);store_le32(dgroup_ptr(m,dgroup(0x2b35u)),4u);store_le32(dgroup_ptr(m,dgroup(0x2b39u)),2u);if(b.call_af08)b.call_af08();if(mode==3u)store_le32(dgroup_ptr(m,dgroup(0x2b4du)),0x800u);u16 ax=0x0f00u,bx=0,cx=0,dx=0;if(b.int10)b.int10(ax,bx,cx,dx);if(static_cast<u8>(ax)!=0x13u){ax=0x13u;if(b.int10)b.int10(ax,bx,cx,dx);}if(b.out16)b.out16(0x3ceu,static_cast<u16>((static_cast<u16>(*dgroup_ptr(m,dgroup(0x2b28u)))<<8)|3u));if(b.out8&&b.in8){b.out8(0x3c4u,4u);u8 v=b.in8(0x3c5u);b.out8(0x3c5u,static_cast<u8>((v&0xf7u)|4u));b.out8(0x3ceu,5u);v=b.in8(0x3cfu);b.out8(0x3cfu,static_cast<u8>(v&0xefu));b.out8(0x3ceu,6u);v=b.in8(0x3cfu);b.out8(0x3cfu,static_cast<u8>(v&0xfdu));}if(b.call_af08)b.call_af08();if(b.out8&&b.in8){b.out8(0x3d4u,9u);u8 v=b.in8(0x3d5u);b.out8(0x3d5u,mode==3u?static_cast<u8>(v&0xe0u):0x41u);b.out8(0x3d4u,0x14u);v=b.in8(0x3d5u);b.out8(0x3d5u,static_cast<u8>(v&0xbfu));b.out8(0x3d4u,0x17u);v=b.in8(0x3d5u);b.out8(0x3d5u,static_cast<u8>(v|0x40u));}if(b.call_af64)b.call_af64(0u);if(b.call_af80)b.call_af80(0xffu);u16 width=0xc8u,height=0x140u;if(mode==3u)width=0x190u;store_le32(dgroup_ptr(m,dgroup(0x2b45u)),width);store_le32(dgroup_ptr(m,dgroup(0x2b29u)),0u);if(b.write8){b.write8(out+1u,2u);b.write8(out+3u,static_cast<u8>(width));b.write8(out+4u,static_cast<u8>(width>>8));b.write8(out+5u,static_cast<u8>(height));b.write8(out+6u,static_cast<u8>(height>>8));}return 0x0du;}static_assert(0x9DF4u-0x9CA0u==340u);
void function_9df4(VgaBoundary12&b)noexcept{if(b.call_af08)b.call_af08();u16 ax=3,bx=0,cx=0,dx=0;if(b.int10)b.int10(ax,bx,cx,dx);}static_assert(0x9E0Fu-0x9DF4u==27u);
void function_9e14(Machine&m,VgaBoundary12&b,u32 x,u32 y,u8 value)noexcept{u8 plane=0;const u32 p=b.map_aea8?b.map_aea8(x,y,plane):0u;if(b.out16)b.out16(0x3ceu,static_cast<u16>((static_cast<u16>(plane)<<8)|4u));if(b.out16)b.out16(0x3c4u,static_cast<u16>((1u<<plane)<<8|2u));if(b.out16)b.out16(0x3ceu,static_cast<u16>((static_cast<u16>(*dgroup_ptr(m,dgroup(0x2b28u)))<<8)|3u));if(b.read8)(void)b.read8(p);if(b.write8)b.write8(p,value);if(b.out16)b.out16(0x3ceu,3u);}static_assert(0x9E60u-0x9E14u==76u);
[[nodiscard]]u8 function_9e60(VgaBoundary12&b,u32 x,u32 y)noexcept{u8 plane=0;const u32 p=b.map_aea8?b.map_aea8(x,y,plane):0u;if(b.out16)b.out16(0x3ceu,static_cast<u16>((static_cast<u16>(plane)<<8)|4u));return b.read8?b.read8(p):0u;}static_assert(0x9E89u-0x9E60u==41u);
void function_9e8c(Machine&m,u32 index)noexcept{static constexpr u8 table[4]={0u,8u,16u,24u};*dgroup_ptr(m,dgroup(0x2b28u))=table[index&3u];}static_assert(0x9EA3u-0x9E8Cu==23u);
'''

s=s.replace(anchor,block+'\n'+anchor)
cpp.write_text(s)

p=progress.read_text()
p=p.replace('verified%20conversion-68.25%25-red','verified%20conversion-72.10%25-red')
p=p.replace('**Verified conversion:** 68.25%','**Verified conversion:** 72.10%')
p=p.replace('**Remaining:** 31.75%','**Remaining:** 27.90%')
p=p.replace('Current verified translated bytes: **48,052**.','Current verified translated bytes: **50,759**.')
p=p.replace('Raw coverage: **68.2537%**, displayed as **68.25%**.','Raw coverage: **72.0988%**, displayed as **72.10%**.')
needle='- Mid asset/input lifecycle routines selected from `87C4..8E1E`, `91D8..95A7`, and `95BC..9BD6` with address-table/alignment islands excluded: **4,116 bytes**.\n'
assert needle in p
p=p.replace(needle,needle+'- Recovery batch: deferred dispatcher `8E3C..91BB`, startup `CD97..D08B` excluding padding, INT24 trampoline `E2C3..E35F`, register-image adapters `FEFC..FFD3`, and exact wrappers `9BE0..9EA2` excluding data/alignment: **2,707 bytes**.\n')
progress.write_text(p)

l=ledger.read_text()
l=re.sub(r'\| `StartupNext\.staging\.cpp` \+ `StartupExit\.reconciled\.cpp` \| `CD97\.\.D08B` excluding proven padding/data \| 747 \|.*?\n','',l)
l=re.sub(r'\| `RuntimeGapE0D0\.staging\.cpp` \| `E2C3\.\.E35F` \| 157 \|.*?\n','',l)
l=l.replace('**Active unique staged bytes: 974.**','**Active unique staged bytes: 0.**')
l+='\n## Batch 12 recovery fold\n`CD97..D08B` startup staging (747 bytes), `E2C3..E35F` INT24 trampoline (157), `FEFC..FFD3` register-image adapters (216), deferred `8E3C..91BB` dispatch (896), and selected `9BE0..9EA2` wrappers (691) are now integrated/countable. Address tables, embedded lookup bytes, NOP/alignment, and the still-untranslated VGA inner loops beginning at `9EB0` remain excluded. Active staging is zero.\n'
ledger.write_text(l)

c=continuity.read_text()
c+='\n\n## Batch 12 recovery checkpoint\nVerified total is now 50,759 / 70,402 = 72.10%. The deferred 8E3C seven-way dispatch is integrated; its table targets are 8F44,8F87,8FC3,8FFF,903B,9077,90B1. Startup CD97..D08B, E2C3..E35F, and FEFC..FFD3 are folded; active staging is now zero. Selected exact wrappers 9BE0..9EA2 are also folded. Do not count 8E20..8E3B or 91BC..91D7 address tables, 9EA3..9EA7 lookup data, or VGA inner loops at 9EB0+ until explicitly translated.\n'
continuity.write_text(c)
print('integrated batch12')
