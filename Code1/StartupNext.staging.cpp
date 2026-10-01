/* CODE.1 startup/runtime continuation staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Candidate code: CD97..CEBF, CEC0..CEC7, CEC8..CEE8,
 * CEF0..CF40, CF44..CF97, CF98..D08B = 747 bytes. NOT COUNTED.
 * CEE9..CEEF = 7 zero bytes; CF41..CF43 = 3-byte alignment NOP.
 */
#include <cstdint>
namespace code1_startup_next {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
struct Boundary {
 u8* dgroup=nullptr; u16 ds=0;
 u8(*read8)(u16,u32)=nullptr; u16(*read16)(u16,u32)=nullptr; u32(*read32)(u16,u32)=nullptr;
 void(*write8)(u16,u32,u8)=nullptr; void(*write16)(u16,u32,u16)=nullptr; void(*write32)(u16,u32,u32)=nullptr;
 void(*runtime_call)(u32)=nullptr; void(*int33)(u32&,u32&,u32&,u32&)=nullptr; u32(*call3)(u32,u32,u32,u32)=nullptr;
};
static u16 g16(const u8*p)noexcept{return static_cast<u16>(p[0]|(static_cast<u16>(p[1])<<8));}
static u32 g32(const u8*p)noexcept{return static_cast<u32>(p[0]|(u32(p[1])<<8)|(u32(p[2])<<16)|(u32(p[3])<<24));}
static void p16(u8*p,u16 v)noexcept{p[0]=u8(v);p[1]=u8(v>>8);} static void p32(u8*p,u32 v)noexcept{p[0]=u8(v);p[1]=u8(v>>8);p[2]=u8(v>>16);p[3]=u8(v>>24);}
struct StartupRegs{u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0,esp=0;u16 ds=0,es=0;};

/* CD97..CE8A startup path. CDB2..CDE8 reproduces the PSP command-tail
 * transfer literally: REPZ SCASB consumes the first non-space byte too, so
 * EDI advances and ECX decrements on that comparison; LEA ESI,[EDI-1] then
 * points back at the consumed byte and INC ECX restores it for REP MOVSB. */
void function_cd97_start(Boundary&b,StartupRegs&r)noexcept{
 if(!b.dgroup)return;b.ds=r.ds;b.dgroup[0x531e]=u8(r.eax);r.es=u16(r.ebx);if(b.write16)b.write16(r.es,0xcc90,r.ds);
 p32(b.dgroup+0x5320,r.esi);p16(b.dgroup+0x5324,u16(r.ecx));const u32 saved_esi=r.esi;r.es=g16(b.dgroup+0x52fc);
 r.edx=0x8e8c+0x0f;r.edx=(r.edx&~0xffu)|((r.edx&0xffu)&0xf0u);r.ecx=b.read8?b.read8(r.es,r.edi-1):0;
 if(b.read8){while(r.ecx!=0){const u8 compared=b.read8(r.es,r.edi++);--r.ecx;if(compared!=0x20)break;}}
 r.esi=r.edi-1;r.edi=r.edx;const u16 psp_sel=r.es;r.es=r.ds;
 if(r.ecx!=0){++r.ecx;while(r.ecx--!=0){const u8 v=b.read8?b.read8(psp_sel,r.esi):0;b.dgroup[r.edi++]=v;++r.esi;}}
 b.dgroup[r.edi++]=0;b.dgroup[r.edi++]=0;r.esi=saved_esi;--r.edi;const u16 env=g16(b.dgroup+0x5324);r.ebp=0;
 for(;;){u32 first=b.read32?b.read32(env,r.esi):0;if((first|0x20202020u)==0x37386f6eu&&b.read8&&b.read8(env,r.esi+4)=='=')++r.ebp;
  while(b.read8&&b.read8(env,r.esi)!=0)++r.esi;++r.esi;if(!b.read8||b.read8(env,r.esi)==0)break;}
 ++r.esi;++r.esi;while(b.read8){const u8 v=b.read8(env,r.esi++);b.dgroup[r.edi++]=v;if(v==0)break;}
 p16(b.dgroup+0x531c,u16(r.ebp));p32(b.dgroup+0x5300,r.edi);p32(b.dgroup+0x52ec,r.esp);
 u32 n=0x8e8c-0x55f8;if(b.dgroup[0x531e]==1&&n>0x1000)n=0x1000;for(u32 i=0;i<n;++i)b.dgroup[0x55f8+i]=0;
 u32 aligned=0x8e8c+0x0f;aligned=(aligned&~0xffu)|((aligned&0xffu)&0xf0u);p32(b.dgroup+0x52f4,aligned);p32(b.dgroup+0x52f8,r.esi);
 if(b.runtime_call)b.runtime_call(0xfce8);r.ebp=0;if(b.runtime_call)b.runtime_call(0xfc98);
}

/* CE8B..CEBF common process-exit/fatal continuation. CE8E is the alternate
 * entry that opens CC92 with AX=3D01, writes the supplied NUL-terminated
 * message with AH=40, then rejoins CE8B/CEB3. DOS calls remain boundary-owned. */
void function_ce8b_exit(Boundary&b,u32 exit_value)noexcept{if(b.runtime_call)b.runtime_call(0xfd2b);(void)exit_value;}

/* CEC0..CEC7: MOV DS,CS:[CC90]; RET. */
[[nodiscard]]u16 function_cec0(u16 cs_saved_ds)noexcept{return cs_saved_ds;}

/* CEC8..CEE8 register adapter. Four dword arguments become ESI,EDI,EDX,EBX.
 * FEFC/FF33 consumes that register image and CEC8 then returns dword [EDX],
 * where EDX is the third caller argument. The dereference is in restored DS. */
struct AdapterArgs{u32 esi,edi,edx,ebx;};
[[nodiscard]]u32 function_cec8(Boundary&b,const AdapterArgs&a)noexcept{if(b.runtime_call)b.runtime_call(0xfefc);return b.read32?b.read32(b.ds,a.edx):0;}

/* CEF0..CF40. 84A0 clears the 12-byte far-cell local. The register image has
 * EAX=300h, EBX=33h, ECX=0 and EBP-image slot (offset 14h) = caller pointer.
 * The far cell receives current DS in its first word. CEC8 service 31h receives
 * EDI=EDX=register-image address and EBX=far-cell address. */
struct Cef0Locals{u32 regs[10]{};u8 far_cell[12]{};};
void function_cef0(Boundary&b,u32 caller_ptr,Cef0Locals&l,u32 regs_guest,u32 far_guest)noexcept{
 if(b.call3)(void)b.call3(0x84a0,far_guest,0,12);l.regs[0]=0x300;l.regs[1]=0x33;l.regs[2]=0;l.regs[5]=caller_ptr;p16(l.far_cell,b.ds);
 AdapterArgs a{0x31,regs_guest,regs_guest,far_guest};(void)function_cec8(b,a);
}

/* CF41..CF43 is LEA EAX,[EAX+0], alignment NOP, not counted. */

/* CF44..CF97: clear 8A28[0..31h], seed fields, invoke CEF0, return 8A44==1. */
[[nodiscard]]bool function_cf44(Boundary&b,Cef0Locals&l,u32 regs_guest,u32 far_guest)noexcept{
 if(!b.dgroup)return false;if(b.call3)(void)b.call3(0x84a0,0x8a28,0,0x32);p32(b.dgroup+0x8a44,0x53c1);p16(b.dgroup+0x8a4a,u16(g32(b.dgroup+0x30a8)>>16));p32(b.dgroup+0x8a3c,0);function_cef0(b,0x8a28,l,regs_guest,far_guest);return g32(b.dgroup+0x8a44)==1;
}

/* CF98..D08B mouse initialization. 6F10 receives addresses 30A8/30AC, not
 * their contents. The no-mouse branch deliberately does not guard [2AA0]
 * against zero: D04D dereferences EAX+8 exactly as encoded. Its terminal path
 * calls 8424, pushes 1 and tail-jumps to CBF0. */
struct MouseResult{u16 ax=0,bx=0,flag=0;u32 pointer=0;bool tail_to_cbf0=false;};
[[nodiscard]]MouseResult function_cf98_d08b(Boundary&b,Cef0Locals&l,u32 regs_guest,u32 far_guest)noexcept{
 MouseResult o{};if(!b.dgroup)return o;u32 eax=0,ebx=0,ecx=0,edx=0;if(b.int33)b.int33(eax,ebx,ecx,edx);o.ax=u16(eax);o.bx=u16(ebx);p16(b.dgroup+0x8a5a,o.ax);p16(b.dgroup+0x8a5c,o.bx);p16(b.dgroup+0x8a5e,0);
 if(o.ax!=0){if(b.call3)o.pointer=b.call3(0x6f10,0x40,0x30a8,0x30ac);p32(b.dgroup+0x30a4,o.pointer);if(o.pointer==0)goto done;if(function_cf44(b,l,regs_guest,far_guest)){o.flag=b.dgroup[o.pointer];p16(b.dgroup+0x8a5e,o.flag);}if(g16(b.dgroup+0x8a5e)==0&&b.call3)(void)b.call3(0x6f74,g16(b.dgroup+0x30ac),0,0);}
 else{if(b.call3)(void)b.call3(0x61d0,0x350,0,0);const u32 p=g32(b.dgroup+0x2aa0);const u32 pending=g32(b.dgroup+0x2b84)|g32(b.dgroup+0x2f48);if(g16(b.dgroup+p+8)!=0x71){if(g16(b.dgroup+p+0x14)!=0&&b.runtime_call)b.runtime_call(0xc4a0);if(b.runtime_call)b.runtime_call(0xb430);if(pending!=0&&b.call3)(void)b.call3(0xe6ea,0,0,0);}if(b.runtime_call)b.runtime_call(0x8424);o.tail_to_cbf0=true;return o;}
 done:o.flag=g16(b.dgroup+0x8a5e);return o;
}
static_assert(0xCEC0u-0xCD97u==297u);static_assert(0xCEC8u-0xCEC0u==8u);static_assert(0xCEE9u-0xCEC8u==33u);static_assert(0xCF41u-0xCEF0u==81u);static_assert(0xCF98u-0xCF44u==84u);static_assert(0xD08Cu-0xCF98u==244u);
constexpr u32 kInstructionBoundedStagedBytes=747u;
} // namespace code1_startup_next
