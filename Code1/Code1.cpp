/* Ravenloft: Strahd's Possession - CODE.1 faithful reconstruction
 * Immutable source: CODE.1, 277787 bytes
 * SHA-256 fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Instruction-accountable reconstruction; unknown semantics are not guessed. */
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>
namespace code1 {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t; using s16=std::int16_t; using s32=std::int32_t;
static_assert(sizeof(u8)==1&&sizeof(u16)==2&&sizeof(u32)==4);
constexpr std::size_t kSourceFileSize=277787u;
constexpr char kSourceSha256[]="fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed";
constexpr u32 kOuterMzFileOffset=0,kBoundBwFileOffset=0xF444u,kApplicationMzFileOffset=0x25D24u,kApplicationLeFileOffset=0x27BDCu,kApplicationDataFileOffset=0x2C724u;
constexpr u32 kEntryOffset=0xCC1Cu,kCodeObjectVirtualSize=0x11302u,kCodeObjectBase=0x10000u,kDataObjectVirtualSize=0x9E90u,kDataObjectBase=0x30000u;
constexpr u32 kEntryFileOffset=0x39340u,kWatcomRuntimeTextFileOffset=0x39342u,kRavenloftTextFileOffset=0x3E7ACu;
[[nodiscard]] constexpr u16 load_le16(const u8*p)noexcept{return static_cast<u16>(p[0]|(static_cast<u16>(p[1])<<8));}
[[nodiscard]] constexpr u32 load_le32(const u8*p)noexcept{return static_cast<u32>(p[0]|(static_cast<u32>(p[1])<<8)|(static_cast<u32>(p[2])<<16)|(static_cast<u32>(p[3])<<24));}
constexpr void store_le16(u8*p,u16 v)noexcept{p[0]=static_cast<u8>(v);p[1]=static_cast<u8>(v>>8);} constexpr void store_le32(u8*p,u32 v)noexcept{p[0]=static_cast<u8>(v);p[1]=static_cast<u8>(v>>8);p[2]=static_cast<u8>(v>>16);p[3]=static_cast<u8>(v>>24);}
struct DgroupOffset{u32 value;}; struct LinearAddress{u32 value;}; struct SelectorOffset{u16 selector;u32 offset;};
[[nodiscard]] constexpr DgroupOffset dgroup(u32 v)noexcept{return{v};} [[nodiscard]] constexpr LinearAddress linear(u32 v)noexcept{return{v};} [[nodiscard]] constexpr SelectorOffset selector_offset(u16 s,u32 o)noexcept{return{s,o};}
struct CpuState{u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0,esp=0;u16 cs=0,ds=0,es=0,ss=0,fs=0,gs=0;bool interrupt_enable=false;};
struct Machine; using ProtectedModeInterrupt=void(*)(Machine&,u8); using LinearRead16=u16(*)(Machine&,LinearAddress); using LinearRead32=u32(*)(Machine&,LinearAddress); using SelectorRead16=u16(*)(Machine&,SelectorOffset);
struct Machine{CpuState cpu{};u8*data=nullptr;ProtectedModeInterrupt protected_interrupt=nullptr;LinearRead16 read_linear16=nullptr;LinearRead32 read_linear32=nullptr;SelectorRead16 read_selector16=nullptr;};
[[nodiscard]] u8*dgroup_ptr(Machine&m,DgroupOffset o)noexcept{return m.data+o.value;} void interrupt(Machine&m,u8 v)noexcept{if(m.protected_interrupt)m.protected_interrupt(m,v);} [[nodiscard]]u16 read16(Machine&m,LinearAddress a)noexcept{return m.read_linear16?m.read_linear16(m,a):0;} [[nodiscard]]u32 read32(Machine&m,LinearAddress a)noexcept{return m.read_linear32?m.read_linear32(m,a):0;} [[nodiscard]]u16 read16(Machine&m,SelectorOffset a)noexcept{return m.read_selector16?m.read_selector16(m,a):0;}
[[nodiscard]]constexpr u8 low8(u32 v)noexcept{return static_cast<u8>(v);} [[nodiscard]]constexpr u8 high8(u32 v)noexcept{return static_cast<u8>(v>>8);} [[nodiscard]]constexpr u16 low16(u32 v)noexcept{return static_cast<u16>(v);} constexpr void set_low8(u32&v,u8 x)noexcept{v=(v&0xffffff00u)|x;} constexpr void set_low16(u32&v,u16 x)noexcept{v=(v&0xffff0000u)|x;} constexpr void set_high8(u32&v,u8 x)noexcept{v=(v&0xffff00ffu)|(static_cast<u32>(x)<<8);}


// 1464..15AD: configure the eleven eight-byte UI slots, materialize optional label 7140,
// run the 10A4 presentation pass, preload visible resources, then dispatch the mode-dependent 07E4 range.
struct MidUiBoundaries{
 void(*call_10a4)(u32)=nullptr; void(*call_7e4)(s32,s32)=nullptr; void(*call_4c)(u32,u32)=nullptr;
 void(*call_72a0)(u32,u32)=nullptr; u32(*call_954c)(u32,u32)=nullptr;
 u32(*call_7868)(u32,u32,u32,u32,u32)=nullptr; void(*call_7830)(u32)=nullptr;
 void(*call_6588)(u32)=nullptr; void(*call_6c60)(s32,s32,s32,s32,u32)=nullptr;
 void(*call_7d80)(s32,s32,u32,u32,u32)=nullptr; void(*call_67c8)(u32,s32,s32,u32)=nullptr;
 s32(*call_43c)(Machine&)=nullptr; void(*call_15b0)(s32,s32,s32,s32)=nullptr;
 void(*call_550c)(u32,s32,s32,s32,s32)=nullptr; void(*call_4b0)(u32)=nullptr;
 void(*call_ec)(s32,s32,s32,u32,u32,u32,u32)=nullptr;
};
void function_1464(Machine&m,MidUiBoundaries&b,u32 arg0,const u32 slots[11],s32 label)noexcept{
 for(u32 i=0;i<11u;++i)store_le32(dgroup_ptr(m,dgroup(0x08f4u+i*8u)),0x15u);
 for(u32 i=0;i<11u;++i)store_le32(dgroup_ptr(m,dgroup(0x08f8u+i*8u)),slots[i]);
 if(label==-1)*dgroup_ptr(m,dgroup(0x7140u))=0;
 else if(label==0x2b){if(b.call_72a0)b.call_72a0(0x7140u,load_le32(dgroup_ptr(m,dgroup(0x04b8u))));}
 else if(b.call_4c)b.call_4c(0x7140u,static_cast<u32>(label));
 if(b.call_10a4)b.call_10a4(arg0);
 for(u32 i=0;i<9u;++i){const s32 v=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08f8u+i*8u))));if(v==-1||v<=10)continue;const s32 base=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu))));if(b.call_954c)(void)b.call_954c(static_cast<u32>(v),(v>=base&&v<base+0x2e)?1u:7u);}
 const s32 mode=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x04d0u))));if(mode<2){if(b.call_7e4)b.call_7e4(2,4);}else if(mode==6){if(b.call_7e4)b.call_7e4(6,9);}else if(b.call_7e4)b.call_7e4(0,6);
}
static_assert(0x15AEu-0x1464u==330u);
// 15AE..15AF is LEA alignment padding.

// 1A04..1BD0: rebuild the two transient overlay resources. Existing handles 095C/0960 are freed first;
// the two flag bytes choose between record-backed 004C/00EC materialization and direct 6C60/7D80 rendering.
void function_1a04(Machine&m,MidUiBoundaries&b)noexcept{
 for(u32 cell:{0x095cu,0x0960u}){const u32 h=load_le32(dgroup_ptr(m,dgroup(cell)));if(h&&b.call_7830)b.call_7830(h);store_le32(dgroup_ptr(m,dgroup(cell)),0u);}
 if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));
 const u32 first=b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u;
 if(*dgroup_ptr(m,dgroup(0x04c4u))==0){if(b.call_4c)b.call_4c(0x55f8u,0u);if(b.call_ec)b.call_ec(0,0,0x7a,0x30,0x55f8u,0,0x5ff8u);}
 else{if(b.call_6c60){b.call_6c60(0,0,0,0x14,7);const s32 id=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(0x680au)))+static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu))))+static_cast<s32>(*dgroup_ptr(m,dgroup(0x680bu)))*0x17;b.call_6c60(0,0x15,0,id,1);}if(b.call_7d80)b.call_7d80(2,0,0x30,0x67f8u,0);}
 store_le32(dgroup_ptr(m,dgroup(0x095cu)),b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u);
 if(*dgroup_ptr(m,dgroup(0x04c5u))==0){if(b.call_4c)b.call_4c(0x55f8u,0u);if(b.call_ec)b.call_ec(0,0,0x7a,0x30,0x55f8u,0,0x5ff8u);}
 else{if(b.call_6c60){b.call_6c60(0,0,0,0x14,7);const s32 id=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(0x6a52u)))+static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu))))+static_cast<s32>(*dgroup_ptr(m,dgroup(0x6a53u)))*0x17;b.call_6c60(0,0x15,0,id,1);}if(b.call_7d80)b.call_7d80(2,0,0x30,0x6a40u,0);}
 store_le32(dgroup_ptr(m,dgroup(0x0960u)),b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u);
 if(b.call_67c8)b.call_67c8(0,0,0,first);if(b.call_7830)b.call_7830(first);
}
static_assert(0x1BD1u-0x1A04u==461u);
// 1BD1..1BD3 is LEA alignment padding.

// 1BD4..1CCF: temporary slot remap used by the selection screen. The loop accepts event 3 immediately;
// event 4 additionally clears the selected actor flag and writes 0x8000 at actor+6808.
void function_1bd4(Machine&m,MidUiBoundaries&b)noexcept{
 store_le32(dgroup_ptr(m,dgroup(0x0904u)),load_le32(dgroup_ptr(m,dgroup(0x0908u))));store_le32(dgroup_ptr(m,dgroup(0x090cu)),load_le32(dgroup_ptr(m,dgroup(0x0910u))));store_le32(dgroup_ptr(m,dgroup(0x0908u)),0x2fu);store_le32(dgroup_ptr(m,dgroup(0x0910u)),0x30u);
 if(b.call_7e4)b.call_7e4(0,6);if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_4b0)b.call_4b0(6u);if(b.call_550c)b.call_550c(load_le32(dgroup_ptr(m,dgroup(0x7138u))),0xc8,0,0xc8,0x140);*dgroup_ptr(m,dgroup(0x7138u))^=1u;
 for(;;){const s32 e=b.call_43c?b.call_43c(m):0;if(e==3)break;if(e==4){const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x04ccu)));const u32 base=actor*0x248u;store_le16(dgroup_ptr(m,dgroup(base+0x6808u)),0x8000u);*dgroup_ptr(m,dgroup(actor+0x04c4u))=0;store_le32(dgroup_ptr(m,dgroup(0x0940u)),2u);break;}}
 store_le32(dgroup_ptr(m,dgroup(0x0904u)),load_le32(dgroup_ptr(m,dgroup(0x0908u))));store_le32(dgroup_ptr(m,dgroup(0x090cu)),load_le32(dgroup_ptr(m,dgroup(0x0910u))));store_le32(dgroup_ptr(m,dgroup(0x0908u)),1u);store_le32(dgroup_ptr(m,dgroup(0x0910u)),1u);function_1a04(m,b);if(b.call_7e4)b.call_7e4(0,6);
}
static_assert(0x1CD0u-0x1BD4u==252u);

// 1CD0..1DD7: actor-choice loop. 0 exits with actor=-1; 3/4 select actor 0/1. An already-present actor enters 1BD4;
// otherwise 15B0 is called and the two per-actor dwords at 6903/6907 are cleared.
void function_1cd0(Machine&m,MidUiBoundaries&b)noexcept{
 function_1a04(m,b);const u32 setup[11]={0xffffffffu,0xffffffffu,1u,1u,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,(*dgroup_ptr(m,dgroup(0x04c4u))&&*dgroup_ptr(m,dgroup(0x04c5u)))?1u:2u,0xffffffffu};function_1464(m,b,0,setup,-1);
 for(;;){const s32 e=b.call_43c?b.call_43c(m):0;if(e==0){store_le32(dgroup_ptr(m,dgroup(0x04ccu)),0xffffffffu);break;}if(e==3||e==4){const u32 actor=static_cast<u32>(e-3);store_le32(dgroup_ptr(m,dgroup(0x04ccu)),actor);if(*dgroup_ptr(m,dgroup(0x04c4u+actor))!=0){function_1bd4(m,b);continue;}if(b.call_15b0)b.call_15b0(-1,-1,-1,-1);store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))+1u);const u32 base=actor*0x248u;store_le32(dgroup_ptr(m,dgroup(base+0x6903u)),0u);store_le32(dgroup_ptr(m,dgroup(base+0x6907u)),0u);break;}}
 for(u32 cell:{0x095cu,0x0960u}){const u32 h=load_le32(dgroup_ptr(m,dgroup(cell)));if(h&&b.call_7830)b.call_7830(h);store_le32(dgroup_ptr(m,dgroup(cell)),0u);}
}
static_assert(0x1DD8u-0x1CD0u==264u);

// 1DD8..1E61: choose actor submode (events 3/4) or cancel (event 0); selected byte 680B receives event-3.
void function_1dd8(Machine&m,MidUiBoundaries&b)noexcept{const u32 slots[11]={0xffffffffu,0xffffffffu,0x0cu,0x0du,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,8u,0xffffffffu};function_1464(m,b,0,slots,0x26);for(;;){const s32 e=b.call_43c?b.call_43c(m):0;if(e==0){store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))-1u);if(b.call_15b0)b.call_15b0(-1,-1,-1,-1);return;}if(e==3||e==4){const u32 base=load_le32(dgroup_ptr(m,dgroup(0x04ccu)))*0x248u;*dgroup_ptr(m,dgroup(base+0x680bu))=static_cast<u8>(e-3);store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))+1u);if(b.call_15b0)b.call_15b0(e-1,0x114,3,1);return;}}}
static_assert(0x1E62u-0x1DD8u==138u);
// 1E62..1E63 is LEA alignment padding.

// 1E64..1EE0: choose one of five byte values for actor+680C; event 0 cancels and decrements mode.
void function_1e64(Machine&m,MidUiBoundaries&b)noexcept{const u32 slots[11]={0u,1u,2u,3u,4u,5u,0xffffffffu,0xffffffffu,0xffffffffu,9u,0xffffffffu};function_1464(m,b,1,slots,0x27);const s32 e=b.call_43c?b.call_43c(m):0;if(e==0){store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))-1u);if(b.call_15b0)b.call_15b0(-1,-1,-1,-1);return;}const u32 base=load_le32(dgroup_ptr(m,dgroup(0x04ccu)))*0x248u;*dgroup_ptr(m,dgroup(base+0x680cu))=static_cast<u8>(e-1);store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))+1u);if(b.call_15b0)b.call_15b0(e-1,0x114,0x38,2);}
static_assert(0x1EE1u-0x1E64u==125u);
// 1EE1..1EE3 is LEA alignment padding.

// 1EE4..1EFE: exact one-byte swap helper.
void function_1ee4(u8*a,u8*b)noexcept{const u8 t=*a;*a=*b;*b=t;} static_assert(0x1EFFu-0x1EE4u==27u);
// 1EFF is NOP alignment; 1F00..1F23 is a nine-entry code-address jump table and is data, not translated code.


// 1F24..2196: normalize the selected actor's three class/order bytes.  The binary derives a
// dispatch key in DL and jumps through the nine-entry table at 1F00; DL is intentionally not
// initialized on entry, so the reconstruction preserves the incoming low EDX value until an
// encoded condition assigns it.
void function_1f24(Machine&m)noexcept{
 const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x04ccu)));u8*p=dgroup_ptr(m,dgroup(actor*0x248u+0x67f8u));
 u8 key=low8(m.cpu.edx);const s32 b1=static_cast<std::int8_t>(p[0x16]),b2=static_cast<std::int8_t>(p[0x17]);
 if(b1==-1)key=p[0x15];
 else if(b2==-1){const u8 a=p[0x15],b=p[0x16];if(a==0){if(b==5)key=6;if(b==3)key=7;if(b==4)key=8;}if(a==5){if(b==0)key=6;if(b==3)key=9;if(b==4)key=12;}if(a==2)key=10;if(a==3){if(b==0)key=7;if(b==5)key=9;if(b==4)key=11;if(b==2)key=10;}if(a==4){if(b==0)key=8;if(b==3)key=11;if(b==5)key=12;}}
 else key=(p[0x15]==5||p[0x16]==5||p[0x17]==5)?13u:14u;
 set_low8(m.cpu.edx,key);
 switch(static_cast<u8>(key-6u)){
  case 0:case 1:case 2:if(p[0x15]!=0)function_1ee4(p+0x15,p+0x16);break;
  case 3:case 4:case 5:if(p[0x15]!=3)function_1ee4(p+0x15,p+0x16);break;
  case 6:if(p[0x15]!=4)function_1ee4(p+0x15,p+0x16);break;
  case 7:p[0x15]=0;p[0x16]=4;p[0x17]=5;break;
  case 8:p[0x15]=0;p[0x16]=4;p[0x17]=3;break;
  default:break;
 }
}
static_assert(0x2197u-0x1F24u==627u);
// 2197 is NOP alignment.

struct ActorRenderBoundaries{
 void(*call_56dc)()=nullptr;u32(*call_71c8)(u32)=nullptr;void(*call_7240)(u32)=nullptr;
 void(*call_6588)(u32)=nullptr;void(*call_6c60)(s32,s32,s32,s32,u32)=nullptr;
 void(*call_66c0)(u32,s32,s32,u32,u32)=nullptr;void(*call_6744)(u32,s32,s32,u32,u32)=nullptr;
 u32(*call_954c)(u32,u32)=nullptr;void(*call_67c8)(u32,s32,s32,u32)=nullptr;void(*call_53e4)(u32)=nullptr;
};
// 24D4..266B: animate the selected slot toward (target_x,target_y) over nine frames. Two
// 0x1E80 scratch surfaces are alternated with 7138 exactly as in the original loop.
void function_24d4(Machine&m,ActorRenderBoundaries&b,u32 slot,s32 target_x,s32 target_y)noexcept{
 if(b.call_56dc)b.call_56dc();const u32 scratch[2]={b.call_71c8?b.call_71c8(0x1e80u):0u,b.call_71c8?b.call_71c8(0x1e80u):0u};
 if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));const s32 x0=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+slot*8u)))),y0=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+slot*8u))));
 if(b.call_6c60)b.call_6c60(0,x0,y0,static_cast<s32>(slot+0x1eu),7u);if(b.call_66c0){b.call_66c0(scratch[0],x0,y0,0x7au,0x30u);b.call_66c0(scratch[1],x0,y0,0x7au,0x30u);}
 s32 old_x[2]={x0,x0},old_y[2]={y0,y0};s32 x=x0,y=y0;const s32 dx=static_cast<s32>(static_cast<u32>(target_x)-static_cast<u32>(x0))/8,dy=static_cast<s32>(static_cast<u32>(target_y)-static_cast<u32>(y0))/8;
 for(u32 frame=0;frame<=8u;++frame){const u32 page=load_le32(dgroup_ptr(m,dgroup(0x7138u)));if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_6744)b.call_6744(scratch[page],old_x[page],old_y[page],0x7au,0x30u);old_x[page]=x;old_y[page]=y;if(b.call_66c0)b.call_66c0(scratch[page],x,y,0x7au,0x30u);const u32 image=b.call_954c?b.call_954c(load_le32(dgroup_ptr(m,dgroup(0x08f8u+slot*8u))),7u):0u;if(b.call_67c8)b.call_67c8(0u,x,y,image);x=static_cast<s32>(static_cast<u32>(x)+static_cast<u32>(dx));y=static_cast<s32>(static_cast<u32>(y)+static_cast<u32>(dy));if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;}
 if(b.call_7240){b.call_7240(scratch[0]);b.call_7240(scratch[1]);}
}
static_assert(0x266Cu-0x24D4u==408u);

// 28E4..2990: build six availability flags from actor bytes 680D..680F.
void function_28e4(Machine&m,u32 out[6])noexcept{for(u32 i=0;i<6u;++i)out[i]=1u;const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x04ccu)));const u8*p=dgroup_ptr(m,dgroup(actor*0x248u+0x680du));if(p[0]==1u){for(u32 i=0;i<6u;++i)out[i]=0u;out[0]=1u;return;}if(p[0]==5u||p[1]==5u||p[2]==5u)out[0]=0u;if(p[0]==2u||p[1]==2u)out[1]=out[3]=out[5]=0u;}
static_assert(0x2991u-0x28E4u==173u);
// 2991..2993 is LEA alignment.

// 2994..2A60: present the six availability choices, then write actor+6812 and advance/cancel
// the mode through the existing 15B0 renderer. Entries mapped to 0x15 are non-selectable.
void function_2994(Machine&m,MidUiBoundaries&b)noexcept{u32 v[6];function_28e4(m,v);for(u32 i=0;i<6u;++i)v[i]=v[i]?i+0x0eu:0x15u;const u32 slots[11]={v[0],v[1],v[2],v[3],v[4],v[5],0xffffffffu,0xffffffffu,0xffffffffu,0x0cu,0xffffffffu};function_1464(m,b,3u,slots,0x29);for(;;){const s32 e=b.call_43c?b.call_43c(m):0;if(e!=0&&v[static_cast<u32>(e-1)]==0x15u)continue;if(e==0){store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))-1u);if(b.call_15b0)b.call_15b0(-1,-1,-1,-1);}else{const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x04ccu)));*dgroup_ptr(m,dgroup(actor*0x248u+0x6812u))=static_cast<u8>(e-1);store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))+1u);if(b.call_15b0)b.call_15b0(e-1,0x114,0xa2,4);}return;}}
static_assert(0x2A61u-0x2994u==205u);
// 2A61..2A63 is LEA alignment.


struct EarlyGameBoundaries; [[nodiscard]]s32 function_0010(EarlyGameBoundaries&,s32,s32,s32,s32)noexcept;
[[nodiscard]]s32 function_035c(Machine&,s32,s32)noexcept;
// 2A64..3883: actor-generation/editor logic.  All actor records remain dgroup-backed guest data.
struct ActorGenerationBoundaries{
 void(*call_6588)(u32)=nullptr;void(*call_6c60)(s32,s32,s32,s32,u32)=nullptr;void(*call_6eb0)(s32,s32,s32,u32,u32,u32)=nullptr;
 void(*call_55dc)(u32,s32,s32,s32,s32)=nullptr;u32(*call_53b4)()=nullptr;void(*call_15b0)(s32,s32,s32,s32)=nullptr;
 void(*call_7f50)()=nullptr;u32(*call_7f50_value)()=nullptr;void(*call_84a0)(u32,u32,u32)=nullptr;void(*call_7d68)(u32,u32)=nullptr;
 u32(*call_7de8)()=nullptr;void(*call_9eb0)(u32,u32,u32,u32,u32)=nullptr;void(*call_550c)(u32,s32,s32,s32,s32)=nullptr;
 u32(*call_831c)()=nullptr;void(*call_53e4)(u32)=nullptr;u32(*call_82c0)()=nullptr;u32(*call_7ab4)(u32,u32)=nullptr;
 u32(*call_7b50)(s32,u32)=nullptr;void(*call_7d80)(s32,s32,u32,u32,u32)=nullptr;void(*call_7830)(u32)=nullptr;
};
static u32 actor_base(Machine&m)noexcept{return load_le32(dgroup_ptr(m,dgroup(0x04ccu)))*0x248u;}
// 2A64..2DF8: six-choice class/order editor.  The six local enable bytes mirror the original
// EBP-relative byte array; hover redraw and the wrap-at-0xFF path are kept separate from selection.
void function_2a64(Machine&m,MidUiBoundaries&ui,ActorGenerationBoundaries&b)noexcept{
 s32 first=1,phase=0;u8 enabled[6]{};const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x04ccu)));const u32 ab=actor*0x248u;const u32 order=*dgroup_ptr(m,dgroup(ab+0x680bu));
 for(;;){if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));
  if(!first)for(u32 i=0;i<6u;++i)store_le32(dgroup_ptr(m,dgroup(0x08f4u+i*8u)),load_le32(dgroup_ptr(m,dgroup(0x08f8u+i*8u))));
  for(s32 i=0;i<6;++i){const u32 other=actor^1u,ob=other*0x248u;const s32 pos=phase+i;if(*dgroup_ptr(m,dgroup(other+0x04c4u))!=0u&&*dgroup_ptr(m,dgroup(ob+0x680bu))==order&&static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ob+0x680au)))==pos){store_le32(dgroup_ptr(m,dgroup(0x08f8u+static_cast<u32>(i)*8u)),0x15u);enabled[i]=0;}else if(pos==0x11){store_le32(dgroup_ptr(m,dgroup(0x08f8u+static_cast<u32>(i)*8u)),0x15u);enabled[i]=0;}else{const u32 id=load_le32(dgroup_ptr(m,dgroup(0x713cu)))+order*0x17u+static_cast<u32>(i)+static_cast<u32>(phase);store_le32(dgroup_ptr(m,dgroup(0x08f8u+static_cast<u32>(i)*8u)),id);enabled[i]=1;}}
  if(first){const u32 slots[11]={load_le32(dgroup_ptr(m,dgroup(0x08f8u))),load_le32(dgroup_ptr(m,dgroup(0x0900u))),load_le32(dgroup_ptr(m,dgroup(0x0908u))),load_le32(dgroup_ptr(m,dgroup(0x0910u))),load_le32(dgroup_ptr(m,dgroup(0x0918u))),load_le32(dgroup_ptr(m,dgroup(0x0920u))),0xffffffffu,0xffffffffu,0xffffffffu,0x0du,4u};function_1464(m,ui,4u,slots,0x2a);first=0;}else if(ui.call_7e4)ui.call_7e4(0,6);
  while((b.call_53b4?b.call_53b4():0u)!=0u){}
  s32 hover=-1;
  for(;;){if((b.call_53b4?b.call_53b4():0u)!=0u)break;const s32 mx=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e6u)))),my=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e8u))));const s32 now=function_035c(m,mx,my);if(now==hover)continue;if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));
   auto redraw=[&](s32 hit,bool old){if(hit<=0||hit==0xff||hit>6||enabled[static_cast<u32>(hit-1)]==0u)return;const u32 idx=static_cast<u32>(hit-1);const s32 x=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+idx*8u)))),y=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+idx*8u))));if(b.call_6c60)b.call_6c60(0,x,y,0x14,7);const u32 id=load_le32(dgroup_ptr(m,dgroup(0x713cu)))+order*0x17u+static_cast<u32>(hit-1)+static_cast<u32>(phase);if(old){if(b.call_6eb0)b.call_6eb0(0,x+0x15,y,id,1,0x5af8u);}else if(b.call_6c60)b.call_6c60(0,x+0x15,y,static_cast<s32>(id),1);if(b.call_55dc)b.call_55dc(load_le32(dgroup_ptr(m,dgroup(0x7138u))),x+0x15,y,0x50,0x30);};
   redraw(hover,true);redraw(now,false);hover=now;
  }
  const s32 mx=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e6u)))),my=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e8u))));const s32 e=function_035c(m,mx,my);if(e<0)continue;if(e>0&&e!=0xff&&e<=6){if(enabled[static_cast<u32>(e-1)]==0u)continue;const u8 picked=static_cast<u8>(static_cast<u8>(phase)+static_cast<u8>(e-1));*dgroup_ptr(m,dgroup(ab+0x680au))=picked;if(b.call_15b0)b.call_15b0(e-1,0x114,0xd7,5);store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))+1u);return;}if(e==0xff){const s32 dividend=static_cast<s32>(static_cast<u32>(phase)+6u);phase=dividend%0x12;first=1;continue;}if(e==0){store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))-1u);if(b.call_15b0)b.call_15b0(-1,-1,-1,-1);return;}}
}
static_assert(0x2DF9u-0x2A64u==917u); // 2DF9..2DFB alignment
// 2DFC..301F: bounded actor-name editor.  Input accepts A-Z, space, and digits; backspace is 8.
void function_2dfc(Machine&m,ActorGenerationBoundaries&b,u32 x,u32 limit)noexcept{
 u32 blink=1,ch=0,len=0;if(b.call_7f50)b.call_7f50();const u32 ab=actor_base(m);if(b.call_84a0)b.call_84a0(ab+0x67f8u,0,0x10);*dgroup_ptr(m,dgroup(ab+0x67f8u))=0;if(b.call_7d68)b.call_7d68(x,0xa0);
 while(ch!=0x0du){if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));ch=b.call_7de8?b.call_7de8():0;if(b.call_9eb0)b.call_9eb0(x,ch,0x0a,1,0);u32 deadline=(b.call_7f50_value?b.call_7f50_value():0u)+9u;if(b.call_550c)b.call_550c(load_le32(dgroup_ptr(m,dgroup(0x7138u))),x,0x88,0x0a,0x30);*dgroup_ptr(m,dgroup(0x7138u))^=1u;
  while((b.call_831c?b.call_831c():0u)==0u){const u32 now=b.call_7f50_value?b.call_7f50_value():deadline;if(deadline>=now)continue;deadline=(b.call_7f50_value?b.call_7f50_value():now)+9u;blink^=1u;if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_9eb0)b.call_9eb0(x,ch,0x0a,1,blink?0u:0x5fu);if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;}
  if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));blink=0;if(b.call_9eb0)b.call_9eb0(x,ch,0x0a,1,0x5f);ch=b.call_82c0?b.call_82c0():0;
  if(ch==8u){if(len){if(b.call_9eb0)b.call_9eb0(x,0x88,0x0a,0x30,0x5f);--len;*dgroup_ptr(m,dgroup(ab+0x67f8u+len))=0;}}
  else{const s32 key=static_cast<s32>(ch);if(((key>='A'&&key<='Z')||key==' '||(key>='0'&&key<='9'))&&len<limit){const s32 w0=static_cast<s32>(b.call_7ab4?b.call_7ab4(ab+0x67f8u,0):0u),w1=static_cast<s32>(b.call_7b50?b.call_7b50(static_cast<s32>(static_cast<std::int8_t>(low8(ch))),0):0u);if(static_cast<s32>(static_cast<u32>(w0)+static_cast<u32>(w1))<0x30){*dgroup_ptr(m,dgroup(ab+0x67f8u+len))=low8(ch);++len;}}}
  if(b.call_9eb0)b.call_9eb0(x,0x88,0x0a,0x30,0x5f);if(b.call_7d80)b.call_7d80(static_cast<s32>(x),0x88,0x30,ab+0x67f8u,0);
 }
}
static_assert(0x3020u-0x2DFCu==548u);
// 3020..3134: name-entry presentation and captured-image refresh.
void function_3020(Machine&m,MidUiBoundaries&ui,ActorGenerationBoundaries&b)noexcept{const u32 slots[11]={0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0x14u,0xffffffffu,0xffffffffu,0xffffffffu};function_1464(m,ui,6,slots,0x2e);if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_6c60)b.call_6c60(0,0x5b,0x88,0x14,7);if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;function_2dfc(m,b,0x93,0x0f);const u32 old=load_le32(dgroup_ptr(m,dgroup(0x0958u)));if(old&&b.call_7830)b.call_7830(old);if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_6c60)b.call_6c60(0,0x5b,0x88,0x14,7);if(b.call_7d80)b.call_7d80(0x93,0x88,0x30,actor_base(m)+0x67f8u,0);store_le32(dgroup_ptr(m,dgroup(0x0958u)),ui.call_7868?ui.call_7868(0x5b,0x88,0x7a,0x30,0xff):0u);store_le32(dgroup_ptr(m,dgroup(0x092cu)),0);store_le32(dgroup_ptr(m,dgroup(0x0930u)),1);if(b.call_15b0)b.call_15b0(-1,-1,-1,-1);}
static_assert(0x3135u-0x3020u==277u); // 3135..3137 alignment
// 3138..3204: final accept/delete decision for the selected actor.
void function_3138(Machine&m,MidUiBoundaries&ui,ActorGenerationBoundaries&b)noexcept{const u32 slots[11]={0xffffffffu,0xffffffffu,0x2fu,0x30u,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu};store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))+1u);function_1464(m,ui,6,slots,0x2d);store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))-1u);for(;;){const u32 saved=load_le32(dgroup_ptr(m,dgroup(0x04d0u)));store_le32(dgroup_ptr(m,dgroup(0x04d0u)),0);const s32 e=ui.call_43c?ui.call_43c(m):0;store_le32(dgroup_ptr(m,dgroup(0x04d0u)),saved);if(e==3){if(b.call_15b0)b.call_15b0(-1,-1,-1,-1);function_3020(m,ui,b);const u32 a=load_le32(dgroup_ptr(m,dgroup(0x04ccu)));*dgroup_ptr(m,dgroup(a+0x04c4u))=1;store_le16(dgroup_ptr(m,dgroup(a*0x248u+0x6808u)),0);return;}if(e==4){if(b.call_15b0)b.call_15b0(-1,-1,-1,-1);const u32 a=load_le32(dgroup_ptr(m,dgroup(0x04ccu)));*dgroup_ptr(m,dgroup(a+0x04c4u))=0;*dgroup_ptr(m,dgroup(a*0x248u+0x6809u))|=0x80u;return;}}}
static_assert(0x3205u-0x3138u==205u); // 3205..3207 alignment
// 3208..32F3: derive per-class progression counts from the 057C word table.
void function_3208(Machine&m)noexcept{const u32 ab=actor_base(m);s32 count=1;if(static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ab+0x680eu)))!=-1)count=2;if(static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ab+0x680fu)))!=-1)++count;for(s32 i=0;i<count;i++){const s32 cls=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ab+0x680du+i)));const s32 signed_quota=0x88bc/count;const u32 quota=static_cast<u32>(signed_quota);u32 n=0;while(n<20u&&quota/1000u>=load_le16(dgroup_ptr(m,dgroup(0x057cu+static_cast<u32>(cls*40)+n*2u))))++n;store_le16(dgroup_ptr(m,dgroup(ab+0x6818u+static_cast<u32>(i)*2u)),static_cast<u16>(n));}}
static_assert(0x32F4u-0x3208u==236u);
// 32F4..3349: base value at 6810 plus one encoded random contribution from table 082E.
void function_32f4(Machine&m,EarlyGameBoundaries&rng)noexcept{const u32 ab=actor_base(m);const u32 k=*dgroup_ptr(m,dgroup(ab+0x680cu))*3u;u16 v=*dgroup_ptr(m,dgroup(0x082eu+k));v=static_cast<u16>(v+function_0010(rng,*dgroup_ptr(m,dgroup(0x082fu+k)),*dgroup_ptr(m,dgroup(0x0830u+k)),0,0));store_le16(dgroup_ptr(m,dgroup(ab+0x6810u)),v);}
static_assert(0x334Au-0x32F4u==86u); // 334A..334B alignment
// 334C..3403: roll six attributes into every other byte at 682A and conditionally seed 6836.
void function_334c(Machine&m,EarlyGameBoundaries&rng)noexcept{const u32 ab=actor_base(m);for(u32 i=0;i<14u;i++)*dgroup_ptr(m,dgroup(ab+0x682au+i))=0;for(u32 i=0;i<6u;i++)*dgroup_ptr(m,dgroup(ab+0x682au+i*2u))=static_cast<u8>(function_0010(rng,3,6,0,0)+3u);if(*dgroup_ptr(m,dgroup(ab+0x682au))>=0x12u&&*dgroup_ptr(m,dgroup(ab+0x680cu))!=2u){const u8 a=*dgroup_ptr(m,dgroup(ab+0x680du)),c=*dgroup_ptr(m,dgroup(ab+0x680eu)),d=*dgroup_ptr(m,dgroup(ab+0x680fu));if(a==0||a==1||a==2||c==0||d==0)*dgroup_ptr(m,dgroup(ab+0x6836u))=static_cast<u8>(function_0010(rng,1,100,0,0));}}
static_assert(0x3404u-0x334Cu==184u);
// 3404..37BD: enforce class/race minima, compute progression aggregates, and clamp 6816/6836.
void function_3404(Machine&m)noexcept{const u32 ab=actor_base(m);const u32 race=*dgroup_ptr(m,dgroup(ab+0x680cu));for(u32 i=0;i<6u;i++){const s32 bonus=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(0x06b4u+race*6u+i)));u8&v=*dgroup_ptr(m,dgroup(ab+0x682au+i*2u));const s32 lo=*dgroup_ptr(m,dgroup(0x066cu+race*12u+i*2u))+bonus,hi=*dgroup_ptr(m,dgroup(0x066du+race*12u+i*2u))+bonus;if(static_cast<s32>(v)<lo)v=static_cast<u8>(lo);if(static_cast<s32>(v)>hi)v=static_cast<u8>(hi);}if(*dgroup_ptr(m,dgroup(ab+0x682au))==0x12u&&!*dgroup_ptr(m,dgroup(ab+0x6836u)))++*dgroup_ptr(m,dgroup(ab+0x6836u));const u8 c0=*dgroup_ptr(m,dgroup(ab+0x680du)),c1=*dgroup_ptr(m,dgroup(ab+0x680eu)),c2=*dgroup_ptr(m,dgroup(ab+0x680fu));if(*dgroup_ptr(m,dgroup(ab+0x682au))<0x12u||((c0!=1&&c0!=2&&c1!=2&&c2!=2&&c0!=0&&c1!=0&&c2!=0))) *dgroup_ptr(m,dgroup(ab+0x6836u))=0;
 for(u32 stat=0;stat<6u;stat++)for(u32 j=0;j<3u;j++){const s32 cls=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ab+0x680du+j)));if(cls<0)continue;const u8 minv=*dgroup_ptr(m,dgroup(0x07d4u+stat*15u+static_cast<u32>(cls)));u8&v=*dgroup_ptr(m,dgroup(ab+0x682au+stat*2u));if(v<minv)v=minv;}
 s32 classes=1;if(static_cast<std::int8_t>(c1)!=-1)classes=2;if(static_cast<std::int8_t>(c2)!=-1)++classes;store_le32(dgroup_ptr(m,dgroup(0x711cu)),0);store_le32(dgroup_ptr(m,dgroup(0x7118u)),0x63);for(s32 i=0;i<classes;i++){const s32 cls=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ab+0x680du+i)));const s32 levels=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(ab+0x6818u+i*2u))));s32 sum=0;for(s32 n=0;n<levels;n++){s32 q=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(0x0853u+static_cast<u32>(cls))));if(q<1)q=1;sum+=q;}store_le32(dgroup_ptr(m,dgroup(0x711cu)),load_le32(dgroup_ptr(m,dgroup(0x711cu)))+static_cast<u32>(sum));if(levels<static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x7118u)))))store_le32(dgroup_ptr(m,dgroup(0x7118u)),static_cast<u32>(levels));}
 s32 maxlevel=0;for(s32 i=0;i<classes;i++){const s32 q=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(ab+0x6818u+i*2u))));if(q>maxlevel)maxlevel=q;}s32 cls=static_cast<std::int8_t>(c0);if(c1==2)cls=2;const s32 average=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x711cu))))/classes;store_le32(dgroup_ptr(m,dgroup(0x711cu)),static_cast<u32>(average));const u32 variant=*dgroup_ptr(m,dgroup(ab+0x682eu));s32 extra=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x04d4u+(cls/3)*0x4cu+variant*4u))));if(extra<0)extra=0;const u32 scaled=static_cast<u32>(extra)*static_cast<u32>(maxlevel);store_le32(dgroup_ptr(m,dgroup(0x711cu)),load_le32(dgroup_ptr(m,dgroup(0x711cu)))+scaled);store_le32(dgroup_ptr(m,dgroup(0x7118u)),load_le32(dgroup_ptr(m,dgroup(0x7118u)))+scaled);*dgroup_ptr(m,dgroup(ab+0x6813u))=10;s32 lo=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x7118u)))),hi=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x711cu))));if(hi<lo)hi=lo;store_le32(dgroup_ptr(m,dgroup(0x711cu)),static_cast<u32>(hi));s32 hp=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(ab+0x6816u))));if(hp<lo)hp=static_cast<s16>(static_cast<u16>(lo));if(hp>hi)hp=static_cast<s16>(static_cast<u16>(hi));if(hp==0)hp=1;store_le16(dgroup_ptr(m,dgroup(ab+0x6816u)),static_cast<u16>(hp));if(*dgroup_ptr(m,dgroup(ab+0x6836u))>100)*dgroup_ptr(m,dgroup(ab+0x6836u))=100;}
static_assert(0x37BEu-0x3404u==954u); // 37BE..37BF alignment
// 37C0..3881: recompute 6816 from per-class random rolls and the class/variant table.
void function_37c0(Machine&m,EarlyGameBoundaries&rng)noexcept{const u32 ab=actor_base(m);s32 classes=1;if(static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ab+0x680eu)))!=-1)classes=2;if(static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ab+0x680fu)))!=-1)++classes;s32 total=0,last_cls=0;for(s32 i=0;i<classes;i++){last_cls=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ab+0x680du+i)));const s32 sides=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(0x0853u+last_cls)));const s32 n=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(ab+0x6818u+i*2u))));total+=static_cast<s32>(function_0010(rng,n,sides,0,0));}s32 hp=total/classes;const u32 variant=*dgroup_ptr(m,dgroup(ab+0x682eu));hp+=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x04d4u+(last_cls/3)*0x4cu+variant*4u))));store_le16(dgroup_ptr(m,dgroup(ab+0x6816u)),static_cast<u16>(hp));}
static_assert(0x3882u-0x37C0u==194u); // 3882..3883 alignment
// 3884..38A3: actor-stat initialization driver.
void function_3884(Machine&m,EarlyGameBoundaries&rng)noexcept{function_3208(m);function_32f4(m,rng);function_334c(m,rng);function_37c0(m,rng);function_3404(m);}static_assert(0x38A4u-0x3884u==32u);


// 38A4..4976: actor-stat presentation/adjustment and the following two-page presentation loop.
// External renderer/audio/resource routines remain explicit boundaries; guest state stays in DGROUP.
struct ActorStatsBoundaries{
 void(*call_6c60)(s32,s32,s32,s32,u32)=nullptr; void(*call_7df4)(u32)=nullptr;
 void(*call_004c)(u32,u32)=nullptr; void(*call_7e44)(s32,s32,u32,u32)=nullptr;
 void(*call_afee3)(u32,u32,u32)=nullptr; void(*call_afee4)(u32,u32,u32,u32)=nullptr;
 u32(*call_88d8)(s32)=nullptr; void(*call_7e04)(s32,s32,u32,u32)=nullptr;
 void(*call_72a0)(u32,u32)=nullptr; void(*call_b01a)(u32,u32)=nullptr;
 void(*call_56dc)()=nullptr; void(*call_6588)(u32)=nullptr;
 void(*call_55dc)(u32,s32,s32,s32,s32)=nullptr; u32(*call_53b4)()=nullptr;
 u32(*call_7868)(s32,s32,s32,s32,u32)=nullptr; void(*call_9eb0)(s32,s32,s32,s32,u32)=nullptr;
 void(*call_7830)(u32)=nullptr; void(*call_67c8)(u32,s32,s32,u32)=nullptr;
 u32(*call_7f50)()=nullptr; void(*call_61ac)(u32)=nullptr; void(*call_5780)()=nullptr;
 void(*call_a3e8)(u32,s32,s32,s32,s32,s32,u32)=nullptr; void(*call_57dc)()=nullptr;
 u32(*call_954c)(u32,u32)=nullptr; void(*call_a5c8)(u32,u32)=nullptr;
 void(*call_6c98)(s32,s32,s32,s32,u32)=nullptr; void(*call_86c4)(s32,s32,u32,u32)=nullptr;
 void(*call_a390)(u32)=nullptr; void(*call_65a4)(u32)=nullptr; void(*call_87c4)()=nullptr;
 void(*call_87d8)(u32,u32)=nullptr; void(*call_bc1c)(u32,u32,u32)=nullptr;
 void(*call_bfe0)(u32,u32,u32,u32,u32,u32)=nullptr; void(*call_c4e0)(u32)=nullptr;
 void(*call_91d8)(u32,u32)=nullptr;
};
[[nodiscard]]static u32 actor_page_xor1(Machine&m)noexcept{u32 v=load_le32(dgroup_ptr(m,dgroup(0x7138u)));set_low8(v,static_cast<u8>(low8(v)^1u));return v;}

// Final binary audit checkpoint 3: raw CODE.1 confirms 67C8 cdecl order (0,x,y,image)
// and the deliberate mixed-width use of 7138 (byte for 6588/65A4, dword for 55DC/53E4/A390).
// 38A4..3C54: render the selected actor's six byte stats, secondary derived values,
// class progression counts, and hit-point field into the 55F8 scratch string.
void function_38a4(Machine&m,ActorStatsBoundaries&b,s32 x,s32 y)noexcept{
 if(b.call_6c60)b.call_6c60(0,x,y,0x14,7);x+=4;
 s32 row=0;const u32 ab=actor_base(m);
 for(;row<6;++row){
  if(row==static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x7124u))))){*dgroup_ptr(m,dgroup(0x5a00u))=0x5au;if(b.call_7df4)b.call_7df4(0x59f8u);}
  if(b.call_004c)b.call_004c(0x55f8u,static_cast<u32>(row+0x2f));
  if(b.call_7e44)b.call_7e44(x,y+1,0x55f8u,0);
  const u32 stat_addr=ab+0x682au+static_cast<u32>(row)*2u;const u32 stat=*dgroup_ptr(m,dgroup(stat_addr));
  if(row==0&&*dgroup_ptr(m,dgroup(ab+0x6836u))!=0u){const u32 rem=*dgroup_ptr(m,dgroup(ab+0x6836u))%100u;if(b.call_afee4)b.call_afee4(0x55f8u,0x54u,stat,rem);}
  else if(b.call_afee3)b.call_afee3(0x55f8u,0x4cu,stat);
  if(b.call_7e04)b.call_7e04(x,y+0x2f,0x55f8u,0);
  if(row==static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x7124u))))){*dgroup_ptr(m,dgroup(0x5a00u))=8u;if(b.call_7df4)b.call_7df4(0);}
  x+=0x0a;
 }
 x+=0x0a;
 if(b.call_004c)b.call_004c(0x55f8u,static_cast<u32>(row+0x2f));
 if(b.call_7e44)b.call_7e44(x,y+1,0x55f8u,0);
 {const s32 v=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(ab+0x6810u))));const u32 q=b.call_88d8?b.call_88d8(v):0;if(b.call_7e04)b.call_7e04(x,y+0x2f,q,0);}
 x+=0x0a;
 if(b.call_004c)b.call_004c(0x55f8u,static_cast<u32>(row+0x30));++row;
 if(b.call_7e44)b.call_7e44(x,y+1,0x55f8u,0);
 {const s32 cls=*reinterpret_cast<std::int8_t*>(dgroup_ptr(m,dgroup(ab+0x682cu)));const s32 bonus=*reinterpret_cast<std::int8_t*>(dgroup_ptr(m,dgroup(0x083fu+static_cast<u32>(static_cast<u8>(cls)))));const s32 v=*reinterpret_cast<std::int8_t*>(dgroup_ptr(m,dgroup(ab+0x6813u)))+bonus;const u32 q=b.call_88d8?b.call_88d8(v):0;if(b.call_7e04)b.call_7e04(x,y+0x2f,q,0);}
 x+=0x0a;
 if(b.call_004c)b.call_004c(0x55f8u,static_cast<u32>(row+0x30));row+=2;
 if(static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ab+0x680eu)))!=-1)if(b.call_004c)b.call_004c(0x55f8u,static_cast<u32>(row+0x2f));
 if(b.call_7e44)b.call_7e44(x,y+1,0x55f8u,0);
 {const s32 v=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(ab+0x6818u))));const u32 q=b.call_88d8?b.call_88d8(v):0;if(b.call_72a0)b.call_72a0(0x55f8u,q);}
 if(static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ab+0x680eu)))!=-1){if(b.call_b01a)b.call_b01a(0x55f8u,0x60u);const s32 v=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(ab+0x681au))));const u32 q=b.call_88d8?b.call_88d8(v):0;if(b.call_b01a)b.call_b01a(0x55f8u,q);}
 if(static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ab+0x680fu)))!=-1){if(b.call_b01a)b.call_b01a(0x55f8u,0x60u);const s32 v=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(ab+0x681cu))));const u32 q=b.call_88d8?b.call_88d8(v):0;if(b.call_b01a)b.call_b01a(0x55f8u,q);}
 if(b.call_7e04)b.call_7e04(x,y+0x2f,0x55f8u,0);x+=0x0a;++row;
 if(load_le32(dgroup_ptr(m,dgroup(0x7124u)))==10u){*dgroup_ptr(m,dgroup(0x5a00u))=0x5au;if(b.call_7df4)b.call_7df4(0x59f8u);}
 if(b.call_004c)b.call_004c(0x55f8u,static_cast<u32>(row+0x2f));
 if(b.call_7e44)b.call_7e44(x,y+1,0x55f8u,0);
 {const s32 v=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(ab+0x6816u))));const u32 q=b.call_88d8?b.call_88d8(v):0;if(b.call_7e04)b.call_7e04(x,y+0x2f,q,0);}
 if(load_le32(dgroup_ptr(m,dgroup(0x7124u)))==10u){*dgroup_ptr(m,dgroup(0x5a00u))=8u;if(b.call_7df4)b.call_7df4(0);}
}
static_assert(0x3C55u-0x38A4u==945u);
// 3C55..3C57 is a three-byte LEA alignment instruction.

static void actor_stats_redraw(Machine&m,ActorStatsBoundaries&b)noexcept{
 const u32 page_byte=*dgroup_ptr(m,dgroup(0x7138u));const u32 page_dword=load_le32(dgroup_ptr(m,dgroup(0x7138u)));if(b.call_6588)b.call_6588(page_byte);function_38a4(m,b,0x5b,0x88);if(b.call_55dc)b.call_55dc(page_dword,0x5b,0x88,0x7a,0x30);while(b.call_53b4&&b.call_53b4()!=0u){}
}
// 3C58..40FA: interactive stat adjustment. Event 7 increments, event 9 decrements,
// event 8 updates the selected column from mouse X, and event 0 exits and rebuilds the two cached surfaces.
void function_3c58(Machine&m,MidUiBoundaries&ui,ActorStatsBoundaries&b)noexcept{
 store_le32(dgroup_ptr(m,dgroup(0x0924u)),load_le32(dgroup_ptr(m,dgroup(0x0928u))));
 store_le32(dgroup_ptr(m,dgroup(0x092cu)),load_le32(dgroup_ptr(m,dgroup(0x0930u))));
 store_le32(dgroup_ptr(m,dgroup(0x0934u)),load_le32(dgroup_ptr(m,dgroup(0x0938u))));
 store_le32(dgroup_ptr(m,dgroup(0x0928u)),0x2cu);store_le32(dgroup_ptr(m,dgroup(0x0938u)),0x2du);store_le32(dgroup_ptr(m,dgroup(0x0940u)),3u);
 if(ui.call_7e4)ui.call_7e4(6,9);if(b.call_56dc)b.call_56dc();store_le32(dgroup_ptr(m,dgroup(0x7124u)),0xffffffffu);
 bool done=false;
 while(!done){const s32 ev=ui.call_43c?ui.call_43c(m):0;if(ev==0){done=true;continue;}const u32 ab=actor_base(m);const s32 sel=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x7124u))));
  if(ev==7&&sel!=-1){
   if(sel<6){u8*stat=dgroup_ptr(m,dgroup(ab+0x682au+static_cast<u32>(sel)*2u));if(sel==0){if((*dgroup_ptr(m,dgroup(0x04d4u))&1u)!=0u){if(*stat==0x12u){const u8 cls=*dgroup_ptr(m,dgroup(ab+0x680du));if(cls==0u||cls==1u||cls==2u)++*dgroup_ptr(m,dgroup(ab+0x6836u));}else ++*stat;}else{*stat=0x14u;*dgroup_ptr(m,dgroup(ab+0x6836u))=0x64u;}}else if((*dgroup_ptr(m,dgroup(0x04d4u))&1u)!=0u)++*stat;else *stat=0x14u;}
   else if(sel==10){u16 v=load_le16(dgroup_ptr(m,dgroup(ab+0x6816u)));v=((*dgroup_ptr(m,dgroup(0x04d4u))&1u)!=0u)?static_cast<u16>(v+1u):static_cast<u16>(1000u);store_le16(dgroup_ptr(m,dgroup(ab+0x6816u)),v);}function_3404(m);actor_stats_redraw(m,b);
  }else if(ev==8){const s32 mx=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e6u))));const s32 q=(mx-0x60)/10;store_le32(dgroup_ptr(m,dgroup(0x7124u)),(q>=0&&(q<6||q==10))?static_cast<u32>(q):0xffffffffu);actor_stats_redraw(m,b);
  }else if(ev==9&&sel!=-1){
   if(sel<6){u8*stat=dgroup_ptr(m,dgroup(ab+0x682au+static_cast<u32>(sel)*2u));if(sel==0){if((*dgroup_ptr(m,dgroup(0x04d4u))&1u)!=0u){if(*stat==0x12u&&*dgroup_ptr(m,dgroup(ab+0x6836u))!=1u)--*dgroup_ptr(m,dgroup(ab+0x6836u));else --*stat;}else{*stat=0;*dgroup_ptr(m,dgroup(ab+0x6836u))=0;}}else if((*dgroup_ptr(m,dgroup(0x04d4u))&1u)!=0u)--*stat;else *stat=0;}
   else if(sel==10){u16 v=load_le16(dgroup_ptr(m,dgroup(ab+0x6816u)));v=((*dgroup_ptr(m,dgroup(0x04d4u))&1u)!=0u)?static_cast<u16>(v-1u):0u;store_le16(dgroup_ptr(m,dgroup(ab+0x6816u)),v);}function_3404(m);actor_stats_redraw(m,b);
  }
 }
 store_le32(dgroup_ptr(m,dgroup(0x7124u)),0xffffffffu);const u32 page=*dgroup_ptr(m,dgroup(0x7138u));if(b.call_6588)b.call_6588(page);
 const u32 temp=b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u;if(b.call_9eb0)b.call_9eb0(0,0,0x7a,0x30,0xfe);function_38a4(m,b,0,0);
 for(u32 cell:{0x0954u,0x0958u}){const u32 h=load_le32(dgroup_ptr(m,dgroup(cell)));if(h&&b.call_7830)b.call_7830(h);store_le32(dgroup_ptr(m,dgroup(cell)),0u);}
 store_le32(dgroup_ptr(m,dgroup(0x0958u)),b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u);store_le32(dgroup_ptr(m,dgroup(0x0954u)),b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u);
 if(b.call_67c8)b.call_67c8(0,0,0,temp);if(b.call_7830)b.call_7830(temp);
 store_le32(dgroup_ptr(m,dgroup(0x0924u)),load_le32(dgroup_ptr(m,dgroup(0x0928u))));store_le32(dgroup_ptr(m,dgroup(0x092cu)),load_le32(dgroup_ptr(m,dgroup(0x0930u))));store_le32(dgroup_ptr(m,dgroup(0x0934u)),load_le32(dgroup_ptr(m,dgroup(0x0938u))));
 store_le32(dgroup_ptr(m,dgroup(0x0928u)),0x16u);store_le32(dgroup_ptr(m,dgroup(0x0938u)),0x2eu);store_le32(dgroup_ptr(m,dgroup(0x0940u)),0x0eu);if(ui.call_7e4)ui.call_7e4(6,9);
}
static_assert(0x40FBu-0x3C58u==1187u);
// 40FB is one-byte NOP alignment.

// 40FC..44E0: actor-generation presentation controller. The local EBX state is preserved as
// three states: 0=continue event polling, 1=return to the inner presentation loop, 2=leave routine.
void function_40fc(Machine&m,MidUiBoundaries&ui,ActorGenerationBoundaries&gen,EarlyGameBoundaries&rng,ActorStatsBoundaries&b)noexcept{
 const u32 tick=b.call_7f50?b.call_7f50():0u;if(b.call_61ac)b.call_61ac(tick);function_3884(m,rng);store_le32(dgroup_ptr(m,dgroup(0x7124u)),0xffffffffu);
 if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));u32 temp=b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u;if(b.call_9eb0)b.call_9eb0(0,0,0x7a,0x30,0xfe);function_38a4(m,b,0,0);store_le32(dgroup_ptr(m,dgroup(0x0958u)),b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u);if(b.call_67c8)b.call_67c8(0,0,0,temp);if(b.call_7830)b.call_7830(temp);
 const u32 slots[11]={0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0x16u,2u,0x2eu,0x0eu,0xffffffffu};function_1464(m,ui,5u,slots,0x2b);
 auto draw_pages=[&](){for(int i=0;i<2;++i){const u32 page_byte=*dgroup_ptr(m,dgroup(0x7138u));const u32 page_dword=load_le32(dgroup_ptr(m,dgroup(0x7138u)));if(b.call_6588)b.call_6588(page_byte);if(b.call_67c8)b.call_67c8(0,0x5b,0x88,load_le32(dgroup_ptr(m,dgroup(0x0958u))));if(b.call_6c60)b.call_6c60(0,0x5b,0xbd,0x2e,7);if(gen.call_53e4)gen.call_53e4(page_dword);*dgroup_ptr(m,dgroup(0x7138u))^=1u;}};draw_pages();
 for(;;){s32 state=0;while(state==0){const s32 ev=ui.call_43c?ui.call_43c(m):0;if(ev==0){state=2;if(ui.call_15b0)ui.call_15b0(-1,-1,-1,-1);store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))-1u);}else if(ev==7){const u32 h=load_le32(dgroup_ptr(m,dgroup(0x0954u)));if(h&&b.call_7830)b.call_7830(h);store_le32(dgroup_ptr(m,dgroup(0x0954u)),load_le32(dgroup_ptr(m,dgroup(0x0958u))));function_3884(m,rng);if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));temp=b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u;if(b.call_9eb0)b.call_9eb0(0,0,0x7a,0x30,0xfe);function_38a4(m,b,0,0);store_le32(dgroup_ptr(m,dgroup(0x0958u)),b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u);if(b.call_67c8)b.call_67c8(0,0,0,temp);if(b.call_7830)b.call_7830(temp);store_le32(dgroup_ptr(m,dgroup(0x0924u)),load_le32(dgroup_ptr(m,dgroup(0x0928u))));store_le32(dgroup_ptr(m,dgroup(0x0934u)),load_le32(dgroup_ptr(m,dgroup(0x0938u))));store_le32(dgroup_ptr(m,dgroup(0x092cu)),load_le32(dgroup_ptr(m,dgroup(0x0930u))));*dgroup_ptr(m,dgroup(0x0930u))^=1u;if(ui.call_7e4)ui.call_7e4(7,8);state=1;}else if(ev==8){if(ui.call_15b0)ui.call_15b0(-1,-1,-1,-1);function_3138(m,ui,gen);state=2;store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))+1u);}else if(ev==9){if(b.call_004c)b.call_004c(0x7140u,0x2cu);if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_6c60)b.call_6c60(0,5,0,0x2a,7);if(gen.call_7d80)gen.call_7d80(5,0,0x140,0x7140u,1);if(b.call_5780)b.call_5780();if(b.call_a3e8)b.call_a3e8(actor_page_xor1(m),5,0,5,0,0x14,0x140);if(b.call_57dc)b.call_57dc();function_3c58(m,ui,b);if(b.call_72a0)b.call_72a0(0x7140u,load_le32(dgroup_ptr(m,dgroup(0x04b8u))));if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_6c60)b.call_6c60(0,5,0,0x2a,7);if(gen.call_7d80)gen.call_7d80(5,0,0x140,0x7140u,1);if(b.call_5780)b.call_5780();if(b.call_a3e8)b.call_a3e8(actor_page_xor1(m),5,0,5,0,0x14,0x140);if(b.call_57dc)b.call_57dc();state=1;}}
  if(state==1){draw_pages();continue;}break;
 }
 for(u32 cell:{0x0954u,0x0958u}){const u32 h=load_le32(dgroup_ptr(m,dgroup(cell)));if(h&&b.call_7830)b.call_7830(h);store_le32(dgroup_ptr(m,dgroup(cell)),0u);}
}
static_assert(0x44E1u-0x40FCu==997u);
// 44E1..44E3 is a three-byte LEA alignment instruction.

// 44E4..451E: advance the paired local counters; the two encoded wrap pairs are (20,-2) and (0,2).
void function_44e4(s32*a,s32*b)noexcept{s32 sum=*a+*b,step=*b;if(sum==0x18){sum=0x14;step=-2;}if(sum==-4){sum=0;step=2;}*a=sum;*b=step;}
static_assert(0x451Fu-0x44E4u==59u);
// 451F is one-byte NOP alignment.

// 4520..45EE: draw one step of the paired-counter indicator using resource 3C/3D and the fixed 3B marker.
void function_4520(ActorStatsBoundaries&b,s32 value,s32 step)noexcept{
 if(value==0x16){const u32 q=b.call_954c?b.call_954c(0x3du,7u):0u;if(b.call_a5c8)b.call_a5c8(0,q);if(b.call_6c98)b.call_6c98(0,0xe5,value-step,0x3d,7);const u32 r=b.call_954c?b.call_954c(0x3du,7u):0u;if(b.call_a5c8)b.call_a5c8(0,r);}
 else if(value==-2){if(b.call_6c98)b.call_6c98(0,0xe5,value-step,0x3d,7);}
 else{if(step>0){const u32 q=b.call_954c?b.call_954c(0x3cu,7u):0u;if(b.call_a5c8)b.call_a5c8(0,q);}if(b.call_6c98)b.call_6c98(0,0xe5,value,0x3c,7);if(step>0){const u32 q=b.call_954c?b.call_954c(0x3cu,7u):0u;if(b.call_a5c8)b.call_a5c8(0,q);}}
 if(b.call_6c60)b.call_6c60(0,0xe5,0,0x3b,7);
}
static_assert(0x45EFu-0x4520u==207u);
// 45EF..45F8 is the ten-byte local lookup table "678456789:"; 45F9..45FB is LEA alignment.

// 45FC..4976: two-pass presentation loop. The local table is indexed as group*5 + floor(frame/2),
// 44E4 advances the paired indicator counters, and the two 87C4/87D8 loops preserve their exact 10/25 iterations.
void function_45fc(Machine&m,ActorStatsBoundaries&b)noexcept{
 static constexpr u8 table[10]={'6','7','8','4','5','6','7','8','9',':'};s32 value=0,step=2;
 if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_86c4)b.call_86c4(0,0,0x1b,7);if(b.call_86c4)b.call_86c4(0xc8,0,0x1c,7);
 const u32 panel=b.call_7868?b.call_7868(0xe5,0,0xaa,0x50,0xff):0u;store_le32(dgroup_ptr(m,dgroup(0x7130u)),panel);function_4520(b,0,2);if(b.call_6c60)b.call_6c60(0,0x15f,0x74,0x31,7);if(b.call_a390)b.call_a390(actor_page_xor1(m));if(b.call_65a4)b.call_65a4(*dgroup_ptr(m,dgroup(0x7138u)));*dgroup_ptr(m,dgroup(0x7138u))^=1u;
 for(u32 id=0x31;id<=0x3d;++id)if(b.call_954c)(void)b.call_954c(id,7u);if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_67c8)b.call_67c8(0,0xe5,0,panel);
 u32 capture=b.call_7868?b.call_7868(0x96,0x8c,0xe2,0x68,0xff):0u;
 for(u32 group=0;group<2u;++group){
  for(s32 frame=0;frame<10;++frame){if(b.call_87c4)b.call_87c4();if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_67c8)b.call_67c8(0,0x96,0x8c,capture);if(b.call_6c60)b.call_6c60(0,0x96,0x8c,table[group*5u+static_cast<u32>(frame/2)],7);if(b.call_67c8)b.call_67c8(0,0xe5,0,panel);function_4520(b,value,step);if(b.call_65a4)b.call_65a4(*dgroup_ptr(m,dgroup(0x7138u)));*dgroup_ptr(m,dgroup(0x7138u))^=1u;if(b.call_87d8)b.call_87d8(2,0);
   if(group==0u&&frame==0){if(b.call_bc1c)b.call_bc1c(0x18,1,0x9c4u);if(b.call_bfe0)b.call_bfe0(0x64f8u,0,0xffu,0,0,0);}if(frame==1&&b.call_c4e0)b.call_c4e0(load_le32(dgroup_ptr(m,dgroup(0x712cu)))+0x0du);function_44e4(&value,&step);
  }
  for(s32 frame=0;frame<25;++frame){if(b.call_87c4)b.call_87c4();if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_67c8)b.call_67c8(0,0x96,0x8c,capture);if(b.call_6c60){if(group==0u)b.call_6c60(0,0x15f,0x95,0x32,7);else b.call_6c60(0,0x15f,0xb2,0x33,7);}if(b.call_67c8)b.call_67c8(0,0xe5,0,panel);function_4520(b,value,step);if(b.call_65a4)b.call_65a4(*dgroup_ptr(m,dgroup(0x7138u)));*dgroup_ptr(m,dgroup(0x7138u))^=1u;if(b.call_87d8)b.call_87d8(2,0);function_44e4(&value,&step);if(group==0u&&frame==15&&b.call_c4e0)b.call_c4e0(load_le32(dgroup_ptr(m,dgroup(0x712cu)))+0x0cu);}
  if(b.call_7830)b.call_7830(capture);capture=b.call_7868?b.call_7868(0x96,0x8c,0xd8,0x68,0xff):0u;
 }
 if(b.call_91d8){b.call_91d8(0x32,7);b.call_91d8(0x33,7);for(u32 id=0x34;id<=0x3a;++id)b.call_91d8(id,7);}if(b.call_7830){b.call_7830(capture);b.call_7830(panel);}
}
static_assert(0x4977u-0x45FCu==891u);


// 4978..59AB: post-generation controller, palette/table preparation, resource bootstrap,
// mouse/cursor page management, and the first 9-byte search-tree insertion helper.
// Calls outside this verified span stay explicit boundaries; all encoded DGROUP offsets are retained.
struct PostActorBoundaries{
 void(*call_c254)(u32,u32,u32,u32,u32)=nullptr; void(*call_mode)(u32)=nullptr;
 void(*call_5a34)(u32)=nullptr; u32(*call_5b64)(u32,u32,u32,u32)=nullptr; void(*call_cbd4)(u32)=nullptr;
 s32(*open_id)(u32)=nullptr; void(*fatal_9814)(u32)=nullptr; void(*close_handle)(u32)=nullptr;
 s32(*seek_handle)(u32,u32,u32)=nullptr; u16(*read_u16_handle)(u32)=nullptr; u32(*read_u32_handle)(u32)=nullptr; void(*read_guest)(Machine&,u32,u32,u32)=nullptr; void(*call_6320)(u32,u32,u32)=nullptr;
 void(*call_b74c)(u32)=nullptr; void(*call_c650)(u32)=nullptr; u32(*alloc_71c8)(u32)=nullptr;
 void(*call_95bc)(u32,u32)=nullptr; void(*call_9724)(u32,u32,u32)=nullptr; void(*call_64dc)(u32)=nullptr;
 void(*call_6618)(u32)=nullptr; void(*call_9ac4)()=nullptr; void(*call_84a0)(u32,u32,u32)=nullptr;
 void(*call_6588)(u32)=nullptr; void(*call_65a4)(u32)=nullptr; s32(*call_53b4)()=nullptr; void(*call_61d0)(u32)=nullptr;
 void(*call_bb80)(u32)=nullptr; void(*call_bcb4)()=nullptr; void(*call_9778)()=nullptr; void(*call_5f4c)()=nullptr;
 void(*terminate_cbf0)(u32)=nullptr; u32(*call_84e0)()=nullptr; void(*call_6c44)(u32)=nullptr;
 void(*call_6bc0)(s32,s32,s32,s32)=nullptr; void(*call_5780)()=nullptr; void(*call_57dc)()=nullptr;
 void(*call_cec8)(u32,u32,u32,u32)=nullptr; u16(*call_84d0)()=nullptr;
 void(*call_d130)(s32,s32)=nullptr; void(*call_d10c)(s32,s32)=nullptr; void(*call_d08c)(s32,s32)=nullptr;
 struct MouseSnapshot{u16 buttons=0;u16 pad=0;u16 y=0;u16 x=0;}; MouseSnapshot(*call_d0b4)()=nullptr;
 u32(*call_954c)(u32,u32)=nullptr; void(*call_d1e8)(u32,u32,u32)=nullptr; void(*call_cf98)()=nullptr;
 void(*call_9ae8)()=nullptr; void(*call_8740)(u32)=nullptr; void(*call_a390)(u32)=nullptr; void(*call_8788)()=nullptr;
 void(*call_a3e8)(u32,s32,s32,s32,s32,s32,s32)=nullptr; void(*call_6744)(u32,s32,s32,u32,u32)=nullptr;
 void(*call_66c0)(u32,s32,s32,u32,u32)=nullptr; void(*call_6d54)(u32,s32,s32,u32)=nullptr;
 void(*call_63a4)(u32)=nullptr; u32(*call_62d8)(u32)=nullptr; u32(*call_62c0)(u32)=nullptr;
};
void function_52ac(Machine&,PostActorBoundaries&)noexcept;
void function_538c(Machine&,PostActorBoundaries&)noexcept;
[[nodiscard]]static s32 div4_toward_zero(s32 v)noexcept{return v/4;}

// 4978..4A09: top-level character-generation controller. Mode 7 wraps back to mode 0;
// a completed pair of actors returns 10, while abandoning with an incomplete pair returns 0.
[[nodiscard]]s32 function_4978(Machine&m,ActorStatsBoundaries&stats,PostActorBoundaries&b)noexcept{
 store_le32(dgroup_ptr(m,dgroup(0x04d0u)),0u);if(b.call_c254)b.call_c254(0x64f8u,0,8,0x64,0x100);function_45fc(m,stats);
 *dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(*dgroup_ptr(m,dgroup(0x7138u))^1u);if(b.call_57dc)b.call_57dc();
 for(;;){u32 mode=load_le32(dgroup_ptr(m,dgroup(0x04d0u)));if(mode==7u){store_le32(dgroup_ptr(m,dgroup(0x04d0u)),0u);continue;}if(b.call_mode)b.call_mode(mode);while(b.call_53b4&&b.call_53b4()!=0u){}if(load_le32(dgroup_ptr(m,dgroup(0x04ccu)))!=0xffffffffu)continue;return (*dgroup_ptr(m,dgroup(0x04c4u))!=0u&&*dgroup_ptr(m,dgroup(0x04c5u))!=0u)?10:0;}
}
static_assert(0x4A0Au-0x4978u==146u); // 4A0A..4A0B is MOV EAX,EAX alignment.

// 4A0C..4AFF: build five 256-entry palette remap rows. Each component moves from the
// 64F8 source palette toward 63 by ((63-component)*row+2)/4 with signed truncation toward zero.
void function_4a0c(Machine&m,PostActorBoundaries&b)noexcept{
 if(b.call_5a34)b.call_5a34(0x64f8u);
 for(s32 row=0;row<5;++row)for(u32 i=0;i<0x100u;++i){const u32 q=0x64f8u+i*3u;const s32 r=*dgroup_ptr(m,dgroup(q)),g=*dgroup_ptr(m,dgroup(q+1u)),bl=*dgroup_ptr(m,dgroup(q+2u));const s32 rr=r+div4_toward_zero((63-r)*row+2),gg=g+div4_toward_zero((63-g)*row+2),bb=bl+div4_toward_zero((63-bl)*row+2);const u32 v=b.call_5b64?b.call_5b64(static_cast<u8>(rr),static_cast<u8>(gg),static_cast<u8>(bb),0x0fu):0u;*dgroup_ptr(m,dgroup(0x5ff8u+static_cast<u32>(row)*0x100u+i))=static_cast<u8>(v);}
}
static_assert(0x4B00u-0x4A0Cu==244u);

// 4B00..4B16: identity byte table at 59F8.
void function_4b00(Machine&m)noexcept{for(u32 i=0;i<0x100u;++i)*dgroup_ptr(m,dgroup(0x59f8u+i))=static_cast<u8>(i);}static_assert(0x4B17u-0x4B00u==23u); // 4B17 NOP alignment.

// 4B18..4C7E: derive the base 256-entry luminance bucket map and four progressively
// brightened rows, then pass the resulting 5*256 bytes through CBD4.
void function_4b18(Machine&m,PostActorBoundaries&b)noexcept{
 for(u32 i=0;i<0x100u;++i){const u32 q=0x64f8u+i*3u;const s32 lum=(static_cast<s32>(*dgroup_ptr(m,dgroup(q)))*300)/1000+(static_cast<s32>(*dgroup_ptr(m,dgroup(q+1u)))*590)/1000+(static_cast<s32>(*dgroup_ptr(m,dgroup(q+2u)))*110)/1000;s32 bucket=0;while(bucket<0x1c&&static_cast<s32>(*dgroup_ptr(m,dgroup(0x64f8u+static_cast<u32>(bucket)*3u)))<=lum)++bucket;--bucket;*dgroup_ptr(m,dgroup(0x5af8u+i))=static_cast<u8>(bucket);}
 for(s32 row=1;row<5;++row)for(u32 i=0;i<0x100u;++i){const u32 src=*dgroup_ptr(m,dgroup(0x5af8u+i));const s32 red=*dgroup_ptr(m,dgroup(0x64f8u+src*3u));const s32 target=red+div4_toward_zero((63-red)*row+2);s32 bucket=0;while(bucket<0x1c&&static_cast<s32>(*dgroup_ptr(m,dgroup(0x64f8u+static_cast<u32>(bucket)*3u)))<=target)++bucket;--bucket;*dgroup_ptr(m,dgroup(0x5af8u+static_cast<u32>(row)*0x100u+i))=static_cast<u8>(bucket);}
 if(b.call_cbd4)b.call_cbd4(0x5af8u);
}
static_assert(0x4C7Fu-0x4B18u==359u); // 4C7F NOP alignment.

// 4C80..4CAF: seek to index*2 and read a 16-bit value into the low word of the handle argument slot.
[[nodiscard]]u32 function_4c80(PostActorBoundaries&b,u32 index,u32 handle)noexcept{u32 result=handle;if(b.seek_handle)(void)b.seek_handle(index*2u,0u,handle);const u16 word=b.read_u16_handle?b.read_u16_handle(handle):0u;set_low16(result,word);return result;}
static_assert(0x4CB0u-0x4C80u==48u);

// 4CB0..4D50: open resource 6C, read indices 0/2/0B into 7134/713C/712C, then open 74
// and retain that second handle at 04C8 after the B74C/C650 setup calls.
void function_4cb0(Machine&m,PostActorBoundaries&b)noexcept{
 s32 h=b.open_id?b.open_id(0x6cu):-1;if(h==-1&&b.fatal_9814)b.fatal_9814(load_le32(dgroup_ptr(m,dgroup(0x04bcu))));
 store_le32(dgroup_ptr(m,dgroup(0x7134u)),function_4c80(b,0u,static_cast<u32>(h)));store_le32(dgroup_ptr(m,dgroup(0x713cu)),function_4c80(b,2u,static_cast<u32>(h)));store_le32(dgroup_ptr(m,dgroup(0x712cu)),function_4c80(b,0x0bu,static_cast<u32>(h)));if(b.close_handle)b.close_handle(static_cast<u32>(h));
 h=b.open_id?b.open_id(0x74u):-1;if(h==-1&&b.fatal_9814)b.fatal_9814(load_le32(dgroup_ptr(m,dgroup(0x04c0u))));if(b.call_b74c)b.call_b74c(5u);if(b.call_c650)b.call_c650(6u);store_le32(dgroup_ptr(m,dgroup(0x04c8u)),static_cast<u32>(h));
}
static_assert(0x4D51u-0x4CB0u==161u); // 4D51..4D53 LEA alignment.

// 4D54..4DC3: load resource 78. The first dword is a record count; nonzero counts allocate
// count*65 bytes at 7128, read the payload, and close the file. A zero count leaves the handle open exactly as encoded.
void function_4d54(Machine&m,PostActorBoundaries&b)noexcept{s32 h=b.open_id?b.open_id(0x78u):-1;const u32 count=b.read_u32_handle?b.read_u32_handle(static_cast<u32>(h)):0u;store_le32(dgroup_ptr(m,dgroup(0x7120u)),count);if(count==0u)return;const u32 bytes=count*65u;const u32 dst=b.alloc_71c8?b.alloc_71c8(bytes):0u;store_le32(dgroup_ptr(m,dgroup(0x7128u)),dst);if(b.read_guest)b.read_guest(m,dst,bytes,static_cast<u32>(h));if(b.close_handle)b.close_handle(static_cast<u32>(h));}
static_assert(0x4DC4u-0x4D54u==112u);

// 4DC4..4EED: resource/video bootstrap for the character generator, including palette-table
// construction and the four 0x248-byte actor records initialized at 67F8.
void function_4dc4(Machine&m,PostActorBoundaries&b)noexcept{
 if(b.call_95bc)b.call_95bc(0x84u,0x0bu);if(b.call_9724)b.call_9724(0x88u,1u,0x3cu);function_4d54(m,b);function_4cb0(m,b);if(b.call_64dc)b.call_64dc(3u);if(b.call_c254)b.call_c254(0x64f8u,0,8,0x64,0x100);if(b.call_6618)b.call_6618(0x64f8u);if(b.call_9ac4)b.call_9ac4();function_4b18(m,b);function_4b00(m);function_4a0c(m,b);if(b.call_84a0)b.call_84a0(0x64f8u,0u,0x300u);if(b.call_6618)b.call_6618(0x64f8u);if(b.call_c254)b.call_c254(0x64f8u,0,8,0x64,0x100);
 store_le32(dgroup_ptr(m,dgroup(0x7138u)),1u);if(b.call_6588)b.call_6588(1u);if(b.call_65a4)b.call_65a4(0u);function_52ac(m,b);if(b.call_84a0)b.call_84a0(0x67f8u,0xffffffffu,0x920u);for(u32 actor=0;actor<4u;++actor){const u32 base=actor*0x248u;*dgroup_ptr(m,dgroup(base+0x6809u))|=0x80u;for(u32 i=0;i<4u;++i)store_le32(dgroup_ptr(m,dgroup(base+0x6903u+i*4u)),0u);}
}
static_assert(0x4EEEu-0x4DC4u==298u); // 4EEE..4EEF MOV EAX,EAX alignment.

// 4EF0..5013: finalize the first two actor records, duplicate primary stat bytes into their
// paired slots, synthesize class flags at 6929, then write the 920-byte actor block through resource 94.
void function_4ef0(Machine&m,PostActorBoundaries&b)noexcept{
 for(u32 actor=0;actor<2u;++actor){const u32 base=actor*0x248u;const u32 start=base+0x67f8u+0x10bu;const u32 end=(actor+1u)*0x248u+0x67f8u;if(b.call_84a0)b.call_84a0(start,0u,end-start);*dgroup_ptr(m,dgroup(base+0x6813u))=0x0au;store_le16(dgroup_ptr(m,dgroup(base+0x6814u)),load_le16(dgroup_ptr(m,dgroup(base+0x6816u))));for(u32 i=0;i<7u;++i)*dgroup_ptr(m,dgroup(base+0x682bu+i*2u))=*dgroup_ptr(m,dgroup(base+0x682au+i*2u));u8 flags=0;for(u32 i=0;i<3u;++i){const u8 c=*dgroup_ptr(m,dgroup(base+0x680du+i));if(c==3u)flags|=1u;if(c==4u)flags|=2u;}*dgroup_ptr(m,dgroup(base+0x6929u))=flags;}
 if(b.call_63a4)b.call_63a4(0x94u);u32 h=b.call_62d8?b.call_62d8(0x94u):0u;if(b.close_handle)b.close_handle(h);h=b.call_62c0?b.call_62c0(0x94u):0u;if(b.call_6320)b.call_6320(0x67f8u,0x920u,h);if(b.close_handle)b.close_handle(h);
}
static_assert(0x5014u-0x4EF0u==292u);

// 5014..5089: shutdown/exit dispatcher. Code 0B is normalized to 0A; code 0A with both
// actor slots populated executes 5F4C and 4EF0 before the shared CBF0 termination tail.
void function_5014(Machine&m,PostActorBoundaries&b,u32 code)noexcept{if(b.call_bb80)b.call_bb80(0x9c4u);if(b.call_bcb4)b.call_bcb4();if(code!=0x0bu){if(b.close_handle)b.close_handle(load_le32(dgroup_ptr(m,dgroup(0x04c8u))));function_538c(m,b);if(b.call_9778)b.call_9778();}else code=0x0au;if(code!=0x0au){if(b.call_64dc)b.call_64dc(0xffu);}else if(*dgroup_ptr(m,dgroup(0x04c4u))&&*dgroup_ptr(m,dgroup(0x04c5u))){if(b.call_5f4c)b.call_5f4c();function_4ef0(m,b);}if(b.terminate_cbf0)b.terminate_cbf0(code);}
static_assert(0x508Au-0x5014u==118u); // 508A..508B MOV EAX,EAX alignment.

// 508C..50BF: entry wrapper. Non-mode-2 invocation reports resource 9C then terminates with 1;
// mode 2 performs bootstrap, runs the controller, and forwards its result to 5014.
void function_508c(Machine&m,ActorStatsBoundaries&stats,PostActorBoundaries&b,u32 mode)noexcept{if(mode!=2u){if(b.call_61d0)b.call_61d0(0x9cu);if(b.terminate_cbf0)b.terminate_cbf0(1u);return;}function_4dc4(m,b);function_5014(m,b,static_cast<u32>(function_4978(m,stats,b)));}
static_assert(0x50C0u-0x508Cu==52u);

// 50C0..522E: cursor-aware rectangle update. Optional 84E0 scaling rewrites the X/Y arguments;
// when the mouse depth is active and coordinates change, the four 2401/03/05/07 words are saved,
// a full-screen rectangle cycle is performed, and those words are restored.
void function_50c0(Machine&m,PostActorBoundaries&b,u32, u16 tag,s32 y,s32 x,const s16 point[2])noexcept{
 y>>=1;if(b.call_84e0&&b.call_84e0()!=0u){x=static_cast<s32>((static_cast<std::int64_t>(point[1]+0x1fc0)*399+0x1fa0)/0x3f40);y=static_cast<s32>((static_cast<std::int64_t>(point[0]+0x1fc0)*319+0x1fa0)/0x3f40);}
 if(load_le16(dgroup_ptr(m,dgroup(0x73ecu)))>=1u&&load_le32(dgroup_ptr(m,dgroup(0x097cu)))==0u){if(static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e6u))))!=x||static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e8u))))!=y){store_le16(dgroup_ptr(m,dgroup(0x71d8u)),static_cast<u16>(x));store_le16(dgroup_ptr(m,dgroup(0x71d6u)),static_cast<u16>(y));store_le16(dgroup_ptr(m,dgroup(0x71d4u)),tag);for(u32 i=0;i<4u;++i)store_le16(dgroup_ptr(m,dgroup(0x7188u+i*2u)),load_le16(dgroup_ptr(m,dgroup(0x2401u+i*2u))));if(b.call_6c44)b.call_6c44(0x7178u);if(b.call_6bc0)b.call_6bc0(0,0,0x18f,0x13f);if(b.call_5780)b.call_5780();if(b.call_57dc)b.call_57dc();if(b.call_6bc0)b.call_6bc0(static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x7178u)))),static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x7180u)))),static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x717cu)))),static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x7184u)))));for(u32 i=0;i<4u;++i)store_le16(dgroup_ptr(m,dgroup(0x2401u+i*2u)),load_le16(dgroup_ptr(m,dgroup(0x7188u+i*2u))));}}
 store_le16(dgroup_ptr(m,dgroup(0x71d4u)),tag);
}
static_assert(0x522Fu-0x50C0u==367u); // 522F NOP alignment.

// 5230..5279: build the 33h callback descriptor at 71AC/71B4/71B8/71C8 and invoke CEC8.
void function_5230(Machine&m,PostActorBoundaries&b,u16 value)noexcept{store_le16(dgroup_ptr(m,dgroup(0x71acu)),0x0cu);store_le16(dgroup_ptr(m,dgroup(0x71b4u)),value);store_le32(dgroup_ptr(m,dgroup(0x71b8u)),0x6104u);store_le16(dgroup_ptr(m,dgroup(0x71c8u)),m.cpu.cs);if(b.call_cec8)b.call_cec8(0x33u,0x71acu,0x7190u,0x71c8u);}
static_assert(0x527Au-0x5230u==74u); // 527A..527B MOV EAX,EAX alignment.

// 527C..52A9: copy cursor X/Y/tag globals to three caller-supplied guest words.
void function_527c(Machine&m,u32 out_x,u32 out_y,u32 out_tag)noexcept{store_le16(dgroup_ptr(m,dgroup(out_x)),load_le16(dgroup_ptr(m,dgroup(0x71d8u))));store_le16(dgroup_ptr(m,dgroup(out_y)),load_le16(dgroup_ptr(m,dgroup(0x71d6u))));store_le16(dgroup_ptr(m,dgroup(out_tag)),load_le16(dgroup_ptr(m,dgroup(0x71d4u))));}
static_assert(0x52AAu-0x527Cu==46u); // 52AA..52AB MOV EAX,EAX alignment.

// 52AC..538B: initialize mouse/cursor state, copy the 20A-byte cursor image to 71DC,
// and install the 33h callback only when 84D0 reports a nonzero mouse capability.
void function_52ac(Machine&m,PostActorBoundaries&b)noexcept{const u16 cap=b.call_84d0?b.call_84d0():0u;store_le16(dgroup_ptr(m,dgroup(0x71dau)),cap);if(cap==0u)return;if(b.call_d130)b.call_d130(0,0x18f);if(b.call_d10c)b.call_d10c(0,0x27e);if(b.call_d08c)b.call_d08c(0xc8,0xa0);const auto snap=b.call_d0b4?b.call_d0b4():PostActorBoundaries::MouseSnapshot{};store_le16(dgroup_ptr(m,dgroup(0x71d8u)),snap.x);store_le16(dgroup_ptr(m,dgroup(0x73e6u)),snap.x);store_le16(dgroup_ptr(m,dgroup(0x71d6u)),snap.y);store_le16(dgroup_ptr(m,dgroup(0x73e8u)),snap.y);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),0);store_le16(dgroup_ptr(m,dgroup(0x71d4u)),0);store_le16(dgroup_ptr(m,dgroup(0x73eau)),0);*dgroup_ptr(m,dgroup(0x73eeu))=*dgroup_ptr(m,dgroup(0x23e6u));const u32 image=b.call_954c?b.call_954c(load_le32(dgroup_ptr(m,dgroup(0x7134u)))+0x2fu,0u):0u;if(b.call_d1e8)b.call_d1e8(0x71dcu,image,0x20au);if(b.call_6bc0)b.call_6bc0(0,0,0x18f,0x13f);function_5230(m,b,0xffffu);}
static_assert(0x538Cu-0x52ACu==224u);

// 538C..53B0: cursor shutdown wrapper.
void function_538c(Machine&m,PostActorBoundaries&b)noexcept{if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;if(load_le16(dgroup_ptr(m,dgroup(0x73ecu)))!=0u&&b.call_5780)b.call_5780();if(b.call_cf98)b.call_cf98();}
static_assert(0x53B1u-0x538Cu==37u); // 53B1..53B3 LEA alignment.

// 53B4..53E1: poll event state; the low mouse-status word is mirrored at 73EA and sign-extended.
[[nodiscard]]s32 function_53b4(Machine&m,PostActorBoundaries&b)noexcept{if(b.call_9ae8)b.call_9ae8();if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return 0;const auto snap=b.call_d0b4?b.call_d0b4():PostActorBoundaries::MouseSnapshot{};store_le16(dgroup_ptr(m,dgroup(0x73eau)),snap.buttons);return static_cast<s16>(snap.buttons);}
static_assert(0x53E2u-0x53B4u==46u); // 53E2..53E3 MOV EAX,EAX alignment.

// 53E4..5450: page switch with cursor suppression/restoration.
void function_53e4(Machine&m,PostActorBoundaries&b,u32 page)noexcept{if(b.call_9ae8)b.call_9ae8();if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;store_le32(dgroup_ptr(m,dgroup(0x097cu)),1u);*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),0);if(b.call_57dc)b.call_57dc();if(b.call_65a4)b.call_65a4(static_cast<u8>(page));*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page^1u);if(b.call_5780)b.call_5780();*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),1);store_le32(dgroup_ptr(m,dgroup(0x097cu)),0u);}
static_assert(0x5451u-0x53E4u==109u); // 5451..5453 LEA alignment.

// 5454..550A: page flip plus full 400-byte cursor backing-buffer copy.
void function_5454(Machine&m,PostActorBoundaries&b,u32 page)noexcept{if(b.call_9ae8)b.call_9ae8();if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;store_le32(dgroup_ptr(m,dgroup(0x097cu)),1u);*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),0);if(b.call_57dc)b.call_57dc();if(b.call_65a4)b.call_65a4(static_cast<u8>(page));if(b.call_8740)b.call_8740(page);u32 other=page;set_low8(other,static_cast<u8>(low8(other)^1u));if(b.call_a390)b.call_a390(other);*dgroup_ptr(m,dgroup(0x73eeu))=low8(other);if(b.call_d1e8)b.call_d1e8(0x73efu+(other<<10),0x73efu+(page<<10),0x400u);if(b.call_5780)b.call_5780();if(b.call_8788)b.call_8788();*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),1);store_le32(dgroup_ptr(m,dgroup(0x097cu)),0u);}
static_assert(0x550Bu-0x5454u==183u); // 550B NOP alignment.

// 550C..55DA: region-aware page flip; A3E8 receives (page^1,x,y,x,y,w,h), then the
// complete 400-byte backing page is copied before cursor restoration.
void function_550c(Machine&m,PostActorBoundaries&b,u32 page,s32 x,s32 y,s32 w,s32 h)noexcept{if(b.call_9ae8)b.call_9ae8();if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;store_le32(dgroup_ptr(m,dgroup(0x097cu)),1u);*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),0);if(b.call_57dc)b.call_57dc();if(b.call_65a4)b.call_65a4(static_cast<u8>(page));if(b.call_8740)b.call_8740(page);u32 other=page;set_low8(other,static_cast<u8>(low8(other)^1u));if(b.call_a3e8)b.call_a3e8(other,x,y,x,y,w,h);*dgroup_ptr(m,dgroup(0x73eeu))=low8(other);if(b.call_d1e8)b.call_d1e8(0x73efu+(other<<10),0x73efu+(page<<10),0x400u);if(b.call_5780)b.call_5780();if(b.call_8788)b.call_8788();*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),1);store_le32(dgroup_ptr(m,dgroup(0x097cu)),0u);}
static_assert(0x55DBu-0x550Cu==207u); // 55DB NOP alignment.

// 55DC..56DB: rectangle update that avoids disturbing the cursor when the 32x16 cursor box
// is disjoint; overlap uses the saved-page/depth path and copies the whole 400-byte backing buffer.
void function_55dc(Machine&m,PostActorBoundaries&b,u32 page,s32 x,s32 y,s32 w,s32 h)noexcept{if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;store_le32(dgroup_ptr(m,dgroup(0x097cu)),1u);const s32 cx=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e6u)))),cy=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e8u))));const bool disjoint=(cx+0x20<x)||(cx>x+w)||(cy+0x10<y)||(cy>y+h);u32 other=page;set_low8(other,static_cast<u8>(low8(other)^1u));if(disjoint){if(b.call_a3e8)b.call_a3e8(other,x,y,x,y,w,h);}else{const u8 saved=*dgroup_ptr(m,dgroup(0x73eeu));*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),static_cast<u16>(load_le16(dgroup_ptr(m,dgroup(0x73ecu)))-1u));if(b.call_57dc)b.call_57dc();if(b.call_a3e8)b.call_a3e8(other,x,y,x,y,w,h);if(b.call_d1e8)b.call_d1e8(0x73efu+(other<<10),0x73efu+(page<<10),0x400u);if(b.call_5780)b.call_5780();*dgroup_ptr(m,dgroup(0x73eeu))=saved;store_le16(dgroup_ptr(m,dgroup(0x73ecu)),static_cast<u16>(load_le16(dgroup_ptr(m,dgroup(0x73ecu)))+1u));}store_le32(dgroup_ptr(m,dgroup(0x097cu)),0u);}
static_assert(0x56DCu-0x55DCu==256u);

// 56DC..577C: full backing-page switch driven by byte 23E6; depth is incremented after restore.
void function_56dc(Machine&m,PostActorBoundaries&b)noexcept{if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;const u8 saved=*dgroup_ptr(m,dgroup(0x73eeu));store_le32(dgroup_ptr(m,dgroup(0x097cu)),1u);const u8 active=*dgroup_ptr(m,dgroup(0x23e6u));if(b.call_8740)b.call_8740(active);if(b.call_a390)b.call_a390(active^1u);*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(active^1u);if(b.call_d1e8)b.call_d1e8(0x73efu+static_cast<u32>(*dgroup_ptr(m,dgroup(0x73eeu)))*0x400u,0x73efu+static_cast<u32>(saved)*0x400u,0x400u);if(b.call_5780)b.call_5780();if(b.call_8788)b.call_8788();*dgroup_ptr(m,dgroup(0x73eeu))=saved;store_le32(dgroup_ptr(m,dgroup(0x097cu)),0u);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),static_cast<u16>(load_le16(dgroup_ptr(m,dgroup(0x73ecu)))+1u));}
static_assert(0x577Du-0x56DCu==161u); // 577D..577F LEA alignment.

// 5780..57DA: decrement cursor nesting depth; on transition to zero restore the saved 32x16 rectangle.
void function_5780(Machine&m,PostActorBoundaries&b)noexcept{if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;const u16 depth=static_cast<u16>(load_le16(dgroup_ptr(m,dgroup(0x73ecu)))-1u);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),depth);if(depth!=0u)return;const u8 page=*dgroup_ptr(m,dgroup(0x73eeu));if(b.call_8740)b.call_8740(page);if(b.call_6744)b.call_6744(0x73efu+static_cast<u32>(page)*0x400u,static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e6u)))),static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e8u)))),0x20u,0x10u);if(b.call_8788)b.call_8788();}
static_assert(0x57DBu-0x5780u==91u); // 57DB NOP alignment.

// 57DC..5878: increment cursor nesting depth. At depth zero, capture the underlying 32x16
// rectangle to the current backing page and draw the cursor resource at 71DC before incrementing.
void function_57dc(Machine&m,PostActorBoundaries&b)noexcept{if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;u16 depth=load_le16(dgroup_ptr(m,dgroup(0x73ecu)));if(depth==0u){const u8 page=*dgroup_ptr(m,dgroup(0x73eeu));if(b.call_8740)b.call_8740(page);function_527c(m,0x73e6u,0x73e8u,0x73eau);const s32 x=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e6u)))),y=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e8u))));if(b.call_66c0)b.call_66c0(0x73efu+static_cast<u32>(page)*0x400u,x,y,0x20u,0x10u);if(b.call_6d54)b.call_6d54(0,x,y,0x71dcu);if(b.call_8788)b.call_8788();}store_le16(dgroup_ptr(m,dgroup(0x73ecu)),static_cast<u16>(depth+1u));}
static_assert(0x5879u-0x57DCu==157u);
// 5879..587F are seven zero bytes and are data/alignment.

// 5880..59AB: insert one 9-byte palette-search record into the binary tree rooted through
// record zero. Components rotate 0->1->2 while descending; word links +5/+7 use 0101h as sentinel.
void function_5880(Machine&m,const u8 packed[9],s16 index)noexcept{const s32 idx=index;const u32 rec=0x7ff0u+static_cast<u32>(idx*9);*dgroup_ptr(m,dgroup(rec+1u))=packed[1];*dgroup_ptr(m,dgroup(rec))=packed[0];*dgroup_ptr(m,dgroup(rec+2u))=packed[2];store_le16(dgroup_ptr(m,dgroup(rec+3u)),load_le16(packed+3));store_le16(dgroup_ptr(m,dgroup(rec+7u)),0x0101u);store_le16(dgroup_ptr(m,dgroup(rec+5u)),0x0101u);s16 node=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x7ff7u)))),parent=0,component=-1;bool right=true;while(node!=0x0101){parent=node;component=(component==2)?0:static_cast<s16>(component+1);right=false;const u32 base=0x7ff0u+static_cast<u32>(static_cast<s32>(node)*9);if(packed[component]>=*dgroup_ptr(m,dgroup(base+static_cast<u32>(component)))){node=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(base+7u))));right=true;}else node=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(base+5u))));}const u32 pbase=0x7ff0u+static_cast<u32>(static_cast<s32>(parent)*9);store_le16(dgroup_ptr(m,dgroup(pbase+(right?7u:5u))),static_cast<u16>(index));}
static_assert(0x59ACu-0x5880u==300u);


struct PaletteTreeBoundaries{
 u32(*read_dgroup32)(u32)=nullptr;
 void(*insert_5880)(u8,u8,u8,u16,s32)=nullptr;
 u8(*read_guest8)(u32)=nullptr; void(*write_guest8)(u32,u8)=nullptr;
 void(*dos_print_dollar_guest)(u32)=nullptr;
 s32(*call_d23a)(u32,u16)=nullptr; s32(*call_d280)(u32,u16,u32)=nullptr; s32(*call_d2ad)(u32)=nullptr;
 u32(*call_d313)(u32,u32)=nullptr; u32(*call_d55e)(u32,u32)=nullptr; u32(*call_d573)(u32)=nullptr;
 s32(*call_d5ad)(u32,void*,u32)=nullptr; s32(*call_d686)(u32,const void*,u32)=nullptr; s32(*call_d85f)(u32,s32,u32)=nullptr;
 s32(*call_d8aa)(const char*)=nullptr; u32(*call_d8e8)(u32,u32*)=nullptr; u32(*call_d8d0)(u32*)=nullptr;
 u32(*call_d906_guest)(u32)=nullptr; u32(*call_d991)(u32)=nullptr; s32(*call_d9a6)(const char*)=nullptr; s32(*call_d9ca)(const char*)=nullptr;
 void(*call_a2b0)(s32,s32)=nullptr; void(*call_a21c)(s32)=nullptr; void(*call_a484)(u32,s32,s32,s32,s32)=nullptr;
 void(*call_a4e8)(u32,s32,s32,s32,s32)=nullptr; void(*call_a6f0)(u32,s32,s32,u32)=nullptr;
 void(*call_a7e8)(u32,s32,s32,u32,u8)=nullptr; void(*call_a8f4)(u32,s32,s32,u32,u32)=nullptr; void(*call_8960)()=nullptr;
};

// 59AC..5A33: scan 256 RGB triples and return the triple index nearest to (31,31,31).
// MOVZX targets AX, not EAX: the upper 16 bits of the incoming guest address survive each byte load.
[[nodiscard]]s32 function_59ac(PaletteTreeBoundaries&b,u32 src)noexcept{
 s32 best_index=0; s16 best=0x200; s32 bx=0; const u32 high=src&0xffff0000u;
 while(static_cast<s16>(bx)<0x300){const s32 off=static_cast<s16>(bx);const u32 a=high|(b.read_guest8?b.read_guest8(src+static_cast<u32>(off)):0u);const u32 c1=high|(b.read_guest8?b.read_guest8(src+static_cast<u32>(off+1)):0u);const u32 c2=high|(b.read_guest8?b.read_guest8(src+static_cast<u32>(off+2)):0u);const s32 da=static_cast<s32>(a)-31,db=static_cast<s32>(c1)-31,dc=static_cast<s32>(c2)-31;const s32 dist=da*da+db*db+dc*dc;if(static_cast<s16>(dist)<best){best=static_cast<s16>(dist);best_index=off/3;}bx+=3;}return best_index;
}
static_assert(0x5A34u-0x59ACu==136u);

// 5A34..5B63: initialize sentinels, choose the closest palette entry, then insert downward and upward.
void function_5a34(Machine&m,PaletteTreeBoundaries&b,u32 src)noexcept{
 store_le16(dgroup_ptr(m,dgroup(0x7ff3u)),0xffffu);store_le16(dgroup_ptr(m,dgroup(0x7ff5u)),0x0101u);store_le16(dgroup_ptr(m,dgroup(0x7ff7u)),0x0101u);store_le16(dgroup_ptr(m,dgroup(0x88fcu)),0xffffu);store_le16(dgroup_ptr(m,dgroup(0x88feu)),0x0101u);store_le16(dgroup_ptr(m,dgroup(0x8900u)),0x0101u);s32 serial=1;const s32 lo=function_59ac(b,src);s32 hi=lo+1;auto ins=[&](s32 idx){const u32 o=static_cast<u32>(idx*3);if(b.insert_5880)b.insert_5880(b.read_guest8?b.read_guest8(src+o):0u,b.read_guest8?b.read_guest8(src+o+1u):0u,b.read_guest8?b.read_guest8(src+o+2u):0u,static_cast<u16>(idx),serial);++serial;};for(s32 i=lo;i>-1;--i)ins(i);while(static_cast<s16>(hi)<0x100){ins(hi);++hi;}store_le16(dgroup_ptr(m,dgroup(0x982u)),1u);
}
static_assert(0x5B64u-0x5A34u==304u);

static s16 tree_word(Machine&m,s16 node,u32 off)noexcept{return static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x7ff0u+static_cast<u32>(static_cast<s32>(node)*9)+off))));}
static u8 tree_byte(Machine&m,s16 node,u32 off)noexcept{return *dgroup_ptr(m,dgroup(0x7ff0u+static_cast<u32>(static_cast<s32>(node)*9)+off));}
// 5B64..5D8E: exact iterative nearest-tree search. The explicit 16-bit stack/count globals and 0x101 sentinel are retained.
// Final audit: the return deliberately overwrites AX only, preserving EAX upper bits from the sign-extended node index.
[[nodiscard]]s32 function_5b64(Machine&m,u8 r,u8 g,u8 bl,u32 radius)noexcept{
 s32 best_dist=0x2e83; s16 best_node=0x0101;
 if(load_le16(dgroup_ptr(m,dgroup(0x982u)))==0u)return -1;
 store_le16(dgroup_ptr(m,dgroup(0x980u)),0u);store_le16(dgroup_ptr(m,dgroup(0x984u)),0u);
 s16 bounds[6]={static_cast<s16>(static_cast<u16>(g)-radius),static_cast<s16>(static_cast<u16>(g)+radius),static_cast<s16>(static_cast<u16>(r)-radius),static_cast<s16>(static_cast<u16>(r)+radius),static_cast<s16>(static_cast<u16>(bl)-radius),static_cast<s16>(static_cast<u16>(bl)+radius)};
 store_le16(dgroup_ptr(m,dgroup(0x7bf0u)),0xffffu);store_le16(dgroup_ptr(m,dgroup(0x980u)),1u);store_le16(dgroup_ptr(m,dgroup(0x7bf2u)),0u);store_le16(dgroup_ptr(m,dgroup(0x980u)),2u);
 for(;;){
  s16 sp=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x980u))));if(sp==0||static_cast<s16>(best_dist)==0)break;
  --sp;store_le16(dgroup_ptr(m,dgroup(0x980u)),static_cast<u16>(sp));s16 popped=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x7bf0u+static_cast<u32>(sp)*2u))));s16 node=tree_word(m,popped,7u);
  --sp;store_le16(dgroup_ptr(m,dgroup(0x980u)),static_cast<u16>(sp));s16 axis=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x7bf0u+static_cast<u32>(sp)*2u))));
  while(node!=0x0101&&static_cast<s16>(best_dist)!=0){
   store_le16(dgroup_ptr(m,dgroup(0x984u)),static_cast<u16>(load_le16(dgroup_ptr(m,dgroup(0x984u)))+1u));axis=(axis==2)?0:static_cast<s16>(axis+1);
   const s16 value=static_cast<s16>(tree_byte(m,node,static_cast<u32>(axis)));
   if(value<=bounds[axis*2]){node=tree_word(m,node,7u);continue;}
   if(value>bounds[axis*2+1]){node=tree_word(m,node,5u);continue;}
   sp=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x980u))));store_le16(dgroup_ptr(m,dgroup(0x7bf0u+static_cast<u32>(sp)*2u)),static_cast<u16>(axis));++sp;store_le16(dgroup_ptr(m,dgroup(0x980u)),static_cast<u16>(sp));store_le16(dgroup_ptr(m,dgroup(0x7bf0u+static_cast<u32>(sp)*2u)),static_cast<u16>(node));++sp;store_le16(dgroup_ptr(m,dgroup(0x980u)),static_cast<u16>(sp));
   const s32 dr=static_cast<s32>(tree_byte(m,node,1u))-r,dg=static_cast<s32>(tree_byte(m,node,0u))-g,db=static_cast<s32>(tree_byte(m,node,2u))-bl;const s32 dist=dr*dr+dg*dg+db*db;
   if(static_cast<s16>(dist)<static_cast<s16>(best_dist)){best_dist=dist;best_node=node;}
   node=tree_word(m,node,5u);
  }
 }
 u32 result=static_cast<u32>(static_cast<s32>(best_node));set_low16(result,load_le16(dgroup_ptr(m,dgroup(0x7ff0u+static_cast<u32>(static_cast<s32>(best_node)*9)+3u))));return static_cast<s32>(result);
}
static_assert(0x5D8Fu-0x5B64u==555u);
// 5D8F is zero alignment; 5D90 is the true next entry.

// 5D90..5F49: map an actor's class tuple to the encoded class-family byte. BL is intentionally an input when no condition overwrites it.
[[nodiscard]]u8 function_5d90(Machine&m,u32 actor,u8 incoming_bl)noexcept{
 const u32 base=actor*0x248u;const std::int8_t primary=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(base+0x680du)));const std::int8_t second=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(base+0x680eu)));u8 out=incoming_bl;
 if(second==-1)return static_cast<u8>(primary);
 const std::int8_t third=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(base+0x680fu)));
 if(third!=-1){if(primary==5||second==5||third==5)return 0x0d;return 0x0e;}
 if(primary==0){if(second==5)out=6;if(second==3)out=7;if(second==4)out=8;}
 if(primary==5){if(second==0)out=6;if(second==3)out=9;if(second==4)out=0x0c;}
 if(primary==2)out=0x0a;
 if(primary==3){if(second==0)out=7;if(second==5)out=9;if(second==4)out=0x0b;if(second==2)out=0x0a;}
 if(primary==4){if(second==0)out=8;if(second==3)out=0x0b;if(second==5)out=0x0c;}
 return out;
}
static_assert(0x5F4Au-0x5D90u==442u);
// 5F4A..5F4B is alignment.

// 5F4C..6053: populate both actor runtime tables from class-family/class-index records and invalidate conflicting 6A80 entries.
// Final audit: the second family probe inherits BL=2, then the first family result; negative 6A80 words are not prefiltered.
void function_5f4c(Machine&m,PaletteTreeBoundaries&b)noexcept{
 for(u32 actor=0;actor<2u;++actor){const u8 family=function_5d90(m,actor,static_cast<u8>(actor));const u32 abase=actor*0x248u;const u8 cls=*dgroup_ptr(m,dgroup(abase+0x680cu));const u32 rec=static_cast<u32>(family)*0xa8u+static_cast<u32>(cls)*0x1cu;const u32 q4[4]={0x98cu,0x98eu,0x98au,0x988u};for(u32 j=0;j<4u;++j)store_le16(dgroup_ptr(m,dgroup(abase+0x6854u+j*2u)),load_le16(dgroup_ptr(m,dgroup(rec+q4[j]))));for(u32 j=0;j<10u;++j)store_le16(dgroup_ptr(m,dgroup(abase+0x6838u+j*2u)),load_le16(dgroup_ptr(m,dgroup(rec+0x990u+j*2u))));}
 {const u8 family0=function_5d90(m,0,2u);const u8 family1=function_5d90(m,1,family0);if(family0==family1&&*dgroup_ptr(m,dgroup(0x680cu))==*dgroup_ptr(m,dgroup(0x6a54u))){for(u32 i=0;i<10u;++i){const s16 v=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x6a80u+i*2u))));const u32 q=load_le32(dgroup_ptr(m,dgroup(0x7128u)))+static_cast<u32>(v)*65u+0x2fu;if(b.read_guest8&&b.read_guest8(q)==1u)store_le16(dgroup_ptr(m,dgroup(0x6a80u+i*2u)),0xffffu);}}}
}
static_assert(0x6054u-0x5F4Cu==264u);

// 6184..6189 and 618A..61AB: RNG state address and MS-compatible LCG step.
[[nodiscard]]u32 function_6184()noexcept{return 0x23ccu;} static_assert(0x618Au-0x6184u==6u);
[[nodiscard]]u32 function_618a(Machine&m)noexcept{u32 v=load_le32(dgroup_ptr(m,dgroup(0x23ccu)));v=v*0x41c64e6du+0x3039u;store_le32(dgroup_ptr(m,dgroup(0x23ccu)),v);return(v>>16)&0x7fffu;} static_assert(0x61ACu-0x618Au==34u);
void function_61ac(Machine&m,u32 seed)noexcept{store_le32(dgroup_ptr(m,dgroup(0x23ccu)),seed);} static_assert(0x61BFu-0x61ACu==19u);
// 61BF is zero alignment.
[[nodiscard]]u32 function_61c0(Machine&m)noexcept{return load_le32(dgroup_ptr(m,dgroup(0x46cu)));} static_assert(0x61CEu-0x61C0u==14u);
// 61CE..61CF is alignment.

// 61D0..61FC: DOS AH=09 string printer; embedded '$' bytes are changed to '*' and the terminating NUL is temporarily changed to '$'.
void function_61d0(PaletteTreeBoundaries&b,u32 s)noexcept{u32 p=s;for(;;){const u8 ch=b.read_guest8?b.read_guest8(p):0u;if(ch==0u)break;if(ch=='$'&&b.write_guest8)b.write_guest8(p,'*');++p;}if(b.write_guest8)b.write_guest8(p,'$');if(b.dos_print_dollar_guest)b.dos_print_dollar_guest(s);if(b.write_guest8)b.write_guest8(p,0u);} static_assert(0x61FDu-0x61D0u==45u);
// 61FD..61FF alignment.
[[nodiscard]]u32 function_6200(PaletteTreeBoundaries&b,u32 path,u16 flags)noexcept{return(b.call_d23a?b.call_d23a(path,flags):-1)==0?1u:0u;} static_assert(0x6220u-0x6200u==32u);
[[nodiscard]]u32 function_6220(PaletteTreeBoundaries&b,u32 path,u16 attr)noexcept{return(b.call_d280&&b.call_d280(path,attr,0x8904u)==0)?0x8922u:0u;} static_assert(0x624Cu-0x6220u==44u);
[[nodiscard]]u32 function_624c(PaletteTreeBoundaries&b)noexcept{return(b.call_d2ad&&b.call_d2ad(0x8904u)==0)?0x8922u:0u;} static_assert(0x626Fu-0x624Cu==35u);
// 626F is NOP alignment.
[[nodiscard]]u8 function_6270(Machine&m)noexcept{return*dgroup_ptr(m,dgroup(0x8919u));} static_assert(0x627Cu-0x6270u==12u);
[[nodiscard]]s32 function_627c(PaletteTreeBoundaries&b,u32 path)noexcept{if(b.call_d280&&b.call_d280(path,0u,0x8904u)==0)return static_cast<s32>(b.read_dgroup32?b.read_dgroup32(0x891eu):0u);return -1;} // patched below: guest 891E is supplied by caller boundary
static_assert(0x62A8u-0x627Cu==44u);
[[nodiscard]]u32 function_62a8(PaletteTreeBoundaries&b,u32 path)noexcept{return b.call_d313?b.call_d313(path,0x200u):0xffffffffu;} static_assert(0x62C0u-0x62A8u==24u);
[[nodiscard]]u32 function_62c0(PaletteTreeBoundaries&b,u32 path)noexcept{return b.call_d313?b.call_d313(path,0x202u):0xffffffffu;} static_assert(0x62D8u-0x62C0u==24u);
[[nodiscard]]u32 function_62d8(PaletteTreeBoundaries&b,u32 path)noexcept{return b.call_d55e?b.call_d55e(path,0x180u):0xffffffffu;} static_assert(0x62F0u-0x62D8u==24u);
[[nodiscard]]u32 function_62f0(PaletteTreeBoundaries&b,u32 h)noexcept{return b.call_d573?b.call_d573(h):0xffffffffu;} static_assert(0x6303u-0x62F0u==19u);
// 6303 alignment.
[[nodiscard]]s32 function_6304(PaletteTreeBoundaries&b,void*dst,u32 count,u32 h)noexcept{return b.call_d5ad?b.call_d5ad(h,dst,count):-1;} static_assert(0x631Fu-0x6304u==27u);
[[nodiscard]]s32 function_6320(PaletteTreeBoundaries&b,const void*src,u32 count,u32 h)noexcept{return b.call_d686?b.call_d686(h,src,count):-1;} static_assert(0x633Bu-0x6320u==27u);
[[nodiscard]]s32 function_633c(PaletteTreeBoundaries&b,s32 off,u32 origin,u32 h)noexcept{return b.call_d85f?b.call_d85f(h,off,origin):-1;} static_assert(0x6357u-0x633Cu==27u);
void function_6358(PaletteTreeBoundaries&b,u32 index,u32 h)noexcept{(void)function_633c(b,static_cast<s32>(index*4u+4u),0u,h);u32 off=0;(void)function_6304(b,&off,4u,h);(void)function_633c(b,static_cast<s32>(off),0u,h);} static_assert(0x63A2u-0x6358u==74u);
[[nodiscard]]s32 function_63a4(PaletteTreeBoundaries&b,const char*path)noexcept{return b.call_d8aa?b.call_d8aa(path):-1;} static_assert(0x63B7u-0x63A4u==19u);
[[nodiscard]]u32 function_63b8(PaletteTreeBoundaries&b,u32 drive)noexcept{u32 n=0;if(b.call_d8e8)(void)b.call_d8e8(drive,&n);return n;} static_assert(0x63D5u-0x63B8u==29u);
[[nodiscard]]u32 function_63d8(PaletteTreeBoundaries&b)noexcept{u32 d=0;if(b.call_d8d0)(void)b.call_d8d0(&d);return d;} static_assert(0x63F1u-0x63D8u==25u);
[[nodiscard]]u32 function_63f4(PaletteTreeBoundaries&b)noexcept{const u32 d=function_63d8(b);return function_63b8(b,d);} static_assert(0x6409u-0x63F4u==21u);
[[nodiscard]]u32 function_640c(PaletteTreeBoundaries&b)noexcept{return b.call_d906_guest?b.call_d906_guest(0x892fu):0u;} static_assert(0x6422u-0x640Cu==22u);
// 6422..6423 alignment.
[[nodiscard]]s32 function_6424(PaletteTreeBoundaries&b,char*path)noexcept{const u32 saved=function_63d8(b);if(path[1]==':'){const u32 upper=b.call_d991?b.call_d991(static_cast<u8>(path[0])):static_cast<u8>(path[0]);const s32 drive=static_cast<s32>(upper)-0x41;function_63b8(b,static_cast<u32>(drive));if(static_cast<s32>(function_63d8(b))!=drive)return -1;path+=2;}const s32 r=b.call_d9a6?b.call_d9a6(path):-1;if(r==-1)function_63b8(b,saved);return r;} static_assert(0x648Du-0x6424u==105u);
[[nodiscard]]s32 function_6490(PaletteTreeBoundaries&b,const char*path)noexcept{return b.call_d9ca?b.call_d9ca(path):-1;} static_assert(0x64A3u-0x6490u==19u);
// 64A3..64AF are zero/alignment bytes.
[[nodiscard]]u32 function_64b0(Machine&m,u32 arg)noexcept{if(static_cast<u16>(arg)==0u)arg=static_cast<u32>(load_le16(dgroup_ptr(m,dgroup(0x46cu))));function_61ac(m,static_cast<u16>(arg));return arg;} static_assert(0x64D9u-0x64B0u==41u);
// 64D9..64DB alignment.

struct DisplayInitBoundaries{u8(*call_9ca0)(u8,u32)=nullptr;void(*call_9df4)()=nullptr;void(*call_a474)(u32)=nullptr;void(*call_a35c)(u8)=nullptr;void(*call_a36c)(u8)=nullptr;void(*call_6bc0)(s32,s32,s32,s32)=nullptr;void(*call_a05c)(s32,s32,s32,s32,u8)=nullptr;void(*call_a010)(u32,u32,u32)=nullptr;};
// 64DC..6587: video mode initialization and global display-state reset.
void function_64dc(Machine&m,DisplayInitBoundaries&b,u32 mode)noexcept{if(mode==0xffu){if(b.call_9df4)b.call_9df4();*dgroup_ptr(m,dgroup(0x23d4u))=0xffu;return;}const u8 caps=b.call_9ca0?b.call_9ca0(static_cast<u8>(mode),0x23d4u):0u;store_le16(dgroup_ptr(m,dgroup(0x23dbu)),0x100u);store_le16(dgroup_ptr(m,dgroup(0x23ddu)),(caps&4u)?1u:0u);if(static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23dfu))))<0xff&&b.call_a474)b.call_a474(0xffu);*dgroup_ptr(m,dgroup(0x23e6u))=0;*dgroup_ptr(m,dgroup(0x23e5u))=0;if(b.call_a35c)b.call_a35c(0);if(b.call_a36c)b.call_a36c(0);if(b.call_6bc0)b.call_6bc0(0,0,static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d7u))))-1,static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d9u))))-1);*dgroup_ptr(m,dgroup(0x23d4u))=static_cast<u8>(mode)&0x7fu;} static_assert(0x6588u-0x64DCu==172u);
void function_6588(Machine&m,DisplayInitBoundaries&b,u8 page)noexcept{if(b.call_a35c)b.call_a35c(page);*dgroup_ptr(m,dgroup(0x23e5u))=page;} static_assert(0x65A4u-0x6588u==28u);
void function_65a4(Machine&m,DisplayInitBoundaries&b,u8 page)noexcept{if(b.call_a36c)b.call_a36c(page);*dgroup_ptr(m,dgroup(0x23e6u))=page;} static_assert(0x65C0u-0x65A4u==28u);
void function_65c0(DisplayInitBoundaries&b,s32 x,s32 y,s32 width,u8 color)noexcept{if(b.call_a05c)b.call_a05c(x,y,x,y+width-1,color);} static_assert(0x65E8u-0x65C0u==40u);
void function_65e8(DisplayInitBoundaries&b,s32 x,s32 y,s32 width,u8 color)noexcept{if(width!=0&&b.call_a05c)b.call_a05c(x,y,x+width-1,y,color);} static_assert(0x6616u-0x65E8u==46u);
// 6616..6617 alignment.
void function_6618(DisplayInitBoundaries&b,u32 dst)noexcept{if(b.call_a010)b.call_a010(dst,0u,0x300u);} static_assert(0x6632u-0x6618u==26u);
void function_6634(DisplayInitBoundaries&b,u32 a,u32 count,u32 dst)noexcept{if(b.call_a010)b.call_a010(dst,a,count*3u);} static_assert(0x6654u-0x6634u==32u);

// 6654..6851: clipped display wrappers. Width/height clipping follows the encoded signed comparisons exactly.
struct DisplayClipBoundaries{void(*a2b0)(s32,s32)=nullptr;void(*a21c)(s32)=nullptr;void(*a484)(u32,s32,s32,s32,s32)=nullptr;void(*a4e8)(u32,s32,s32,s32,s32)=nullptr;void(*a6f0)(u32,s32,s32,u32)=nullptr;void(*a7e8)(u32,s32,s32,u32,u8)=nullptr;void(*a8f4)(u32,s32,s32,u32,u32)=nullptr;void(*flush_8960)()=nullptr;};
void function_6654(Machine&m,DisplayClipBoundaries&b,s32 n)noexcept{if(b.a2b0)b.a2b0(static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d7u))))-n,n);} static_assert(0x6672u-0x6654u==30u);
void function_6674(Machine&m,DisplayClipBoundaries&b,s32 n)noexcept{const s32 w=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d7u))));if(b.a2b0)b.a2b0(w-n,n-w);} static_assert(0x6694u-0x6674u==32u);
void function_6694(DisplayClipBoundaries&b,s32 n)noexcept{if(b.a21c)b.a21c(n);} static_assert(0x66A7u-0x6694u==19u);
void function_66a8(DisplayClipBoundaries&b,s32 n)noexcept{if(b.a21c)b.a21c(-n);} static_assert(0x66BDu-0x66A8u==21u);
// 66BD..66BF alignment.
static void clip_rect_dims(Machine&m,s32&x,s32&y,s32&w,s32&h)noexcept{if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}const s32 sw=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d7u)))),sh=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d9u))));if(x+w>sw)w=sw-x;if(y+h>sh)h=sh-y;}
void function_66c0(Machine&m,DisplayClipBoundaries&b,u32 p,s32 x,s32 y,s32 w,s32 h)noexcept{clip_rect_dims(m,x,y,w,h);if(w>0&&h>0&&b.a484)b.a484(p,x,y,w,h);} static_assert(0x6743u-0x66C0u==131u);
void function_6744(Machine&m,DisplayClipBoundaries&b,u32 p,s32 x,s32 y,s32 w,s32 h)noexcept{clip_rect_dims(m,x,y,w,h);if(w>0&&h>=0&&b.a4e8)b.a4e8(p,x,y,w,h);} static_assert(0x67C7u-0x6744u==131u);
void function_67c8(DisplayClipBoundaries&b,u32 p,s32 x,s32 y,u32 n)noexcept{if(n!=0u){if(b.a6f0)b.a6f0(p,x,y,n);if(b.flush_8960)b.flush_8960();}} static_assert(0x67F2u-0x67C8u==42u);
void function_67f4(DisplayClipBoundaries&b,u32 p,s32 x,s32 y,u32 n,u8 c)noexcept{if(n!=0u){if(b.a7e8)b.a7e8(p,x,y,n,c);if(b.flush_8960)b.flush_8960();}} static_assert(0x6823u-0x67F4u==47u);
void function_6824(DisplayClipBoundaries&b,u32 p,s32 x,s32 y,u32 n,u32 c)noexcept{if(n!=0u){if(b.a8f4)b.a8f4(p,x,y,n,c);if(b.flush_8960)b.flush_8960();}} static_assert(0x6852u-0x6824u==46u);
// 6852..6853 alignment.



// 6854..6EE8: clipped sprite/image wrappers and nesting adapters.
struct MidGraphicsBoundaries{
 u8(*read8)(u32)=nullptr; u16(*read16)(u32)=nullptr; u32(*read32)(u32)=nullptr;
 void(*write8)(u32,u8)=nullptr; void(*write16)(u32,u16)=nullptr; void(*write32)(u32,u32)=nullptr;
 void(*call_8960)()=nullptr; u32(*call_954c)(u32,u32)=nullptr;
 void(*call_a6f0)(u32,s32,s32,u32)=nullptr; void(*call_aa20)(u32,s32,s32,u32,u32)=nullptr;
 void(*call_a7e8)(u32,s32,s32,u32,u8)=nullptr; void(*call_ac10)(u32,s32,s32,u32,u32,u8)=nullptr;
 void(*call_862c)(u32,u32,u32)=nullptr; void(*call_a8f4)(u32,s32,s32,u32,u32)=nullptr;
 void(*call_da18)()=nullptr; void(*call_da1a)()=nullptr;
};
static u8 mg8(MidGraphicsBoundaries&b,u32 a)noexcept{return b.read8?b.read8(a):0u;}
static u16 mg16(MidGraphicsBoundaries&b,u32 a)noexcept{return b.read16?b.read16(a):0u;}
static void mgw32(MidGraphicsBoundaries&b,u32 a,u32 v)noexcept{if(b.write32)b.write32(a,v);}
static void mgw16(MidGraphicsBoundaries&b,u32 a,u16 v)noexcept{if(b.write16)b.write16(a,v);}
static u32 clip_prepare(Machine&m,MidGraphicsBoundaries&b,s32&x,s32&y,u32 image,u32 clip)noexcept{
 if(image==0u)return 0u;u32 w=mg16(b,image+4u),h=static_cast<u32>(mg8(b,image+6u))*8u;mgw32(b,clip,w);mgw32(b,clip+4u,0);mgw32(b,clip+8u,w);mgw32(b,clip+0x0cu,0);mgw32(b,clip+0x10u,0);mgw32(b,clip+0x14u,h);
 s32 d=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x23edu))))-x;if(d>0){w-=static_cast<u32>(d);mgw32(b,clip+8u,w);mgw32(b,clip,static_cast<u32>(d*static_cast<s32>(mg8(b,image+6u)*8u)));x=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x23edu))));}
 d=x+static_cast<s32>(w)-1-static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x23f1u))));if(d>0){w-=static_cast<u32>(d);mgw32(b,clip+8u,w);mgw32(b,clip+4u,static_cast<u32>(d*static_cast<s32>(mg8(b,image+6u)*8u)));}
 h=static_cast<u32>(mg8(b,image+6u))*8u;mgw32(b,clip+0x14u,h);d=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x23f5u))))-y;if(d>0){h-=static_cast<u32>(d);mgw32(b,clip+0x14u,h);mgw32(b,clip+0x0cu,static_cast<u32>(d));y=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x23f5u))));}d=y+static_cast<s32>(h)-1-static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x23f9u))));if(d>0){h-=static_cast<u32>(d);mgw32(b,clip+0x14u,h);mgw32(b,clip+0x10u,static_cast<u32>(d));}
 store_le16(dgroup_ptr(m,dgroup(0x2401u)),static_cast<u16>(x));store_le16(dgroup_ptr(m,dgroup(0x2403u)),static_cast<u16>(y));store_le16(dgroup_ptr(m,dgroup(0x2405u)),static_cast<u16>(w));store_le16(dgroup_ptr(m,dgroup(0x2407u)),static_cast<u16>(h));return (w==mg16(b,image+4u)&&h==static_cast<u32>(mg8(b,image+6u))*8u)?2u:((static_cast<s32>(w)>0&&static_cast<s32>(h)>0)?1u:0u);
}
[[nodiscard]]u32 function_6854(Machine&m,MidGraphicsBoundaries&b,u32 dst,s32 x,s32 y,u32 image,u32 clip)noexcept{if(b.call_8960)b.call_8960();const u32 mode=clip_prepare(m,b,x,y,image,clip);if(mode==2u){if(b.call_a6f0)b.call_a6f0(dst,x,y,image);}else if(mode==1u){if(b.call_aa20)b.call_aa20(dst,x,y,clip,image);}return mode;} static_assert(0x6A03u-0x6854u==431u);
[[nodiscard]]u32 function_6a04(Machine&m,MidGraphicsBoundaries&b,u32 dst,s32 x,s32 y,u32 image,u8 colour,u32 clip)noexcept{if(b.call_8960)b.call_8960();const u32 mode=clip_prepare(m,b,x,y,image,clip);if(mode==2u){if(b.call_a7e8)b.call_a7e8(dst,x,y,image,colour);}else if(mode==1u){if(b.call_ac10)b.call_ac10(dst,x,y,clip,image,colour);}return mode;} static_assert(0x6BBDu-0x6A04u==441u);
void function_6bc0(Machine&m,s32 left,s32 top,s32 right,s32 bottom)noexcept{if(left<0)left=0;if(top<0)top=0;const s32 maxx=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d7u))));const s32 maxy=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d9u))));if(maxx<=right)right=maxx-1;if(maxy<=bottom)bottom=maxy-1;top&=~3;const s32 q=(bottom+((bottom<0)?15:0))/4;bottom=q*4+3;store_le32(dgroup_ptr(m,dgroup(0x23edu)),static_cast<u32>(left));store_le32(dgroup_ptr(m,dgroup(0x23f5u)),static_cast<u32>(top));store_le32(dgroup_ptr(m,dgroup(0x23f1u)),static_cast<u32>(right));store_le32(dgroup_ptr(m,dgroup(0x23f9u)),static_cast<u32>(bottom));} static_assert(0x6C43u-0x6BC0u==131u);
void function_6c44(Machine&m,u32 dst)noexcept{std::memcpy(dgroup_ptr(m,dgroup(dst)),dgroup_ptr(m,dgroup(0x23edu)),0x10u);} static_assert(0x6C5Eu-0x6C44u==26u);
void function_6c60(MidGraphicsBoundaries&b,u32 dst,s32 x,s32 y,u32 id,u32 bank)noexcept{const u32 image=b.call_954c?b.call_954c(id,bank):0u;if(image){if(b.call_a6f0)b.call_a6f0(dst,x,y,image);if(b.call_8960)b.call_8960();}} static_assert(0x6C95u-0x6C60u==53u);
[[nodiscard]]u32 function_6c98(Machine&m,MidGraphicsBoundaries&b,u32 dst,s32 x,s32 y,u32 id,u32 bank)noexcept{if(b.call_da18)b.call_da18();u32 depth=load_le32(dgroup_ptr(m,dgroup(0x246cu))),clip=0x240cu+depth*0x18u;store_le32(dgroup_ptr(m,dgroup(0x246cu)),depth+1u);if(b.call_da1a)b.call_da1a();const u32 image=b.call_954c?b.call_954c(id,bank):0u;const u32 r=function_6854(m,b,dst,x,y,image,clip);if(b.call_da18)b.call_da18();store_le32(dgroup_ptr(m,dgroup(0x246cu)),load_le32(dgroup_ptr(m,dgroup(0x246cu)))-1u);if(b.call_da1a)b.call_da1a();return r;} static_assert(0x6CFDu-0x6C98u==101u);
void function_6d00(Machine&m,MidGraphicsBoundaries&b,u32 a,u32 c,u32 id,u32 bank)noexcept{const u32 image=b.call_954c?b.call_954c(id,bank):0u;if(!image)return;if(b.call_da18)b.call_da18();store_le32(dgroup_ptr(m,dgroup(0x246cu)),load_le32(dgroup_ptr(m,dgroup(0x246cu)))+1u);if(b.call_da1a)b.call_da1a();if(b.call_862c)b.call_862c(a,c,image);if(b.call_da18)b.call_da18();store_le32(dgroup_ptr(m,dgroup(0x246cu)),load_le32(dgroup_ptr(m,dgroup(0x246cu)))-1u);if(b.call_da1a)b.call_da1a();} static_assert(0x6D51u-0x6D00u==81u);
[[nodiscard]]u32 function_6d54(Machine&m,MidGraphicsBoundaries&b,u32 dst,s32 x,s32 y,u32 image)noexcept{if(b.call_da18)b.call_da18();const u32 depth=load_le32(dgroup_ptr(m,dgroup(0x246cu))),clip=0x240cu+depth*0x18u;store_le32(dgroup_ptr(m,dgroup(0x246cu)),depth+1u);if(b.call_da1a)b.call_da1a();const u32 r=function_6854(m,b,dst,x,y,image,clip);if(b.call_da18)b.call_da18();store_le32(dgroup_ptr(m,dgroup(0x246cu)),load_le32(dgroup_ptr(m,dgroup(0x246cu)))-1u);if(b.call_da1a)b.call_da1a();return r;} static_assert(0x6DACu-0x6D54u==88u);
void function_6dac(MidGraphicsBoundaries&b,u32 dst,s32 x,s32 y,u32 id,u32 bank,u8 colour)noexcept{const u32 image=b.call_954c?b.call_954c(id,bank):0u;if(image){if(b.call_a7e8)b.call_a7e8(dst,x,y,image,colour);if(b.call_8960)b.call_8960();}} static_assert(0x6DE6u-0x6DACu==58u);
[[nodiscard]]u32 function_6de8(Machine&m,MidGraphicsBoundaries&b,u32 dst,s32 x,s32 y,u32 id,u32 bank,u8 colour)noexcept{if(b.call_da18)b.call_da18();const u32 depth=load_le32(dgroup_ptr(m,dgroup(0x246cu))),clip=0x240cu+depth*0x18u;store_le32(dgroup_ptr(m,dgroup(0x246cu)),depth+1u);if(b.call_da1a)b.call_da1a();const u32 image=b.call_954c?b.call_954c(id,bank):0u;const u32 r=function_6a04(m,b,dst,x,y,image,colour,clip);if(b.call_da18)b.call_da18();store_le32(dgroup_ptr(m,dgroup(0x246cu)),load_le32(dgroup_ptr(m,dgroup(0x246cu)))-1u);if(b.call_da1a)b.call_da1a();return r;} static_assert(0x6E4Fu-0x6DE8u==103u);
[[nodiscard]]u32 function_6e50(Machine&m,MidGraphicsBoundaries&b,u32 dst,s32 x,s32 y,u32 image,u8 colour)noexcept{if(b.call_da18)b.call_da18();const u32 depth=load_le32(dgroup_ptr(m,dgroup(0x246cu))),clip=0x240cu+depth*0x18u;store_le32(dgroup_ptr(m,dgroup(0x246cu)),depth+1u);if(b.call_da1a)b.call_da1a();const u32 r=function_6a04(m,b,dst,x,y,image,colour,clip);if(b.call_da18)b.call_da18();store_le32(dgroup_ptr(m,dgroup(0x246cu)),load_le32(dgroup_ptr(m,dgroup(0x246cu)))-1u);if(b.call_da1a)b.call_da1a();return r;} static_assert(0x6EADu-0x6E50u==93u);
void function_6eb0(MidGraphicsBoundaries&b,u32 dst,s32 x,s32 y,u32 id,u32 bank,u32 mode)noexcept{const u32 image=b.call_954c?b.call_954c(id,bank):0u;if(image){if(b.call_a8f4)b.call_a8f4(dst,x,y,image,mode);if(b.call_8960)b.call_8960();}} static_assert(0x6EE9u-0x6EB0u==57u);
[[nodiscard]]u32 function_6ee9(const char*s)noexcept{return static_cast<u32>(std::strlen(s));} static_assert(0x6F05u-0x6EE9u==28u);

// 6F10..71C7: DPMI/extender wrappers. Host-side interrupt execution stays behind explicit boundaries.
struct MidDpmiBoundaries{
 u32(*alloc_dos)(u32 paragraphs,u16*out_sel)=nullptr; s32(*free_dos)(u16)=nullptr; u32(*get_base)(u16)=nullptr;
 u32(*alloc_linear)(u32 bytes,u32*handle)=nullptr; s32(*free_linear)(u32 handle)=nullptr; u32(*alloc_selector)()=nullptr;
 u32(*query_mem)()=nullptr; void(*write32_guest)(u32,u32)=nullptr; void(*zero)(void*,u32)=nullptr; void(*fatal_64dc)(u32)=nullptr; void(*print_61d0)(u32)=nullptr;
 void(*call_cf98)()=nullptr; void(*call_8424)()=nullptr; void(*call_97d4)(u32)=nullptr; void(*call_daa6)()=nullptr;
};
[[nodiscard]]u32 function_6f10(MidDpmiBoundaries&b,s32 bytes,u32*out_linear,u16*out_sel)noexcept{const s32 paragraphs=(bytes+0x10+((bytes+0x10<0)?15:0))/16;u16 sel=0;const u32 seg=b.alloc_dos?b.alloc_dos(static_cast<u32>(paragraphs),&sel):0u;if(seg==0u)return 0u;if(out_sel)*out_sel=sel;if(out_linear)*out_linear=seg<<16;return seg<<4;} static_assert(0x6F72u-0x6F10u==98u);
[[nodiscard]]s32 function_6f74(MidDpmiBoundaries&b,u16 sel)noexcept{return b.free_dos?b.free_dos(sel):-1;} static_assert(0x6FACu-0x6F74u==56u);
void function_6fac(MidDpmiBoundaries&b,u16 sel)noexcept{(void)(b.free_dos?b.free_dos(sel):0);} static_assert(0x6FC0u-0x6FACu==20u);
[[nodiscard]]u32 function_6fc0(MidDpmiBoundaries&b,u16 sel)noexcept{return b.get_base?b.get_base(sel):0u;} static_assert(0x6FF2u-0x6FC0u==50u);
[[nodiscard]]u32 function_6ff4(MidDpmiBoundaries&b,s32 bytes)noexcept{const s32 paras=(bytes+15+((bytes+15<0)?15:0))/16;if(paras>0xffff)return 0u;u16 sel=0;const u32 seg=b.alloc_dos?b.alloc_dos(static_cast<u32>(paras),&sel):0u;if(!seg)return 0u;const u32 p=b.get_base?b.get_base(sel):0u;return p;} static_assert(0x705Bu-0x6FF4u==103u);
[[nodiscard]]s32 function_705c(MidDpmiBoundaries&b,u32 handle)noexcept{return b.free_linear?b.free_linear(handle):-1;} static_assert(0x7099u-0x705Cu==61u);
[[nodiscard]]u32 function_709c(MidDpmiBoundaries&b,u32 bytes)noexcept{u32 h=0;const u32 p=b.alloc_linear?b.alloc_linear(bytes,&h):0u;if(p&&b.write32_guest)b.write32_guest(p,h);return p;} static_assert(0x70FCu-0x709Cu==96u);
[[nodiscard]]u32 function_70fc(MidDpmiBoundaries&b)noexcept{return b.alloc_selector?b.alloc_selector():0u;} static_assert(0x713Bu-0x70FCu==63u);
[[nodiscard]]u32 function_713c(MidDpmiBoundaries&b)noexcept{return b.query_mem?b.query_mem():0u;} static_assert(0x7182u-0x713Cu==70u);
void function_7184(MidDpmiBoundaries&b)noexcept{if(b.fatal_64dc)b.fatal_64dc(0xffu);if(b.print_61d0)b.print_61d0(0xb8u);if(b.call_cf98)b.call_cf98();if(b.call_8424)b.call_8424();if(b.call_97d4)b.call_97d4(0x0au);} static_assert(0x71B9u-0x7184u==53u);
void function_71bc(MidDpmiBoundaries&b)noexcept{if(b.call_daa6)b.call_daa6();} static_assert(0x71C8u-0x71BCu==12u);

// 72A0..7669: byte-copy and buffered stream/decode helpers.
struct MidStreamBoundaries{
 u8(*read8)(u32)=nullptr; u16(*read16)(u32)=nullptr; void(*write8)(u32,u8)=nullptr; void(*copy)(u32,u32,u32)=nullptr;
 s32(*seek)(s32,u32,u32)=nullptr; s32(*read)(u32,u32,u32)=nullptr; void(*flush_9ae8)()=nullptr;
 void(*emit_a57c)(u32,u32,u32,u32,u32)=nullptr; void(*decode_db80)()=nullptr; void(*zero_84a0)(u32,u32,u32)=nullptr;
};
[[nodiscard]]u32 function_72a0(MidStreamBoundaries&b,u32 dst,u32 src)noexcept{const u32 ret=dst;for(;;){u8 c=b.read8?b.read8(src):0u;if(b.write8)b.write8(dst,c);if(c==0u)break;c=b.read8?b.read8(src+1u):0u;if(b.write8)b.write8(dst+1u,c);src+=2u;dst+=2u;if(c==0u)break;}return ret;} static_assert(0x72C9u-0x72A0u==41u);
void function_72d0(Machine&m,u32 h)noexcept{store_le32(dgroup_ptr(m,dgroup(0x2904u)),h);} static_assert(0x72DFu-0x72D0u==15u);
void function_72e0(Machine&m,MidStreamBoundaries&b)noexcept{const u32 h=load_le32(dgroup_ptr(m,dgroup(0x2904u)));s32 pos=b.seek?b.seek(0,1u,h):0;const u32 count=0x400u-(static_cast<u32>(pos)&0x3ffu);const s32 got=b.read?b.read(0x2470u,count,h):0;store_le32(dgroup_ptr(m,dgroup(0x2874u)),static_cast<u32>(got));store_le32(dgroup_ptr(m,dgroup(0x2870u)),0x2470u);} static_assert(0x7328u-0x72E0u==72u);
void function_7328(Machine&m,MidStreamBoundaries&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2874u)))!=0u)return;const u32 h=load_le32(dgroup_ptr(m,dgroup(0x2904u)));const s32 got=b.read?b.read(0x2470u,0x400u,h):0;store_le32(dgroup_ptr(m,dgroup(0x2874u)),static_cast<u32>(got));store_le32(dgroup_ptr(m,dgroup(0x2870u)),0x2470u);} static_assert(0x735Fu-0x7328u==55u);
[[nodiscard]]u8 function_7360(Machine&m,MidStreamBoundaries&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2874u)))==0u)function_7328(m,b);u32 p=load_le32(dgroup_ptr(m,dgroup(0x2870u)));store_le32(dgroup_ptr(m,dgroup(0x2874u)),load_le32(dgroup_ptr(m,dgroup(0x2874u)))-1u);const u8 v=b.read8?b.read8(p):0u;store_le32(dgroup_ptr(m,dgroup(0x2870u)),p+1u);return v;} static_assert(0x738Cu-0x7360u==44u);
void function_738c(Machine&m,MidStreamBoundaries&b,u8 v)noexcept{if(*dgroup_ptr(m,dgroup(0x89ceu))!=0u)return;u32 used=load_le32(dgroup_ptr(m,dgroup(0x89b8u))),limit=load_le32(dgroup_ptr(m,dgroup(0x89a4u)));if(*dgroup_ptr(m,dgroup(0x89cfu))!=0u){const u32 base=load_le32(dgroup_ptr(m,dgroup(0x89c8u)));if(b.write8)b.write8(base+used,v);++used;store_le32(dgroup_ptr(m,dgroup(0x89b8u)),used);if(used>=limit){if(b.flush_9ae8)b.flush_9ae8();const u32 rec=load_le32(dgroup_ptr(m,dgroup(0x8994u)));const u32 flag=b.read8?b.read8(rec+5u):0u;if(b.emit_a57c)b.emit_a57c(load_le32(dgroup_ptr(m,dgroup(0x89b4u))),limit,base,load_le32(dgroup_ptr(m,dgroup(0x899cu))),flag);store_le32(dgroup_ptr(m,dgroup(0x89b8u)),0u);store_le32(dgroup_ptr(m,dgroup(0x89b4u)),load_le32(dgroup_ptr(m,dgroup(0x89b4u)))+1u);}const u32 rec=load_le32(dgroup_ptr(m,dgroup(0x8994u)));const s16 rows=static_cast<s16>(b.read16?b.read16(rec):0u);if(rows<=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x89b4u)))))*dgroup_ptr(m,dgroup(0x89ceu))=1u;}else{const u32 base=load_le32(dgroup_ptr(m,dgroup(0x89c4u)));if(b.write8)b.write8(base+used,v);++used;store_le32(dgroup_ptr(m,dgroup(0x89b8u)),used);if(used>=limit){if(b.flush_9ae8)b.flush_9ae8();store_le32(dgroup_ptr(m,dgroup(0x89b8u)),0u);*dgroup_ptr(m,dgroup(0x89ceu))=1u;}}} static_assert(0x7469u-0x738Cu==221u);
void function_746c(Machine&m,MidStreamBoundaries&b,u32 arg0,u32 rec,u32 buf,u32 limit,u8 mode)noexcept{*dgroup_ptr(m,dgroup(0x89cfu))=mode;*dgroup_ptr(m,dgroup(0x89ceu))=0;store_le32(dgroup_ptr(m,dgroup(0x8994u)),rec);store_le32(dgroup_ptr(m,dgroup(0x89b8u)),0u);store_le32(dgroup_ptr(m,dgroup(0x89b4u)),0u);store_le32(dgroup_ptr(m,dgroup(0x89a4u)),limit);store_le32(dgroup_ptr(m,dgroup(0x899cu)),arg0);store_le32(dgroup_ptr(m,dgroup(mode?0x89c8u:0x89c4u)),buf);if(b.decode_db80)b.decode_db80();} static_assert(0x74CEu-0x746Cu==98u);
void function_74d0(Machine&m,MidStreamBoundaries&b,u32 dst,u32 count)noexcept{function_7328(m,b);u32 avail=load_le32(dgroup_ptr(m,dgroup(0x2874u))),src=load_le32(dgroup_ptr(m,dgroup(0x2870u)));if(avail>=count){if(b.copy)b.copy(dst,src,count);store_le32(dgroup_ptr(m,dgroup(0x2870u)),src+count);}else{if(b.copy)b.copy(dst,src,avail);count-=avail;const u32 h=load_le32(dgroup_ptr(m,dgroup(0x2904u)));const s32 got=b.read?b.read(0x2470u,0x400u,h):0;store_le32(dgroup_ptr(m,dgroup(0x2874u)),static_cast<u32>(got));if(b.copy)b.copy(dst+avail,0x2470u,count);store_le32(dgroup_ptr(m,dgroup(0x2870u)),0x2470u+count);}store_le32(dgroup_ptr(m,dgroup(0x2874u)),load_le32(dgroup_ptr(m,dgroup(0x2874u)))-count);} static_assert(0x756Au-0x74D0u==154u);
[[nodiscard]]u8 function_756c(Machine&m,MidStreamBoundaries&b)noexcept{if(*dgroup_ptr(m,dgroup(0x2878u))==0u){store_le32(dgroup_ptr(m,dgroup(0x287cu)),0x2880u);const s32 n=static_cast<signed char>(function_7360(m,b));if(n>=0){*dgroup_ptr(m,dgroup(0x2878u))=static_cast<u8>(n+1);function_74d0(m,b,0x2880u,static_cast<u8>(n+1));}else{const u8 c=static_cast<u8>(-n+1);*dgroup_ptr(m,dgroup(0x2878u))=c;const u8 first=function_7360(m,b);*dgroup_ptr(m,dgroup(0x2880u))=first;if(b.zero_84a0)b.zero_84a0(0x2881u,first,static_cast<u32>(c-1u));}}u32 p=load_le32(dgroup_ptr(m,dgroup(0x287cu)));*dgroup_ptr(m,dgroup(0x2878u))=static_cast<u8>(*dgroup_ptr(m,dgroup(0x2878u))-1u);const u8 out=b.read8?b.read8(p):0u;store_le32(dgroup_ptr(m,dgroup(0x287cu)),p+1u);return out;} static_assert(0x75EDu-0x756Cu==129u);
void function_75f0(Machine&m,MidStreamBoundaries&b,u32 dst,u32 a,u32 c)noexcept{const u32 n=a*c*8u;for(u32 i=0;i<n;++i){if(b.write8)b.write8(dst+i,function_756c(m,b));}} static_assert(0x762Cu-0x75F0u==60u);
void function_762c(Machine&m,MidStreamBoundaries&b,u32 dst,u32 a,u32 c)noexcept{const u32 n=a*c*8u;for(u32 i=0;i<n;++i){if(b.write8)b.write8(dst+i,function_7360(m,b));}} static_assert(0x766Au-0x762Cu==62u);


// 71C8..723C: guarded guest allocator. The ten-byte prefix and magic at +6 are part of the
// encoded ownership contract; recovery callbacks are attempted in the exact 0,1,2 sequence.
struct MidUi10Boundaries{
 u8(*read8)(u32)=nullptr;u16(*read16)(u32)=nullptr;u32(*read32)(u32)=nullptr;
 void(*write8)(u32,u8)=nullptr;void(*write16)(u32,u16)=nullptr;void(*write32)(u32,u32)=nullptr;
 u32(*alloc_dac9)(u32)=nullptr;void(*free_db4b)(u32)=nullptr;
 u32(*call_9378)(u32)=nullptr;void(*call_94d0)()=nullptr;void(*call_9520)()=nullptr;void(*call_7184)()=nullptr;
 void(*call_9814)(u32)=nullptr;void(*call_9874)(u32,u32)=nullptr;
 u32(*call_a558)(s32,s32,u32,u32,u32)=nullptr;void(*call_9ae8)()=nullptr;
 void(*call_762c)(u32,s32,u32)=nullptr;void(*call_75f0)(u32,s32,u32)=nullptr;
 void(*call_a57c)(s32,u32,u32,u32,u32)=nullptr;void(*call_746c)(u32,u32,u32,u32,u32)=nullptr;
 void(*call_dc90)(u32)=nullptr;void(*call_66c0)(u32,s32,s32,s32,s32)=nullptr;
 u32(*call_9ad4)()=nullptr;void(*call_9ab4)()=nullptr;void(*call_9ac4)()=nullptr;
 u32(*call_767c_local)(const u8*,u32)=nullptr;u32(*call_954c)(u32,u32)=nullptr;
 void(*call_6824)(s32,u32,u32,u32,u32)=nullptr;void(*call_67c8)(s32,u32,u32,u32)=nullptr;
 void(*critical_da18)()=nullptr;void(*critical_da1a)()=nullptr;
 void(*call_6588)(u32)=nullptr;void(*call_65c0)(s32,s32,s32,u8)=nullptr;void(*call_65e8)(s32,s32,s32,u8)=nullptr;
 void(*call_ae0c)(u32,u32,u32)=nullptr;void(*call_6d54)(s32,s32,s32,u32)=nullptr;
 void(*call_a5c8)(u32,u32)=nullptr;void(*call_a678)(u32,u32)=nullptr;
 void(*get_vector_de35)(u32,u32*,u16*)=nullptr;void(*set_vector_ddfe)(u32,u32,u16)=nullptr;u16 cs_selector=0;
 void(*call_de70)(u32,u32,u32)=nullptr;
};
static u8 mu8(MidUi10Boundaries&b,u32 a)noexcept{return b.read8?b.read8(a):0u;}
static u16 mu16(MidUi10Boundaries&b,u32 a)noexcept{return b.read16?b.read16(a):0u;}
static u32 mu32(MidUi10Boundaries&b,u32 a)noexcept{return b.read32?b.read32(a):0u;}
static void mw8(MidUi10Boundaries&b,u32 a,u8 v)noexcept{if(b.write8)b.write8(a,v);}
static void mw16(MidUi10Boundaries&b,u32 a,u16 v)noexcept{if(b.write16)b.write16(a,v);}
static void mw32(MidUi10Boundaries&b,u32 a,u32 v)noexcept{if(b.write32)b.write32(a,v);}
static s32 div8_zero(s32 v)noexcept{return v>=0?v/8:-((-v)/8);}
[[nodiscard]]u32 function_71c8(MidUi10Boundaries&b,u32 request)noexcept{
 u8 stage=0;u32 block=0;
 while(block==0u){block=b.alloc_dac9?b.alloc_dac9(request+10u):0u;if(block!=0u)break;if((b.call_9378?b.call_9378(1u):0u)!=0u)continue;if(stage==0u){if(b.call_94d0)b.call_94d0();++stage;}else if(stage==1u){if(b.call_9520)b.call_9520();++stage;}else if(stage==2u){if(b.call_7184)b.call_7184();}else break;}
 if(block==0u)return 0u;mw32(b,block+6u,0x12345678u);return block+10u;
}
static_assert(0x723Du-0x71C8u==117u);
// 723D..723F LEA alignment.

// 7240..729F: paired release path for 71C8, including both encoded magic diagnostics.
void function_7240(MidUi10Boundaries&b,u32 ptr)noexcept{u32 block=ptr-10u;if(block==0u){if(b.call_9874)b.call_9874(0xe8u,0u);return;}const u32 magic=mu32(b,block+6u);if(magic==0xabcdef98u){if(b.call_9814)b.call_9814(0x118u);return;}if(magic!=0x12345678u){if(b.call_9814)b.call_9814(0x138u);return;}mw32(b,block+6u,0xabcdef98u);if(b.free_db4b)b.free_db4b(block);}
static_assert(0x72A0u-0x7240u==96u);

// 766C..767B is the four-entry CS jump table {7756,77A4,77E7,77EE}; 766A..766B is alignment.
// 767C..782E: load/expand one encoded image descriptor. The mode byte selects the exact table target.
[[nodiscard]]u32 function_767c(Machine&m,MidUi10Boundaries&b,u32 desc,u32 source)noexcept{
 store_le32(dgroup_ptr(m,dgroup(0x2904u)),source);const u32 h=mu32(b,desc+5u);const s16 count=static_cast<s16>(mu16(b,h));if(count==0&&b.call_9814)b.call_9814(0x158u);
 const s16 rows=static_cast<s16>(mu16(b,h+2u));const u8 width=mu8(b,h+4u),key=mu8(b,h+5u);
 const u32 expanded=b.call_a558?b.call_a558(count,rows,width,8u,key):0u;const u32 out=function_71c8(b,expanded+8u);
 if(count!=0)mw16(b,out,static_cast<u16>(static_cast<s32>(expanded)/static_cast<s32>(count)));mw16(b,out+4u,static_cast<u16>(rows));mw8(b,out+6u,width);mw16(b,out+2u,static_cast<u16>(count));mw8(b,out+7u,key==0xffu?0u:0x80u);
 const u32 payload_size=static_cast<u32>(static_cast<s32>(rows)*static_cast<s32>(width))*8u;const u32 payload=function_71c8(b,payload_size);if(payload==0u&&b.call_9814)b.call_9814(0x17cu);
 switch(mu8(b,h+6u)){
  case 0:for(s32 i=0;i<count;++i){if(b.call_9ae8)b.call_9ae8();if(b.call_762c)b.call_762c(payload,rows,width);if(b.call_a57c)b.call_a57c(i,payload_size,payload,out,key);}break;
  case 1:for(s32 i=0;i<count;++i){if(b.call_9ae8)b.call_9ae8();if(b.call_75f0)b.call_75f0(payload,rows,width);if(b.call_a57c)b.call_a57c(i,payload_size,payload,out,key);}break;
  case 2:if(b.call_9814)b.call_9814(0x1a0u);break;
  case 3:if(b.call_746c)b.call_746c(out,h,payload,payload_size,1u);break;
  default:if(b.call_9814)b.call_9814(0x1bcu);break;
 }
 function_7240(b,payload);if(mu8(b,h+7u)!=0u&&b.call_dc90)b.call_dc90(out);return out;
}
static_assert(0x782Fu-0x767Cu==435u);
// 782F NOP alignment.
void function_7830(MidUi10Boundaries&b,u32 p)noexcept{function_7240(b,p);} static_assert(0x7843u-0x7830u==19u);
// 7843 NOP alignment.
// 7844..7867: size helper; the original writes only DX, so incoming EDX high bits are explicit.
[[nodiscard]]u32 function_7844(MidUi10Boundaries&b,u32 p,u32 incoming_edx)noexcept{u32 edx=(incoming_edx&0xffff0000u)|mu16(b,p);const u16 prod=static_cast<u16>(static_cast<s16>(low16(edx))*static_cast<s16>(mu16(b,p+2u)));edx=(edx&0xffff0000u)|prod;return edx+8u+((mu8(b,p+7u)&0x80u)!=0u?1u:0u);} static_assert(0x7868u-0x7844u==36u);

// 7868..7941: allocate a one-row image descriptor and capture pixels through 66C0.
[[nodiscard]]u32 function_7868(MidUi10Boundaries&b,s32 x,s32 y,s32 w,s32 pixel_bits,u32 key)noexcept{if(((static_cast<u32>(y)&3u)!=0u||(static_cast<u32>(pixel_bits)&7u)!=0u)&&b.call_9814)b.call_9814(0x1e0u);const s32 bytes=div8_zero(pixel_bits);const u32 expanded=b.call_a558?b.call_a558(1,w,static_cast<u32>(bytes),8u,key):0u;const u32 out=function_71c8(b,static_cast<u32>(low16(expanded))+8u);mw16(b,out,low16(expanded));mw16(b,out+4u,static_cast<u16>(w));mw8(b,out+6u,static_cast<u8>(bytes));mw16(b,out+2u,1u);mw8(b,out+7u,key==0xffu?0u:0x80u);if(b.call_66c0)b.call_66c0(out+8u+(key==0xffu?0u:1u),x,y,w,pixel_bits);if(key!=0xffu)mw8(b,out+8u,static_cast<u8>(key));return out;} static_assert(0x7942u-0x7868u==218u);
// 7942..7943 MOV EAX,EAX alignment.

// 7944..79C5: initialize an existing one-row image descriptor and capture into its payload.
[[nodiscard]]u32 function_7944(MidUi10Boundaries&b,u32 out,u32 first_word,s32 x,s32 y,s32 w,s32 pixel_bits,u32 key)noexcept{mw16(b,out,static_cast<u16>(first_word));mw16(b,out+4u,static_cast<u16>(w));mw8(b,out+6u,static_cast<u8>(div8_zero(pixel_bits)));mw16(b,out+2u,1u);mw8(b,out+7u,key==0xffu?0u:0x80u);if(b.call_66c0)b.call_66c0(out+8u+(key==0xffu?0u:1u),x,y,w,pixel_bits);if(key!=0xffu)mw8(b,out+8u,static_cast<u8>(key));return out;} static_assert(0x79C6u-0x7944u==130u);
// 79C6..79C7 MOV EAX,EAX alignment.

// 79C8..7A60: clone a variable descriptor, rewrite its local +3/+5 fields, recurse through 767C,
// and preserve the uninitialized local byte as an explicit incoming value when 9AD4 returns zero.
[[nodiscard]]u32 function_79c8(MidUi10Boundaries&b,u32 src,u32 source,u8 incoming_local_f0)noexcept{u8 flag=incoming_local_f0;if((b.call_9ad4?b.call_9ad4():0u)!=0u){flag=1u;if(b.call_9ab4)b.call_9ab4();}const s16 off=static_cast<s16>(mu16(b,src+3u));const u32 text=mu32(b,src+5u);const u32 rec=text+static_cast<u32>(static_cast<s32>(off))-8u;const s16 len=static_cast<s16>(mu16(b,rec));const u32 out=function_71c8(b,static_cast<u32>(static_cast<s32>(len)+4));for(s32 i=0;i<len;++i)mw8(b,out+4u+static_cast<u32>(i),mu8(b,text+static_cast<u32>(i)));u8 local[9]{};for(u32 i=0;i<9u;++i)local[i]=mu8(b,src+i);store_le16(local+3,8u);store_le32(local+5,rec);const u32 child=b.call_767c_local?b.call_767c_local(local,source):0u;mw32(b,out,child);if(flag!=0u&&b.call_9ac4)b.call_9ac4();return out;} static_assert(0x7A61u-0x79C8u==153u);
// 7A61..7A63 LEA alignment.
void function_7a64(MidUi10Boundaries&b,u32 p)noexcept{function_7830(b,mu32(b,p));function_7240(b,p);} static_assert(0x7A84u-0x7A64u==32u);
[[nodiscard]]u32 function_7a84(MidUi10Boundaries&b,u32 s,u8 delta)noexcept{const u32 first=s;for(u32 p=s;p!=0u&&mu8(b,p)!=0u;++p)mw8(b,p,static_cast<u8>(mu8(b,p)+delta));return first;} static_assert(0x7AB1u-0x7A84u==45u);
// 7AB1..7AB3 LEA alignment.

static u32 font_ptr_10(MidUi10Boundaries&b,u32 id)noexcept{return b.call_954c?b.call_954c(id,0u):0u;}
static u8 glyph_width_signed_10(MidUi10Boundaries&b,u32 font,u8 ch)noexcept{return mu8(b,font+4u+static_cast<u32>(static_cast<s32>(static_cast<std::int8_t>(ch))));}
static u8 glyph_width_unsigned_10(MidUi10Boundaries&b,u32 font,u8 ch)noexcept{return mu8(b,font+4u+static_cast<u32>(ch));}
[[nodiscard]]u32 function_7ab4(Machine&m,MidUi10Boundaries&b,u32 s,u32 font_id)noexcept{const u32 font=font_ptr_10(b,font_id);store_le32(dgroup_ptr(m,dgroup(0x8988u)),font);u32 total=0;for(u32 p=s;p!=0u&&mu8(b,p)!=0u;++p)total+=glyph_width_signed_10(b,font,mu8(b,p));return total;} static_assert(0x7AFCu-0x7AB4u==72u);
[[nodiscard]]u32 function_7afc(Machine&m,MidUi10Boundaries&b,u32 s,u32 font_id,u32 count)noexcept{const u32 font=font_ptr_10(b,font_id);store_le32(dgroup_ptr(m,dgroup(0x8988u)),font);u32 total=0,p=s,n=count;while(p!=0u&&mu8(b,p)!=0u&&n!=0u){total+=glyph_width_signed_10(b,font,mu8(b,p));++p;--n;}return total;} static_assert(0x7B50u-0x7AFCu==84u);
[[nodiscard]]u32 function_7b50(Machine&m,MidUi10Boundaries&b,u8 ch,u32 font_id)noexcept{const u32 font=font_ptr_10(b,font_id);store_le32(dgroup_ptr(m,dgroup(0x8988u)),font);return glyph_width_signed_10(b,font,ch);} static_assert(0x7B73u-0x7B50u==35u);
// 7B73 NOP alignment.
[[nodiscard]]u32 function_7b74(Machine&m,MidUi10Boundaries&b,u32 font_id)noexcept{const u32 font=font_ptr_10(b,font_id);store_le32(dgroup_ptr(m,dgroup(0x8988u)),font);return mu16(b,mu32(b,font)+4u);} static_assert(0x7B94u-0x7B74u==32u);

static void draw_char_10(Machine&m,MidUi10Boundaries&b,u8 ch,bool sign_char)noexcept{const u32 font=load_le32(dgroup_ptr(m,dgroup(0x8988u)));const u32 x=load_le32(dgroup_ptr(m,dgroup(0x8990u))),y=load_le32(dgroup_ptr(m,dgroup(0x898cu))),data=mu32(b,font);const s32 c=sign_char?static_cast<s32>(static_cast<std::int8_t>(ch)):static_cast<s32>(ch);const u32 extra=load_le32(dgroup_ptr(m,dgroup(0x2908u)));if(extra!=0u){if(b.call_6824)b.call_6824(c,x,y,data,extra);}else if(b.call_67c8)b.call_67c8(c,x,y,data);store_le32(dgroup_ptr(m,dgroup(0x898cu)),y+(sign_char?glyph_width_signed_10(b,font,ch):glyph_width_unsigned_10(b,font,ch)));}
void function_7b94(Machine&m,MidUi10Boundaries&b,u8 ch,u32 font_id)noexcept{store_le32(dgroup_ptr(m,dgroup(0x8988u)),font_ptr_10(b,font_id));draw_char_10(m,b,ch,true);} static_assert(0x7C09u-0x7B94u==117u);
// 7C09..7C0B LEA alignment.
void function_7c0c(Machine&m,MidUi10Boundaries&b,u32 s,u32 font_id)noexcept{store_le32(dgroup_ptr(m,dgroup(0x8988u)),font_ptr_10(b,font_id));for(u32 p=s;p!=0u&&mu8(b,p)!=0u;++p)draw_char_10(m,b,mu8(b,p),true);} static_assert(0x7CB0u-0x7C0Cu==164u);
void function_7cb0(Machine&m,MidUi10Boundaries&b,u32 s,u32 font_id,u32 count)noexcept{store_le32(dgroup_ptr(m,dgroup(0x8988u)),font_ptr_10(b,font_id));u32 p=s,n=count;while(p!=0u&&mu8(b,p)!=0u&&n!=0u){draw_char_10(m,b,mu8(b,p),true);++p;--n;}} static_assert(0x7D67u-0x7CB0u==183u);

void function_7d68(Machine&m,u32 x,u32 y)noexcept{store_le32(dgroup_ptr(m,dgroup(0x8990u)),x);store_le32(dgroup_ptr(m,dgroup(0x898cu)),y);} static_assert(0x7D7Fu-0x7D68u==23u);
// 7D7F NOP alignment.
static s32 sar1_exact_10(s32 v)noexcept{return static_cast<s32>((static_cast<u32>(v)>>1)|(static_cast<u32>(v)&0x80000000u));}
void function_7d80(Machine&m,MidUi10Boundaries&b,u32 y,u32 left,u32 span,u32 s,u32 font_id)noexcept{if(s==0u)return;store_le32(dgroup_ptr(m,dgroup(0x8988u)),font_ptr_10(b,font_id));store_le32(dgroup_ptr(m,dgroup(0x8990u)),y);const s32 width=static_cast<s32>(function_7ab4(m,b,s,font_id));store_le32(dgroup_ptr(m,dgroup(0x898cu)),left+static_cast<u32>(sar1_exact_10(static_cast<s32>(span)-width)));function_7c0c(m,b,s,font_id);} static_assert(0x7DDCu-0x7D80u==92u);
[[nodiscard]]u32 function_7ddc(Machine&m)noexcept{return load_le32(dgroup_ptr(m,dgroup(0x8990u)));} static_assert(0x7DE8u-0x7DDCu==12u);
[[nodiscard]]u32 function_7de8(Machine&m)noexcept{return load_le32(dgroup_ptr(m,dgroup(0x898cu)));} static_assert(0x7DF4u-0x7DE8u==12u);
void function_7df4(Machine&m,u32 extra)noexcept{store_le32(dgroup_ptr(m,dgroup(0x2908u)),extra);} static_assert(0x7E03u-0x7DF4u==15u);
// 7E03 NOP alignment.
void function_7e04(Machine&m,MidUi10Boundaries&b,u32 x,u32 right,u32 s,u32 font_id)noexcept{if(s==0u)return;const u32 width=function_7ab4(m,b,s,font_id);function_7d68(m,x,right-width);function_7c0c(m,b,s,font_id);} static_assert(0x7E43u-0x7E04u==63u);
// 7E43 NOP alignment.
void function_7e44(Machine&m,MidUi10Boundaries&b,u32 x,u32 y,u32 s,u32 font_id)noexcept{function_7d68(m,x,y);function_7c0c(m,b,s,font_id);} static_assert(0x7E6Bu-0x7E44u==39u);
// 7E6B NOP alignment.
void function_7e6c(Machine&m,MidUi10Boundaries&b,u32 x,u32 y,u32 s,u32 font_id,s32 remaining)noexcept{function_7d68(m,x,y);const u32 font=font_ptr_10(b,font_id);store_le32(dgroup_ptr(m,dgroup(0x8988u)),font);if(s==0u)return;u32 p=s;remaining-=glyph_width_unsigned_10(b,font,mu8(b,p));while(mu8(b,p)!=0u&&remaining>0){draw_char_10(m,b,mu8(b,p),false);++p;remaining-=glyph_width_unsigned_10(b,font,mu8(b,p));}} static_assert(0x7F41u-0x7E6Cu==213u);
// 7F41..7F4F zero/alignment island; the interrupt-facing sequence begins at 7F50.

// 7F8C..7FA4: update the interrupt tick/state dword under the encoded DA18/DA1A critical section.
void function_7f8c(Machine&m,MidUi10Boundaries&b,u32 value)noexcept{if(b.critical_da18)b.critical_da18();store_le32(dgroup_ptr(m,dgroup(0x2934u)),value);if(b.critical_da1a)b.critical_da1a();} static_assert(0x7FA5u-0x7F8Cu==25u);
// 7FA5..7FA7 LEA alignment.

// 7FA8..8028: remove one byte from the ordered 89D0 queue and refresh 291A/2919 from its tail.
void function_7fa8(Machine&m,u32 value)noexcept{u32 i=0;u8 count=*dgroup_ptr(m,dgroup(0x2930u));for(;i<count;++i)if(*dgroup_ptr(m,dgroup(0x89d0u+i))==static_cast<u8>(value)){const u32 n=static_cast<u32>(count)-i-1u;std::memmove(dgroup_ptr(m,dgroup(0x89d0u+i)),dgroup_ptr(m,dgroup(0x89d0u+i+1u)),n);--count;*dgroup_ptr(m,dgroup(0x2930u))=count;break;}u8 active=0;if(count!=0u)active=*dgroup_ptr(m,dgroup(0x89d0u+static_cast<u32>(count)-1u));*dgroup_ptr(m,dgroup(0x291au))=active;*dgroup_ptr(m,dgroup(0x2919u))=active;} static_assert(0x8029u-0x7FA8u==129u);
// 8029..802B LEA alignment.
void function_802c(Machine&m,u32 value)noexcept{u8 count=*dgroup_ptr(m,dgroup(0x2930u));for(u32 i=0;i<count;++i)if(*dgroup_ptr(m,dgroup(0x89d0u+i))==static_cast<u8>(value)){function_7fa8(m,value);count=*dgroup_ptr(m,dgroup(0x2930u));break;}*dgroup_ptr(m,dgroup(0x89d0u+count))=static_cast<u8>(value);++count;*dgroup_ptr(m,dgroup(0x2930u))=count;*dgroup_ptr(m,dgroup(0x291au))=static_cast<u8>(value);*dgroup_ptr(m,dgroup(0x2919u))=static_cast<u8>(value);} static_assert(0x808Au-0x802Cu==94u);
// 808A..808B MOV EAX,EAX alignment.
[[nodiscard]]u32 function_808c(Machine&m,u32 value)noexcept{const u8 count=*dgroup_ptr(m,dgroup(0x2930u));for(u32 i=0;i<count;++i)if(*dgroup_ptr(m,dgroup(0x89d0u+i))==static_cast<u8>(value))return 1u;return 0u;} static_assert(0x80BAu-0x808Cu==46u);
// 80BA..826F contains the second interrupt handler/state machine and remains uncounted here.

void function_8270()noexcept{} static_assert(0x8277u-0x8270u==7u); // 8277 NOP.
[[nodiscard]]u32 function_8278(Machine&m,u32 mask)noexcept{return static_cast<u32>(*dgroup_ptr(m,dgroup(0x291bu)))&mask;} static_assert(0x8289u-0x8278u==17u);
// 8289..828B LEA alignment.
[[nodiscard]]u32 function_828c(Machine&m)noexcept{const u8 v=*dgroup_ptr(m,dgroup(0x291cu));*dgroup_ptr(m,dgroup(0x291cu))=0;return v;} static_assert(0x82A1u-0x828Cu==21u);
// 82A1..82A3 LEA alignment.
[[nodiscard]]u32 function_82a4(Machine&m,MidUi10Boundaries&b)noexcept{if(b.call_9ae8)b.call_9ae8();return *dgroup_ptr(m,dgroup(0x291au))!=0u?1u:0u;} static_assert(0x82BDu-0x82A4u==25u);
// 82BD..82BF LEA alignment.
[[nodiscard]]u32 function_82c0(Machine&m,MidUi10Boundaries&b)noexcept{while(*dgroup_ptr(m,dgroup(0x291au))==0u){if(b.call_9ae8)b.call_9ae8();else break;}const u8 idx=*dgroup_ptr(m,dgroup(0x291au));const u32 v=load_le32(dgroup_ptr(m,dgroup(0x2938u+static_cast<u32>(idx)*4u)));*dgroup_ptr(m,dgroup(0x291au))=0u;return v;} static_assert(0x82ECu-0x82C0u==44u);
void function_82ec(Machine&m)noexcept{*dgroup_ptr(m,dgroup(0x2930u))=0u;*dgroup_ptr(m,dgroup(0x2919u))=0u;} static_assert(0x8301u-0x82ECu==21u);
// 8301..8303 LEA alignment.
void function_8304(Machine&m)noexcept{*dgroup_ptr(m,dgroup(0x291au))=*dgroup_ptr(m,dgroup(0x2919u));*dgroup_ptr(m,dgroup(0x291cu))=0u;} static_assert(0x831Cu-0x8304u==24u);
[[nodiscard]]u32 function_831c(Machine&m,MidUi10Boundaries&b)noexcept{if(b.call_9ae8)b.call_9ae8();const u8 idx=*dgroup_ptr(m,dgroup(0x291au));return load_le32(dgroup_ptr(m,dgroup(0x2938u+static_cast<u32>(idx)*4u)));} static_assert(0x8336u-0x831Cu==26u);
// 8336..8397 is an interrupt handler and remains uncounted.

// 8398..8420: install vectors 9,8,0 once, preserving each previous far pointer in 291E/22,292A/2E,2924/28.
void function_8398(Machine&m,MidUi10Boundaries&b)noexcept{if(*dgroup_ptr(m,dgroup(0x291du))!=0u)return;u32 off=0;u16 sel=0;if(b.get_vector_de35)b.get_vector_de35(9u,&off,&sel);store_le16(dgroup_ptr(m,dgroup(0x2922u)),sel);store_le32(dgroup_ptr(m,dgroup(0x291eu)),off);if(b.set_vector_ddfe)b.set_vector_ddfe(9u,0x80bcu,b.cs_selector);if(b.get_vector_de35)b.get_vector_de35(8u,&off,&sel);store_le16(dgroup_ptr(m,dgroup(0x292eu)),sel);store_le32(dgroup_ptr(m,dgroup(0x292au)),off);if(b.set_vector_ddfe)b.set_vector_ddfe(8u,0x7f5cu,b.cs_selector);if(b.get_vector_de35)b.get_vector_de35(0u,&off,&sel);store_le16(dgroup_ptr(m,dgroup(0x2928u)),sel);store_le32(dgroup_ptr(m,dgroup(0x2924u)),off);if(b.set_vector_ddfe)b.set_vector_ddfe(0u,0x8338u,b.cs_selector);*dgroup_ptr(m,dgroup(0x291du))=1u;} static_assert(0x8421u-0x8398u==137u);
// 8421..8423 LEA alignment.
void function_8424(Machine&m,MidUi10Boundaries&b)noexcept{if(*dgroup_ptr(m,dgroup(0x291du))==0u)return;if(b.set_vector_ddfe){b.set_vector_ddfe(9u,load_le32(dgroup_ptr(m,dgroup(0x291eu))),load_le16(dgroup_ptr(m,dgroup(0x2922u))));b.set_vector_ddfe(8u,load_le32(dgroup_ptr(m,dgroup(0x292au))),load_le16(dgroup_ptr(m,dgroup(0x292eu))));b.set_vector_ddfe(0u,load_le32(dgroup_ptr(m,dgroup(0x2924u))),load_le16(dgroup_ptr(m,dgroup(0x2928u))));}*dgroup_ptr(m,dgroup(0x291du))=0u;} static_assert(0x8483u-0x8424u==95u);
// 8483 NOP.
[[nodiscard]]u32 function_8484(Machine&m)noexcept{return *dgroup_ptr(m,dgroup(0x291du));} static_assert(0x8492u-0x8484u==14u);
// 8492..849F zero/alignment island.
[[nodiscard]]u32 function_84a0(MidUi10Boundaries&b,u32 a,u32 fill,u32 c)noexcept{const u32 byte=fill&0xffu,packed=byte*0x01010101u;if(b.call_de70)b.call_de70(a,packed,c);return a;} static_assert(0x84C2u-0x84A0u==34u);
// 84C2..84CF zero/alignment island.
[[nodiscard]]s32 function_84d0(Machine&m,MidUi10Boundaries&b)noexcept{return static_cast<s16>(mu16(b,load_le32(dgroup_ptr(m,dgroup(0x2a9cu)))));} static_assert(0x84DFu-0x84D0u==15u);
// 84DF NOP.
[[nodiscard]]u32 function_84e0(Machine&m,MidUi10Boundaries&b)noexcept{return mu16(b,load_le32(dgroup_ptr(m,dgroup(0x2a9cu)))+4u)==1u?1u:0u;} static_assert(0x84F7u-0x84E0u==23u);
// 84F7 NOP.
[[nodiscard]]s32 function_84f8(Machine&m,MidUi10Boundaries&b)noexcept{const u32 p=load_le32(dgroup_ptr(m,dgroup(0x2a9cu)));return mu16(b,p)!=0u?static_cast<s16>(mu16(b,p+2u)):0;} static_assert(0x8514u-0x84F8u==28u);
void function_8514(MidUi10Boundaries&b,s32 x,s32 y,s32 w,s32 h,u8 color)noexcept{if(b.call_65c0)b.call_65c0(x,y,h,color);if(w>2&&b.call_65e8)b.call_65e8(x+1,y,w-2,color);if(w>1&&b.call_65c0)b.call_65c0(x+w-1,y,h,color);if(w>2&&h>1&&b.call_65e8)b.call_65e8(x+1,y+h-1,w-2,color);} static_assert(0x85A7u-0x8514u==147u);
// 85A7 NOP.
void function_85a8(MidUi10Boundaries&b,u32 a,u32 object)noexcept{const u32 n=mu16(b,object+2u);for(u32 i=0;i<n;++i)if(b.call_ae0c)b.call_ae0c(i,a,object);} static_assert(0x85D3u-0x85A8u==43u);
// 85D3 NOP.

void function_85d4(MidUi10Boundaries&b,s32 x,s32 y,s32 start,s32 stop,u32 object)noexcept{if(stop==-1)stop=static_cast<s32>(mu16(b,object+2u));else stop+=start;const s32 stride=static_cast<s32>(mu16(b,object+4u));for(s32 i=start;i<stop;++i){if(b.call_6d54)b.call_6d54(i,x,y,object);x+=stride;}} static_assert(0x862Cu-0x85D4u==88u);
void function_862c(MidUi10Boundaries&b,s32 x,s32 y,u32 object)noexcept{function_85d4(b,x,y,0,-1,object);} static_assert(0x864Bu-0x862Cu==31u);
// 864B NOP.
void function_864c(MidUi10Boundaries&b,s32 x,s32 y,s32 start,s32 stop,u32 object)noexcept{if(stop==-1)stop=static_cast<s32>(mu16(b,object+2u));else stop+=start;const s32 stride=static_cast<s32>(mu16(b,object+4u));for(s32 i=start;i<stop;++i){if(b.call_67c8)b.call_67c8(i,static_cast<u32>(x),static_cast<u32>(y),object);x+=stride;}} static_assert(0x86A4u-0x864Cu==88u);
void function_86a4(MidUi10Boundaries&b,s32 x,s32 y,u32 object)noexcept{function_864c(b,x,y,0,-1,object);} static_assert(0x86C3u-0x86A4u==31u);
// 86C3 NOP.
void function_86c4(MidUi10Boundaries&b,s32 x,s32 y,u32 resource,u32 variant)noexcept{const u32 object=b.call_954c?b.call_954c(resource,variant):0u;function_864c(b,x,y,0,-1,object);} static_assert(0x86F0u-0x86C4u==44u);
void function_86f0(MidUi10Boundaries&b,u32 object)noexcept{const u32 n=mu16(b,object+2u);for(u32 i=0;i<n;++i)if(b.call_a5c8)b.call_a5c8(i,object);} static_assert(0x8717u-0x86F0u==39u);
// 8717 NOP.
void function_8718(MidUi10Boundaries&b,u32 object)noexcept{const u32 n=mu16(b,object+2u);for(u32 i=0;i<n;++i)if(b.call_a678)b.call_a678(i,object);} static_assert(0x873Fu-0x8718u==39u);
// 873F NOP.
void function_8740(Machine&m,MidUi10Boundaries&b,u8 page)noexcept{u8 depth=*dgroup_ptr(m,dgroup(0x2b20u));if(depth==0x0au&&b.call_9814)b.call_9814(0x208u);*dgroup_ptr(m,dgroup(0x89ecu+depth))=*dgroup_ptr(m,dgroup(0x23e5u));++depth;*dgroup_ptr(m,dgroup(0x2b20u))=depth;if(b.call_6588)b.call_6588(page);} static_assert(0x8787u-0x8740u==71u);
// 8787 NOP.
void function_8788(Machine&m,MidUi10Boundaries&b)noexcept{u8 depth=*dgroup_ptr(m,dgroup(0x2b20u));if(depth==0u&&b.call_9814)b.call_9814(0x21cu);--depth;*dgroup_ptr(m,dgroup(0x2b20u))=depth;if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x89ecu+depth)));} static_assert(0x87C2u-0x8788u==58u);


// CC1C..CD96: startup recovery scaffolding.
enum class StartupPath:u32{FallThroughCcde=0xCCDEu,BranchCd17=0xCD17u};
[[nodiscard]] StartupPath translate_cc1c_ccdd(Machine&m)noexcept{m.cpu.interrupt_enable=true;m.cpu.esp&=0xfffffffcu;m.cpu.ebx=m.cpu.esp;store_le32(dgroup_ptr(m,dgroup(0x5304)),m.cpu.ebx);store_le32(dgroup_ptr(m,dgroup(0x52f0)),m.cpu.ebx);set_low16(m.cpu.eax,0x24);store_le16(dgroup_ptr(m,dgroup(0x52fc)),low16(m.cpu.eax));m.cpu.ebx=0x50484152;m.cpu.eax=0;set_high8(m.cpu.eax,0x30);interrupt(m,0x21);*dgroup_ptr(m,dgroup(0x5326))=low8(m.cpu.eax);*dgroup_ptr(m,dgroup(0x5327))=high8(m.cpu.eax);m.cpu.ecx=m.cpu.eax;m.cpu.esi=0;m.cpu.edi=0x81;m.cpu.eax>>=16;return low16(m.cpu.eax)!=0x4458?StartupPath::BranchCd17:StartupPath::FallThroughCcde;}
[[nodiscard]]u32 translate_ccde_cd16(Machine&m,u32 es005c)noexcept{const u8 bl=static_cast<u8>(low8(m.cpu.ebx)-0x30);set_low8(m.cpu.ebx,bl);set_low8(m.cpu.eax,bl);const u32 saved=m.cpu.eax;m.cpu.es=load_le16(dgroup_ptr(m,dgroup(0x52fc)));m.cpu.ebx=es005c;m.cpu.ebx=(m.cpu.ebx+0xfff)&0xfffff000;store_le32(dgroup_ptr(m,dgroup(0x52f0)),m.cpu.ebx);m.cpu.ebx>>=12;set_low16(m.cpu.eax,m.cpu.ds);m.cpu.es=m.cpu.ds;set_high8(m.cpu.eax,0x4a);interrupt(m,0x21);m.cpu.eax=saved;set_low16(m.cpu.ebx,m.cpu.ds);set_low16(m.cpu.ecx,0x2c);return 0xcd4e;}
enum class Cd17Path:u32{BranchCd50=0xcd50,JumpCd97=0xcd97};
[[nodiscard]]Cd17Path translate_cd17_cd4f(Machine&m)noexcept{if(low16(m.cpu.eax)!=0x4243)return Cd17Path::BranchCd50;store_le32(dgroup_ptr(m,dgroup(0x3094)),m.cpu.edx);m.cpu.esi=m.cpu.edx;m.cpu.edx=read32(m,linear(m.cpu.esi+0x1c));m.cpu.ebx=m.cpu.esp-m.cpu.edx;set_high8(m.cpu.eax,0x4a);interrupt(m,0x21);set_low16(m.cpu.ebx,m.cpu.ds);store_le16(dgroup_ptr(m,dgroup(0x52fc)),m.cpu.ds);m.cpu.eax=read32(m,linear(m.cpu.esi+0x10));m.cpu.edi+=m.cpu.eax;m.cpu.esi=0;set_low16(m.cpu.esi,read16(m,linear(m.cpu.eax+0x2c)));m.cpu.esi<<=4;set_low16(m.cpu.ecx,m.cpu.ds);set_low8(m.cpu.eax,9);return Cd17Path::JumpCd97;}
[[nodiscard]]u32 translate_cd50_cd96(Machine&m)noexcept{set_low16(m.cpu.edx,0x78);set_low16(m.cpu.eax,0xff00);interrupt(m,0x21);if(low8(m.cpu.eax)){set_low16(m.cpu.eax,m.cpu.gs);if(low16(m.cpu.eax))store_le16(dgroup_ptr(m,dgroup(0x3098)),low16(m.cpu.eax));set_low8(m.cpu.eax,1);set_low16(m.cpu.ebx,m.cpu.ds);store_le16(dgroup_ptr(m,dgroup(0x52fc)),m.cpu.es);set_low16(m.cpu.ecx,read16(m,selector_offset(m.cpu.es,0x2c)));return 0xcd97;}set_low16(m.cpu.edx,m.cpu.ds);set_low16(m.cpu.ecx,0x24);m.cpu.ds=low16(m.cpu.ecx);set_low16(m.cpu.ecx,read16(m,selector_offset(m.cpu.ds,0x2c)));m.cpu.ds=low16(m.cpu.edx);set_low16(m.cpu.ebx,0x17);set_low8(m.cpu.eax,0);return 0xcd97;}


// 0010..0048: accumulation helper. 618A supplies the original per-iteration value; signed loop/count and 32-bit overflow behavior are preserved.
struct EarlyGameBoundaries{u32(*call_618a)()=nullptr;s32(*seek_633c)(u32,u32,u32)=nullptr;s32(*read_6304)(void*,u32,u32)=nullptr;};
[[nodiscard]]s32 function_0010(EarlyGameBoundaries&b,s32 count,s32 scale,s32 add_each,s32 add_final)noexcept{u32 total=0;for(s32 i=0;i<count;++i){u32 v=b.call_618a?b.call_618a():0u;v=static_cast<u32>(v*static_cast<u32>(scale));v>>=15;++v;total+=v;total+=static_cast<u32>(add_each);}total+=static_cast<u32>(add_final);return static_cast<s32>(total)<1?1:static_cast<s32>(total);} static_assert(0x49u-0x10u==57u);
// 0049..004B is LEA alignment padding.
// 004C..00EA: indexed record reader/deobfuscator using the handle stored at DGROUP+04C8. The payload bytes are XORed with DCh in reverse order exactly as encoded.
void function_004c(Machine&m,EarlyGameBoundaries&b,u8*out,u32 index)noexcept{const u32 h=load_le32(dgroup_ptr(m,dgroup(0x04c8u)));u32 offset=index*4u+4u;if(b.seek_633c)(void)b.seek_633c(offset,0u,h);u32 payload_off=0;if(b.read_6304)(void)b.read_6304(&payload_off,4u,h);if(b.seek_633c)(void)b.seek_633c(payload_off,0u,h);u16 n=0;if(b.read_6304)(void)b.read_6304(&n,2u,h);if(b.read_6304)(void)b.read_6304(out,n,h);out[n]=0;while(n!=0u){out[n-1u]^=0xdcu;--n;}} static_assert(0xEBu-0x4Cu==159u);

// 035C..043A: screen-coordinate hit test over the nine 8-byte coordinate records at 087C/0880, plus the two encoded special rectangles.
[[nodiscard]]s32 function_035c(Machine&m,s32 x,s32 y)noexcept{s32 result=-1;const s32 mode=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x04d0u))));s32 i=(mode==6)?6:0;const s32 limit=(mode==6)?9:6;for(;i<limit;++i){const u32 q=0x087cu+static_cast<u32>(i)*8u;const s32 left=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(q))));const s32 top=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(q+4u))));if(x>=left&&x<left+0x7a&&y>=top&&y<top+0x30)result=i+1;}if(x>=0x114&&x<0x18e&&y>=0x10c&&y<0x13c)result=0;if((mode==3||mode==5)){const s32 left=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08e4u))));const s32 top=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08a0u))));if(x>=left&&x<left+0x7a&&y>=top&&y<top+0x30)result=0xff;}return result;} static_assert(0x43Bu-0x35Cu==223u);
// 043B is one-byte NOP alignment.
struct EarlyUiBoundaries{u32(*call_53b4)()=nullptr;u32(*call_831c)()=nullptr;void(*call_6eb0)(s32,s32,s32,s32,u32,u32)=nullptr;void(*call_6c60)(s32,s32,s32,s32,u32)=nullptr;void(*call_7d80)(s32,s32,u32,u32,u32)=nullptr;void(*call_67c8)(u32,s32,s32,u32)=nullptr;void(*call_38a4)(s32,s32)=nullptr;void(*call_00ec)(s32,s32,s32,s32,u32,u32,u32)=nullptr;};
// 043C..04AF: poll 53B4 until an event is available; mode 6 recognizes the two 831C key codes, otherwise the event coordinates are passed through 035C.
[[nodiscard]]s32 function_043c(Machine&m,EarlyUiBoundaries&b)noexcept{while((b.call_53b4?b.call_53b4():0u)!=0u){}for(;;){if((b.call_53b4?b.call_53b4():0u)==0u){if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==6u){if((b.call_831c?b.call_831c():0u)==0x14eu)return 7;if((b.call_831c?b.call_831c():0u)==0x14au)return 9;}continue;}const s32 x=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e6u))));const s32 y=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e8u))));store_le32(dgroup_ptr(m,dgroup(0x04d4u)),static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73eau)))))));const s32 hit=function_035c(m,x,y);if(hit>=0)return hit;}} static_assert(0x4B0u-0x43Cu==116u);

// 04B0..0648: staged UI painter. Calls retain their original argument order; all data-dependent byte loads use the exact DGROUP offsets encoded in CODE.1.
void function_04b0(Machine&m,EarlyUiBoundaries&b,s32 stage)noexcept{
 const s32 actor=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x04ccu))));const u32 base=static_cast<u32>(actor)*0x248u;
 if(stage>=1&&b.call_6eb0)b.call_6eb0(0,0x114,3,static_cast<s32>(*dgroup_ptr(m,dgroup(base+0x680bu)))+0x0c,7,0x5ff8u);
 if(stage>=2&&b.call_6eb0)b.call_6eb0(0,0x114,0x38,*dgroup_ptr(m,dgroup(base+0x680cu)),7,0x5ff8u);
 if(stage>=3)for(s32 i=2;i>=0;--i){const std::int8_t v=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(base+0x680du+static_cast<u32>(i))));if(v!=-1&&b.call_6eb0)b.call_6eb0(0,0x114-i*0x14,0x6d-i,static_cast<s32>(v)+6,7,0x5ff8u);}
 if(stage>=4&&b.call_6eb0)b.call_6eb0(0,0x114,0xa2,static_cast<s32>(*dgroup_ptr(m,dgroup(base+0x6812u)))+0x0e,7,0x5ff8u);
 if(stage>=5){if(b.call_6c60)b.call_6c60(0,0x114,0xd7,0x14,7);const u32 v=static_cast<u32>(*dgroup_ptr(m,dgroup(base+0x680bu)))*0x17u+load_le32(dgroup_ptr(m,dgroup(0x713cu)))+static_cast<s32>(static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(base+0x680au))));if(b.call_6c60)b.call_6c60(0,0x129,0xd7,static_cast<s32>(v),1);if(*dgroup_ptr(m,dgroup(static_cast<u32>(actor)+0x04c4u))!=0u&&b.call_7d80)b.call_7d80(0x116,0xd7,0x30u,static_cast<u32>(actor)*0x248u+0x67f8u,0u);}
 if(stage>=6&&b.call_38a4)b.call_38a4(0x114,0x10c);
} static_assert(0x649u-0x4B0u==409u);
// 0649..064B is LEA alignment padding.
// 064C..07E1: render the nine coordinate/status slots, then the two optional 0940/0948 records.
void function_064c(Machine&m,EarlyUiBoundaries&b,EarlyGameBoundaries&io)noexcept{
 for(s32 i=0;i<9;++i){const u32 q=0x08f8u+static_cast<u32>(i)*8u;const s32 state=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(q))));if(state==-1)continue;const s32 x=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+static_cast<u32>(i)*8u))));const s32 y=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+static_cast<u32>(i)*8u))));
  if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&state==1){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0954u+static_cast<u32>(i)*4u))));continue;}
  if(i==7&&state!=0x14){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0958u))));continue;}
  const s32 pivot=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu))));if(state>=pivot&&state<pivot+0x2e){if(b.call_6c60)b.call_6c60(0,x,y,0x14,7);if(b.call_6eb0)b.call_6eb0(0,x+0x15,y,state,1,0x5af8u);}else if(b.call_6eb0)b.call_6eb0(0,x,y,state,7,0x5ff8u);
 }
 u8 tmp[256]{};for(const u32 slot:{0x0940u,0x0948u}){const u32 rec=load_le32(dgroup_ptr(m,dgroup(slot)));if(rec==0xffffffffu)continue;function_004c(m,io,tmp,rec);if(b.call_00ec)b.call_00ec(0x114,slot==0x0940u?0x10c:0xd7,0x7a,0x30,0x55f8u,0u,0x5ff8u);}
} static_assert(0x7E2u-0x64Cu==406u);

struct MouseRegs{u16 ax=0,bx=0,cx=0,dx=0;}; struct MouseSnapshot{u16 buttons;u16 reserved1;s16 x_half;u16 y;}; static_assert(sizeof(MouseSnapshot)==8);
struct NativeBoundaries{bool(*call_84d0)()=nullptr;void(*mouse_int33)(MouseRegs&)=nullptr;void(*call_8960)()=nullptr;u32(*call_82a4)()=nullptr;void(*call_82ec)()=nullptr;void(*call_8304)()=nullptr;void(*call_82c0)()=nullptr;};
void function_d08c(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=4;r.dx=static_cast<u16>(a);r.cx=static_cast<u16>(c<<1);if(b.mouse_int33)b.mouse_int33(r);}
static_assert(0xD0B1u-0xD08Cu==37u);
// D0B1..D0B3: excluded by the original 274-byte structured-mouse fold; exact non-code subtype is not preserved.
[[nodiscard]]MouseSnapshot*function_d0b4(NativeBoundaries&b,u8*d)noexcept{auto*out=reinterpret_cast<MouseSnapshot*>(d+0x8a60);if(!b.call_84d0||!b.call_84d0())return out;MouseRegs r{};r.ax=3;if(b.mouse_int33)b.mouse_int33(r);out->buttons=r.bx;out->y=r.dx;out->x_half=static_cast<s16>(static_cast<s16>(r.cx)>>1);return out;}
static_assert(0xD109u-0xD0B4u==85u);
// D109..D10B: excluded by the original 274-byte structured-mouse fold; exact non-code subtype is not preserved.
void function_d10c(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=7;r.cx=static_cast<u16>(a);r.dx=static_cast<u16>(c);if(b.mouse_int33)b.mouse_int33(r);}
static_assert(0xD12Fu-0xD10Cu==35u);
// D12F: excluded by the original 274-byte structured-mouse fold; exact non-code subtype is not preserved.
void function_d130(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=8;r.cx=static_cast<u16>(a);r.dx=static_cast<u16>(c);if(b.mouse_int33)b.mouse_int33(r);}
static_assert(0xD153u-0xD130u==35u);
// D153: excluded by the original 274-byte structured-mouse fold; exact non-code subtype is not preserved.
// D154..D178: drain mouse snapshots while the first returned word remains nonzero.
void function_d154(Machine&m,NativeBoundaries&b)noexcept{if(!b.call_84d0||!b.call_84d0())return;for(;;){MouseSnapshot*p=function_d0b4(b,m.data);if(b.call_8960)b.call_8960();if(p->buttons==0)return;}}
// D179..D17B is LEA EAX,[EAX+0] alignment padding and is intentionally not translated.
// D17C..D1E7: exact repeated 82A4/D0B4/8960 polling sequence; D1B6 calls 82A4 again.
void function_d17c(Machine&m,NativeBoundaries&b)noexcept{if(b.call_84d0&&b.call_84d0()){for(;;){MouseSnapshot*p=function_d0b4(b,m.data);if(b.call_8960)b.call_8960();if(p->buttons==0)break;}for(;;){const u32 ready=b.call_82a4?b.call_82a4():0;if(ready!=0)break;MouseSnapshot*p=function_d0b4(b,m.data);if(p->buttons!=0)break;if(b.call_8960)b.call_8960();}const u32 ready=b.call_82a4?b.call_82a4():0;if(ready!=0){if(b.call_82ec)b.call_82ec();if(b.call_8304)b.call_8304();}else{for(;;){if(b.call_8960)b.call_8960();MouseSnapshot*p=function_d0b4(b,m.data);if(p->buttons==0)break;}}}else if(b.call_82c0)b.call_82c0();}
void*function_d1e8(void*dst,const void*src,u32 n)noexcept{return std::memmove(dst,src,static_cast<std::size_t>(n));}
static_assert(0xD23Au-0xD1E8u==82u);


// Batch 12: directly reconciled CODE.1 runtime/startup/asset-dispatch regions.
// Hardware port, far-call, selector, and interrupt-frame effects remain explicit boundary operations.
struct Batch12Boundaries{
 u8(*read8)(u32)=nullptr;u16(*read16)(u32)=nullptr;u32(*read32)(u32)=nullptr;
 void(*write8)(u32,u8)=nullptr;void(*write16)(u32,u16)=nullptr;void(*write32)(u32,u32)=nullptr;
 u8(*in8)(u16)=nullptr;void(*out8)(u16,u8)=nullptr;void(*out16)(u16,u16)=nullptr;
 void(*sync_af08)()=nullptr;void(*sync_af34)(u32)=nullptr;void(*call_af64)(u32)=nullptr;void(*call_af80)(u32)=nullptr;
 u8(*bios_get_mode)()=nullptr;void(*bios_set_mode)(u8)=nullptr;
 void(*translate_aea8)(u32,u32,u32*,u8*)=nullptr;
 void(*asset_bad_index)(s32,u32)=nullptr;void(*asset_preload)(s32,u32)=nullptr;
 void(*asset_header_8d0c)(u8*,s32,u32,u32)=nullptr;
 u32(*asset_load[7])(const u8*,u32)={};void(*fatal_9814)(u32)=nullptr;void(*free_7240)(u32)=nullptr;u32(*tick_7f50)()=nullptr;
 void(*planar_fill)(u32,u32,u32,u8,u8,u8,u32,u8)=nullptr;
 void(*call_86a4)(u32,u32,u32)=nullptr;void(*call_c7a0)(u32,u32,u32,u32,u32)=nullptr;void(*call_c8c4)(u32,u32,u32,u32,u32)=nullptr;
 void(*call_cacc)(u32,u32,u32,u32,u32,u32,u32)=nullptr;void(*call_c9d4)(u32,u32,u32,u32,u32,u32,u32)=nullptr;
 void(*video_copy)(u32,u32,u32,bool)=nullptr;
};
static u8 b12r8(Batch12Boundaries&b,u32 a)noexcept{return b.read8?b.read8(a):0u;} static u16 b12r16(Batch12Boundaries&b,u32 a)noexcept{return b.read16?b.read16(a):0u;} static u32 b12r32(Batch12Boundaries&b,u32 a)noexcept{return b.read32?b.read32(a):0u;}
static void b12w8(Batch12Boundaries&b,u32 a,u8 v)noexcept{if(b.write8)b.write8(a,v);} static void b12w16(Batch12Boundaries&b,u32 a,u16 v)noexcept{if(b.write16)b.write16(a,v);} static void b12w32(Batch12Boundaries&b,u32 a,u32 v)noexcept{if(b.write32)b.write32(a,v);}

// 6060..60B1: save DGROUP 2B28..2B7C and VGA sequencer/graphics-controller index/data state.
void function_6060(Machine&m,Batch12Boundaries&b)noexcept{for(s32 i=0x54;i>=0;--i)*dgroup_ptr(m,dgroup(0x136eu+static_cast<u32>(i)))=*dgroup_ptr(m,dgroup(0x2b28u+static_cast<u32>(i)));auto in=[&](u16 p){return b.in8?b.in8(p):0u;};auto out=[&](u16 p,u8 v){if(b.out8)b.out8(p,v);};*dgroup_ptr(m,dgroup(0x136au))=in(0x3c4);out(0x3c4,2);*dgroup_ptr(m,dgroup(0x1369u))=in(0x3c5);*dgroup_ptr(m,dgroup(0x136cu))=in(0x3ce);out(0x3ce,5);*dgroup_ptr(m,dgroup(0x136bu))=in(0x3cf);out(0x3ce,4);*dgroup_ptr(m,dgroup(0x136du))=in(0x3cf);} static_assert(0x60B2u-0x6060u==82u);
// 60B4..6100: exact inverse restore sequence.
void function_60b4(Machine&m,Batch12Boundaries&b)noexcept{for(s32 i=0x54;i>=0;--i)*dgroup_ptr(m,dgroup(0x2b28u+static_cast<u32>(i)))=*dgroup_ptr(m,dgroup(0x136eu+static_cast<u32>(i)));if(b.out16)b.out16(0x3c4,static_cast<u16>(2u|(u16(*dgroup_ptr(m,dgroup(0x1369u)))<<8)));if(b.out8)b.out8(0x3c4,*dgroup_ptr(m,dgroup(0x136au)));if(b.out16){b.out16(0x3ce,static_cast<u16>(5u|(u16(*dgroup_ptr(m,dgroup(0x136bu)))<<8)));b.out16(0x3ce,static_cast<u16>(4u|(u16(*dgroup_ptr(m,dgroup(0x136du)))<<8)));}if(b.out8)b.out8(0x3ce,*dgroup_ptr(m,dgroup(0x136cu)));} static_assert(0x6101u-0x60B4u==77u);
struct Interrupt6104Boundary{u16(*restore_ds_cec0)()=nullptr;void(*call_50c0)(u32,u32,u32,u32,u32,u32)=nullptr;u16 current_ss=0;u32 current_esp=0;};
// 6104..6183: far interrupt wrapper. Host stack switching is represented explicitly by saved SS:ESP fields.
void function_6104(Machine&m,Batch12Boundaries&v,Interrupt6104Boundary&b,u32 eax,u32 ebx,u32 ecx,u32 edx,u32 esi)noexcept{if(*dgroup_ptr(m,dgroup(0x1368u))!=0u)return;*dgroup_ptr(m,dgroup(0x1368u))=1;store_le16(dgroup_ptr(m,dgroup(0x23c3u)),b.current_ss);store_le32(dgroup_ptr(m,dgroup(0x23c5u)),b.current_esp);const u16 ds=b.restore_ds_cec0?b.restore_ds_cec0():0u;(void)ds;store_le32(dgroup_ptr(m,dgroup(0x136eu)),eax);function_6060(m,v);if(b.call_50c0)b.call_50c0(eax,ebx,ecx,edx,(esi&0xffffu)<<4,0x23bfu);function_60b4(m,v);b.current_esp=load_le32(dgroup_ptr(m,dgroup(0x23c5u)));b.current_ss=load_le16(dgroup_ptr(m,dgroup(0x23c3u)));*dgroup_ptr(m,dgroup(0x1368u))=0;} static_assert(0x6184u-0x6104u==128u);

struct Startup12Boundary{u16 ds=0;u8(*read8)(u16,u32)=nullptr;u16(*read16)(u16,u32)=nullptr;u32(*read32)(u16,u32)=nullptr;void(*write16)(u16,u32,u16)=nullptr;void(*runtime_call)(u32)=nullptr;void(*int33)(u32&,u32&,u32&,u32&)=nullptr;u32(*call3)(u32,u32,u32,u32)=nullptr;};
struct StartupRegs12{u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0,esp=0;u16 ds=0,es=0;};
// CD97..CE8A: PSP command tail/environment startup path. CE8B..CEBF remains excluded.
void function_cd97(Machine&m,Startup12Boundary&b,StartupRegs12&r)noexcept{b.ds=r.ds;*dgroup_ptr(m,dgroup(0x531eu))=static_cast<u8>(r.eax);r.es=static_cast<u16>(r.ebx);if(b.write16)b.write16(r.es,0xcc90u,r.ds);store_le32(dgroup_ptr(m,dgroup(0x5320u)),r.esi);store_le16(dgroup_ptr(m,dgroup(0x5324u)),static_cast<u16>(r.ecx));const u32 saved_esi=r.esi;r.es=load_le16(dgroup_ptr(m,dgroup(0x52fcu)));r.edx=(0x8e9bu&~0xffu)|((0x8e9bu&0xffu)&0xf0u);r.ecx=b.read8?b.read8(r.es,r.edi-1u):0u;while(r.ecx&&b.read8){u8 c=b.read8(r.es,r.edi++);--r.ecx;if(c!=0x20u)break;}r.esi=r.edi-1u;r.edi=r.edx;const u16 psp=r.es;r.es=r.ds;if(r.ecx){++r.ecx;while(r.ecx--){*dgroup_ptr(m,dgroup(r.edi++))=b.read8?b.read8(psp,r.esi):0u;++r.esi;}}*dgroup_ptr(m,dgroup(r.edi++))=0;*dgroup_ptr(m,dgroup(r.edi++))=0;r.esi=saved_esi;--r.edi;const u16 env=load_le16(dgroup_ptr(m,dgroup(0x5324u)));r.ebp=0;for(;;){u32 first=b.read32?b.read32(env,r.esi):0u;if((first|0x20202020u)==0x37386f6eu&&b.read8&&b.read8(env,r.esi+4u)=='=')++r.ebp;while(b.read8&&b.read8(env,r.esi)!=0u)++r.esi;++r.esi;if(!b.read8||b.read8(env,r.esi)==0u)break;}r.esi+=2u;while(b.read8){u8 c=b.read8(env,r.esi++);*dgroup_ptr(m,dgroup(r.edi++))=c;if(c==0u)break;}store_le16(dgroup_ptr(m,dgroup(0x531cu)),static_cast<u16>(r.ebp));store_le32(dgroup_ptr(m,dgroup(0x5300u)),r.edi);store_le32(dgroup_ptr(m,dgroup(0x52ecu)),r.esp);u32 n=0x8e8cu-0x55f8u;if(*dgroup_ptr(m,dgroup(0x531eu))==1u&&n>0x1000u)n=0x1000u;std::memset(dgroup_ptr(m,dgroup(0x55f8u)),0,n);u32 aligned=0x8e9bu;aligned=(aligned&~0xffu)|((aligned&0xffu)&0xf0u);store_le32(dgroup_ptr(m,dgroup(0x52f4u)),aligned);store_le32(dgroup_ptr(m,dgroup(0x52f8u)),r.esi);if(b.runtime_call)b.runtime_call(0xfce8u);r.ebp=0;if(b.runtime_call)b.runtime_call(0xfc98u);} static_assert(0xCE8Bu-0xCD97u==244u);
[[nodiscard]]u16 function_cec0(u16 saved_ds)noexcept{return saved_ds;} static_assert(0xCEC8u-0xCEC0u==8u);
struct RegBridge12{u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0;u16 ds=0,es=0,fs=0,gs=0;u32 flags=0;};
struct RegisterBridge12Boundary{void(*invoke_short)(u32,RegBridge12&,bool*)=nullptr;void(*invoke_full)(u32,RegBridge12&)=nullptr;};
// FEFC..FFD3 register/segment adapters. Veneer address is FFD4+3*vector.
void function_fefc(RegisterBridge12Boundary&b,u32 service,RegBridge12&image)noexcept{bool carry=false;if(b.invoke_short)b.invoke_short(service,image,&carry);image.flags=carry?0xffffffffu:0u;} void function_ff57(RegisterBridge12Boundary&b,u32 vector,RegBridge12&image)noexcept{if(b.invoke_full)b.invoke_full(vector&0xffu,image);} static_assert(0xFFD4u-0xFEFCu==216u);
// CEC8..CEE8: four-register adapter around FEFC; return dword through restored DS:EDX is boundary-owned.
[[nodiscard]]u32 function_cec8(Startup12Boundary&b,RegisterBridge12Boundary&rb,u32 esi,u32 edi,u32 edx,u32 ebx)noexcept{RegBridge12 r{};r.esi=esi;r.edi=edi;r.edx=edx;r.ebx=ebx;r.ds=b.ds;function_fefc(rb,esi,r);return b.read32?b.read32(b.ds,edx):0u;} static_assert(0xCEE9u-0xCEC8u==33u);
struct Cef0Locals12{u32 regs[10]{};u8 far_cell[12]{};};
void function_cef0(Startup12Boundary&b,RegisterBridge12Boundary&rb,u32 caller,Cef0Locals12&l,u32 regs_guest,u32 far_guest)noexcept{if(b.call3)(void)b.call3(0x84a0u,far_guest,0,12);l.regs[0]=0x300;l.regs[1]=0x33;l.regs[2]=0;l.regs[5]=caller;l.far_cell[0]=static_cast<u8>(b.ds);l.far_cell[1]=static_cast<u8>(b.ds>>8);(void)function_cec8(b,rb,0x31u,regs_guest,regs_guest,far_guest);} static_assert(0xCF41u-0xCEF0u==81u);
[[nodiscard]]bool function_cf44(Machine&m,Startup12Boundary&b,RegisterBridge12Boundary&rb,Cef0Locals12&l,u32 regs_guest,u32 far_guest)noexcept{if(b.call3)(void)b.call3(0x84a0u,0x8a28u,0,0x32u);store_le32(dgroup_ptr(m,dgroup(0x8a44u)),0x53c1u);store_le16(dgroup_ptr(m,dgroup(0x8a4au)),static_cast<u16>(load_le32(dgroup_ptr(m,dgroup(0x30a8u)))>>16));store_le32(dgroup_ptr(m,dgroup(0x8a3cu)),0u);function_cef0(b,rb,0x8a28u,l,regs_guest,far_guest);return load_le32(dgroup_ptr(m,dgroup(0x8a44u)))==1u;} static_assert(0xCF98u-0xCF44u==84u);
void function_cf98(Machine&m,Startup12Boundary&b,RegisterBridge12Boundary&rb,Cef0Locals12&l,u32 regs_guest,u32 far_guest)noexcept{u32 eax=0,ebx=0,ecx=0,edx=0;if(b.int33)b.int33(eax,ebx,ecx,edx);store_le16(dgroup_ptr(m,dgroup(0x8a5au)),static_cast<u16>(eax));store_le16(dgroup_ptr(m,dgroup(0x8a5cu)),static_cast<u16>(ebx));store_le16(dgroup_ptr(m,dgroup(0x8a5eu)),0);if(static_cast<u16>(eax)!=0u){u32 p=b.call3?b.call3(0x6f10u,0x40u,0x30a8u,0x30acu):0u;store_le32(dgroup_ptr(m,dgroup(0x30a4u)),p);if(p!=0u){if(function_cf44(m,b,rb,l,regs_guest,far_guest)){u16 f=b.read8?b.read8(b.ds,p):0u;store_le16(dgroup_ptr(m,dgroup(0x8a5eu)),f);}if(load_le16(dgroup_ptr(m,dgroup(0x8a5eu)))==0u&&b.call3)(void)b.call3(0x6f74u,load_le16(dgroup_ptr(m,dgroup(0x30acu))),0,0);}}else{if(b.call3)(void)b.call3(0x61d0u,0x350u,0,0);u32 p=load_le32(dgroup_ptr(m,dgroup(0x2aa0u)));u32 pending=load_le32(dgroup_ptr(m,dgroup(0x2b84u)))|load_le32(dgroup_ptr(m,dgroup(0x2f48u)));if(load_le16(dgroup_ptr(m,dgroup(p+8u)))!=0x71u){if(load_le16(dgroup_ptr(m,dgroup(p+0x14u)))!=0u&&b.runtime_call)b.runtime_call(0xc4a0u);if(b.runtime_call)b.runtime_call(0xb430u);if(pending&&b.call3)(void)b.call3(0xe6eau,0,0,0);}if(b.runtime_call)b.runtime_call(0x8424u);if(b.runtime_call)b.runtime_call(0xcbf0u);}} static_assert(0xD08Cu-0xCF98u==244u);

struct Int24Frame12{u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0;u16 ds=0,es=0,ss=0;u32 esp=0;};
struct Int24Boundary12{u16(*restore_ds_cec0)()=nullptr;u32(*far_callback)(u32,u32,u32,u32,u16)=nullptr;};
// E2C3..E35F: IRET trampoline represented as an interrupt frame, including SS:ESP swap and far callback selector choice.
void function_e2c3(Machine&m,Int24Boundary12&b,Int24Frame12&f)noexcept{f.ds=b.restore_ds_cec0?b.restore_ds_cec0():f.ds;f.es=f.ds;store_le32(dgroup_ptr(m,dgroup(0x50fcu)),f.esp);store_le16(dgroup_ptr(m,dgroup(0x5100u)),f.ss);u32 old_top=load_le32(dgroup_ptr(m,dgroup(0x5300u)));store_le32(dgroup_ptr(m,dgroup(0x5108u)),old_top);store_le32(dgroup_ptr(m,dgroup(0x5300u)),0x8a68u);f.ss=f.ds;f.esp=0x8e68u;u32 linear=((f.ebp&0xffffu)<<4);linear=(linear&0xffff0000u)|static_cast<u16>(static_cast<u16>(linear)+static_cast<u16>(f.esi));u16 sel=0x34u;u8 mode=*dgroup_ptr(m,dgroup(0x531eu));if(mode==1u)sel=f.ds;else if(mode>=2u&&mode<=4u)sel=0x34u;u32 ret=b.far_callback?b.far_callback(f.edx&0xffffu,f.edi&0xffffu,linear,f.eax&0xffffu,sel):0u;f.eax=ret;f.esp=load_le32(dgroup_ptr(m,dgroup(0x50fcu)));f.ss=load_le16(dgroup_ptr(m,dgroup(0x5100u)));store_le32(dgroup_ptr(m,dgroup(0x5300u)),load_le32(dgroup_ptr(m,dgroup(0x5108u))));} static_assert(0xE360u-0xE2C3u==157u);

// 8E3C..91BB: seven-way asset loader dispatch through table 8E20; table bytes remain data and are not counted.
void function_8e3c(Machine&m,Batch12Boundaries&b,s32 index,u32 group)noexcept{u32 groups=load_le32(dgroup_ptr(m,dgroup(0x2b08u)));u32 gr=groups+(group<<4);if(index<0||static_cast<u32>(index)>=b12r32(b,gr+8u)){if(b.asset_bad_index)b.asset_bad_index(index,group);}u32 ent=b12r32(b,gr+0x0cu)+static_cast<u32>(index*9);if(b12r8(b,ent+2u)!=0u){u32 t=b.tick_7f50?b.tick_7f50():0u;b12w16(b,ent+3u,static_cast<u16>(t>>5));return;}u32 cb=load_le32(dgroup_ptr(m,dgroup(0x2b1cu)));if(cb&&b.asset_preload)b.asset_preload(index,group);u8 local[0x18]{};if(b.asset_header_8d0c)b.asset_header_8d0c(local,index,b12r32(b,gr),group);u8 type=local[2];u32 result=0;if(type>=1u&&type<=7u){auto fn=b.asset_load[type-1u];if(fn)result=fn(local,b12r32(b,gr));}else if(b.fatal_9814)b.fatal_9814(0x2d4u);b12w32(b,ent+5u,result);b12w8(b,ent+2u,1u);u8 flags=b12r8(b,ent);flags=static_cast<u8>((flags&0xf0u)|(type&0x0fu));flags=static_cast<u8>(flags&0xefu);b12w8(b,ent,flags);u32 temp=load_le32(local+5);if(temp&&b.free_7240)b.free_7240(temp);b12w32(b,gr+4u,b12r32(b,gr+4u)+1u);u32 t=b.tick_7f50?b.tick_7f50():0u;b12w16(b,ent+3u,static_cast<u16>(t>>5));} static_assert(0x91BCu-0x8E3Cu==896u);

// 9BE0..9C97: two ABI-preserving draw dispatch wrappers.
void function_9be0(Batch12Boundaries&b,u32 mode,u32 a,u32 c,u32 d,u32 e,u32 f)noexcept{if(a==c){if(b.call_86a4)b.call_86a4(e,f,d);}else if(mode){if(b.call_c7a0)b.call_c7a0(a,c,d,e,f);}else if(b.call_c8c4)b.call_c8c4(a,c,d,e,f);} void function_9c44(Batch12Boundaries&b,u32 mode,u32 a,u32 c,u32 d,u32 e,u32 f,u32 g,u32 h)noexcept{if(mode){if(b.call_cacc)b.call_cacc(a,c,d,e,f,g,h);}else if(b.call_c9d4)b.call_c9d4(a,c,d,e,f,g,h);} static_assert(0x9C98u-0x9BE0u==184u);
// 9CA0..9DF3: mode-13/VGA initialization; every encoded port read/modify/write is retained.
[[nodiscard]]u32 function_9ca0(Machine&m,Batch12Boundaries&b,u32 mode,u32 out)noexcept{store_le32(dgroup_ptr(m,dgroup(0x2b29u)),mode&0x80u);mode&=0x7fu;store_le32(dgroup_ptr(m,dgroup(0x2b2du)),mode);store_le32(dgroup_ptr(m,dgroup(0x2b3du)),0xc8u);store_le32(dgroup_ptr(m,dgroup(0x2b41u)),0x50u);store_le32(dgroup_ptr(m,dgroup(0x2b49u)),0xa0000u);store_le32(dgroup_ptr(m,dgroup(0x2b31u)),0u);*dgroup_ptr(m,dgroup(0x2b28u))=0;store_le32(dgroup_ptr(m,dgroup(0x2b4du)),0x400u);store_le32(dgroup_ptr(m,dgroup(0x2b35u)),4u);store_le32(dgroup_ptr(m,dgroup(0x2b39u)),2u);if(b.sync_af08)b.sync_af08();if(mode==3u)store_le32(dgroup_ptr(m,dgroup(0x2b4du)),0x800u);u8 vm=b.bios_get_mode?b.bios_get_mode():0x13u;if(vm!=0x13u&&b.bios_set_mode)b.bios_set_mode(0x13u);auto in=[&](u16 p){return b.in8?b.in8(p):0u;};auto o8=[&](u16 p,u8 v){if(b.out8)b.out8(p,v);};auto o16=[&](u16 p,u16 v){if(b.out16)b.out16(p,v);};o16(0x3ce,static_cast<u16>(3u|(u16(*dgroup_ptr(m,dgroup(0x2b28u)))<<8)));o8(0x3c4,4);u8 x=in(0x3c5);o8(0x3c5,static_cast<u8>((x&0xf7u)|4u));o8(0x3ce,5);x=in(0x3cf);o8(0x3cf,static_cast<u8>(x&0xefu));o8(0x3ce,6);x=in(0x3cf);o8(0x3cf,static_cast<u8>(x&0xfdu));if(b.sync_af08)b.sync_af08();o8(0x3d4,9);x=in(0x3d5);o8(0x3d5,mode==3u?static_cast<u8>(x&0xe0u):0x41u);o8(0x3d4,0x14);x=in(0x3d5);o8(0x3d5,static_cast<u8>(x&0xbfu));o8(0x3d4,0x17);x=in(0x3d5);o8(0x3d5,static_cast<u8>(x|0x40u));if(b.call_af64)b.call_af64(0u);if(b.call_af80)b.call_af80(0xffu);u32 height=0xc8u,width=0x140u;if(mode==3u)height<<=1;store_le32(dgroup_ptr(m,dgroup(0x2b45u)),height);store_le32(dgroup_ptr(m,dgroup(0x2b29u)),0u);b12w8(b,out+1u,2u);b12w16(b,out+3u,static_cast<u16>(height));b12w16(b,out+5u,static_cast<u16>(width));return 0x0du;} static_assert(0x9DF4u-0x9CA0u==340u);
void function_9df4(Batch12Boundaries&b)noexcept{if(b.sync_af08)b.sync_af08();if(b.bios_set_mode)b.bios_set_mode(3u);} static_assert(0x9E0Fu-0x9DF4u==27u);
void function_9e14(Machine&m,Batch12Boundaries&b,u32 x,u32 y,u8 value)noexcept{u32 p=0;u8 plane=0;if(b.translate_aea8)b.translate_aea8(x,y,&p,&plane);if(b.out16){b.out16(0x3ce,static_cast<u16>(4u|(u16(plane)<<8)));b.out16(0x3c4,static_cast<u16>(2u|(u16(1u<<plane)<<8)));b.out16(0x3ce,static_cast<u16>(3u|(u16(*dgroup_ptr(m,dgroup(0x2b28u)))<<8)));}b12w8(b,p,value);if(b.out16)b.out16(0x3ce,3u);} static_assert(0x9E60u-0x9E14u==76u);
[[nodiscard]]u8 function_9e60(Batch12Boundaries&b,u32 x,u32 y)noexcept{u32 p=0;u8 plane=0;if(b.translate_aea8)b.translate_aea8(x,y,&p,&plane);if(b.out16)b.out16(0x3ce,static_cast<u16>(4u|(u16(plane)<<8)));return b12r8(b,p);} static_assert(0x9E89u-0x9E60u==41u);
void function_9e8c(Machine&m,u32 index)noexcept{static constexpr u8 map[4]={0,8,0x10,0x18};*dgroup_ptr(m,dgroup(0x2b28u))=map[index&3u];} static_assert(0x9EA3u-0x9E8Cu==23u);
// 9EB0..A00D: planar rectangle fill. Arithmetic/masks are translated here; bus-level plane writes are the boundary primitive.
void function_9eb0(Machine&m,Batch12Boundaries&b,s32 x,s32 y,s32 w,s32 h,u8 color)noexcept{if(w<=0||h<=0)return;u32 base=0;u8 plane=0;if(b.translate_aea8)b.translate_aea8(static_cast<u32>(x),static_cast<u32>(y),&base,&plane);u8 left=static_cast<u8>(0x0fu<<plane);u32 q=static_cast<u32>(h-1);u32 phase=q&3u;q-=phase;phase^=3u;u8 right=static_cast<u8>(~(static_cast<u8>(0xf0u>>phase)));u32 middle=q>>2;if(q<=static_cast<u32>(phase)){q+=3u;phase=q;phase^=3u;right=static_cast<u8>(~(static_cast<u8>(0xf0u>>phase)));left=static_cast<u8>(left&right);right=0;middle=0;}u32 stride=load_le32(dgroup_ptr(m,dgroup(0x2b41u)))-middle-1u-(right?1u:0u);if(b.planar_fill)b.planar_fill(base,static_cast<u32>(w),middle,left,right,color,stride,*dgroup_ptr(m,dgroup(0x2b28u)));} static_assert(0xA00Eu-0x9EB0u==350u);
// A010..A058: DAC palette stream, broken into <=C0-byte bursts with AF34 pacing.
void function_a010(Batch12Boundaries&b,u32 src,u32 first,u32 count)noexcept{if(b.out8)b.out8(0x3c8,static_cast<u8>(first));u32 p=src,n=count;while(n){u32 chunk=n>0xc0u?0xc0u:n;if(b.sync_af34)b.sync_af34(chunk);for(u32 i=0;i<chunk;++i)if(b.out8)b.out8(0x3c9,b12r8(b,p+i));p+=chunk;n-=chunk;}} static_assert(0xA059u-0xA010u==73u);

// A558..A57B: encoded image storage size, with one transparency byte per frame when key != FFh.
[[nodiscard]]u32 function_a558(u32 frames,u32 rows,u32 bytes_per_row,u32,u32 key)noexcept{u32 v=rows*bytes_per_row;v*=frames;v<<=3;if(key!=0xffu)v+=frames;return v;} static_assert(0xA57Cu-0xA558u==36u);
// A57C..A5C5: transpose four interleaved plane bytes into one encoded frame.
void function_a57c(Batch12Boundaries&b,u32 frame,u32 count,u32 src,u32 desc,u32 key)noexcept{u32 dst=desc+8u+static_cast<u32>(b12r16(b,desc))*frame;if(static_cast<u8>(key)!=0xffu)b12w8(b,dst++,static_cast<u8>(key));u32 groups=count>>2;if(groups==0)return;u32 step=groups-1u;u32 s=src;for(u32 i=0;i<groups;++i){u32 base=dst+i;for(u32 plane=0;plane<4u;++plane){b12w8(b,base+plane*groups,b12r8(b,s++));}dst=base+1u;(void)step;}} static_assert(0xA5C6u-0xA57Cu==74u);
// A5C8..A674: in-place vertical reversal of each plane in one frame, preserving optional key byte.
void function_a5c8(Batch12Boundaries&b,u32 frame,u32 desc)noexcept{u32 payload=desc+8u+static_cast<u32>(b12r16(b,desc))*frame;u16 rows=b12r16(b,desc+4u);u16 plane_bytes=static_cast<u16>(static_cast<u16>(b12r8(b,desc+6u))<<1);u8 flags=b12r8(b,desc+7u);if(flags&0x80u)++payload;u16 span=static_cast<u16>(rows*plane_bytes);u32 half=span;for(u32 pass=0;pass<2u;++pass){u32 lo=payload+pass*span;u32 hi=lo+static_cast<u32>(span)*3u+static_cast<u32>(plane_bytes-1u);for(u16 r=0;r<rows;++r){for(u16 c=0;c<plane_bytes;++c){u8 a=b12r8(b,lo);u8 z=b12r8(b,hi);b12w8(b,lo,z);b12w8(b,hi,a);++lo;--hi;}hi=lo+static_cast<u32>(span)*3u+static_cast<u32>(plane_bytes-1u);}half=span;(void)half;}} static_assert(0xA675u-0xA5C8u==173u);
// A678..A6EF: horizontal byte-pair reversal for all four planes of a frame.
void function_a678(Batch12Boundaries&b,u32 frame,u32 desc)noexcept{u32 p=desc+8u+static_cast<u32>(b12r16(b,desc))*frame;u32 rows=b12r16(b,desc+4u),width=b12r8(b,desc+6u);if(b12r8(b,desc+7u)&0x80u)++p;if(width==0||rows==0)return;for(u32 plane=0;plane<4u;++plane){u32 plane_base=p+plane*rows*width;for(u32 r=0;r<rows;++r){u32 l=plane_base+r*width,rr=l+width-1u;for(u32 c=0;c<(width>>1);++c){u8 a=b12r8(b,l+c),z=b12r8(b,rr-c);b12w8(b,l+c,z);b12w8(b,rr-c,a);}}}} static_assert(0xA6F0u-0xA678u==120u);

struct DosAttrResult{u16 ax=0;u16 cx=0;bool carry=false;};
struct DosHandleResult{u16 ax=0;bool carry=false;};
struct FileBoundaries{DosAttrResult(*dos_get_attr)(u32)=nullptr;u32(*call_103c3)(u32)=nullptr;u32(*call_10397)(u32)=nullptr;u32*(*call_103b7)()=nullptr;s32(*call_10314)(u16)=nullptr;u32(*call_1042d)()=nullptr;void(*call_1046f)()=nullptr;s32(*dos_read)(u16,void*,u32)=nullptr;s32(*dos_write)(u16,const void*,u32)=nullptr;s32(*dos_seek)(u16,s32,u8)=nullptr;s32(*dos_delete)(const char*)=nullptr;bool(*dos_close)(u16)=nullptr;DosHandleResult(*dos_open_raw)(u32,u8)=nullptr;DosHandleResult(*dos_create_raw)(u32,u16)=nullptr;DosHandleResult(*dos_close_raw)(u16)=nullptr;DosHandleResult(*dos_write_zero_raw)(u16)=nullptr;u8(*read_guest8)(u32)=nullptr;u32(*read_dgroup32)(u32)=nullptr;void(*set_fd_state_10415)(u32,u32)=nullptr;bool(*ioctl_get_mode)(u16,u8&)=nullptr;bool(*ioctl_set_mode)(u16,u8)=nullptr;u32(*open_core_d332)(u32,u32,u32,u32)=nullptr;DosAttrResult(*dos_find_first)(u32,u16,u32)=nullptr;DosAttrResult(*dos_find_next)(u32)=nullptr;void(*dta_pre_find_next)(u32)=nullptr;void(*dta_post_find)(u32)=nullptr;s32(*compare_guest_string)(const char*,u32)=nullptr;};




// 87C4..9BD6 selected asset/input lifecycle code, lifted directly from CODE.1.
// Address tables 8E20..8E3B and 91BC..91D7 plus alignment bytes are intentionally excluded.
struct MidAssetBoundaries{
 u8(*read8)(u32)=nullptr;u16(*read16)(u32)=nullptr;u32(*read32)(u32)=nullptr;
 void(*write8)(u32,u8)=nullptr;void(*write16)(u32,u16)=nullptr;void(*write32)(u32,u32)=nullptr;
 u32(*tick_7f50)()=nullptr;u32(*key_ready_82a4)()=nullptr;u32(*key_get_82c0)()=nullptr;u32(*mouse_ready_84d0)()=nullptr;
 u16(*mouse_word_d0b4)()=nullptr;void(*idle_9ae8)()=nullptr;void(*idle_8960)()=nullptr;
 void(*critical_df13)(u32)=nullptr;void(*critical_df73)()=nullptr;void(*print_61d0)(u32)=nullptr;
 u32(*open_62a8)(u32)=nullptr;u32(*close_62f0)(u32)=nullptr;s32(*read_6304)(u32,u32,u32)=nullptr;void(*seek_6358)(u32,u32)=nullptr;
 u32(*alloc_71c8)(u32)=nullptr;void(*free_7240)(u32)=nullptr;u32(*load_767c)(u32,u32)=nullptr;void(*free_7830)(u32)=nullptr;
 u32(*load_79c8)(u32,u32)=nullptr;u32(*load_b51c)(u32,u32)=nullptr;u32(*load_c5b0)(u32,u32)=nullptr;
 void(*free_e174)(u32)=nullptr;void(*free_e1f4)(u32)=nullptr;void(*free_e26c)(u32)=nullptr;
 void(*concat_b01a)(u32,u32)=nullptr;void(*fatal_9814)(u32)=nullptr;
 void(*record_load_8d0c)(u32,u32,u32,u32)=nullptr;u32(*install_int24_e280)(u32,u32)=nullptr;void(*install_8398)()=nullptr;void(*restore_8424)()=nullptr;
 void(*display_64dc)(u32)=nullptr;void(*zero_84a0)(u32,u32,u32)=nullptr;u32(*runtime_e360)(u32)=nullptr;
 u32(*platform_cf98)()=nullptr;void(*setup_c2d0)(u32,s32,s32,s32,s32)=nullptr;void(*setup_b0f8)(u32,s32,s32,s32,s32)=nullptr;
 void(*mouse_d10c)(u32,u32)=nullptr;void(*mouse_d130)(u32,u32)=nullptr;void(*shutdown_b430)()=nullptr;void(*shutdown_c4a0)()=nullptr;void(*shutdown_e6ea)(u32)=nullptr;
 void(*exit_ede0)()=nullptr;void(*exit_cbf0)(u32)=nullptr;
 u32(*text_width_7ab4)(u32,u32)=nullptr;void(*text_pos_7d68)(s32,s32)=nullptr;void(*text_draw_7c0c)(u32,u32)=nullptr;
 void(*capture_66c0)(u32,s32,s32,s32,s32)=nullptr;void(*restore_6744)(u32,s32,s32,s32,s32)=nullptr;void(*page_6588)(u32)=nullptr;
 void(*line_a05c)(s32,s32,s32,s32,s32)=nullptr;void(*slider_a474)(u32)=nullptr;
 void(*callback_2b1c)(u32,u32)=nullptr;void(*callback_2b18)()=nullptr;
 void(*release_type)(u32,u32,u32)=nullptr;u32(*modal_9874)(u32,u32)=nullptr;void(*exit_97d4)(u32)=nullptr;
};
static u8 ma8(MidAssetBoundaries&b,u32 a)noexcept{return b.read8?b.read8(a):0u;}static u16 ma16(MidAssetBoundaries&b,u32 a)noexcept{return b.read16?b.read16(a):0u;}static u32 ma32(MidAssetBoundaries&b,u32 a)noexcept{return b.read32?b.read32(a):0u;}static void maw8(MidAssetBoundaries&b,u32 a,u8 v)noexcept{if(b.write8)b.write8(a,v);}static void maw16(MidAssetBoundaries&b,u32 a,u16 v)noexcept{if(b.write16)b.write16(a,v);}static void maw32(MidAssetBoundaries&b,u32 a,u32 v)noexcept{if(b.write32)b.write32(a,v);}

void function_87c4(Machine&m,MidAssetBoundaries&b)noexcept{store_le32(dgroup_ptr(m,dgroup(0x89e8u)),b.tick_7f50?b.tick_7f50():0u);}static_assert(0x87D5u-0x87C4u==17u);
[[nodiscard]]u32 function_87d8(Machine&m,MidAssetBoundaries&b,u32 delta,u32 flags)noexcept{u32 base=load_le32(dgroup_ptr(m,dgroup(0x89e8u)));if(base==0u)base=b.tick_7f50?b.tick_7f50():0u;const u32 deadline=base+delta;flags&=~4u;for(;;){if(b.idle_9ae8)b.idle_9ae8();if((flags&1u)&&(b.key_ready_82a4?b.key_ready_82a4():0u)){store_le32(dgroup_ptr(m,dgroup(0x89e8u)),0u);return b.key_get_82c0?b.key_get_82c0():0u;}if((flags&2u)&&(b.mouse_ready_84d0?b.mouse_ready_84d0():0u)){u16 w=b.mouse_word_d0b4?b.mouse_word_d0b4():0u;while(w!=0u)w=b.mouse_word_d0b4?b.mouse_word_d0b4():0u;store_le32(dgroup_ptr(m,dgroup(0x89e8u)),0u);return static_cast<u32>(w)+0x1f8u;}if(flags&4u){store_le32(dgroup_ptr(m,dgroup(0x89e8u)),0u);return 0x1f0u;}const u32 now=b.tick_7f50?b.tick_7f50():0u;if(now>=deadline){store_le32(dgroup_ptr(m,dgroup(0x89e8u)),0u);return 0u;}}}static_assert(0x88B0u-0x87D8u==216u);
void function_88b0(Machine&m,MidAssetBoundaries&b)noexcept{if(b.critical_df13)b.critical_df13(0x416u);(void)function_87d8(m,b,6u,0u);if(b.critical_df73)b.critical_df73();}static_assert(0x88D5u-0x88B0u==37u);
[[nodiscard]]constexpr u32 function_88d8(u32)noexcept{return 0x89f6u;}static_assert(0x88F7u-0x88D8u==31u);
[[nodiscard]]constexpr u32 function_88f8(u32)noexcept{return 0x89f6u;}static_assert(0x8917u-0x88F8u==31u);
[[nodiscard]]s32 function_8918(s32 v)noexcept{return v<0?-1:(v>0?1:0);}static_assert(0x893Fu-0x8918u==39u);
void function_8940(Machine&m,u32 p)noexcept{store_le32(dgroup_ptr(m,dgroup(0x2b1cu)),p);}static_assert(0x894Fu-0x8940u==15u);
void function_8950(Machine&m,u32 p)noexcept{store_le32(dgroup_ptr(m,dgroup(0x2b18u)),p);}static_assert(0x895Fu-0x8950u==15u);
void function_8960(Machine&m,MidAssetBoundaries&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b18u)))!=0u&&b.callback_2b18)b.callback_2b18();}static_assert(0x8976u-0x8960u==22u);

void function_8978(Machine&m,MidAssetBoundaries&b)noexcept{const u32 h=b.open_62a8?b.open_62a8(0x234u):0xffffffffu;if(h==0xffffffffu&&b.fatal_9814)b.fatal_9814(0x240u);if(b.read_6304)b.read_6304(0x2aa4u,13u,h);for(u32 i=0;i<13u;++i)*dgroup_ptr(m,dgroup(0x2aa4u+i))=static_cast<u8>(*dgroup_ptr(m,dgroup(0x2aa4u+i))-0x30u);u16 n=0;if(b.read_6304)b.read_6304(0x2ab1u,2u,h);n=load_le16(dgroup_ptr(m,dgroup(0x2ab1u)));if(b.read_6304)b.read_6304(0x2ab1u,static_cast<u32>(static_cast<s16>(n)),h);if(b.close_62f0)(void)b.close_62f0(h);}static_assert(0x8A01u-0x8978u==137u);

void function_8a04(Machine&m,MidAssetBoundaries&b,u32 prefix,u32 groups)noexcept{function_8978(m,b);const u32 table=b.alloc_71c8?b.alloc_71c8(groups*16u):0u;store_le32(dgroup_ptr(m,dgroup(0x2b08u)),table);store_le32(dgroup_ptr(m,dgroup(0x2b04u)),groups);for(u32 g=0;g<groups;++g){u32 h=b.open_62a8?b.open_62a8(g):0xffffffffu;if(h==0xffffffffu&&b.fatal_9814)b.fatal_9814(0x26cu);const u32 rec=table+g*16u;maw32(b,rec,h);u32 count=0;if(b.read_6304)b.read_6304(rec+8u,4u,h);count=ma32(b,rec+8u);maw32(b,rec+4u,0u);const u32 slots=b.alloc_71c8?b.alloc_71c8(count*9u):0u;maw32(b,rec+12u,slots);for(u32 i=0;i<count;++i){const u32 e=slots+i*9u;maw8(b,e,static_cast<u8>((ma8(b,e)&0xf0u)|0x0fu));maw8(b,e,static_cast<u8>(ma8(b,e)&0xefu));maw8(b,e+2u,0u);maw16(b,e+3u,0u);maw32(b,e+5u,0u);} (void)prefix;}}static_assert(0x8B93u-0x8A04u==399u);
void function_8b94(Machine&m,MidAssetBoundaries&b)noexcept{const u32 table=load_le32(dgroup_ptr(m,dgroup(0x2b08u)));for(u32 g=0;g<load_le32(dgroup_ptr(m,dgroup(0x2b04u)));++g){const u32 n=ma32(b,table+g*16u+8u);for(u32 i=0;i<n;++i){if(b.release_type)b.release_type(i,g,0u);}}}static_assert(0x8BE3u-0x8B94u==79u);
void function_8be4(Machine&m,MidAssetBoundaries&b,u32 ticks)noexcept{ticks>>=5;const u32 table=load_le32(dgroup_ptr(m,dgroup(0x2b08u)));for(u32 g=0;g<load_le32(dgroup_ptr(m,dgroup(0x2b04u)));++g){const u32 n=ma32(b,table+g*16u+8u),slots=ma32(b,table+g*16u+12u);for(u32 i=0;i<n;++i)if(ma8(b,slots+i*9u+2u)!=0u)maw16(b,slots+i*9u+3u,static_cast<u16>(ticks));}}static_assert(0x8C3Du-0x8BE4u==89u);
void function_8c40(Machine&m,MidAssetBoundaries&b)noexcept{function_8b94(m,b);const u32 table=load_le32(dgroup_ptr(m,dgroup(0x2b08u)));for(u32 g=0;g<load_le32(dgroup_ptr(m,dgroup(0x2b04u)));++g){const u32 rec=table+g*16u,h=ma32(b,rec);if(h!=0u){if(b.close_62f0)(void)b.close_62f0(h);maw32(b,rec,0u);maw32(b,rec+8u,0u);}const u32 slots=ma32(b,rec+12u);if(slots!=0u){if(b.free_7240)b.free_7240(slots);maw32(b,rec+12u,0u);}}if(table&&b.free_7240)b.free_7240(table);store_le32(dgroup_ptr(m,dgroup(0x2b08u)),0u);}static_assert(0x8CD4u-0x8C40u==148u);
[[nodiscard]]u32 function_8cd4(Machine&m,MidAssetBoundaries&b)noexcept{u32 total=0,table=load_le32(dgroup_ptr(m,dgroup(0x2b08u)));for(u32 g=0;g<load_le32(dgroup_ptr(m,dgroup(0x2b04u)));++g)total+=ma32(b,table+g*16u+4u);return total;}static_assert(0x8D0Cu-0x8CD4u==56u);
void function_8d0c(MidAssetBoundaries&b,u32 dst,u32 index,u32 h,u32 kind)noexcept{if(b.seek_6358)b.seek_6358(index,h);if(b.read_6304)b.read_6304(dst,5u,h);const s16 n=static_cast<s16>(ma16(b,dst+3u));if(n!=0){const u32 p=b.alloc_71c8?b.alloc_71c8(static_cast<u32>(n)):0u;maw32(b,dst+5u,p);if(b.read_6304)b.read_6304(p,static_cast<u32>(n),h);}else maw32(b,dst+5u,0u);if(ma16(b,dst)!=0x4845u&&b.fatal_9814)b.fatal_9814(kind);}static_assert(0x8DEDu-0x8D0Cu==225u);
[[nodiscard]]u32 function_8df0(Machine&m,MidAssetBoundaries&b,u32 g)noexcept{return ma32(b,load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+g*16u);}static_assert(0x8E06u-0x8DF0u==22u);
[[nodiscard]]u32 function_8e08(Machine&m,MidAssetBoundaries&b,u32 g)noexcept{return ma32(b,load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+g*16u+8u);}static_assert(0x8E1Fu-0x8E08u==23u);

void function_91d8(Machine&m,MidAssetBoundaries&b,u32 index,u32 group)noexcept{const u32 table=load_le32(dgroup_ptr(m,dgroup(0x2b08u))),rec=table+group*16u,slots=ma32(b,rec+12u),e=slots+index*9u;if((ma8(b,e)&0x10u)!=0u||ma8(b,e+2u)==0u)return;const u32 obj=ma32(b,e+5u);const u32 type=(ma8(b,e)&0x0fu);if(b.release_type)b.release_type(type,obj,ma8(b,e+2u));maw8(b,e+2u,0u);maw16(b,e+3u,0u);maw32(b,e+5u,0u);maw32(b,rec+4u,ma32(b,rec+4u)-1u);}static_assert(0x9377u-0x91D8u==415u);
[[nodiscard]]u32 function_9378(Machine&m,MidAssetBoundaries&b,u32 kind)noexcept{u32 bg=0,bi=0,best=0xffffffffu;bool found=false;const u32 table=load_le32(dgroup_ptr(m,dgroup(0x2b08u)));for(u32 g=0;g<load_le32(dgroup_ptr(m,dgroup(0x2b04u)));++g){const u32 rec=table+g*16u;if(ma32(b,rec+4u)==0u)continue;const u32 slots=ma32(b,rec+12u),n=ma32(b,rec+8u);for(u32 i=0;i<n;++i){const u32 e=slots+i*9u;if(ma8(b,e+2u)!=kind||(ma8(b,e)&0x10u))continue;const u32 age=ma16(b,e+3u);if(!found||age<best){found=true;best=age;bg=g;bi=i;}}}if(!found)return 0u;function_91d8(m,b,bi,bg);return 1u;}static_assert(0x943Eu-0x9378u==198u);
void function_9440(Machine&m,MidAssetBoundaries&b,u32 index,u32 group)noexcept{const u32 e=ma32(b,load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+group*16u+12u)+index*9u;if(ma8(b,e+2u)!=0u)maw8(b,e,static_cast<u8>(ma8(b,e)|0x10u));}static_assert(0x9488u-0x9440u==72u);
void function_9488(Machine&m,MidAssetBoundaries&b,u32 index,u32 group)noexcept{const u32 e=ma32(b,load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+group*16u+12u)+index*9u;if(ma8(b,e+2u)!=0u)maw8(b,e,static_cast<u8>(ma8(b,e)&0xefu));}static_assert(0x94D0u-0x9488u==72u);
void function_94d0(Machine&m,MidAssetBoundaries&b)noexcept{for(u32 g=0;g<load_le32(dgroup_ptr(m,dgroup(0x2b04u)));++g)for(u32 i=0;i<function_8e08(m,b,g);++i)if(i!=load_le32(dgroup_ptr(m,dgroup(0x2b10u))))function_9488(m,b,i,g);}static_assert(0x951Du-0x94D0u==77u);
void function_9520(Machine&m,MidAssetBoundaries&b)noexcept{function_8b94(m,b);const u32 i=load_le32(dgroup_ptr(m,dgroup(0x2b10u)));if(i!=0xffffffffu){if(b.release_type)b.release_type(i,0u,1u);function_9440(m,b,i,0u);}}static_assert(0x954Cu-0x9520u==44u);
[[nodiscard]]u32 function_954c(Machine&m,MidAssetBoundaries&b,u32 index,u32 group)noexcept{if(b.release_type)b.release_type(index,group,2u);const u32 e=ma32(b,load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+group*16u+12u)+index*9u;return ma8(b,e+2u)==1u?ma32(b,e+5u):0u;}static_assert(0x95A8u-0x954Cu==92u);

void function_95bc(Machine&m,MidAssetBoundaries&b,u32 arg0,u32 arg1)noexcept{store_le16(dgroup_ptr(m,dgroup(0x23ebu)),0x182u);*dgroup_ptr(m,dgroup(0x23d6u))=0x0c;*dgroup_ptr(m,dgroup(0x2400u))=1;*dgroup_ptr(m,dgroup(0x23d4u))=0xff;store_le32(dgroup_ptr(m,dgroup(0x2b10u)),0xffffffffu);if(b.install_int24_e280)(void)b.install_int24_e280(0x95a8u,static_cast<u32>(m.cpu.cs));if(b.install_8398)b.install_8398();function_64b0(m,0u);u32 cfg=0;if(load_le32(dgroup_ptr(m,dgroup(0x309cu)))!=2u){cfg=b.alloc_71c8?b.alloc_71c8(0x100u):0u;if(b.zero_84a0)b.zero_84a0(cfg,0u,0x100u);}else{const u32 argv=load_le32(dgroup_ptr(m,dgroup(0x30a0u)));cfg=b.runtime_e360?b.runtime_e360(ma32(b,argv+4u)):0u;}store_le32(dgroup_ptr(m,dgroup(0x2aa0u)),cfg);if(ma8(b,cfg+0x64u)==0u){const u32 h=b.open_62a8?b.open_62a8(0x2ecu):0xffffffffu;if(b.read_6304)b.read_6304(cfg,0x3bu,h);if(b.close_62f0)(void)b.close_62f0(h);maw8(b,cfg+0x64u,1u);maw8(b,cfg+0x66u,1u);}store_le32(dgroup_ptr(m,dgroup(0x2a9cu)),b.platform_cf98?b.platform_cf98():0u);if(ma16(b,cfg+8u)!=0x71u){if(ma16(b,cfg+0x14u)!=0u&&b.setup_c2d0)b.setup_c2d0(cfg+0x24u,static_cast<s16>(ma16(b,cfg+0x0au)),static_cast<s16>(ma16(b,cfg+0x0cu)),static_cast<s16>(ma16(b,cfg+0x0eu)),static_cast<s16>(ma16(b,cfg+0x10u)));if(b.setup_b0f8)b.setup_b0f8(cfg+0x16u,static_cast<s16>(ma16(b,cfg)),static_cast<s16>(ma16(b,cfg+2u)),static_cast<s16>(ma16(b,cfg+4u)),static_cast<s16>(ma16(b,cfg+6u)));}if(b.mouse_d10c)b.mouse_d10c(0u,0x27eu);if(b.mouse_d130)b.mouse_d130(0u,0xc7u);if(arg0)function_8a04(m,b,arg0,arg1);}static_assert(0x9724u-0x95BCu==360u);
void function_9724(Machine&m,MidAssetBoundaries&b,u32 text,u32 selected,u32 maxch)noexcept{const u32 old=load_le32(dgroup_ptr(m,dgroup(0x2b10u)));if(old!=0xffffffffu)function_9488(m,b,old,0u);store_le32(dgroup_ptr(m,dgroup(0x2b10u)),selected);if(b.release_type)b.release_type(selected,0u,2u);function_9440(m,b,selected,0u);store_le32(dgroup_ptr(m,dgroup(0x2b0cu)),text);*dgroup_ptr(m,dgroup(0x2b14u))=static_cast<u8>(maxch);}static_assert(0x9777u-0x9724u==83u);
void function_9778(Machine&m,MidAssetBoundaries&b)noexcept{const u32 cfg=load_le32(dgroup_ptr(m,dgroup(0x2aa0u)));const u32 state=(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))|load_le32(dgroup_ptr(m,dgroup(0x2f48u))));if(ma16(b,cfg+8u)!=0x71u){if(b.shutdown_b430)b.shutdown_b430();if(ma16(b,cfg+0x14u)!=0u&&b.shutdown_c4a0)b.shutdown_c4a0();if(state&&b.shutdown_e6ea)b.shutdown_e6ea(0u);}if(load_le32(dgroup_ptr(m,dgroup(0x2b08u)))!=0u)function_8c40(m,b);if(b.restore_8424)b.restore_8424();}static_assert(0x97D4u-0x9778u==92u);
void function_97d4(MidAssetBoundaries&b,u32 code)noexcept{if(b.exit_ede0)b.exit_ede0();if(b.exit_cbf0)b.exit_cbf0(code);}static_assert(0x97E5u-0x97D4u==17u);
void function_97e8(Machine&m,MidAssetBoundaries&b,u32 do_exit)noexcept{if(b.display_64dc)b.display_64dc(0xffu);function_9778(m,b);if(do_exit)function_97d4(b,1u);}static_assert(0x9811u-0x97E8u==41u);
void function_9814(Machine&m,MidAssetBoundaries&b,u32 text)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b10u)))!=0xffffffffu&&((*dgroup_ptr(m,dgroup(0x2b01u))&0x80u)==0u)){const u32 k=b.modal_9874?b.modal_9874(text,0u):0u;function_97e8(m,b,0u);if(k==0x54u){if(b.print_61d0)b.print_61d0(text);}}else{function_97e8(m,b,0u);if(b.print_61d0)b.print_61d0(text);}function_97d4(b,1u);}static_assert(0x9871u-0x9814u==93u);
[[nodiscard]]u32 function_9874(Machine&m,MidAssetBoundaries&b,u32 text,u32 mode)noexcept{if(*dgroup_ptr(m,dgroup(0x23d4u))==0xffu){if(b.print_61d0)b.print_61d0(text);return 0u;}const u8 saved_page=*dgroup_ptr(m,dgroup(0x23e5u));const u32 rows=*dgroup_ptr(m,dgroup(0x23d6u));u32 scratch=b.alloc_71c8?b.alloc_71c8(rows*0x140u):0u;if(b.capture_66c0)b.capture_66c0(scratch,0,0,static_cast<s32>(rows),0x140);if(text&&load_le32(dgroup_ptr(m,dgroup(0x2b10u)))!=0xffffffffu){const u32 w=b.text_width_7ab4?b.text_width_7ab4(text,load_le32(dgroup_ptr(m,dgroup(0x2b10u)))):0u;if(b.text_pos_7d68)b.text_pos_7d68(0xa0-static_cast<s32>(w/2u),0);if(b.text_draw_7c0c)b.text_draw_7c0c(text,load_le32(dgroup_ptr(m,dgroup(0x2b10u))));}u32 key=0;if(mode>1u)key=function_87d8(m,b,mode,0u);else if(mode==1u){do{key=b.key_get_82c0?b.key_get_82c0():0u;}while(key!=0x4eu&&key!=0x59u&&key!=0x53u);if(key==0x53u)key=0x59u;}else{do{key=b.key_get_82c0?b.key_get_82c0():0u;}while(key!=0x20u&&key!=0x54u);}if(b.restore_6744)b.restore_6744(scratch,0,0,static_cast<s32>(rows),0x140);if(b.free_7240)b.free_7240(scratch);if(b.page_6588)b.page_6588(saved_page);return key;}static_assert(0x9AB2u-0x9874u==574u);
void function_9ab4(Machine&m)noexcept{*dgroup_ptr(m,dgroup(0x2b01u))|=0x80u;}static_assert(0x9AC2u-0x9AB4u==14u);
void function_9ac4(Machine&m)noexcept{*dgroup_ptr(m,dgroup(0x2b01u))&=0x7fu;}static_assert(0x9AD2u-0x9AC4u==14u);
[[nodiscard]]u32 function_9ad4(Machine&m)noexcept{return (*dgroup_ptr(m,dgroup(0x2b01u))&0x80u)==0u?1u:0u;}static_assert(0x9AE8u-0x9AD4u==20u);
void function_9ae8(Machine&m,MidAssetBoundaries&b)noexcept{if(b.callback_2b18)b.callback_2b18();u8&state=*dgroup_ptr(m,dgroup(0x2b01u));u8&busy=*dgroup_ptr(m,dgroup(0x2b02u));if((state&2u)&&!(state&0x80u)&&busy==0u){busy=1u;state=0u;if(b.release_type)b.release_type(0x0au,0u,0u);}if(state!=0u&&!(state&0x80u)&&busy==0u){busy=1u;state=0u;u32 k=b.modal_9874?b.modal_9874(0x300u,1u):0u;if(k==0x59u)function_97e8(m,b,1u);busy=0u;state=0u;}}static_assert(0x9BC2u-0x9AE8u==218u);
void function_9bc4(Machine&m,MidAssetBoundaries&b)noexcept{*dgroup_ptr(m,dgroup(0x2b01u))|=1u;function_9ae8(m,b);}static_assert(0x9BD7u-0x9BC4u==19u);

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
// D23A..D27C: INT 21h/AH=43h attribute query. Caller flag bit 1 plus returned CX bit 0 synthesizes DOS error 6; 102D4/102EC/10314 reduce CF to 0 on success or mapped -1 on failure.
[[nodiscard]]s32 function_d23a(FileBoundaries&b,u32 path,u32 flags)noexcept{DosAttrResult r=b.dos_get_attr?b.dos_get_attr(path):DosAttrResult{1,0,true};if(!r.carry&&(flags&2u)&&(r.cx&1u)){r.ax=0x0600u;r.carry=true;}if(!r.carry)return 0;return b.call_10314?b.call_10314(r.ax):-1;}
static_assert(0xD27Du-0xD23Au==67u);
static void set_runtime_error(FileBoundaries&b,u32 v)noexcept{if(b.call_103b7){if(u32*p=b.call_103b7())*p=v;}} static s32 map_dos_error(FileBoundaries&b,s32 v)noexcept{return b.call_10314?b.call_10314(static_cast<u16>(v)):v;}
[[nodiscard]]u32 function_d332(FileBoundaries&,u32,u32,u32,u32)noexcept;
// D280..D2AC: set DOS DTA, execute AH=4Eh find-first, map AX+CF through 10384 semantics, then enter the exact D2CD mode-9 post-find shuttle boundary.
[[nodiscard]]s32 function_d280(FileBoundaries&b,u32 path,u16 attr,u32 dta)noexcept{DosAttrResult r=b.dos_find_first?b.dos_find_first(path,attr,dta):DosAttrResult{1,0,true};const s32 out=r.carry?map_dos_error(b,r.ax):0;if(b.dta_post_find)b.dta_post_find(dta);return out;}
static_assert(0xD2ADu-0xD280u==45u);
// D2AD..D2CC: set DTA, execute the D2F0 mode-9 pre-find shuttle, AH=4Fh find-next, 10384 mapping, then D2CD post-find shuttle. D2CD/D2F0 remain machine/selector boundaries.
[[nodiscard]]s32 function_d2ad(FileBoundaries&b,u32 dta)noexcept{if(b.dta_pre_find_next)b.dta_pre_find_next(dta);DosAttrResult r=b.dos_find_next?b.dos_find_next(dta):DosAttrResult{1,0,true};const s32 out=r.carry?map_dos_error(b,r.ax):0;if(b.dta_post_find)b.dta_post_find(dta);return out;}
static_assert(0xD2CDu-0xD2ADu==32u);
// D313..D331: variadic front wrapper. The first variadic dword is forwarded as D332 arg4; arg3 is exactly zero.

// D2CD..D312: mode-9 DTA shuttles. AH=2F is executed with the live machine selectors; the REP MOVSB direction is preserved instead of flattening DS/ES into one host pointer.
struct DtaCopyBoundary{void(*copy)(u16,u32,u16,u32,u32)=nullptr;};
void function_d2cd(Machine&m,DtaCopyBoundary&b)noexcept{if(*dgroup_ptr(m,dgroup(0x531eu))!=9u)return;const u32 save_eax=m.cpu.eax,save_ebx=m.cpu.ebx,save_ecx=m.cpu.ecx,save_esi=m.cpu.esi,save_edi=m.cpu.edi;set_high8(m.cpu.eax,0x2fu);interrupt(m,0x21);m.cpu.esi=m.cpu.ebx;m.cpu.edi=m.cpu.edx;m.cpu.ecx=0x2bu;if(b.copy)b.copy(m.cpu.es,m.cpu.edi,m.cpu.ds,m.cpu.esi,0x2bu);m.cpu.edi=save_edi;m.cpu.esi=save_esi;m.cpu.ecx=save_ecx;m.cpu.ebx=save_ebx;m.cpu.eax=save_eax;}
void function_d2f0(Machine&m,DtaCopyBoundary&b)noexcept{if(*dgroup_ptr(m,dgroup(0x531eu))!=9u)return;const u32 save_eax=m.cpu.eax,save_ebx=m.cpu.ebx,save_ecx=m.cpu.ecx,save_esi=m.cpu.esi,save_edi=m.cpu.edi;set_high8(m.cpu.eax,0x2fu);interrupt(m,0x21);m.cpu.edi=m.cpu.ebx;m.cpu.esi=m.cpu.edx;m.cpu.ecx=0x2bu;if(b.copy)b.copy(m.cpu.es,m.cpu.edi,m.cpu.ds,m.cpu.esi,0x2bu);m.cpu.edi=save_edi;m.cpu.esi=save_esi;m.cpu.ecx=save_ecx;m.cpu.ebx=save_ebx;m.cpu.eax=save_eax;}
static_assert(0xD313u-0xD2CDu==70u);

[[nodiscard]]u32 function_d313(FileBoundaries&b,u32 path,u32 flags,u32 first_variadic)noexcept{return function_d332(b,path,flags,0u,first_variadic);}
static_assert(0xD332u-0xD313u==31u);
// D332..D4F1: DOS open/create/truncate state machine. All DOS calls retain AX+CF explicitly; no host fopen semantics are substituted.
[[nodiscard]]u32 function_d332(FileBoundaries&b,u32 path,u32 flags,u32 share,u32 create_mode)noexcept{
 while(b.read_guest8&&b.read_guest8(path)==0x20u)++path;
 u32 mode=flags&0x83u; const u8 open_mode=static_cast<u8>(mode|share);
 DosHandleResult opened=b.dos_open_raw?b.dos_open_raw(path,open_mode):DosHandleResult{0xffffu,true};
 u32 handle=opened.carry?0xffffffffu:static_cast<u32>(opened.ax); u16 error_ax=opened.ax;
 if((flags&3u)!=0&&handle!=0xffffffffu&&(b.call_10397?b.call_10397(handle):0u)==0u){
  if((flags&0x400u)!=0u){if(b.dos_close_raw)(void)b.dos_close_raw(static_cast<u16>(handle));set_runtime_error(b,7u);return 0xffffffffu;}
  if((flags&0x40u)!=0u){const DosHandleResult wr=b.dos_write_zero_raw?b.dos_write_zero_raw(static_cast<u16>(handle)):DosHandleResult{0xffffu,true};if(wr.carry){if(b.dos_close_raw)(void)b.dos_close_raw(static_cast<u16>(handle));return static_cast<u32>(map_dos_error(b,static_cast<s32>(wr.ax)));}}
 }
 if(handle==0xffffffffu){
  if((flags&0x20u)==0u||error_ax!=2u)return static_cast<u32>(map_dos_error(b,static_cast<s32>(error_ax)));
  u32 cm=create_mode?create_mode:0x180u; const u32 mask=b.read_dgroup32?b.read_dgroup32(0x5340u):0u; cm^=mask; const u16 attr=(static_cast<u8>(cm)&0x80u)?0u:1u;
  const DosHandleResult cr=b.dos_create_raw?b.dos_create_raw(path,attr):DosHandleResult{0xffffu,true};if(cr.carry)return static_cast<u32>(map_dos_error(b,static_cast<s32>(cr.ax)));
  const DosHandleResult cl=b.dos_close_raw?b.dos_close_raw(cr.ax):DosHandleResult{0xffffu,true};if(cl.carry)return static_cast<u32>(map_dos_error(b,static_cast<s32>(cl.ax)));
  opened=b.dos_open_raw?b.dos_open_raw(path,open_mode):DosHandleResult{0xffffu,true};if(opened.carry)return static_cast<u32>(map_dos_error(b,static_cast<s32>(opened.ax)));handle=static_cast<u32>(opened.ax);
 }
 u32 state=b.call_103c3?b.call_103c3(handle):0u; state=(state&~0xffu)|static_cast<u8>(state&0x3cu);
 if((b.call_10397?b.call_10397(handle):0u)!=0u)state|=0x2000u; mode&=0x7fu;
 if(mode==2u)state|=3u;if(mode==0u)state|=1u;if(mode==1u)state|=2u;if((flags&0x10u)!=0u)state|=0x80u;
 const u32 text_state=(state&~0xffu)|static_cast<u8>(state|0x40u);
 if((flags&0x300u)!=0u){if((flags&0x200u)!=0u)state=text_state;}else if((b.read_dgroup32?b.read_dgroup32(0x55a5u):0u)==0x200u)state=text_state;
 if(b.set_fd_state_10415)b.set_fd_state_10415(handle,state);return handle;
}
static_assert(0xD4F2u-0xD332u==448u);

// D4F2..D55D: commit descriptor state with AL|40h; for device descriptors perform INT 21h/AH=44h get/set, mapping the encoded DX error word through 10314.
[[nodiscard]]s32 function_d4f2(FileBoundaries&b,u32 h)noexcept{u32 state=b.call_103c3?b.call_103c3(h):0;state=(state&~0xffu)|static_cast<u8>(state|0x40u);if(b.set_fd_state_10415)b.set_fd_state_10415(h,state);if((state&0x2000u)==0)return 0;u8 mode=0;if(!b.ioctl_get_mode||b.ioctl_get_mode(static_cast<u16>(h),mode)){return b.call_10314?b.call_10314(mode):-1;}mode=static_cast<u8>(mode|0x20u);if(!b.ioctl_set_mode||b.ioctl_set_mode(static_cast<u16>(h),mode)){return b.call_10314?b.call_10314(mode):-1;}return 0;}
static_assert(0xD55Eu-0xD4F2u==108u);
// D55E..D572: fixed wrapper calls D313(path,62h,mode), which forwards mode as D332 arg4.
[[nodiscard]]u32 function_d55e(FileBoundaries&b,u32 path,u32 mode)noexcept{return function_d313(b,path,0x62u,mode);}
static_assert(0xD573u-0xD55Eu==21u);
// D573..D5AC: AH=3Eh close. RCL/ROR materializes CF in EAX sign; success clears descriptor state, failure stores runtime errno 4.
[[nodiscard]]u32 function_d573(FileBoundaries&b,u32 h)noexcept{const bool carry=b.dos_close?b.dos_close(static_cast<u16>(h)):true;if(!carry){if(b.set_fd_state_10415)b.set_fd_state_10415(h,0);return 0;}set_runtime_error(b,4);return 0xffffffffu;}
static_assert(0xD5ADu-0xD573u==58u);
[[nodiscard]]s32 function_d5ad(FileBoundaries&b,u16 handle,u8*dst,u32 count)noexcept{const u32 flags=b.call_103c3?b.call_103c3(handle):0;if(flags==0){set_runtime_error(b,4);return -1;}if((flags&1u)==0){set_runtime_error(b,6);return -1;}if(flags&0x40u){const s32 n=b.dos_read?b.dos_read(handle,dst,count):-1;return n<0?map_dos_error(b,n):n;}u32 total=0,remaining=count;u8*writep=dst;while(remaining){const s32 n=b.dos_read?b.dos_read(handle,writep,remaining):-1;if(n<0)return map_dos_error(b,n);if(n==0)break;const u8*src=writep;const u8*end=writep+static_cast<u32>(n);u32 kept=0;while(src<end){const u8 ch=*src++;if(ch==0x1a){total+=kept;return static_cast<s32>(total);}if(ch==0x0d)continue;writep[kept++]=ch;}total+=kept;remaining-=kept;writep+=kept;if(flags&0x2000u)break;}return static_cast<s32>(total);}
[[nodiscard]]s32 function_d686(FileBoundaries&b,u16 handle,const u8*src,u32 count)noexcept{const u32 flags=b.call_103c3?b.call_103c3(handle):0;if(flags==0){set_runtime_error(b,4);return -1;}if((flags&2u)==0){set_runtime_error(b,6);return -1;}if(flags&0x80u){const s32 pos=b.dos_seek?b.dos_seek(handle,0,2):-1;if(pos<0)return map_dos_error(b,pos);}if(flags&0x40u){const s32 n=b.dos_write?b.dos_write(handle,src,count):-1;if(n<0)return map_dos_error(b,n);if(static_cast<u32>(n)!=count)set_runtime_error(b,12);return n;}const u32 stack_distance=b.call_1042d?b.call_1042d():0;if(stack_distance<0xb0u&&b.call_1046f)b.call_1046f();const u32 cap=stack_distance<0x230u?0x80u:0x200u;u8 temp[0x200]{};u32 consumed=0,last_flushed_input=0,fill=0;auto flush=[&](u32 amount)->s32{const s32 n=b.dos_write?b.dos_write(handle,temp,amount):-1;if(n<0)return map_dos_error(b,n);if(static_cast<u32>(n)!=amount){set_runtime_error(b,12);return static_cast<s32>(last_flushed_input+static_cast<u32>(n));}last_flushed_input=consumed;fill=0;return -0x40000000;};while(consumed<count){const u8 ch=src[consumed];if(ch==0x0a){temp[fill++]=0x0d;if(fill==cap){const s32 r=flush(cap);if(r!=-0x40000000)return r;}}temp[fill++]=ch;++consumed;if(fill==cap){const s32 r=flush(cap);if(r!=-0x40000000)return r;}}if(fill){const s32 n=b.dos_write?b.dos_write(handle,temp,fill):-1;if(n<0)return map_dos_error(b,n);if(static_cast<u32>(n)!=fill){set_runtime_error(b,12);return static_cast<s32>(last_flushed_input+static_cast<u32>(n));}}return static_cast<s32>(count);}
[[nodiscard]]s32 function_d85f(FileBoundaries&b,u16 handle,s32 offset,u8 origin)noexcept{const s32 r=b.dos_seek?b.dos_seek(handle,offset,origin):-1;return r<0?map_dos_error(b,r):r;} [[nodiscard]]s32 function_d8aa(FileBoundaries&b,const char*path)noexcept{const s32 r=b.dos_delete?b.dos_delete(path):-1;return r<0?map_dos_error(b,r):0;}

// F750..F7B2: source reader used by the following resource parsers. Bit 0 selects an in-memory source; otherwise seek/read through the DOS wrappers and return the handle's resulting current position.
[[nodiscard]]u32 function_f750(FileBoundaries&b,u32 source,u32 offset,u32 flags,u8*dst,u32 count)noexcept{
 if((flags&1u)!=0u){for(u32 i=0;i<count;++i)dst[i]=b.read_guest8?b.read_guest8(source+offset+i):0u;return offset+count;}
 (void)function_d85f(b,static_cast<u16>(source),static_cast<s32>(offset),0u);
 (void)function_d5ad(b,static_cast<u16>(source),dst,count);
 return static_cast<u32>(function_d85f(b,static_cast<u16>(source),0,1u));
}
static_assert(0xF7B3u-0xF750u==99u);

// F7B8..F8BC: determine required resource image size. The two-byte signature is read at the header-supplied offset and compared with the DGROUP string at 038Ch; each 24-byte record contributes its first dword.
[[nodiscard]]u32 function_f7b8(FileBoundaries&b,u32 source,u32 flags)noexcept{
 u32 h=source;if((flags&1u)==0u){h=function_d313(b,source,0x200u,0u);if(h==0xffffffffu)return 0u;}
 u8 offbuf[4]{}; (void)function_f750(b,h,0x3cu,flags,offbuf,4u); const u32 header=load_le32(offbuf);
 char sig[4]={' ',' ',0,0}; (void)function_f750(b,h,header,flags,reinterpret_cast<u8*>(sig),2u);
 const s32 mismatch=b.compare_guest_string?b.compare_guest_string(sig,0x38cu):1;
 if(mismatch!=0){(void)function_d573(b,h);return 0u;}
 u8 hdr[0xacu]{}; (void)function_f750(b,h,header,flags,hdr,0xacu);
 u32 pos=header+load_le32(hdr+0x40u); const u32 records=load_le32(hdr+0x44u); u32 total=0u;
 for(u32 i=0;i<records;++i){u8 rec[0x18u]{};pos=function_f750(b,h,pos,flags,rec,0x18u);total+=load_le32(rec);}
 if((flags&1u)==0u)(void)function_d573(b,h);return total+0x0fu;
}
static_assert(0xF8BDu-0xF7B8u==261u);


// F8C4..FC96: resource image loader/relocator. Allocated 32-bit guest addresses never become native pointers: the encoded F750 call whose destination is the loaded image and the final relocation dword store remain explicit guest-memory boundaries.
struct ResourceLoadBoundaries{u32(*alloc_71c8)(u32)=nullptr;void(*zero_84a0)(u32,u32,u32)=nullptr;u32(*read_f750_guest)(u32,u32,u32,u32,u32)=nullptr;void(*write_guest32)(u32,u32)=nullptr;};
[[nodiscard]]u32 function_f8c4(FileBoundaries&b,ResourceLoadBoundaries&lb,u32 source,u32 flags,u32 destination)noexcept{
 u32 handle=source,chunk_count=0,cursor=0;u32 segment_base[64]{},chunk_base[144]{};
 if((flags&1u)==0u){handle=function_d313(b,source,0x200u,0u);if(handle==0xffffffffu)return 0u;}
 const u32 image_size=function_f7b8(b,source,flags);
 if((flags&4u)!=0u){destination=lb.alloc_71c8?lb.alloc_71c8(image_size):0u;if(destination==0u){if((flags&1u)==0u)(void)function_d573(b,handle);return 0u;}}
 if(lb.zero_84a0)lb.zero_84a0(destination,0u,image_size);
 u32 header_off=0;cursor=function_f750(b,handle,0x3cu,flags,reinterpret_cast<u8*>(&header_off),4u);
 char sig[4]={' ',' ',0,0};cursor=function_f750(b,handle,header_off,flags,reinterpret_cast<u8*>(sig),2u);
 if((b.compare_guest_string?b.compare_guest_string(sig,0x38cu):1)!=0){if((flags&1u)==0u)(void)function_d573(b,handle);return 0u;}
 u8 h[0xacu]{};cursor=function_f750(b,handle,header_off,flags,h,0xacu);
 const u32 object_table_off=load_le32(h+0x40u),object_count=load_le32(h+0x44u),page_map_off=load_le32(h+0x48u),data_pages_off=load_le32(h+0x80u),fixup_page_off=load_le32(h+0x68u),fixup_record_off=load_le32(h+0x6cu),page_size=load_le32(h+0x28u),module_pages=load_le32(h+0x14u);const u8 page_shift=h[0x2cu];
 cursor=header_off+object_table_off;
 for(u32 object=0;object<object_count;++object){u8 rec[0x18u]{};cursor=function_f750(b,handle,cursor,flags,rec,0x18u);const u32 virtual_size=load_le32(rec),object_flags=load_le32(rec+8u),page_count=load_le32(rec+0x10u);u32 page_cursor=header_off+page_map_off;
  for(u32 page=0;page<page_count;++page){u8 map[8]{};page_cursor=function_f750(b,handle,page_cursor,flags,map,8u);if(page==0u){if((object_flags&2u)!=0u&&(object_flags&1u)!=0u&&(destination&0xfu)!=0u)destination+=0x10u-(destination&0xfu);segment_base[object]=destination;}chunk_base[chunk_count++]=destination;const u32 used=destination-segment_base[object];const u32 remain=virtual_size-used;const u32 encoded=static_cast<u32>(load_le16(map+4u));const u32 bytes=remain<encoded?remain:encoded;const u32 data_off=(load_le32(map)<<page_shift)+data_pages_off;if(lb.read_f750_guest)(void)lb.read_f750_guest(handle,data_off,flags,destination,bytes);destination+=bytes;}cursor=page_cursor;}
 u32 fix_cursor=header_off+fixup_page_off,previous=0;fix_cursor=function_f750(b,handle,fix_cursor,flags,reinterpret_cast<u8*>(&previous),4u);
 for(u32 page=0;page<module_pages;++page){u32 next=0;const u32 after=function_f750(b,handle,fix_cursor,flags,reinterpret_cast<u8*>(&next),4u);fix_cursor=after;if(previous!=next){u32 rec_cursor=header_off+fixup_record_off+previous;do{u8 source_type=0,target_flags=0;rec_cursor=function_f750(b,handle,rec_cursor,flags,&source_type,1u);rec_cursor=function_f750(b,handle,rec_cursor,flags,&target_flags,1u);if((source_type&7u)==0u||((target_flags&4u)!=0u&&(target_flags&0x20u)==0u)||(source_type&0x20u)!=0u){if((flags&1u)==0u)(void)function_d573(b,handle);return 0u;}u8 offbuf[2]{},idx=0,addbuf[2]{};rec_cursor=function_f750(b,handle,rec_cursor,flags,offbuf,2u);rec_cursor=function_f750(b,handle,rec_cursor,flags,&idx,1u);rec_cursor=function_f750(b,handle,rec_cursor,flags,addbuf,2u);const u32 src_off=load_le16(offbuf);if(src_off<=page_size){const u32 patch=chunk_base[page]+src_off,value=segment_base[idx]+load_le16(addbuf);if(lb.write_guest32)lb.write_guest32(patch,value);}}while(rec_cursor<header_off+fixup_record_off+next);fix_cursor=after;}previous=next;}
 if((flags&1u)==0u)(void)function_d573(b,handle);return destination;
}
static_assert(0xFC97u-0xF8C4u==979u);

// FC97: one-byte RET callback. The original LE fixup record table contains an
// internal 32-bit offset relocation (07 00 28 03 01 97 FC) targeting object 1
// offset FC97h, so this byte is executable code rather than alignment filler.
void function_fc97()noexcept{}
static_assert(0xFC98u-0xFC97u==1u);

// FC98..FCE6: runtime launch wrapper. The stack-allocation address itself is represented by a boundary because host ESP is not guest linear memory; all encoded DGROUP updates and the non-returning CBF0 tail are explicit.
struct LaunchBoundaries{u32(*stack_probe_1042d)()=nullptr;u32(*alloca_guest)(u32)=nullptr;void(*init_10d1c)()=nullptr;s32(*main_508c)(u32,u32)=nullptr;void(*exit_cbf0)(s32)=nullptr;};
void function_fc98(Machine&m,LaunchBoundaries&b)noexcept{u32 n=load_le32(dgroup_ptr(m,dgroup(0x5308u)));n=(n+3u)&0xfffffffcu;const u32 probe=b.stack_probe_1042d?b.stack_probe_1042d():0u;u32 p=0;if(n<probe&&b.alloca_guest)p=b.alloca_guest(n);p+=load_le32(dgroup_ptr(m,dgroup(0x5308u)));store_le32(dgroup_ptr(m,dgroup(0x530cu)),p);if(b.init_10d1c)b.init_10d1c();const s32 rc=b.main_508c?b.main_508c(load_le32(dgroup_ptr(m,dgroup(0x8e80u))),load_le32(dgroup_ptr(m,dgroup(0x8e7cu)))):0;if(b.exit_cbf0)b.exit_cbf0(rc);} static_assert(0xFCE7u-0xFC98u==79u);
// FCE7 is the zero alignment byte immediately before the FCE8 dispatcher.

// FCE8..FD2A: execute unprocessed six-byte runtime records in 55E0..55F1 by ascending priority. FCE8 is PUSH ESI; the previous accounting began one byte late. Equal priorities select the later record because JA rejects only strictly larger values.
struct RuntimeTailBoundaries{void(*call_guest_near)(u32)=nullptr;};
void function_fce8(Machine&m,RuntimeTailBoundaries&b)noexcept{
 for(;;){u32 best=0x55f2u;u8 priority=0xffu;for(u32 rec=0x55e0u;rec<0x55f2u;rec+=6u){const u8 state=*dgroup_ptr(m,dgroup(rec));const u8 pr=*dgroup_ptr(m,dgroup(rec+1u));if(state!=2u&&pr<=priority){best=rec;priority=pr;}}if(best==0x55f2u)return;const u32 fn=load_le32(dgroup_ptr(m,dgroup(best+2u)));if(fn&&b.call_guest_near)b.call_guest_near(fn);*dgroup_ptr(m,dgroup(best))=2u;}
}
static_assert(0xFD2Bu-0xFCE8u==67u);
// FD2B..FD6D: same six-byte record dispatcher for 55F2..55F7, selecting the greatest priority (again preferring the later record on equality).
void function_fd2b(Machine&m,RuntimeTailBoundaries&b)noexcept{
 for(;;){u32 best=0x55f8u;u8 priority=0u;for(u32 rec=0x55f2u;rec<0x55f8u;rec+=6u){const u8 state=*dgroup_ptr(m,dgroup(rec));const u8 pr=*dgroup_ptr(m,dgroup(rec+1u));if(state!=2u&&pr>=priority){best=rec;priority=pr;}}if(best==0x55f8u)return;const u32 fn=load_le32(dgroup_ptr(m,dgroup(best+2u)));if(fn&&b.call_guest_near)b.call_guest_near(fn);*dgroup_ptr(m,dgroup(best))=2u;}
}
static_assert(0xFD6Eu-0xFD2Bu==67u);


// FD6E..FE20: argv/environment builder. Guest pointers stay 32-bit addresses; FE21 tokenization is invoked through a guest-memory boundary rather than cast to native pointers.
struct ArgvBuildBoundaries{u32(*tokenize_fe21)(u32,u32,u32*)=nullptr;u32(*alloc_dac9)(u32)=nullptr;void(*copy_d9ee)(u32,u32,u32)=nullptr;void(*write32)(u32,u32)=nullptr;};
void function_fd6e(Machine&m,ArgvBuildBoundaries&b)noexcept{const u32 text=load_le32(dgroup_ptr(m,dgroup(0x52f4u)));u32 end=text;const u32 argc=b.tokenize_fe21?b.tokenize_fe21(text,0u,&end):0u;const u32 slots=argc+1u;const u32 text_bytes=end-text+1u;const u32 bytes=slots*4u+4u+text_bytes;const u32 block=b.alloc_dac9?b.alloc_dac9(bytes):0u;if(block!=0u){const u32 argv=block+text_bytes;store_le32(dgroup_ptr(m,dgroup(0x30a0u)),argv);if(b.copy_d9ee)b.copy_d9ee(block,text,text_bytes);if(b.write32)b.write32(argv,load_le32(dgroup_ptr(m,dgroup(0x52f8u))));u32 stop=block;if(b.tokenize_fe21)(void)b.tokenize_fe21(block,argv+4u,&stop);if(b.write32)b.write32(argv+slots*4u,0u);store_le32(dgroup_ptr(m,dgroup(0x309cu)),slots);}else{store_le32(dgroup_ptr(m,dgroup(0x30a0u)),0u);store_le32(dgroup_ptr(m,dgroup(0x309cu)),0u);}store_le32(dgroup_ptr(m,dgroup(0x8e80u)),slots);store_le32(dgroup_ptr(m,dgroup(0x8e7cu)),load_le32(dgroup_ptr(m,dgroup(0x30a0u))));} static_assert(0xFE21u-0xFD6Eu==179u);

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

// FFD4..102D3: 256 fixed three-byte interrupt veneers. All vectors except 3 encode `CD imm8; C3`; vector 3 is `CC 90 C3` (INT3, NOP, RET) so its slot remains three bytes. The generic reconstruction preserves the semantic interrupt and exact slot-address formula.
void function_interrupt_stub_ffd4(Machine&m,u8 vector)noexcept{interrupt(m,vector);}
[[nodiscard]]constexpr u32 interrupt_stub_address_ffd4(u8 vector)noexcept{return 0xffd4u+3u*static_cast<u32>(vector);}
static_assert(0x102D4u-0xFFD4u==256u*3u);


// 104E0..1083E: Watcom-style guest heap core. All heap addresses remain 32-bit guest addresses; no host malloc/free or native-pointer aliasing is substituted.
struct HeapCoreBoundaries{
 u32(*read32)(u32)=nullptr; void(*write32)(u32,u32)=nullptr;
 void(*dpmi_free_0502)(u32)=nullptr; void(*dos_free_49)(u16)=nullptr;
};
static u32 hread(HeapCoreBoundaries&b,u32 a)noexcept{return b.read32?b.read32(a):0u;} static void hwrite(HeapCoreBoundaries&b,u32 a,u32 v)noexcept{if(b.write32)b.write32(a,v);}
// 104E0..1058A: allocate from one arena descriptor. Size is rounded exactly as encoded; free-list links are split or unlinked in place.
[[nodiscard]]u32 function_104e0(HeapCoreBoundaries&b,u32 request,u32 arena)noexcept{
 if(request==0u)return 0u;u32 n=request+7u;if(n<request)return 0u;n&=0xfffffffcu;const u32 overhead=12u;n=n<overhead?overhead:n;
 if(n>hread(b,arena+0x10u))return 0u;u32 node=hread(b,arena+8u),scan=hread(b,arena+0x0cu);if(n<=scan){node=hread(b,arena+0x24u);scan=0u;}
 const u32 sentinel=arena+0x1cu;for(;;){u32 size=hread(b,node);if(n<=size){u32 remain=size-n;hwrite(b,arena+0x0cu,scan);hwrite(b,arena+0x14u,hread(b,arena+0x14u)+1u);u32 next=hread(b,node+8u);if(remain>=12u){u32 tail=node+n;hwrite(b,arena+8u,tail);hwrite(b,tail,remain);hwrite(b,node,n);u32 prev=hread(b,node+4u);hwrite(b,tail+4u,prev);hwrite(b,tail+8u,next);hwrite(b,prev+8u,tail);hwrite(b,next+4u,tail);}else{hwrite(b,arena+0x18u,hread(b,arena+0x18u)-1u);u32 prev=hread(b,node+4u);hwrite(b,prev+8u,next);hwrite(b,next+4u,prev);hwrite(b,arena+8u,prev);}hwrite(b,node,hread(b,node)|1u);return node+4u;}scan=scan-size;node=hread(b,node+8u);if(node==sentinel){hwrite(b,arena+0x10u,scan);return 0u;}}
}
static_assert(0x1058Bu-0x104E0u==171u);
// 1058B is the shared free epilogue target. 10590..1069A returns a block to the address-ordered free list and coalesces adjacent blocks.
void function_10590(HeapCoreBoundaries&b,u32 ptr,u32 arena)noexcept{
 if(ptr==0u)return;u32 block=ptr-4u,size=hread(b,block);if((size&1u)==0u)return;size&=~1u;u32 after=block+size;
 if((hread(b,after)&1u)==0u){if(after==hread(b,arena+8u))hwrite(b,arena+8u,block);size+=hread(b,after);hwrite(b,block,size);u32 p=hread(b,after+4u),n=hread(b,after+8u);hwrite(b,p+8u,n);hwrite(b,n+4u,p);hwrite(b,arena+0x18u,hread(b,arena+0x18u)-1u);}else hwrite(b,block,size);
 u32 cur=hread(b,arena+8u);if(block<cur){if(block>hread(b,cur+4u))cur=hread(b,arena+0x24u);}else{cur=hread(b,cur+8u);while(block>=cur){cur=hread(b,cur+8u);if(cur==arena+0x1cu)break;}}
 u32 prev=hread(b,cur+4u);u32 merged_size=hread(b,block);if(prev+hread(b,prev)==block){merged_size+=hread(b,prev);hwrite(b,prev,merged_size);if(block==hread(b,arena+8u))hwrite(b,arena+8u,prev);block=prev;}else{hwrite(b,arena+0x18u,hread(b,arena+0x18u)+1u);hwrite(b,block+8u,cur);hwrite(b,block+4u,prev);hwrite(b,prev+8u,block);hwrite(b,cur+4u,block);}hwrite(b,arena+0x14u,hread(b,arena+0x14u)-1u);if(block<hread(b,arena+8u)&&merged_size>hread(b,arena+0x0cu))hwrite(b,arena+0x0cu,merged_size);if(merged_size>hread(b,arena+0x10u))hwrite(b,arena+0x10u,merged_size);
}
static_assert(0x1069Bu-0x10590u==267u);
// 1069B..10714: insert a newly acquired arena into the global address-ordered arena list and construct its initial terminal/free block metadata.
[[nodiscard]]u32 function_1069b(Machine&m,HeapCoreBoundaries&b,u32 raw)noexcept{
 u32 next=load_le32(dgroup_ptr(m,dgroup(0x30b4))),prev=0;while(next&&raw>=next){prev=next;next=hread(b,next+8u);}hwrite(b,raw+4u,prev);hwrite(b,raw+8u,next);if(prev){hwrite(b,prev+8u,raw);hwrite(b,prev+hread(b,prev)+4u,raw+0x0cu);}else{store_le32(dgroup_ptr(m,dgroup(0x30b0)),raw);store_le32(dgroup_ptr(m,dgroup(0x30b4)),raw);}if(next){hwrite(b,next+4u,raw);next+=0x0cu;}u32 size=hread(b,raw);u32 freep=raw+0x0cu,free_size=size-0x0cu;hwrite(b,freep,free_size);if(freep<=load_le32(dgroup_ptr(m,dgroup(0x30c0)))&&free_size>load_le32(dgroup_ptr(m,dgroup(0x30c4))))store_le32(dgroup_ptr(m,dgroup(0x30c4)),free_size);hwrite(b,freep+free_size,0xffffffffu);hwrite(b,freep+free_size+4u,next);return freep;
}
static_assert(0x10715u-0x1069Bu==122u);
// 10715..10730: unlink one arena-list node.
void function_10715(HeapCoreBoundaries&b,u32 node)noexcept{u32 p=hread(b,node+4u),n=hread(b,node+8u);if(p)hwrite(b,p+8u,n);if(n)hwrite(b,n+4u,p);}
static_assert(0x10731u-0x10715u==28u);
// 10731..107CA: detach an arena descriptor and its embedded free-list nodes from global lists.
void function_10731(Machine&m,HeapCoreBoundaries&b,u32 previous,u32 arena)noexcept{
 if(hread(b,arena+0x18u)!=0u){u32 x=hread(b,arena+0x18u),end=x+hread(b,x)+4u,src=arena+0x14u+hread(b,arena+0x14u)+4u;hwrite(b,end,hread(b,src));}
 if(previous==0u)store_le32(dgroup_ptr(m,dgroup(0x8e74)),hread(b,arena+8u));else hwrite(b,previous+8u,hread(b,arena+8u));u32 n=arena+0x14u;if(n==load_le32(dgroup_ptr(m,dgroup(0x30b0))))store_le32(dgroup_ptr(m,dgroup(0x30b0)),hread(b,arena+0x1cu));if(n==load_le32(dgroup_ptr(m,dgroup(0x30b4))))store_le32(dgroup_ptr(m,dgroup(0x30b4)),hread(b,arena+0x1cu));function_10715(b,n);if((hread(b,arena+0x20u)&1u)==0u){u32 q=arena+0x20u;function_10715(b,q);store_le32(dgroup_ptr(m,dgroup(0x30d0)),load_le32(dgroup_ptr(m,dgroup(0x30d0)))-1u);if(q==load_le32(dgroup_ptr(m,dgroup(0x30c0))))store_le32(dgroup_ptr(m,dgroup(0x30c0)),hread(b,arena+0x28u));}
}
static_assert(0x107CBu-0x10731u==154u);
// 107CB..1083E: reclaim arenas whose terminal free block spans the whole arena; use DPMI 0502h or DOS AH=49h according to the stored arena kind.
void function_107cb(Machine&m,HeapCoreBoundaries&b)noexcept{u32 prev=0,arena=load_le32(dgroup_ptr(m,dgroup(0x8e74)));while(arena){if(hread(b,arena+0x20u)+0x0cu==hread(b,arena+0x14u)){u32 old=arena,next=hread(b,arena+8u);function_10731(m,b,prev,arena);if(hread(b,old+0x10u)==0u){if(b.dpmi_free_0502)b.dpmi_free_0502(hread(b,old));}else if(b.dos_free_49)b.dos_free_49(static_cast<u16>(hread(b,old+0x10u)));arena=next;}else{prev=arena;arena=hread(b,arena+8u);}}}
static_assert(0x1083Fu-0x107CBu==116u);


struct DpmiBlockResult{bool carry=false;u32 linear=0;u32 handle=0;};
struct HeapGrowthBoundaries{DpmiBlockResult(*resize_0503)(u32,u32)=nullptr;DpmiBlockResult(*alloc_0501)(u32)=nullptr;bool(*dos_alloc_48)(u16,u16&)=nullptr;void(*dos_free_49)(u16)=nullptr;u32(*segment_base_0006)(u16)=nullptr;u32(*legacy_alloc_80004800)(u32)=nullptr;u32(*extend_10dd7)(u32)=nullptr;void(*free_db5b)(u32)=nullptr;};
[[nodiscard]]u32 function_10974(Machine&,HeapCoreBoundaries&,u32)noexcept;
// 1083F..10973: arena resize. DPMI 0503 linear address and SI:DI handle remain distinct boundary results.
[[nodiscard]]u32 function_1083f(Machine&m,HeapCoreBoundaries&hb,HeapGrowthBoundaries&gb,u32 terminal,u32 requested)noexcept{function_107cb(m,hb);u32 previous=0,arena=load_le32(dgroup_ptr(m,dgroup(0x8e74)));while(arena){if(terminal==arena+0x20u){u32 terminal_size=hread(hb,terminal)&0xfffffffeu;if(terminal_size+0x0cu!=hread(hb,arena+0x14u)||hread(hb,arena+0x10u)!=0u)return 0u;function_10731(m,hb,previous,arena);const u32 bytes=(requested+0x2cu+0xfffu)&0xfffff000u;DpmiBlockResult r=gb.resize_0503?gb.resize_0503(hread(hb,arena),bytes):DpmiBlockResult{true,0,0};u32 new_arena=r.carry?arena:r.linear;if(!r.carry){hwrite(hb,new_arena,r.handle);hwrite(hb,new_arena+4u,bytes);hwrite(hb,new_arena+0x14u,bytes-0x1cu);}hwrite(hb,new_arena+8u,load_le32(dgroup_ptr(m,dgroup(0x8e74))));store_le32(dgroup_ptr(m,dgroup(0x8e74)),new_arena);hwrite(hb,new_arena+0x0cu,0u);hwrite(hb,new_arena+0x10u,0u);u32 freep=function_1069b(m,hb,new_arena+0x14u);if(!r.carry){const u32 split=hread(hb,new_arena+0x20u)-requested;if(split>=0x0cu){hwrite(hb,new_arena+0x20u,requested|1u);u32 tail=new_arena+0x20u+requested;hwrite(hb,tail,split|1u);store_le32(dgroup_ptr(m,dgroup(0x30c8)),0xffffffffu);store_le32(dgroup_ptr(m,dgroup(0x30cc)),load_le32(dgroup_ptr(m,dgroup(0x30cc)))+1u);if(gb.free_db5b)gb.free_db5b(tail+4u);}}hwrite(hb,new_arena+0x20u,hread(hb,new_arena+0x20u)|1u);return r.carry?0u:freep;}previous=arena;arena=hread(hb,arena+8u);}return 0u;}
static_assert(0x10974u-0x1083Fu==309u);
// 109C1..10AA8: raw arena acquisition, first DPMI 0501h and then the encoded DOS-memory fallback.
[[nodiscard]]u32 function_109c1(Machine&m,HeapCoreBoundaries&hb,HeapGrowthBoundaries&gb,u32 bytes)noexcept{function_107cb(m,hb);DpmiBlockResult r=gb.alloc_0501?gb.alloc_0501(bytes):DpmiBlockResult{true,0,0};if(!r.carry&&r.linear){hwrite(hb,r.linear,r.handle);hwrite(hb,r.linear+4u,bytes);hwrite(hb,r.linear+8u,load_le32(dgroup_ptr(m,dgroup(0x8e74))));store_le32(dgroup_ptr(m,dgroup(0x8e74)),r.linear);hwrite(hb,r.linear+0x10u,0u);return r.linear+0x0cu;}if((load_le16(dgroup_ptr(m,dgroup(0x8e8a)))&0xfff0u)!=0u)store_le32(dgroup_ptr(m,dgroup(0x8e88)),0x000fffffu);if(bytes>0x10000u)return 0u;u16 probe=0;const u16 probe_par=static_cast<u16>((load_le32(dgroup_ptr(m,dgroup(0x8e88)))>>4)|1u);if(!gb.dos_alloc_48||!gb.dos_alloc_48(probe_par,probe))return 0u;if(gb.dos_free_49)gb.dos_free_49(probe);u16 seg=0;if(!gb.dos_alloc_48(static_cast<u16>(bytes>>4),seg))return 0u;u32 linear=gb.segment_base_0006?gb.segment_base_0006(seg):0u;if(!linear){if(gb.dos_free_49)gb.dos_free_49(seg);return 0u;}hwrite(hb,linear+4u,bytes);hwrite(hb,linear+8u,load_le32(dgroup_ptr(m,dgroup(0x8e74))));store_le32(dgroup_ptr(m,dgroup(0x8e74)),linear);hwrite(hb,linear+0x10u,seg);return linear+0x0cu;}
static_assert(0x10AA9u-0x109C1u==232u);
// 10AA9..10C45: runtime heap-extension driver; mode 1, mode 9, and break-extension paths remain separate.
[[nodiscard]]u32 function_10aa9(Machine&m,HeapCoreBoundaries&hb,HeapGrowthBoundaries&gb,u32 request)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x55cc)))==0u||load_le32(dgroup_ptr(m,dgroup(0x52f0)))==0xfffffffeu)return 0u;u32 size=(request+3u)&0xfffffffcu;if(size==0u)return 0u;const u8 mode=*dgroup_ptr(m,dgroup(0x531e));if(mode==1u||mode==9u)size+=0x2cu;else size-=function_10974(m,hb,load_le32(dgroup_ptr(m,dgroup(0x52f0))));if(size+0x20u<size)return 0u;size+=0x20u;u32 minimum=load_le32(dgroup_ptr(m,dgroup(0x55d0)));if(size<minimum)size=minimum&0xfffffffeu;size=(size+0xfffu)&0xfffff000u;u32 raw=0,usable=size;if(mode==1u){raw=function_109c1(m,hb,gb,size);usable-=0x14u;}else if(mode==9u){raw=gb.legacy_alloc_80004800?gb.legacy_alloc_80004800(size):0u;usable-=8u;if(raw){hwrite(hb,raw,0u);raw+=8u;}}else{u32 limit=load_le32(dgroup_ptr(m,dgroup(0x52f0)));u32 desired=size+limit;if(desired<limit)desired=0xfffffffeu;u32 old=gb.extend_10dd7?gb.extend_10dd7(desired):0xffffffffu;if(old==0xffffffffu||old>0xfffffff8u||size<=old)return 0u;raw=old;usable=size-old;}if(raw==0u)return 0u;u32 block_size=usable-8u;if(block_size>usable)return 0u;u32 block=raw;if(load_le32(dgroup_ptr(m,dgroup(0x30b0)))!=0u){u32 top=load_le32(dgroup_ptr(m,dgroup(0x30b0)));u32 end=top+hread(hb,top);if(raw-8u==end){hwrite(hb,top,hread(hb,top)+block_size+8u);hwrite(hb,end+block_size+8u,0xffffffffu);hwrite(hb,end+block_size+0x0cu,0u);block=end;goto finish;}}if(block_size<0x18u)return 0u;hwrite(hb,raw,block_size);block=function_1069b(m,hb,raw);finish:hwrite(hb,block,hread(hb,block)|1u);block+=4u;store_le32(dgroup_ptr(m,dgroup(0x30c8)),0xffffffffu);store_le32(dgroup_ptr(m,dgroup(0x30cc)),load_le32(dgroup_ptr(m,dgroup(0x30cc)))+1u);if(gb.free_db5b)gb.free_db5b(block);return 1u;}
static_assert(0x10C46u-0x10AA9u==413u);


// 10C6D..10C6E are two zero alignment bytes. 10C6F starts the next routine.
// 10C6F..10D1B: optimized NUL-terminated byte-string comparison. Four dwords are tested per iteration; mismatching dwords are resolved bytewise in little-endian order.
[[nodiscard]]s32 function_10c6f(const char*a,const char*b)noexcept{if(a==b)return 0;for(;;){for(u32 word=0;word<4;++word){u32 x=load_le32(reinterpret_cast<const u8*>(a+word*4)),y=load_le32(reinterpret_cast<const u8*>(b+word*4));if(x!=y){for(u32 i=0;i<4;++i){u8 xb=static_cast<u8>(x>>(i*8)),yb=static_cast<u8>(y>>(i*8));if(xb!=yb)return xb<yb?-1:1;if(xb==0)return 0;}}else{u32 z=(x+0xfefefeffu)&~x&0x80808080u;if(z)return 0;}}a+=16;b+=16;}}
static_assert(0x10D1Cu-0x10C6Fu==173u);
// 10D1C..10D39: runtime heap-growth defaults/setter.
void function_10d1c(Machine&m)noexcept{store_le32(dgroup_ptr(m,dgroup(0x55d0)),0x8000u);} static_assert(0x10D27u-0x10D1Cu==11u);
[[nodiscard]]u32 function_10d27(Machine&m,u32 value)noexcept{u32 old=load_le32(dgroup_ptr(m,dgroup(0x55cc)));store_le32(dgroup_ptr(m,dgroup(0x55cc)),value);return old;} static_assert(0x10D3Au-0x10D27u==19u);
struct RuntimeIoBoundaries{bool(*resize_break_mode0)(u32)=nullptr;bool(*resize_break_other)(u32)=nullptr;u32(*alloc_node8)()=nullptr;u32(*alloc_stream34)()=nullptr;void(*free_node)(u32)=nullptr;u8(*read8)(u32)=nullptr;u32(*read32)(u32)=nullptr;void(*write8)(u32,u8)=nullptr;void(*write32)(u32,u32)=nullptr;s32(*unlink_path)(const char*)=nullptr;u32(*process_id)()=nullptr;s32(*write_handle)(u32,u32,u32)=nullptr;u32(*fd_flags)(u32)=nullptr;};
[[nodiscard]]u8 tr8(RuntimeIoBoundaries&b,u32 a)noexcept{return b.read8?b.read8(a):0;} [[nodiscard]]u32 tr32(RuntimeIoBoundaries&b,u32 a)noexcept{return b.read32?b.read32(a):0;} void tw8(RuntimeIoBoundaries&b,u32 a,u8 v)noexcept{if(b.write8)b.write8(a,v);} void tw32(RuntimeIoBoundaries&b,u32 a,u32 v)noexcept{if(b.write32)b.write32(a,v);}
[[nodiscard]]u32 function_10dd7(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept;
// 10D3A..10DD6: allocation/break wrapper. Modes 1 and 9 round positive requests to 4 KiB; other modes add the current break and delegate to 10DD7.
[[nodiscard]]u32 function_10d3a(Machine&m,FileBoundaries&f,HeapGrowthBoundaries&gb,RuntimeIoBoundaries&tb,s32 request)noexcept{const u8 mode=*dgroup_ptr(m,dgroup(0x531e));if(mode!=1u&&mode!=9u)return function_10dd7(m,f,tb,static_cast<u32>(request)+load_le32(dgroup_ptr(m,dgroup(0x52f0))));if(request<=0){if(f.call_103b7)if(u32*p=f.call_103b7())*p=9u;return 0xffffffffu;}u32 n=(static_cast<u32>(request)+0xfffu)&0xfffff000u;u32 out=0;if(mode==1u){DpmiBlockResult r=gb.alloc_0501?gb.alloc_0501(n):DpmiBlockResult{true,0,0};out=r.carry?0u:r.linear;}else out=gb.legacy_alloc_80004800?gb.legacy_alloc_80004800(n):0u;if(!out&&f.call_103b7)if(u32*p=f.call_103b7())*p=5u;return out?out:0xffffffffu;}
static_assert(0x10DD7u-0x10D3Au==157u);
// 10DD7..10E80: program-break resize. The two DOS/extender resize mechanisms remain explicit OS boundaries; success returns the previous 52F0 break and stores the requested break.
[[nodiscard]]u32 function_10dd7(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b,u32 requested)noexcept{bool ok=false;if(requested>=load_le32(dgroup_ptr(m,dgroup(0x5304)))){if(*dgroup_ptr(m,dgroup(0x531e))==0u){u32 paragraphs=(requested+0x0fu)>>4;if(paragraphs==0u)paragraphs=0x0fffffffu;ok=b.resize_break_mode0?b.resize_break_mode0(paragraphs):false;}else ok=b.resize_break_other?b.resize_break_other(requested):false;}if(!ok){if(f.call_103b7)if(u32*p=f.call_103b7())*p=5u;return 0xffffffffu;}u32 old=load_le32(dgroup_ptr(m,dgroup(0x52f0)));store_le32(dgroup_ptr(m,dgroup(0x52f0)),requested);return old;}
static_assert(0x10E81u-0x10DD7u==170u);

[[nodiscard]]s32 function_1104a(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32,u32)noexcept; void function_10fc9(Machine&,RuntimeIoBoundaries&,u32)noexcept;
// 10E81..10EC5: build the stream-registration list from the fixed 26-byte records beginning at DGROUP 539C.
void function_10e81(Machine&m,RuntimeIoBoundaries&b)noexcept{*dgroup_ptr(m,dgroup(0x53dd))&=0xf8u;*dgroup_ptr(m,dgroup(0x53dd))|=4u;for(u32 stream=0x539cu;load_le32(dgroup_ptr(m,dgroup(stream+0x0cu)))!=0u;stream+=0x1au){u32 node=b.alloc_node8?b.alloc_node8():0u;if(!node)break;tw32(b,node+4u,stream);tw32(b,node,load_le32(dgroup_ptr(m,dgroup(0x8e84))));store_le32(dgroup_ptr(m,dgroup(0x8e84)),node);}store_le32(dgroup_ptr(m,dgroup(0x8e70)),0u);} static_assert(0x10EC6u-0x10E81u==69u);
// 10EC6..10EDF: close-all wrappers.
[[nodiscard]]s32 function_10ee0(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept;
[[nodiscard]]s32 function_10ec6(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b)noexcept{(void)function_10ee0(m,f,b,0u);return 0;} static_assert(0x10ED5u-0x10EC6u==15u);
[[nodiscard]]s32 function_10ed5(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b)noexcept{return function_10ee0(m,f,b,5u);} static_assert(0x10EE0u-0x10ED5u==11u);
// 10EE0..10F2D: walk registered streams, closing records selected by the threshold and 0x40 flag.
[[nodiscard]]s32 function_10ee0(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b,u32 index)noexcept{u32 threshold=0x539cu+index*0x1au,count=0;for(u32 node=load_le32(dgroup_ptr(m,dgroup(0x8e84)));node;){u32 next=tr32(b,node),stream=tr32(b,node+4u);if(stream>=threshold){u32 close_handle=((tr8(b,stream+0x0du)&0x40u)!=0u||stream>=0x541eu)?1u:0u;++count;(void)function_1104a(m,f,b,stream,close_handle);}node=next;}return static_cast<s32>(count);} static_assert(0x10F2Eu-0x10EE0u==78u);
// 10F2E..10FC8: acquire/register a stream record, preferring the recycled-node list, then unused fixed records, then a 34-byte dynamic allocation.
[[nodiscard]]u32 function_10f2e(Machine&m,RuntimeIoBoundaries&b)noexcept{u32 node=load_le32(dgroup_ptr(m,dgroup(0x8e70))),stream=0,flags=0;if(node){store_le32(dgroup_ptr(m,dgroup(0x8e70)),tr32(b,node));stream=tr32(b,node+4u);flags=(tr32(b,stream+0x0cu)&0x4003u)|3u;}else{for(stream=0x539cu;stream<0x55a4u;stream+=0x1au)if((load_le32(dgroup_ptr(m,dgroup(stream+0x0cu)))&3u)==0u){node=b.alloc_node8?b.alloc_node8():0u;if(!node)return 0u;flags=3u;break;}if(stream>=0x55a4u){node=b.alloc_stream34?b.alloc_stream34():0u;if(!node)return 0u;flags=0x4003u;stream=node+8u;}}tw32(b,stream+0x0cu,flags);tw32(b,stream,0u);tw32(b,node+4u,stream);tw32(b,node,load_le32(dgroup_ptr(m,dgroup(0x8e84))));store_le32(dgroup_ptr(m,dgroup(0x8e84)),node);return stream;} static_assert(0x10FC9u-0x10F2Eu==155u);
// 10FC9..10FFB: unregister stream and recycle its registration node.
void function_10fc9(Machine&m,RuntimeIoBoundaries&b,u32 stream)noexcept{u32 link=0x8e84u,node=load_le32(dgroup_ptr(m,dgroup(link)));while(node){if(tr32(b,node+4u)==stream){tw8(b,stream+0x0cu,static_cast<u8>(tr8(b,stream+0x0cu)|3u));u32 next=tr32(b,node);if(link==0x8e84u)store_le32(dgroup_ptr(m,dgroup(link)),next);else tw32(b,link,next);tw32(b,node,load_le32(dgroup_ptr(m,dgroup(0x8e70))));store_le32(dgroup_ptr(m,dgroup(0x8e70)),node);return;}link=node;node=tr32(b,node);}} static_assert(0x10FFCu-0x10FC9u==51u);
// 10FFC..1101D: release every recycled registration node.
void function_10ffc(Machine&m,RuntimeIoBoundaries&b)noexcept{while(load_le32(dgroup_ptr(m,dgroup(0x8e70)))!=0u){u32 node=load_le32(dgroup_ptr(m,dgroup(0x8e70))),next=tr32(b,node);if(b.free_node)b.free_node(node);store_le32(dgroup_ptr(m,dgroup(0x8e70)),next);}} static_assert(0x1101Eu-0x10FFCu==34u);
// 1101E..11049: close a stream only if it is currently registered.
[[nodiscard]]s32 function_1101e(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream)noexcept{for(u32 node=load_le32(dgroup_ptr(m,dgroup(0x8e84)));node;node=tr32(b,node))if(tr32(b,node+4u)==stream)return function_1104a(m,f,b,stream,1u);return -1;} static_assert(0x1104Au-0x1101Eu==44u);
[[nodiscard]]s32 function_11100(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32,u32)noexcept;
// 1104A..1106D: close implementation followed by list removal.
[[nodiscard]]s32 function_1104a(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream,u32 close_handle)noexcept{s32 r=function_11100(m,f,b,stream,close_handle);function_10fc9(m,b,stream);return r;} static_assert(0x1106Eu-0x1104Au==36u);
// 1106E..11080: hexadecimal digit helper used by temporary-name construction.
[[nodiscard]]u8 function_1106e(u32 nibble)noexcept{u32 c=nibble+0x30u;if(c>0x39u)c+=0x27u;return static_cast<u8>(c);} static_assert(0x11081u-0x1106Eu==19u);
// 11081..110FF: construct tXXXX_YY.tmp from process-id low 16 bits and an 8-bit suffix.
void function_11081(RuntimeIoBoundaries&b,char*out,u32 suffix)noexcept{u32 pid=b.process_id?b.process_id():0u;out[0]='t';for(int i=4;i>=1;--i){out[i]=static_cast<char>(function_1106e(pid&0x0fu));pid>>=4;}out[5]='_';out[6]=static_cast<char>(function_1106e((suffix>>4)&0x0fu));out[7]=static_cast<char>(function_1106e(suffix&0x0fu));out[8]='.';out[9]='t';out[10]='m';out[11]='p';out[12]=0;} static_assert(0x11100u-0x11081u==127u);
[[nodiscard]]s32 function_111a4(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11241(FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept;
// 11100..1119D: stream close/flush driver, including optional seek correction, handle close, owned-buffer free, and temporary-file deletion.
[[nodiscard]]s32 function_11100(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream,u32 close_handle)noexcept{if(tr32(b,stream+0x0cu)==0u)return -1;s32 status=0;if(tr8(b,stream+0x0du)&0x10u)status=function_111a4(m,f,b,stream);s32 pos=function_11241(f,b,stream);if(pos!=-1&&f.dos_seek)(void)f.dos_seek(static_cast<u16>(tr32(b,stream+0x10u)),pos,0u);if(close_handle&&function_d573(f,tr32(b,stream+0x10u))!=0)status=-1;if(tr8(b,stream+0x0cu)&8u){if(b.free_node)b.free_node(tr32(b,stream+8u));tw32(b,stream+8u,0u);}if(tr8(b,stream+0x0du)&8u){char name[16]{};function_11081(b,name,tr8(b,stream+0x19u));if(b.unlink_path)(void)b.unlink_path(name);}return status;} static_assert(0x1119Eu-0x11100u==158u);
// 1119E..111A3: process-id getter.
[[nodiscard]]u32 function_1119e(Machine&m)noexcept{return load_le32(dgroup_ptr(m,dgroup(0x52fcu)));} static_assert(0x111A4u-0x1119Eu==6u);
// 111A4..11240: flush buffered stream. Write-mode buffers call 11285; read-mode buffers seek backward by the unread count unless flag 0x20 suppresses it.
[[nodiscard]]s32 function_111a4(Machine&,FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream)noexcept{s32 status=0;if(!(tr8(b,stream+0x0du)&0x10u))goto finish;tw8(b,stream+0x0du,static_cast<u8>(tr8(b,stream+0x0du)&0xefu));if(tr8(b,stream+0x0cu)&2u){if(tr32(b,stream+8u)){s32 wrote=b.write_handle?b.write_handle(tr32(b,stream+0x10u),tr32(b,stream+8u),tr32(b,stream+4u)):-1;if(wrote==-1)status=-1;else if(static_cast<u32>(wrote)!=tr32(b,stream+4u))status=-1;}}else if(tr32(b,stream+8u)){tw8(b,stream+0x0cu,static_cast<u8>(tr8(b,stream+0x0cu)&0xefu));if(!(tr8(b,stream+0x0du)&0x20u)){s32 n=static_cast<s32>(tr32(b,stream+4u));if(n!=0&&f.dos_seek&&f.dos_seek(static_cast<u16>(tr32(b,stream+0x10u)),-n,1u)==-1)status=-1;}}if(status==-1)tw8(b,stream+0x0cu,static_cast<u8>(tr8(b,stream+0x0cu)|0x20u));finish:tw32(b,stream,tr32(b,stream+8u));tw32(b,stream+4u,0u);return status;} static_assert(0x11241u-0x111A4u==157u);
// 11241..11274: logical stream position from handle position adjusted by buffered byte count.
[[nodiscard]]s32 function_11241(FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream)noexcept{s32 p=f.dos_seek?f.dos_seek(static_cast<u16>(tr32(b,stream+0x10u)),0,1u):-1;if(p==-1)return -1;u32 buffered=tr32(b,stream+4u);if(buffered)p+=(tr8(b,stream+0x0du)&0x10u)?static_cast<s32>(buffered):-static_cast<s32>(buffered);return p;} static_assert(0x11275u-0x11241u==52u);
// 11275..11284: unlink wrapper.
[[nodiscard]]s32 function_11275(RuntimeIoBoundaries&b,const char*path)noexcept{return b.unlink_path?b.unlink_path(path):-1;} static_assert(0x11285u-0x11275u==16u);
// 11285..11301: low-level write. Character devices first seek to end; DOS write errors are mapped, and short writes set errno 12 while returning the short byte count.
[[nodiscard]]s32 function_11285(FileBoundaries&f,RuntimeIoBoundaries&b,u32 handle,u32 buffer,u32 count)noexcept{if(b.fd_flags&&(b.fd_flags(handle)&0x80u)){if(f.dos_seek&&f.dos_seek(static_cast<u16>(handle),0,2u)==-1)return -1;}s32 wrote=b.write_handle?b.write_handle(handle,buffer,count):-1;if(wrote<0)return -1;if(static_cast<u32>(wrote)!=count&&f.call_103b7)if(u32*p=f.call_103b7())*p=12u;return wrote;} static_assert(0x11302u-0x11285u==125u);
// 10974..109C0: report the top arena's terminal extent only when it exactly reaches the current selector/stack limit.
[[nodiscard]]u32 function_10974(Machine&m,HeapCoreBoundaries&b,u32 selector_limit)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x30b0)))==0u)return 0u;u32 a=load_le32(dgroup_ptr(m,dgroup(0x30d8)));u32 extent=hread(b,a);u32 end=a+extent+8u;if(end!=selector_limit)return 0u;return extent;}
static_assert(0x109C1u-0x10974u==77u);
// 10C46..10C4C: constant-success/zero helper.
[[nodiscard]]u32 function_10c46()noexcept{return 0u;} static_assert(0x10C4Du-0x10C46u==7u);
// 10C4D..10C58: fatal runtime path, encoded as CE8E(0x390,1).
struct HeapTailBoundaries{void(*fatal_ce8e)(u32,u32)=nullptr;}; void function_10c4d(HeapTailBoundaries&b)noexcept{if(b.fatal_ce8e)b.fatal_ce8e(0x390u,1u);} static_assert(0x10C59u-0x10C4Du==12u);
// 10C59..10C6C: seek wrapper equivalent to D85F(handle,0,SEEK_CUR).
[[nodiscard]]s32 function_10c59(FileBoundaries&b,u32 h)noexcept{return function_d85f(b,static_cast<u16>(h),0,1u);} static_assert(0x10C6Du-0x10C59u==20u);

struct DirectoryBoundaries{s32(*map_error)(u16)=nullptr;s32(*get_current_directory)(u8 drive,char*out)=nullptr;u8(*get_current_drive)()=nullptr;s32(*set_current_drive)(u8 drive,u8*count)=nullptr;s32(*change_directory)(const char*)=nullptr;s32(*make_directory)(const char*)=nullptr;};
[[nodiscard]]u16 function_d8d0(DirectoryBoundaries&b,u32*out)noexcept{const u8 drive=b.get_current_drive?b.get_current_drive():0;if(out)*out=static_cast<u32>(drive)+1u;return 0;}
[[nodiscard]]u32 function_d8e8(DirectoryBoundaries&b,u32 drive,u32*out)noexcept{u8 count=0;const s32 r=b.set_current_drive?b.set_current_drive(static_cast<u8>(drive-1u),&count):0;if(out)*out=count;(void)r;return 0;}
struct CurrentDirBoundaries:DirectoryBoundaries{char*(*allocate)(u32)=nullptr;u32(*string_length)(const char*)=nullptr;void(*copy_string)(char*,const char*)=nullptr;};
[[nodiscard]]char* function_d906(CurrentDirBoundaries&b,char*out)noexcept{char tail[112]{};if(b.get_current_directory&&b.get_current_directory(0,tail)<0)return nullptr;char prefix[3]={static_cast<char>((b.get_current_drive?b.get_current_drive():0)+'A'),':','\\'};u32 tail_len=b.string_length?b.string_length(tail):static_cast<u32>(std::strlen(tail));char*dst=out;if(!dst){dst=b.allocate?b.allocate(tail_len+4u):nullptr;if(!dst)return nullptr;}dst[0]=prefix[0];dst[1]=prefix[1];dst[2]=prefix[2];if(b.copy_string)b.copy_string(dst+3,tail);else std::memcpy(dst+3,tail,tail_len+1u);return dst;}
[[nodiscard]]u32 function_d991(u32 ch)noexcept{return(ch>=0x61u&&ch<=0x7au)?ch-0x20u:ch;}
[[nodiscard]]s32 function_d9a6(DirectoryBoundaries&b,const char*path)noexcept{const s32 r=b.change_directory?b.change_directory(path):-1;return r<0?(b.map_error?b.map_error(static_cast<u16>(-r)):r):0;}
[[nodiscard]]s32 function_d9ca(DirectoryBoundaries&b,const char*path)noexcept{const s32 r=b.make_directory?b.make_directory(path):-1;return r<0?(b.map_error?b.map_error(static_cast<u16>(-r)):r):0;}
void* function_d9ee(void*dst,const void*src,u32 count)noexcept{return std::memcpy(dst,src,static_cast<std::size_t>(count));}
void function_da18(Machine&m)noexcept{m.cpu.interrupt_enable=false;} void function_da1a(Machine&m)noexcept{m.cpu.interrupt_enable=true;}

struct DosMemResult{bool carry=false;u16 ax=0;u16 bx=0;};
struct LowLevelBoundaries{DosMemResult(*allocate_paragraphs)(u16)=nullptr;DosMemResult(*free_segment)(u16)=nullptr;DosMemResult(*resize_segment)(u16,u16)=nullptr;u8(*in8)(u16)=nullptr;void(*out8)(u16,u8)=nullptr;void(*set_vector_legacy)(u8,u16,u32)=nullptr;void(*set_vector_2504)(u8,u16,u32)=nullptr;void(*get_vector_legacy)(u8,u16*,u32*)=nullptr;void(*get_vector_2502)(u8,u16*,u32*)=nullptr;s32(*map_dos_error)(u16)=nullptr;};
[[nodiscard]]s32 dos_mem_result_status(LowLevelBoundaries&b,const DosMemResult&r)noexcept{return r.carry?(b.map_dos_error?b.map_dos_error(r.ax):-1):0;}
[[nodiscard]]s32 function_da48(LowLevelBoundaries&b,u32 paragraphs,u32*out_value)noexcept{const DosMemResult r=b.allocate_paragraphs?b.allocate_paragraphs(static_cast<u16>(paragraphs)):DosMemResult{true,1,0};if(out_value)*out_value=r.carry?static_cast<u32>(r.bx):static_cast<u32>(r.ax);return dos_mem_result_status(b,r);}
[[nodiscard]]s32 function_da6c(LowLevelBoundaries&b,u32 segment)noexcept{const DosMemResult r=b.free_segment?b.free_segment(static_cast<u16>(segment)):DosMemResult{true,1,0};return dos_mem_result_status(b,r);}
[[nodiscard]]s32 function_da7e(LowLevelBoundaries&b,u32 paragraphs,u32 segment,u32*out_max)noexcept{const DosMemResult r=b.resize_segment?b.resize_segment(static_cast<u16>(segment),static_cast<u16>(paragraphs)):DosMemResult{true,1,0};if(r.carry&&out_max)*out_max=r.bx;return dos_mem_result_status(b,r);}
[[nodiscard]]u32 function_daa6(Machine&m)noexcept{u32 node=load_le32(dgroup_ptr(m,dgroup(0x30dc))),largest=0;while(node!=0x30d4u){const u32 size=load_le32(dgroup_ptr(m,dgroup(node)))-4u;if(size>largest)largest=size;node=load_le32(dgroup_ptr(m,dgroup(node+8u)));}return largest;}
void function_dde0()noexcept{}
[[nodiscard]]u8 function_dde7(LowLevelBoundaries&b,u32 port)noexcept{return b.in8?b.in8(static_cast<u16>(port)):0;}
void function_ddf2(LowLevelBoundaries&b,u32 port,u32 value)noexcept{if(b.out8)b.out8(static_cast<u16>(port),static_cast<u8>(value));}
void function_ddfe(LowLevelBoundaries&b,u8 extender_mode,u32 vector,u32 offset,u16 selector)noexcept{if(extender_mode>=2&&extender_mode<=8){if(b.set_vector_2504)b.set_vector_2504(static_cast<u8>(vector),selector,offset);}else if(b.set_vector_legacy)b.set_vector_legacy(static_cast<u8>(vector),selector,offset);}
struct VectorAddress{u32 offset=0;u16 selector=0;};
[[nodiscard]]VectorAddress function_de35(LowLevelBoundaries&b,u8 extender_mode,u32 vector)noexcept{VectorAddress r{};if(extender_mode>=2&&extender_mode<=8){if(b.get_vector_2502)b.get_vector_2502(static_cast<u8>(vector),&r.selector,&r.offset);}else if(b.get_vector_legacy)b.get_vector_legacy(static_cast<u8>(vector),&r.selector,&r.offset);return r;}
[[nodiscard]]u8* function_dea7(u8*dst,u32 pattern,u32 dword_count)noexcept{u8*p=dst;while(dword_count&&((reinterpret_cast<std::uintptr_t>(p)&31u)!=0)){store_le32(p,pattern);p+=4;--dword_count;}while(dword_count>=8){for(unsigned i=0;i<8;++i)store_le32(p+i*4u,pattern);p+=32;dword_count-=8;}while(dword_count){store_le32(p,pattern);p+=4;--dword_count;}return p;}
void function_df13(LowLevelBoundaries&b,u32 divisor_source)noexcept{if(divisor_source<=0x12u)return;u32 divisor=0x1234dcu/divisor_source;u8 gate=function_dde7(b,0x61);if((gate&3u)==0){function_ddf2(b,0x61,static_cast<u8>(gate|3u));function_ddf2(b,0x43,0xb6);}function_ddf2(b,0x42,divisor);function_ddf2(b,0x42,divisor>>8);}
static_assert(0xDF73u-0xDF13u==96u);
void function_df73(LowLevelBoundaries&b)noexcept{function_ddf2(b,0x61,static_cast<u8>(function_dde7(b,0x61)&0xfcu));}
constexpr char kRadixDigits[]="0123456789abcdefghijklmnopqrstuvwxyz";
static char* convert_unsigned(u32 value,char*out,u32 radix)noexcept{char tmp[40]{};char*p=tmp+1;do{const u32 q=value/radix;const u32 rem=value%radix;*p++=kRadixDigits[rem];value=q;}while(value);char*d=out;do{*d++=*--p;}while(p!=tmp);return out;}
[[nodiscard]]char* function_dfb3(u32 value,char*out,u32 radix)noexcept{return convert_unsigned(value,out,radix);}
[[nodiscard]]char* function_e050(u32 value,char*out,u32 radix)noexcept{return convert_unsigned(value,out,radix);}
static char* convert_signed(s32 value,char*out,u32 radix,bool second)noexcept{char*digits=out;u32 magnitude=static_cast<u32>(value);if(radix==10u&&value<0){*digits++='-';magnitude=0u-magnitude;}if(second)(void)function_e050(magnitude,digits,radix);else(void)function_dfb3(magnitude,digits,radix);return out;}
[[nodiscard]]char* function_dffb(s32 value,char*out,u32 radix)noexcept{return convert_signed(value,out,radix,false);}
[[nodiscard]]char* function_e098(s32 value,char*out,u32 radix)noexcept{return convert_signed(value,out,radix,true);}

// E3B4..E4C6: verified callback dispatcher and INT 8 paths.
struct DispatcherBoundary{u32(*invoke)(u32,const u32*,u32)=nullptr;void(*invoke_timer_callback)(u32)=nullptr;void(*out8)(u16,u8)=nullptr;u32(*allocate_timer)(u32)=nullptr;void(*release_timer)(u32)=nullptr;void(*set_timer_rate)(u32,u32)=nullptr;void(*enable_timer)(u32)=nullptr;};
[[nodiscard]]u32 function_e3b4(Machine&m,u32 event,u32 slot)noexcept{if(slot>=16)return 0;u32 entry=load_le32(m.data+0x5216u+slot*4u);if(entry==0)return 0;for(;;entry+=8u){const u32 key=load_le32(m.data+entry);if(key==event)return load_le32(m.data+entry+4u);if(key==0xffffffffu)return 0;}}
[[nodiscard]]u32 function_e3e0(Machine&m,DispatcherBoundary&b,u32 event,const u32*args,u32 count)noexcept{if(count==0)return 0;const u32 target=function_e3b4(m,event,args[0]);return target&&b.invoke?b.invoke(target,args,count):0;}
enum class Irq8Action:u8{Handled,ChainSavedVector}; struct Irq8Result{Irq8Action action=Irq8Action::Handled;u16 selector=0;u32 offset=0;};
[[nodiscard]]Irq8Result function_e3f2(Machine&m,DispatcherBoundary&b)noexcept{if(load_le16(m.data+0x510e)!=0)return{Irq8Action::ChainSavedVector,static_cast<u16>(load_le32(m.data+0x5206)),load_le32(m.data+0x5202)};store_le16(m.data+0x510e,1);store_le32(m.data+0x520a,0);for(u32 slot=0;slot<=16;++slot){if(load_le16(m.data+0x5154u+slot*2u)==2){const u32 i=slot*4u;u32 accumulated=load_le32(m.data+0x5176u+i)+load_le32(m.data+0x51fe);const u32 period=load_le32(m.data+0x51bau+i);if(accumulated>=period){accumulated-=period;store_le32(m.data+0x5176u+i,accumulated);if(slot>=16)store_le16(m.data+0x510e,0);if(b.invoke_timer_callback)b.invoke_timer_callback(load_le32(m.data+0x5110u+i));}else store_le32(m.data+0x5176u+i,accumulated);}store_le32(m.data+0x520a,slot+1u);}store_le16(m.data+0x510e,0);if(b.out8)b.out8(0x20,0x20);return{};}

// E4C7..EA0A: verified timer/PIT setup and slot-state routines.
static bool irq_save_cli(Machine&m)noexcept{const bool prior=m.cpu.interrupt_enable;m.cpu.interrupt_enable=false;return prior;}
static void irq_restore(Machine&m,bool prior)noexcept{m.cpu.interrupt_enable=prior;}
void function_e4c7(Machine&m)noexcept{const bool prior=irq_save_cli(m);store_le32(dgroup_ptr(m,dgroup(0x51fe)),0xffffffffu);for(u32 i=0;i<17;++i)store_le16(dgroup_ptr(m,dgroup(0x5154u+i*2u)),0);for(u32 i=0;i<17;++i)store_le32(dgroup_ptr(m,dgroup(0x5176u+i*4u)),0);for(u32 i=0;i<17;++i)store_le32(dgroup_ptr(m,dgroup(0x51bau+i*4u)),0);irq_restore(m,prior);}
void function_e514(Machine&m,LowLevelBoundaries&b,u16 code_selector)noexcept{const bool prior=irq_save_cli(m);VectorAddress old=function_de35(b,0,8);m.cpu.edx=(m.cpu.edx&0xffff0000u)|old.selector;store_le32(dgroup_ptr(m,dgroup(0x5202)),old.offset);store_le32(dgroup_ptr(m,dgroup(0x5206)),m.cpu.edx);store_le32(dgroup_ptr(m,dgroup(0x5150)),0xe4a7u);if(b.set_vector_legacy)b.set_vector_legacy(8,code_selector,0xe3f2u);irq_restore(m,prior);}
void function_e563(Machine&m,LowLevelBoundaries&b)noexcept{const bool prior=irq_save_cli(m);store_le32(dgroup_ptr(m,dgroup(0x520a)),0xffffffffu);if(b.set_vector_legacy)b.set_vector_legacy(8,static_cast<u16>(load_le32(dgroup_ptr(m,dgroup(0x5206)))),load_le32(dgroup_ptr(m,dgroup(0x5202))));irq_restore(m,prior);}
void function_e59b(Machine&m,LowLevelBoundaries&b,u32 reload)noexcept{const bool prior=irq_save_cli(m);function_ddf2(b,0x43,0x36);store_le32(dgroup_ptr(m,dgroup(0x5212)),reload);function_ddf2(b,0x40,reload);function_ddf2(b,0x40,reload>>8);irq_restore(m,prior);}
void function_e5c9(Machine&m,LowLevelBoundaries&b,u32 period)noexcept{u32 reload=0;if(period<0xd68du)reload=static_cast<u32>((static_cast<std::uint64_t>(period)*0x2710u)/0x20bcu);function_e59b(m,b,reload);}
void function_e5fa(Machine&m,LowLevelBoundaries&b)noexcept{const bool prior=irq_save_cli(m);u32 minimum=0xffffffffu;for(u32 i=0;i<=16;++i){if(load_le16(dgroup_ptr(m,dgroup(0x5154u+i*2u)))!=0){const u32 candidate=load_le32(dgroup_ptr(m,dgroup(0x51bau+i*4u)));if(candidate<minimum)minimum=candidate;}}store_le32(dgroup_ptr(m,dgroup(0x520e)),minimum);if(minimum!=load_le32(dgroup_ptr(m,dgroup(0x51fe)))){store_le32(dgroup_ptr(m,dgroup(0x520a)),0xffffffffu);store_le32(dgroup_ptr(m,dgroup(0x51fe)),minimum);function_e5c9(m,b,minimum);for(u32 i=0;i<17;++i)store_le32(dgroup_ptr(m,dgroup(0x5176u+i*4u)),0);}irq_restore(m,prior);}
void function_e680(Machine&m)noexcept{const bool prior=irq_save_cli(m);store_le16(dgroup_ptr(m,dgroup(0x52e6)),m.cpu.ds);store_le16(dgroup_ptr(m,dgroup(0x510c)),0);store_le16(dgroup_ptr(m,dgroup(0x510e)),0);for(u32 i=0;i<16;++i)store_le32(dgroup_ptr(m,dgroup(0x5216u+i*4u)),0);for(u32 i=0;i<16;++i)store_le32(dgroup_ptr(m,dgroup(0x5256u+i*4u)),0xffffffffu);for(u32 i=0;i<16;++i)store_le32(dgroup_ptr(m,dgroup(0x5296u+i*4u)),0);irq_restore(m,prior);}

struct TimerContinuationBoundaries{void(*call_ebb6)(u32 slot,u32 argument)=nullptr;u16 code_selector=0;};
void function_e809(Machine&m,LowLevelBoundaries&b,u32 slot)noexcept; void function_e856(Machine&m,LowLevelBoundaries&b)noexcept; void function_e87b(Machine&m,u32 slot)noexcept; void function_e925(Machine&m,LowLevelBoundaries&b,u32 slot,u32 period)noexcept;
void function_e6ea(Machine&m,LowLevelBoundaries&b,TimerContinuationBoundaries&x,u32 argument)noexcept{const bool prior=irq_save_cli(m);store_le32(dgroup_ptr(m,dgroup(0x52da)),0);do{const u32 slot=load_le32(dgroup_ptr(m,dgroup(0x52da)));const u32 i=slot*4u;const u32 handle=load_le32(dgroup_ptr(m,dgroup(0x5256u+i)));if(load_le32(dgroup_ptr(m,dgroup(0x5216u+i)))!=0){if(handle!=0xffffffffu)function_e809(m,b,handle);if(x.call_ebb6)x.call_ebb6(slot,argument);}store_le32(dgroup_ptr(m,dgroup(0x52da)),slot+1u);}while(load_le32(dgroup_ptr(m,dgroup(0x52da)))!=16u);function_e856(m,b);irq_restore(m,prior);}
[[nodiscard]]u32 function_e759(Machine&m,LowLevelBoundaries&b,TimerContinuationBoundaries&x,u32 value)noexcept{const bool prior=irq_save_cli(m);u32 byte_index=0;while(byte_index<0x20u&&load_le16(dgroup_ptr(m,dgroup(0x5154u+byte_index)))!=0)byte_index+=2u;if(byte_index==0x20u){irq_restore(m,prior);return 0xffffffffu;}const u32 slot=byte_index>>1;store_le16(dgroup_ptr(m,dgroup(0x5154u+byte_index)),1);const u32 dword_index=slot*4u;store_le32(dgroup_ptr(m,dgroup(0x5110u+dword_index)),value);store_le32(dgroup_ptr(m,dgroup(0x51bau+dword_index)),0xffffffffu);const u16 count=static_cast<u16>(load_le16(dgroup_ptr(m,dgroup(0x510c)))+1u);store_le16(dgroup_ptr(m,dgroup(0x510c)),count);if(count==1u){function_e4c7(m);store_le16(dgroup_ptr(m,dgroup(0x5174)),1);function_e514(m,b,x.code_selector);function_e925(m,b,16,0xd68du);function_e87b(m,16);store_le16(dgroup_ptr(m,dgroup(0x5154u+slot*2u)),1);store_le32(dgroup_ptr(m,dgroup(0x51bau+dword_index)),0xffffffffu);}irq_restore(m,prior);return slot;}
void function_e809(Machine&m,LowLevelBoundaries&b,u32 slot)noexcept{const bool prior=irq_save_cli(m);if(slot!=0xffffffffu){const u32 wi=slot*2u;if(load_le16(dgroup_ptr(m,dgroup(0x5154u+wi)))!=0){store_le16(dgroup_ptr(m,dgroup(0x5154u+wi)),0);const u16 count=static_cast<u16>(load_le16(dgroup_ptr(m,dgroup(0x510c)))-1u);store_le16(dgroup_ptr(m,dgroup(0x510c)),count);if(count==0){function_e59b(m,b,0);function_e563(m,b);}}}irq_restore(m,prior);}
void function_e856(Machine&m,LowLevelBoundaries&b)noexcept{const bool prior=irq_save_cli(m);for(s32 slot=15;slot>=0;--slot)function_e809(m,b,static_cast<u32>(slot));irq_restore(m,prior);}
void function_e87b(Machine&m,u32 slot)noexcept{const bool prior=irq_save_cli(m);u8*p=dgroup_ptr(m,dgroup(0x5154u+slot*2u));if(load_le16(p)==1)store_le16(p,2);irq_restore(m,prior);}
void function_e8ab(Machine&m)noexcept{const bool prior=irq_save_cli(m);for(s32 slot=15;slot>=0;--slot)function_e87b(m,static_cast<u32>(slot));irq_restore(m,prior);}
void function_e8d0(Machine&m,u32 slot)noexcept{const bool prior=irq_save_cli(m);u8*p=dgroup_ptr(m,dgroup(0x5154u+slot*2u));if(load_le16(p)==2)store_le16(p,1);irq_restore(m,prior);}
void function_e900(Machine&m)noexcept{const bool prior=irq_save_cli(m);for(s32 slot=15;slot>=0;--slot)function_e8d0(m,static_cast<u32>(slot));irq_restore(m,prior);}
void function_e925(Machine&m,LowLevelBoundaries&b,u32 slot,u32 period)noexcept{const bool prior=irq_save_cli(m);u8*state=dgroup_ptr(m,dgroup(0x5154u+slot*2u));const u16 saved=load_le16(state);store_le16(state,1);store_le32(dgroup_ptr(m,dgroup(0x51bau+slot*4u)),period);store_le32(dgroup_ptr(m,dgroup(0x5176u+slot*4u)),0);function_e5fa(m,b);store_le16(state,saved);irq_restore(m,prior);}
void function_e97a(Machine&m,LowLevelBoundaries&b,u32 slot,u32 divisor)noexcept{const bool prior=irq_save_cli(m);function_e925(m,b,slot,1000000u/divisor);irq_restore(m,prior);}
void function_e9ad(Machine&m,LowLevelBoundaries&b,u32 slot,u32 value)noexcept{const bool prior=irq_save_cli(m);u32 period=0xd68du;if(value!=0)period=static_cast<u32>((static_cast<std::uint64_t>(0x2710u)*value)/0x2e9cu);function_e925(m,b,slot,period);irq_restore(m,prior);}
[[nodiscard]]u32 function_e9f2(Machine&m)noexcept{const bool prior=irq_save_cli(m);const u32 value=load_le32(dgroup_ptr(m,dgroup(0x5212)));irq_restore(m,prior);return value;}

// EA0B..EDE6: verified registration, event dispatch wrappers, and frame no-op.
void function_ea9a(Machine&m,u32 slot)noexcept{const bool prior=irq_save_cli(m);if(slot<16)store_le32(m.data+0x5216u+slot*4u,0);irq_restore(m,prior);}
[[nodiscard]]u32 function_eac8(Machine&m,DispatcherBoundary&b,u32 slot)noexcept{const u32 args[2]={slot,0xe9f2u};return function_e3e0(m,b,0x64u,args,2);}
[[nodiscard]]u32 function_ea0b(Machine&m,DispatcherBoundary&b,u32 descriptor)noexcept{const bool prior=irq_save_cli(m);u32 slot=0;while(slot<16&&load_le32(m.data+0x5216u+slot*4u)!=0)++slot;u32 result=0xffffffffu;if(slot<16&&load_le32(m.data+descriptor+4u)==0x79706f43u){store_le32(m.data+0x5216u+slot*4u,load_le32(m.data+descriptor));const u32 returned=function_eac8(m,b,slot);if(returned!=0&&load_le32(m.data+returned)<=load_le32(m.data+0x52e8u))result=slot;}irq_restore(m,prior);return result;}
[[nodiscard]]u32 function_eae7(Machine&m,DispatcherBoundary&b,const u32*args,u32 count)noexcept{return function_e3e0(m,b,0x65u,args,count);}
void function_eaf1(Machine&m,DispatcherBoundary&b,u32 a1,u32 a2,u32 a3,u32 a4,u32 a5)noexcept{const bool prior=irq_save_cli(m);if(a1<16){store_le32(m.data+0x52e2,0xffffffffu);const u32 record=function_eac8(m,b,a1);const u32 divisor=load_le32(m.data+record+0x20u);if(divisor!=0xffffffffu){const u32 target=function_e3b4(m,0x67u,a1);if(target!=0&&b.allocate_timer){const u32 timer=b.allocate_timer(target);store_le32(m.data+0x5256u+a1*4u,timer);store_le32(m.data+0x52e2,timer);if(b.set_timer_rate)b.set_timer_rate(timer,divisor);}}const u32 args[5]={a1,a2,a3,a4,a5};(void)function_e3e0(m,b,0x66u,args,5);store_le32(m.data+0x5296u+a1*4u,1);const u32 timer=load_le32(m.data+0x52e2);if(timer!=0xffffffffu&&b.enable_timer)b.enable_timer(timer);}irq_restore(m,prior);}
[[nodiscard]]u32 function_ebb6(Machine&m,DispatcherBoundary&b,u32 slot,u32 argument)noexcept{if(slot>=16)return 0;const u32 i=slot*4u;const u32 pending=load_le32(m.data+0x5296u+i);store_le32(m.data+0x5296u+i,0);if(pending==0)return 0;const u32 timer=load_le32(m.data+0x5256u+i);if(timer!=0xffffffffu&&b.release_timer)b.release_timer(timer);const u32 args[2]={slot,argument};return function_e3e0(m,b,0x68u,args,2);}
constexpr u32 kWrapperEvents[48]={0x78,0x79,0x86,0x7a,0x7b,0x85,0x7c,0x7d,0x7e,0x7f,0x80,0x81,0x82,0x83,0x84,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,0xaa,0xab,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,0xc0,0xc1,0xc2};
[[nodiscard]]u32 function_ebf3_edd2(Machine&m,DispatcherBoundary&b,u32 wrapper_index,const u32*args,u32 count)noexcept{return wrapper_index<48?function_e3e0(m,b,kWrapperEvents[wrapper_index],args,count):0;}
void function_ede0()noexcept{}

// F007..F741: verified formatter parser, conversion materializer, and helpers.
struct FormatState{u8 opaque0[4];u32 width;u32 precision;u32 zero_count;u8 opaque10[4];u8 flags;u8 conversion;u8 pad;u8 prefix[3];}; static_assert(sizeof(FormatState)==0x1c); static_assert(offsetof(FormatState,width)==0x04&&offsetof(FormatState,precision)==0x08&&offsetof(FormatState,zero_count)==0x0c&&offsetof(FormatState,flags)==0x14&&offsetof(FormatState,conversion)==0x15&&offsetof(FormatState,pad)==0x16&&offsetof(FormatState,prefix)==0x17);
[[nodiscard]]const u8* function_f110(const u8*p,FormatState&s)noexcept{s.flags=0;for(;;++p){switch(*p){case '-':s.flags|=0x08;break;case '#':s.flags|=0x01;break;case '+':s.flags=static_cast<u8>((s.flags|0x04)&0xfd);break;case ' ':if((s.flags&0x04)==0)s.flags|=0x02;break;case '0':s.pad='0';break;default:return p;}}}
[[nodiscard]]const u8* function_f007(const u8*p,u8*&arg_cursor,FormatState&s)noexcept{s.prefix[0]=0;s.pad=' ';p=function_f110(p,s);s.width=0;if(*p>='0'&&*p<='9'){do{s.width=s.width*10u+static_cast<u32>(*p-'0');++p;}while(*p>='0'&&*p<='9');}else if(*p=='*'){arg_cursor+=4;s.width=load_le32(arg_cursor-4);if(static_cast<s32>(s.width)<0){s.width=0u-s.width;s.flags|=0x08;}++p;}s.precision=0xffffffffu;if(*p=='.'){++p;s.precision=0;if(*p=='*'){arg_cursor+=4;s.precision=load_le32(arg_cursor-4);if(static_cast<s32>(s.precision)<0)s.precision=0xffffffffu;++p;}else while(*p>='0'&&*p<='9'){s.precision=s.precision*10u+static_cast<u32>(*p-'0');++p;}}const u8 c=*p;const u8*next=p+1;if(c=='l'||c=='w'){++p;s.flags|=0x20;}else if(c=='h'){p=next;s.flags|=0x10;}else if(c=='F'){p=next;s.flags|=0x80;}else if(c=='N'||c=='L'){p=next;s.flags|=0x40;}return p;}
struct FarReadBoundary{u8(*read8)(u16,u32)=nullptr;u16(*read16)(u16,u32)=nullptr;};
[[nodiscard]]u32 function_f163(FarReadBoundary&b,u32 offset,u16 selector,u32 limit)noexcept{u32 n=0;while(n!=limit){if(!b.read8||b.read8(selector,offset)==0)break;++offset;++n;}return n;}
[[nodiscard]]u32 function_f18c(FarReadBoundary&b,u32 offset,u16 selector,u32 limit)noexcept{u32 n=0;while(n!=limit){if(!b.read16||b.read16(selector,offset)==0)break;offset+=2;++n;}return n;}
struct StringBoundary{u32(*length)(const char*)=nullptr;};
void function_f1b3(StringBoundary&b,u32 value,char*out,u32 width)noexcept{(void)function_dffb(static_cast<s32>(value),out,16);const u32 n=b.length?b.length(out):static_cast<u32>(std::strlen(out));s32 pad=static_cast<s32>(width)-1;char*src=out+n;char*dst=out+pad;while(src!=out){--src;*dst--=*src;--pad;}char*z=out+pad;while(pad>=0){*z--='0';--pad;}out[width]='\0';}
void function_f722(char*s)noexcept{while(*s){*s=static_cast<char>(function_d991(static_cast<u8>(*s)));++s;}}

// F206..F2D1: signed 16.16 fixed-point decimal materialization.
void function_f206(char*out,s32 value,FormatState&s)noexcept{char*p=out;u32 fixed=static_cast<u32>(value);if(value<0){fixed=0u-fixed;*p++='-';}if(s.precision==0xffffffffu)s.precision=4;(void)function_dffb(static_cast<s32>(static_cast<u16>(fixed>>16)),p,10);char*integer_begin=p;char*end=p;while(*end)++end;if(s.precision!=0){*end++='.';for(u32 i=0;i<s.precision;++i){fixed&=0x0000ffffu;fixed*=10u;*end++=static_cast<char>(static_cast<u8>(fixed>>16)+static_cast<u8>('0'));}*end='\0';}if((fixed&0x00008000u)!=0){char*q=end;for(;;){if(q==integer_begin){char*scan=integer_begin+1;*integer_begin='1';while(*scan=='0')++scan;if(*scan=='.'){*scan='0';char*dot=scan+1;*dot='.';scan=dot+1;while(*scan=='0')++scan;}*scan++='0';*scan='\0';break;}--q;if(*q=='.')--q;if(*q!='9'){++*q;break;}*q='0';}}}

// F2D2..F2E8: indirect conversion thunk; callee returns EAX:DX.
struct FarReturn{u32 offset=0;u16 selector=0;};
struct FormatterBoundary{FarReturn(*call_55c4)(u32,u32,u32)=nullptr;u8(*read_linear8)(u32)=nullptr;u16(*read_linear16)(u32)=nullptr;u32(*read_linear32)(u32)=nullptr;void(*write_linear32)(u32,u32)=nullptr;u8(*read_selector8)(u16,u32)=nullptr;u16(*read_selector16)(u16,u32)=nullptr;};
[[nodiscard]]FarReturn function_f2d2(FormatterBoundary&b,u32 scratch,u32 cursor_cell,u32 state)noexcept{return b.call_55c4?b.call_55c4(scratch,cursor_cell,state):FarReturn{};}
static u32 formatter_get32(FormatterBoundary&b,u32 a)noexcept{return b.read_linear32?b.read_linear32(a):0;}
static void formatter_put32(FormatterBoundary&b,u32 a,u32 v)noexcept{if(b.write_linear32)b.write_linear32(a,v);}
static u32 formatter_take32(FormatterBoundary&b,u32 c)noexcept{const u32 n=formatter_get32(b,c)+4u;formatter_put32(b,c,n);return formatter_get32(b,n-4u);}
static s32 formatter_take_s16(FormatterBoundary&b,u32 c)noexcept{return static_cast<s16>(formatter_take32(b,c));}
static u16 formatter_take_u16(FormatterBoundary&b,u32 c)noexcept{return static_cast<u16>(formatter_take32(b,c));}
static u32 formatter_bounded8(FormatterBoundary&b,u32 p,u16 s,u32 l)noexcept{FarReadBoundary r{b.read_selector8,b.read_selector16};return function_f163(r,p,s,l);}
static u32 formatter_bounded16(FormatterBoundary&b,u32 p,u16 s,u32 l)noexcept{FarReadBoundary r{b.read_selector8,b.read_selector16};return function_f18c(r,p,s,l);}
static u32 formatter_strlen(const char*p)noexcept{return static_cast<u32>(std::strlen(p));}
static void formatter_hexpad(u32 v,char*out,u32 w)noexcept{StringBoundary sb{formatter_strlen};function_f1b3(sb,v,out,w);}
struct MaterializeResult{u32 source_offset=0;u16 source_selector=0;u32 accounting_eax=0;};

// F2E9..F721: conversion switch and common tail. The original jumps to the
// caller's F001 epilogue; the returned structure is the EAX/DX-equivalent
// payload state immediately before that shared epilogue.
[[nodiscard]]MaterializeResult function_f2e9(FormatterBoundary&b,char*scratch,u32 scratch_guest,u32 cursor_cell,FormatState&s,u32 state_guest,u16 ds,u32 null_string_offset)noexcept{MaterializeResult r{scratch_guest,ds,0};u32 numeric=0;bool sign_processing=true;const u8 conv=s.conversion;s.zero_count=0;if(conv=='X'||conv=='o'||conv=='u'||conv=='x'){if(s.flags&0x20u)numeric=formatter_take32(b,cursor_cell);else if(s.flags&0x10u)numeric=formatter_take_u16(b,cursor_cell);else numeric=formatter_take32(b,cursor_cell);s.flags=static_cast<u8>(s.flags&0xf9u);}if(s.precision!=0xffffffffu&&(conv=='X'||conv=='d'||conv=='i'||conv=='o'||conv=='u'||conv=='x'))s.pad=' ';switch(conv){case 'E':case 'F':case 'G':case 'e':case 'f':case 'g':if((conv=='F'||conv=='f')&&(s.flags&0x10u)){function_f206(scratch,static_cast<s32>(formatter_take32(b,cursor_cell)),s);r.accounting_eax=formatter_strlen(scratch);}else{const FarReturn q=function_f2d2(b,scratch_guest,cursor_cell,state_guest);r.source_offset=q.offset;r.source_selector=q.selector;r.accounting_eax=formatter_bounded8(b,q.offset,q.selector,0xffffffffu);}break;case 'o':{char*digits=scratch;if(s.flags&1u)*digits++='0';(void)function_e098(static_cast<s32>(numeric),digits,8);if(s.precision==0&&digits[0]=='0')scratch[0]=0;r.accounting_eax=formatter_strlen(scratch);break;}case 'd':case 'i':{s32 value;if(s.flags&0x20u)value=static_cast<s32>(formatter_take32(b,cursor_cell));else if(s.flags&0x10u)value=formatter_take_s16(b,cursor_cell);else value=static_cast<s32>(formatter_take32(b,cursor_cell));if(s.flags&0x30u)(void)function_e098(value,scratch,10);else(void)function_dffb(value,scratch,10);if(s.precision==0&&scratch[0]=='0')scratch[0]=0;r.accounting_eax=formatter_strlen(scratch);break;}case 'u':(void)function_e050(numeric,scratch,10);if(s.precision==0&&scratch[0]=='0')scratch[0]=0;r.accounting_eax=formatter_strlen(scratch);break;case 'x':case 'X':if((s.flags&1u)&&numeric!=0){s.prefix[0]='0';s.prefix[1]=conv;s.prefix[2]=0;}(void)function_e050(numeric,scratch,16);if(conv=='X')function_f722(scratch);if(s.precision==0&&scratch[0]=='0')scratch[0]=0;r.accounting_eax=formatter_strlen(scratch);break;case 's':case 'S':{u32 p;u16 sel=ds;if(s.flags&0x80u){const u32 n=formatter_get32(b,cursor_cell)+8u;formatter_put32(b,cursor_cell,n);p=formatter_get32(b,n-8u);sel=b.read_linear16?b.read_linear16(n-4u):0;if(p==0&&sel==0){p=null_string_offset;sel=ds;}}else{p=formatter_take32(b,cursor_cell);if(p==0)p=null_string_offset;}r.source_offset=p;r.source_selector=sel;s.flags=static_cast<u8>(s.flags&0xf9u);sign_processing=false;if(conv=='S'){if(s.flags&0x20u){r.accounting_eax=b.read_selector16?b.read_selector16(sel,p):0;r.source_offset=p+2u;}else{r.accounting_eax=b.read_selector8?b.read_selector8(sel,p):0;r.source_offset=p+1u;}}else r.accounting_eax=(s.flags&0x20u)?formatter_bounded16(b,p,sel,s.precision):formatter_bounded8(b,p,sel,s.precision);break;}case 'p':case 'P':{if(s.width==0)s.width=(s.flags&0x80u)?13u:8u;s.flags=static_cast<u8>(s.flags&0xf9u);const u32 off=formatter_take32(b,cursor_cell);if(s.flags&0x80u){const u16 sel=formatter_take_u16(b,cursor_cell);formatter_hexpad(sel,scratch,4);scratch[4]=':';formatter_hexpad(off,scratch+5,8);}else formatter_hexpad(off,scratch,8);if(conv=='P')function_f722(scratch);r.accounting_eax=formatter_strlen(scratch);break;}case 'c':scratch[0]=static_cast<char>(formatter_take32(b,cursor_cell));scratch[1]=0;sign_processing=false;s.precision=1;s.flags=static_cast<u8>(s.flags&0xf9u);r.accounting_eax=1;break;default:s.width=0;scratch[0]=static_cast<char>(conv);scratch[1]=0;sign_processing=false;s.precision=1;s.flags=static_cast<u8>(s.flags&0xf9u);r.accounting_eax=1;break;}if(sign_processing){const bool local_source=(r.source_selector==ds&&r.source_offset==scratch_guest);const u8 first=local_source?static_cast<u8>(scratch[0]):(b.read_selector8?b.read_selector8(r.source_selector,r.source_offset):0);if(first=='-'){--r.accounting_eax;s.prefix[0]='-';s.prefix[1]=0;++r.source_offset;}else if(s.flags&2u){s.prefix[0]=' ';s.prefix[1]=0;}else if(s.flags&4u){s.prefix[0]='+';s.prefix[1]=0;}if(static_cast<s32>(r.accounting_eax)>static_cast<s32>(s.precision))s.precision=r.accounting_eax;else s.zero_count=s.precision-r.accounting_eax;}if(s.pad=='*'){s.prefix[0]=0;s.flags=static_cast<u8>(s.flags&0xf9u);}if((s.precision==0xffffffffu||static_cast<s32>(r.accounting_eax)<static_cast<s32>(s.precision))&&conv!='c')s.precision=r.accounting_eax;const u32 occupied=formatter_strlen(reinterpret_cast<const char*>(s.prefix))+s.precision+s.zero_count;s.width-=occupied;return r;}

// EDE7..F006: verified formatter driver. F2E9 shares the original F001
// epilogue; the C++ boundary represents that encoded edge as a normal return.
struct FormatterDriverBoundary{
 const u8*(*parse_f007)(const u8*,u32 cursor_cell,FormatState&)=nullptr;
 MaterializeResult(*materialize_f2e9)(u32 scratch_guest,u32 cursor_cell,FormatState&,u32 state_guest)=nullptr;
 void(*emit)(FormatState&,u8)=nullptr;
 u32(*read_linear32)(u32)=nullptr;u16(*read_linear16)(u32)=nullptr;
 void(*write_linear32)(u32,u32)=nullptr;void(*write_linear16)(u32,u16)=nullptr;
 u8(*read_selector8)(u16,u32)=nullptr;
 void(*write_selector32)(u16,u32,u32)=nullptr;void(*write_selector16)(u16,u32,u16)=nullptr;
};
static u32 driver_rd32(FormatterDriverBoundary&b,u32 a)noexcept{return b.read_linear32?b.read_linear32(a):0;}
static u16 driver_rd16(FormatterDriverBoundary&b,u32 a)noexcept{return b.read_linear16?b.read_linear16(a):0;}
static void driver_wr32(FormatterDriverBoundary&b,u32 a,u32 v)noexcept{if(b.write_linear32)b.write_linear32(a,v);}
static u32 driver_take32(FormatterDriverBoundary&b,u32 c)noexcept{const u32 n=driver_rd32(b,c)+4u;driver_wr32(b,c,n);return driver_rd32(b,n-4u);}
static u32 driver_count(const FormatState&s)noexcept{return load_le32(s.opaque10);}
static void driver_set_count(FormatState&s,u32 v)noexcept{store_le32(s.opaque10,v);}
static void driver_output(FormatterDriverBoundary&b,FormatState&s,u8 ch)noexcept{if(b.emit)b.emit(s,ch);}
[[nodiscard]]u32 function_ede7(FormatterDriverBoundary&b,u32 initial_context,const u8*format,u32 cursor_cell,u32 scratch_guest,u32 state_guest)noexcept{
 FormatState s{};store_le32(s.opaque0,initial_context);driver_set_count(s,0);const u8*p=format;
 for(;;){
  if(*p==0)return driver_count(s);
  if(*p!='%'){driver_output(b,s,*p++);continue;}
  // EE72/EE75 increment EAX before EE79 reaches EE10: F007 receives p+1.
  if(!b.parse_f007)return driver_count(s);const u8*conversion=b.parse_f007(p+1,cursor_cell,s);s.conversion=*conversion;p=conversion+1;if(s.conversion==0)return driver_count(s);
  if(s.conversion=='n'){
   const u32 count=driver_count(s);
   if(s.flags&0x20u){if(s.flags&0x80u){const u32 base=driver_rd32(b,cursor_cell)+8u;driver_wr32(b,cursor_cell,base);const u32 off=driver_rd32(b,base-8u);const u16 sel=driver_rd16(b,base-4u);if(b.write_selector32)b.write_selector32(sel,off,count);}else{const u32 dst=driver_take32(b,cursor_cell);driver_wr32(b,dst,count);}}
   else if(s.flags&0x10u){if(s.flags&0x80u){const u32 base=driver_rd32(b,cursor_cell)+8u;driver_wr32(b,cursor_cell,base);const u32 off=driver_rd32(b,base-8u);const u16 sel=driver_rd16(b,base-4u);if(b.write_selector16)b.write_selector16(sel,off,static_cast<u16>(count));}else{const u32 dst=driver_take32(b,cursor_cell);if(b.write_linear16)b.write_linear16(dst,static_cast<u16>(count));}}
   else{if(s.flags&0x80u){const u32 base=driver_rd32(b,cursor_cell)+8u;driver_wr32(b,cursor_cell,base);const u32 off=driver_rd32(b,base-8u);const u16 sel=driver_rd16(b,base-4u);if(b.write_selector32)b.write_selector32(sel,off,count);}else{const u32 dst=driver_take32(b,cursor_cell);driver_wr32(b,dst,count);}}continue;
  }
  if(!b.materialize_f2e9)return driver_count(s);MaterializeResult r=b.materialize_f2e9(scratch_guest,cursor_cell,s,state_guest);u32 source=r.source_offset;const u16 selector=r.source_selector;
  if((s.flags&0x08u)==0&&s.pad==' '){while(static_cast<s32>(--s.width)>=0)driver_output(b,s,s.pad);}
  for(u32 i=0;i<3u&&s.prefix[i]!=0;++i)driver_output(b,s,s.prefix[i]);
  for(;;){const u32 old=s.zero_count--;if(old==0)break;driver_output(b,s,'0');}
  if((s.flags&0x08u)==0&&s.pad!=' '){while(static_cast<s32>(--s.width)>=0)driver_output(b,s,s.pad);}
  for(;;){const u32 old=s.precision--;if(old==0)break;const u8 ch=b.read_selector8?b.read_selector8(selector,source):0;driver_output(b,s,ch);source+=((s.conversion=='s'||s.conversion=='S')&&(s.flags&0x20u))?2u:1u;}
  if(s.flags&0x08u){while(static_cast<s32>(s.width)>0){const u32 old=s.width--;if(old==0)break;driver_output(b,s,' ');}}
 }
}

// DA1C..DA44: 41-byte wrapper. 104B1 initializes the 12-byte local adapter
// cell; CEC8 receives the original three dword arguments followed by that cell.
// The boundary stays address-named because CEC8/FEFC carries selector/register
// state that is not yet representable as an ordinary host call.
struct DosAdapterBoundary{void(*call_104b1)(u32 guest_cell)=nullptr;u32(*call_cec8)(u32,u32,u32,u32)=nullptr;};
[[nodiscard]]u32 function_da1c(DosAdapterBoundary&b,u32 a1,u32 a2,u32 a3,u32 local_guest)noexcept{if(b.call_104b1)b.call_104b1(local_guest);return b.call_cec8?b.call_cec8(a1,a2,a3,local_guest):0;}

// DAD9..DB7F. The encoded routine calls the allocator core at 104E0 with
// EAX=request, EDX=DS, EBX=30B8h, then tries 10AA9 once and 10C46 as the
// retry gate. DB5B passes the pointer to the free core at 10590. Names stay
// address-based until the surrounding runtime contracts are integrated.
struct AllocBoundary{
 u32(*call_104e0)(u32 request,u16 ds,u32 arena)=nullptr;
 u32(*call_10aa9)(u32 request)=nullptr;
 u32(*call_10c46)(u32 request)=nullptr;
 void(*call_10590)(u32 pointer,u16 ds,u32 arena)=nullptr;
 void(*write_linear8)(u32,u8)=nullptr;
 u16 ds=0;
};
[[nodiscard]]u32 function_dad9(AllocBoundary&b,u32 request)noexcept{
 if(request==0)return 0;
 bool retried_10aa9=false;
 for(;;){
  const u32 result=b.call_104e0?b.call_104e0(request,b.ds,0x30b8u):0;
  if(result!=0){if(b.write_linear8)b.write_linear8(0x8e78u,0);return result;}
  if(!retried_10aa9){if(b.call_10aa9&&b.call_10aa9(request)!=0){retried_10aa9=true;continue;}}
  if(!b.call_10c46||b.call_10c46(request)==0){if(b.write_linear8)b.write_linear8(0x8e78u,0);return 0;}
 }
}
void function_db5b(AllocBoundary&b,u32 pointer)noexcept{if(b.call_10590)b.call_10590(pointer,b.ds,0x30b8u);if(b.write_linear8)b.write_linear8(0x8e78u,0);}
void function_db4b(AllocBoundary&b,u32 pointer)noexcept{function_db5b(b,pointer);}
// DAC9..DAD8: exact 16-byte frame wrapper around DAD9.
[[nodiscard]]u32 function_dac9(AllocBoundary&b,u32 request)noexcept{return function_dad9(b,request);}

// DB80..DC8C. Exact 4096-byte ring dictionary walk. 7360 returns a
// zero-extended input byte; 738C consumes one output byte. The dictionary is
// initialized to FEh, write index FEEh. SI is shifted before testing the 0100h
// refill marker. Back-references combine two bytes as
// ((second & F0h)<<4)|first and copy (second&0Fh)+3 bytes.
struct DecodeBoundary{u8(*input_7360)()=nullptr;void(*output_738c)(u8)=nullptr;u8(*read_linear8)(u32)=nullptr;};
void function_db80(DecodeBoundary&b)noexcept{
 u8 dict[4096];std::memset(dict,0xfe,sizeof(dict));u16 write_index=0x0feeu;u16 si=0;
 for(;;){
  si=static_cast<u16>(si>>1);
  if((si&0x0100u)==0){if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 v=b.input_7360?b.input_7360():0;si=static_cast<u16>(0xff00u|v);}
  if(si&1u){
   if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 v=b.input_7360?b.input_7360():0;if(b.output_738c)b.output_738c(v);dict[write_index]=v;write_index=static_cast<u16>((write_index+1u)&0x0fffu);
  }else{
   if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 first=b.input_7360?b.input_7360():0;
   if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 second=b.input_7360?b.input_7360():0;
   const u16 source=static_cast<u16>((static_cast<u16>(second&0xf0u)<<4)|first);const u16 last=static_cast<u16>((second&0x0fu)+2u);
   for(u16 i=0;i<=last;++i){const u8 v=dict[(source+i)&0x0fffu];if(b.output_738c)b.output_738c(v);dict[write_index]=v;write_index=static_cast<u16>((write_index+1u)&0x0fffu);}
  }
 }
}

// DC90..DDD7. The routine operates only when header word +4 == 1. It builds
// a word permutation table in workspace 30E8h, then uses REPNE SCASW to find
// the current index in the remaining suffix. SCASW compares the initial EDI
// element before advancing EDI; after the instruction the encoded SUB EDI,2
// restores EDI to the matched element (or final examined element on failure).
struct TransformBoundary{u16(*read16)(u32)=nullptr;u8(*read8)(u32)=nullptr;void(*write16)(u32,u16)=nullptr;void(*write8)(u32,u8)=nullptr;};
static u16 tr16(TransformBoundary&b,u32 a)noexcept{return b.read16?b.read16(a):0;}static u8 tr8(TransformBoundary&b,u32 a)noexcept{return b.read8?b.read8(a):0;}static void tw16(TransformBoundary&b,u32 a,u16 v)noexcept{if(b.write16)b.write16(a,v);}static void tw8(TransformBoundary&b,u32 a,u8 v)noexcept{if(b.write8)b.write8(a,v);}
void function_dc90(TransformBoundary&b,u32 base)noexcept{
 if(tr16(b,base+4)!=1)return;u32 dst=base+8u;u32 source_base=base+8u;if(tr8(b,base+7u)&0x80u){++dst;++source_base;}
 const u32 row=tr16(b,base);const u32 stride=static_cast<u32>(tr8(b,base+6u))<<1;u32 remaining=4u*tr16(b,base+2u);const u32 table_words=remaining;const u32 wrap=remaining<<1;
 u32 bx=0,eax=0;for(u32 ecx=table_words;ecx!=0;--ecx){tw16(b,0x30e8u+bx,static_cast<u16>(eax));bx+=8u;++eax;if(bx>=wrap){bx-=wrap;bx+=2u;}}
 --remaining;dst+=stride;u32 index=1;
 while(remaining!=0){
  const u32 table_entry=0x30e8u+(index<<1);const u16 saved_table_value=tr16(b,table_entry);u32 scan=remaining;u32 after=table_entry;
  if(scan!=0){do{const u16 examined=tr16(b,after);after+=2u;--scan;if(examined==static_cast<u16>(index))break;}while(scan!=0);}
  const u32 match=after-2u;const u16 displaced=tr16(b,match);tw16(b,match,saved_table_value);const u32 mapped=(match-0x30e8u)>>1;tw16(b,table_entry,displaced);
  u32 a=source_base+(index>>2)*row+(index&3u)*stride;u32 s=source_base+(mapped>>2)*row+(mapped&3u)*stride;const u32 saved=a;
  for(u32 n=stride;n!=0;--n){const u8 av=tr8(b,a),sv=tr8(b,s);tw8(b,a,sv);tw8(b,s,av);++a;++s;}
  for(u32 n=0;n<stride;++n)tw8(b,dst+n,tr8(b,saved+n));++index;dst+=stride;--remaining;
 }
 const u16 old_count=tr16(b,base+2u);tw16(b,base+4u,old_count);tw16(b,base+2u,1);const u16 product=static_cast<u16>(static_cast<u16>(old_count)*static_cast<u16>(static_cast<u16>(tr8(b,base+6u))<<3));tw16(b,base,product);
}

static_assert(0xDB80u-0xDAD9u==167u); static_assert(0xDC8Du-0xDB80u==269u); static_assert(0xDDD8u-0xDC90u==328u);
static_assert(0xE4C7u-0xE3B4u==275u); static_assert(0xF007u-0xEDE7u==544u); static_assert(0xEBF3u-0xEA0Bu==488u); static_assert(0xEDD3u-0xEBF3u==480u); static_assert(0xEDE7u-0xEDE0u==7u);
static_assert(0xF110u-0xF007u==265u); static_assert(0xF163u-0xF110u==83u); static_assert(0xF18Cu-0xF163u==41u); static_assert(0xF1B3u-0xF18Cu==39u); static_assert(0xF206u-0xF1B3u==83u); static_assert(0xF2D2u-0xF206u==204u); static_assert(0xF2E9u-0xF2D2u==23u); static_assert(0xF722u-0xF2E9u==1081u); static_assert(0xF742u-0xF722u==32u);
// E0D0..E173: allocate and clone the encoded variable-size object. Source object pointer is the unaligned dword at arg1+5.
// The 28-byte fixed prefix is copied exactly (eleven words, one dword, two bytes); payload length is source[16h]-1 and 6304 receives (dst+1Ch,count,arg2).
struct RuntimeCloneBoundary{u32(*alloc_71c8)(u32)=nullptr;u16(*read16)(u32)=nullptr;u32(*read32)(u32)=nullptr;u8(*read8)(u32)=nullptr;void(*write16)(u32,u16)=nullptr;void(*write32)(u32,u32)=nullptr;void(*write8)(u32,u8)=nullptr;void(*call_6304)(u32,u32,u32)=nullptr;};
[[nodiscard]]u32 function_e0d0(RuntimeCloneBoundary&b,u32 arg1,u32 payload)noexcept{const u32 src=b.read32?b.read32(arg1+5u):0u;const u32 n=b.read32?b.read32(src+0x16u):0u;const u32 dst=b.alloc_71c8?b.alloc_71c8(0x1bu+n):0u;if(b.read16&&b.write16)for(u32 o=0;o<=0x14u;o+=2u)b.write16(dst+o,b.read16(src+o));if(b.read32&&b.write32)b.write32(dst+0x16u,b.read32(src+0x16u));if(b.read8&&b.write8){b.write8(dst+0x1au,b.read8(src+0x1au));b.write8(dst+0x1bu,b.read8(src+0x1bu));}if(b.call_6304)b.call_6304(dst+0x1cu,n-1u,payload);return dst;}
static_assert(0xE174u-0xE0D0u==164u);

// E174..E186, E1F4..E206, E26C..E27E: three address-distinct wrappers with identical encoded bodies.
// Each establishes EBP, forwards its sole dword argument to 7240, discards the pushed argument, and returns EAX unchanged.
struct Runtime7240Boundary{u32(*call_7240)(u32)=nullptr;};
[[nodiscard]]u32 function_e174(Runtime7240Boundary&b,u32 a)noexcept{return b.call_7240?b.call_7240(a):0u;}
[[nodiscard]]u32 function_e1f4(Runtime7240Boundary&b,u32 a)noexcept{return b.call_7240?b.call_7240(a):0u;}
[[nodiscard]]u32 function_e26c(Runtime7240Boundary&b,u32 a)noexcept{return b.call_7240?b.call_7240(a):0u;}
static_assert(0xE187u-0xE174u==19u); static_assert(0xE207u-0xE1F4u==19u); static_assert(0xE27Fu-0xE26Cu==19u);

// E190..E1F0 and E208..E268: address-distinct constructor-like readers. The first stack dword is encoded but unused.
// 6304 is the read wrapper: these bodies request exactly two bytes from source into [EBP-8], zero-extend that word, allocate len+2, store len, set 2904 through 72D0, run 72E0, then call 746C(0,0,dst+2,len,0).
struct RuntimeConstructBoundary{u16(*read_word_6304)(u32)=nullptr;u32(*alloc_71c8)(u32)=nullptr;void(*write16)(u32,u16)=nullptr;void(*call_72d0)(u32)=nullptr;void(*call_72e0)()=nullptr;void(*call_746c)(u32,u32,u32,u32,u32)=nullptr;};
[[nodiscard]]u32 runtime_construct_e190_body(RuntimeConstructBoundary&b,u32 source)noexcept{const u16 len=b.read_word_6304?b.read_word_6304(source):0u;const u32 dst=b.alloc_71c8?b.alloc_71c8(static_cast<u32>(len)+2u):0u;if(b.write16)b.write16(dst,len);if(b.call_72d0)b.call_72d0(source);if(b.call_72e0)b.call_72e0();if(b.call_746c)b.call_746c(0u,0u,dst+2u,static_cast<u32>(len),0u);return dst;}
[[nodiscard]]u32 function_e190(RuntimeConstructBoundary&b,u32 /*unused_arg1*/,u32 source)noexcept{return runtime_construct_e190_body(b,source);}
[[nodiscard]]u32 function_e208(RuntimeConstructBoundary&b,u32 /*unused_arg1*/,u32 source)noexcept{return runtime_construct_e190_body(b,source);}
static_assert(0xE1F1u-0xE190u==97u); static_assert(0xE269u-0xE208u==97u);

// E280..E2C2: install the E2C3 INT 24h trampoline while preserving the caller-supplied far callback at 5102:5106.
// Runtime modes 2..4 use the extender-specific INT 21h AX=2506h, CL=24h form; all other modes use AH=25h, AL=24h. The encoded routine returns EAX=0.
struct Int24InstallBoundary{void(*store_callback)(u32,u16)=nullptr;u8(*runtime_mode)()=nullptr;void(*install_extender_vector)(u8,u32)=nullptr;void(*install_dos_vector)(u8,u32)=nullptr;};
[[nodiscard]]u32 function_e280(Int24InstallBoundary&b,u32 callback_offset,u32 callback_selector)noexcept{if(b.store_callback)b.store_callback(callback_offset,static_cast<u16>(callback_selector));const u8 mode=b.runtime_mode?b.runtime_mode():0u;if(mode>=2u&&mode<=4u){if(b.install_extender_vector)b.install_extender_vector(0x24u,0xE2C3u);}else if(b.install_dos_vector)b.install_dos_vector(0x24u,0xE2C3u);return 0u;}
static_assert(0xE2C3u-0xE280u==67u);

// E360..E3B2: signed decimal parser. Character classes are indexed by unsigned byte+1 at DGROUP+03B4; bit 02h skips leading class, bit 20h accepts digits.
[[nodiscard]]u32 function_e360(const u8*data,u32 text)noexcept{u32 p=text;auto cls=[&](u8 c)noexcept->u8{return data[0x3b4u+static_cast<u32>(static_cast<u8>(c+1u))];};while((cls(data[p])&0x02u)!=0)++p;const u8 sign=data[p];if(sign==0x2bu||sign==0x2du)++p;u32 v=0;while((cls(data[p])&0x20u)!=0){v=v*10u+static_cast<u32>(data[p]);++p;v-=0x30u;}if(sign==0x2du)v=0u-v;return v;}
static_assert(0xE3B3u-0xE360u==83u);




// Batch 14: early text layout/presentation and BE70..C79E palette/resource continuation.
// CODE.1 remains authoritative; alignment/data islands and the self-modifying C79F..CBD0 VGA blitters are deliberately excluded.
struct EarlyText14Boundary{
 u8(*read8)(u32)=nullptr;void(*write8)(u32,u8)=nullptr;u32(*strlen_6ee9)(u32)=nullptr;u32(*alloc_71c8)(u32)=nullptr;
 void(*copy_72a0)(u32,u32)=nullptr;void(*free_7240)(u32)=nullptr;
 void(*call_6eb0)(s32,s32,s32,u32,u32,u32)=nullptr;void(*call_6c60)(s32,s32,s32,u32,u32)=nullptr;
 void(*call_7d80)(s32,s32,u32,u32,u32)=nullptr;
};
static u8 e14r8(EarlyText14Boundary&b,u32 a)noexcept{return b.read8?b.read8(a):0u;}static void e14w8(EarlyText14Boundary&b,u32 a,u8 v)noexcept{if(b.write8)b.write8(a,v);}
// 00EC..035B: six layout modes for the same label/text primitive. Mode 0 and modes 4/5 split a copied string in-place on spaces.
void function_00ec(EarlyText14Boundary&b,s32 x,s32 y,s32 width,u32 style,u32 text,u32 mode,u32 extra)noexcept{
 if(mode==0u){if(b.call_6eb0)b.call_6eb0(0,x,y,0x1au,7u,extra);const u32 len=b.strlen_6ee9?b.strlen_6ee9(text):0u;const u32 tmp=b.alloc_71c8?b.alloc_71c8(len+1u):0u;if(b.copy_72a0)b.copy_72a0(tmp,text);u32 scan=tmp,words=0;while(e14r8(b,scan)!=0u){while(e14r8(b,scan)!=0x20u&&e14r8(b,scan)!=0u)++scan;if(e14r8(b,scan)==0x20u){e14w8(b,scan,0u);++scan;}++words;}s32 px=x+width/2-static_cast<s32>(words*5u);u32 p=tmp;if(words!=0u){for(u32 n=0;n<words;++n){if(b.call_7d80)b.call_7d80(px,y,style,p,0u);while(e14r8(b,p)!=0u)++p;++p;px+=10;}}if(b.free_7240)b.free_7240(tmp);return;}
 if(mode==1u){if(b.call_6c60)b.call_6c60(0,x,y,0x1au,7u);if(b.call_7d80)b.call_7d80(x+8,y,style,text,0u);return;}
 if(mode==2u){if(b.call_6c60)b.call_6c60(0,x,y,0x1au,7u);if(b.call_7d80)b.call_7d80(x+width-0x11,y,style,text,0u);return;}
 if(mode==3u){if(b.call_6c60)b.call_6c60(0,x,y,0x17u,7u);if(b.call_7d80)b.call_7d80(x+width/2-5,y,style,text,0u);return;}
 if(mode==4u||mode==5u){if(mode!=5u&&b.call_6c60)b.call_6c60(0,x,y,0x1au,7u);const u32 len=b.strlen_6ee9?b.strlen_6ee9(text):0u;const u32 tmp=b.alloc_71c8?b.alloc_71c8(len+1u):0u;if(b.copy_72a0)b.copy_72a0(tmp,text);u32 scan=tmp;while(e14r8(b,scan)!=0u){while(e14r8(b,scan)!=0x20u&&e14r8(b,scan)!=0u)++scan;if(e14r8(b,scan)==0x20u){e14w8(b,scan,0u);++scan;}}if(b.call_7d80)b.call_7d80(x+8,y,style,tmp,0u);u32 second=tmp;while(e14r8(b,second)!=0u)++second;++second;if(b.call_7d80)b.call_7d80(x+width-0x10,y,style,second,0u);if(b.free_7240)b.free_7240(tmp);}
}
static_assert(0x035Cu-0x00ECu==624u);

struct Presentation14Boundary{
 u8(*read8)(u32)=nullptr;u16(*read16)(u32)=nullptr;u32(*alloc_71c8)(u32)=nullptr;void(*free_7240)(u32)=nullptr;
 void(*call_6588)(u32)=nullptr;void(*call_86c4)(s32,s32,u32,u32)=nullptr;void(*call_6c60)(s32,s32,s32,u32,u32)=nullptr;
 void(*call_7d80)(s32,s32,u32,u32,u32)=nullptr;void(*call_04b0)(u32)=nullptr;void(*call_53e4)(u32)=nullptr;
 u32(*call_954c)(u32,u32)=nullptr;void(*call_6744)(u32,s32,s32,u32,u32)=nullptr;void(*call_66c0)(u32,s32,s32,u32,u32)=nullptr;
 void(*call_6d54)(u32,s32,s32,u32)=nullptr;void(*call_a5c8)(u32,u32)=nullptr;
};
static u8 p14r8(Presentation14Boundary&b,u32 a)noexcept{return b.read8?b.read8(a):0u;}static u16 p14r16(Presentation14Boundary&b,u32 a)noexcept{return b.read16?b.read16(a):0u;}
// 10A4..1461: double-buffered twelve-step presentation used by 1464. The two page-local rectangles are preserved explicitly.
void function_10a4(Machine&m,Presentation14Boundary&b,u32 arg)noexcept{
 const u32 buffer[2]={b.alloc_71c8?b.alloc_71c8(0x3540u):0u,b.alloc_71c8?b.alloc_71c8(0x3540u):0u};
 for(u32 pass=0;pass<2u;++pass){const u32 page=*dgroup_ptr(m,dgroup(0x7138u));if(b.call_6588)b.call_6588(page);if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&b.call_86c4)b.call_86c4(0,0,0x18u,7u);if(b.call_86c4)b.call_86c4(0xc8,0,0x19u,7u);if(b.call_6c60)b.call_6c60(0,5,0,0x2au,7u);if(b.call_7d80)b.call_7d80(5,0,0x140u,0x7140u,1u);for(u32 i=0;i<9u;++i)if(b.call_6c60)b.call_6c60(0,static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+i*8u)))),static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+i*8u)))),i+0x1eu,7u);if(arg!=0u&&b.call_04b0)b.call_04b0(arg);if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;}
 for(u32 id:{0x15u,0x27u,0x28u,0x29u})if(b.call_954c)(void)b.call_954c(id,7u);
 for(u32 slot=0;slot<9u;++slot){if(load_le32(dgroup_ptr(m,dgroup(0x08f8u+slot*8u)))==0xffffffffu)continue;const s32 target_y=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+slot*8u))))+0x18;const s32 target_x=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+slot*8u))))+0x3d;const s32 base_y=0xa0,base_x=-0x38;u32 seq=0;struct Rect{s32 x=0,y=0;u32 w=0,h=0;} rect[2];
  for(s32 step=0;step<12;++step){const u32 page=*dgroup_ptr(m,dgroup(0x7138u));if(b.call_6588)b.call_6588(page);if(step>=2&&b.call_6744)b.call_6744(buffer[page],rect[page].x,rect[page].y,rect[page].w,rect[page].h);const u32 image=b.call_954c?b.call_954c(p14r8(b,0x0964u+seq),7u):0u;if(p14r8(b,0x0970u+seq)!=0u&&b.call_a5c8)b.call_a5c8(0u,image);const u32 w=p14r16(b,image+4u),h=static_cast<u32>(p14r8(b,image+6u))*8u;const s32 anchor_y=base_y+((target_y-base_y)*(step+1)+6)/12;const s32 anchor_x=base_x+((target_x-base_x)*(step+1)+6)/12;rect[page]={anchor_x-static_cast<s32>(w)/2,anchor_y-static_cast<s32>(h*2u),w,h};if(b.call_66c0)b.call_66c0(buffer[page],rect[page].x,rect[page].y,w,h);if(b.call_6d54)b.call_6d54(0u,rect[page].x,rect[page].y,image);if(p14r8(b,0x0970u+seq)!=0u&&b.call_a5c8)b.call_a5c8(0u,image);seq=(seq+1u)%12u;if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;}
  for(u32 finish=0;finish<2u;++finish){const u32 page=*dgroup_ptr(m,dgroup(0x7138u));if(b.call_6588)b.call_6588(page);if(b.call_6744)b.call_6744(buffer[page],rect[page].x,rect[page].y,rect[page].w,rect[page].h);if(b.call_6c60)b.call_6c60(0,static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+slot*8u)))),static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+slot*8u)))),0x15u,7u);if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;}
 }
 if(b.free_7240){b.free_7240(buffer[0]);b.free_7240(buffer[1]);}
}
static_assert(0x1462u-0x10A4u==958u);
// 10A2..10A3 is MOV EAX,EAX alignment.

struct LowContinuation14Boundary{
 u8(*read8)(u32)=nullptr;u16(*read16)(u32)=nullptr;u32(*read32)(u32)=nullptr;void(*write8)(u32,u8)=nullptr;void(*write32)(u32,u32)=nullptr;
 void(*divide_error)()=nullptr;void(*copy_d9ee)(u32,u32,u32)=nullptr;void(*call_6618)(u32)=nullptr;void(*call_9ab4)()=nullptr;void(*call_9ac4)()=nullptr;
 u32(*call_62a8)(u32)=nullptr;u32(*call_627c)(u32)=nullptr;void(*call_62f0)(u32)=nullptr;void(*call_6304)(u32,u32,u32)=nullptr;u32(*call_6304_local32)(u32)=nullptr;void(*call_633c)(u32,u32,u32)=nullptr;void(*call_9814)(u32)=nullptr;
 u32(*call_6f10)(u32,u32,u32)=nullptr;void(*call_6f74)(u32)=nullptr;u32(*call_71c8)(u32)=nullptr;void(*call_7240)(u32)=nullptr;u32(*call_f8c4)(u32,u32,u32)=nullptr;
 void(*call_e680)()=nullptr;void(*call_e6ea)(u32)=nullptr;u32(*call_ea0b)(u32)=nullptr;u32(*call_eac8)(u32)=nullptr;u32(*call_eae7)(u32,u32,u32,u32,u32)=nullptr;void(*call_eaf1)(u32,u32,u32,u32,u32)=nullptr;void(*call_ea9a)(u32)=nullptr;
 u32(*call_954c)(u32,u32)=nullptr;void(*call_ec25)(u32,u32,u32,u32)=nullptr;void(*call_ec1b)(u32,u32,u32,u32,u32)=nullptr;void(*call_ec39)(u32)=nullptr;u32(*call_ec2f)(u32)=nullptr;void(*call_ec43)(u32)=nullptr;
};
static u8 lc14r8(LowContinuation14Boundary&b,u32 a)noexcept{return b.read8?b.read8(a):0u;}static u16 lc14r16(LowContinuation14Boundary&b,u32 a)noexcept{return b.read16?b.read16(a):0u;}static u32 lc14r32(LowContinuation14Boundary&b,u32 a)noexcept{return b.read32?b.read32(a):0u;}static void lc14w8(LowContinuation14Boundary&b,u32 a,u8 v)noexcept{if(b.write8)b.write8(a,v);}static void lc14w32(LowContinuation14Boundary&b,u32 a,u32 v)noexcept{if(b.write32)b.write32(a,v);}
void function_c57c(Machine&,LowContinuation14Boundary&)noexcept;void function_c4e0(Machine&,LowContinuation14Boundary&,u32)noexcept;
// BE70..BF34: scale each nonzero palette component toward a brightness-adjusted maximum. A zero divisor follows the binary's IDIV fault path.
void function_be70(Machine&m,LowContinuation14Boundary&b,u32 palette)noexcept{u32 strength=lc14r8(b,load_le32(dgroup_ptr(m,dgroup(0x2aa0u)))+0x6cu);if(strength>0x3fu)strength=0x3fu;if(strength==0u)return;for(u32 i=0;i<0x100u;++i){const u32 p=palette+i*3u;u32 mx=0;for(u32 c=0;c<3u;++c)if(lc14r8(b,p+c)>mx)mx=lc14r8(b,p+c);const s32 scaled=static_cast<s32>(mx)+(static_cast<s32>(0x3f-mx)*static_cast<s32>(strength)+0x1f)/0x3f;if(scaled==0)continue;for(u32 c=0;c<3u;++c){const u8 v=lc14r8(b,p+c);if(v==0u)continue;if(mx==0u){if(b.divide_error)b.divide_error();return;}lc14w8(b,p+c,static_cast<u8>((static_cast<s32>(v)*scaled+static_cast<s32>(mx)/2)/static_cast<s32>(mx)));}}}static_assert(0xBF35u-0xBE70u==197u);
static void function_c07c(LowContinuation14Boundary&b,u32 dst,u32 di,u32 src,u32 si,u32 count)noexcept{if(b.copy_d9ee)b.copy_d9ee(dst+di*3u,src+si*3u,count*3u);}static_assert(0xC0B8u-0xC07Cu==60u);
static void function_c110(LowContinuation14Boundary&b,u32 palette,u32 start,u32 count,s32 factor,u8 shift,const u8 target[3])noexcept{u32 p=palette+start*3u;for(u32 n=0;n<count;++n)for(u32 c=0;c<3u;++c,++p){const s32 base=target[c];const s32 cur=lc14r8(b,p);lc14w8(b,p,static_cast<u8>(base+((cur-base)*factor>>shift)));}}static_assert(0xC17Bu-0xC110u==107u);
// BF38..BFDE and BFE0..C078: eight-step palette fades in opposite directions, restoring the selected source segment after each displayed step.
void function_bf38(LowContinuation14Boundary&b,u32 source,u32 first,u32 count,u8 r,u8 g,u8 bl)noexcept{const u8 rgb[3]={r,g,bl};function_c07c(b,0x2c48u,0u,source,0u,0x100u);for(s32 step=7;step>=0;--step){function_c110(b,0x2c48u,first,count,step,3u,rgb);if(b.call_6618)b.call_6618(0x2c48u);function_c07c(b,0x2c48u,first,source,first,count);}if(b.call_9ab4)b.call_9ab4();}static_assert(0xBFDFu-0xBF38u==167u);
void function_bfe0(LowContinuation14Boundary&b,u32 source,u32 first,u32 count,u8 r,u8 g,u8 bl)noexcept{const u8 rgb[3]={r,g,bl};function_c07c(b,0x2c48u,0u,source,0u,0x100u);for(s32 step=1;step<=8;++step){function_c110(b,0x2c48u,first,count,step,3u,rgb);if(b.call_6618)b.call_6618(0x2c48u);function_c07c(b,0x2c48u,first,source,first,count);}if(b.call_9ac4)b.call_9ac4();}static_assert(0xC079u-0xBFE0u==153u);
// C0B8..C10E: fill count consecutive RGB entries, starting at base+index*3.
void function_c0b8(LowContinuation14Boundary&b,u32 base,u32 index,u8 r,u8 g,u8 bl,u32 count)noexcept{u32 p=base+index*3u;for(u32 i=0;i<count;++i){lc14w8(b,p++,r);lc14w8(b,p++,g);lc14w8(b,p++,bl);}}static_assert(0xC10Fu-0xC0B8u==87u);
void function_c17c(LowContinuation14Boundary&b)noexcept{function_c0b8(b,0x2c48u,0u,0u,0u,0u,0x100u);if(b.call_6618)b.call_6618(0x2c48u);if(b.call_9ab4)b.call_9ab4();}static_assert(0xC1AFu-0xC17Cu==51u);
// C1B0..C226: rotate one RGB triplet to the end of a selected run when run length exceeds one.
void function_c1b0(LowContinuation14Boundary&b,u32 base,u32 first,u32 offset,u32 run)noexcept{if(run<=1u)return;u32 p=base+(first+offset)*3u;const u32 bytes=run*3u-3u;const u8 save[3]={lc14r8(b,p),lc14r8(b,p+1u),lc14r8(b,p+2u)};for(u32 i=0;i<bytes;++i)lc14w8(b,p+i,lc14r8(b,p+3u+i));for(u32 i=0;i<3u;++i)lc14w8(b,p+bytes+i,save[i]);}static_assert(0xC227u-0xC1B0u==119u);
[[nodiscard]]u32 function_c228(LowContinuation14Boundary&b,u32 path)noexcept{const u32 h=b.call_62a8?b.call_62a8(path):0xffffffffu;if(h==0xffffffffu&&b.call_9814)b.call_9814(0x334u);return h;}static_assert(0xC254u-0xC228u==44u);
// C254..C2C2: seek to a 3*index page record, read 3*count palette bytes, postprocess BE70, then close.
void function_c254(Machine&m,LowContinuation14Boundary&b,u32 dst,u32 dst_index,u32 page,u32 path,u32 count)noexcept{const u32 h=function_c228(b,path);if(b.call_633c)b.call_633c((page*3u<<8u)+1u,1u,h);if(b.call_6304)b.call_6304(dst+dst_index*3u,count*3u,h);function_be70(m,b,dst);if(b.call_62f0)b.call_62f0(h);}static_assert(0xC2C3u-0xC254u==111u);
// C2D0..C49C: initialize the second external resource subsystem. Its EAC8 type check is exactly 2.
void function_c2d0(Machine&m,LowContinuation14Boundary&b,u32 path,u32 p2,u32 p3,u32 p4,u32 p5)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))!=0u)return;const u32 area=b.call_6f10?b.call_6f10(0x612b0u,0x2f6cu,0x2f70u):0u;store_le32(dgroup_ptr(m,dgroup(0x2f68u)),area);if(area==0u)return;const u32 h=b.call_62a8?b.call_62a8(path):0xffffffffu;if(h==0xffffffffu){if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2f70u))));return;}const u32 size=b.call_627c?b.call_627c(path):0u;const u32 tmp=b.call_71c8?b.call_71c8(size):0u;if(tmp==0u){if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2f70u))));return;}if(b.call_6304)b.call_6304(tmp,size,h);if(b.call_62f0)b.call_62f0(h);const u32 image=b.call_f8c4?b.call_f8c4(tmp,5u,0u):0u;if(image==0u){if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2f70u))));if(b.call_7240)b.call_7240(tmp);return;}if(b.call_7240)b.call_7240(tmp);if(b.call_e680)b.call_e680();const u32 sys=b.call_ea0b?b.call_ea0b(image):0xffffffffu;store_le32(dgroup_ptr(m,dgroup(0x2f74u)),sys);auto fail=[&](){if(b.call_e6ea)b.call_e6ea(0u);if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2f70u))));};if(sys==0xffffffffu){fail();return;}const u32 info=b.call_eac8?b.call_eac8(sys):0u;if(lc14r32(b,info+4u)!=2u){fail();return;}if((b.call_eae7?b.call_eae7(sys,p2,p3,p4,p5):0u)==0u){fail();return;}if(b.call_eaf1)b.call_eaf1(sys,p2,p3,p4,p5);store_le32(dgroup_ptr(m,dgroup(0x2f48u)),1u);}static_assert(0xC49Du-0xC2D0u==461u);
void function_c4a0(Machine&m,LowContinuation14Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))==0u)return;function_c57c(m,b);if(b.call_ea9a)b.call_ea9a(load_le32(dgroup_ptr(m,dgroup(0x2f74u))));if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2f70u))));store_le32(dgroup_ptr(m,dgroup(0x2f48u)),0u);}static_assert(0xC4E0u-0xC4A0u==64u);
// C4E0..C57A: materialize one record into 2F68 and submit it through the paired selector-aware external calls.
void function_c4e0(Machine&m,LowContinuation14Boundary&b,u32 id)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))==0u||*dgroup_ptr(m,dgroup(0x2f78u))==0u)return;const u32 rec=b.call_954c?b.call_954c(id,load_le32(dgroup_ptr(m,dgroup(0x2f4cu)))):0u;const u32 len=lc14r32(b,rec),src=rec+4u;function_c57c(m,b);if(b.copy_d9ee)b.copy_d9ee(load_le32(dgroup_ptr(m,dgroup(0x2f68u))),src,len);const u32 sys=load_le32(dgroup_ptr(m,dgroup(0x2f74u))),buf=load_le32(dgroup_ptr(m,dgroup(0x2f68u)));if(b.call_ec25)b.call_ec25(sys,buf,static_cast<u32>(m.cpu.ds),0xffffffffu);if(b.call_ec1b)b.call_ec1b(sys,buf,static_cast<u32>(m.cpu.ds),load_le32(dgroup_ptr(m,dgroup(0x2f6cu))),0xffffffffu);if(b.call_ec39)b.call_ec39(sys);}static_assert(0xC57Bu-0xC4E0u==155u);
void function_c57c(Machine&m,LowContinuation14Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))==0u)return;const u32 sys=load_le32(dgroup_ptr(m,dgroup(0x2f74u)));if((b.call_ec2f?b.call_ec2f(sys):0u)==2u&&b.call_ec43)b.call_ec43(sys);}static_assert(0xC5B0u-0xC57Cu==52u);
[[nodiscard]]u32 function_c5b0(Machine&m,LowContinuation14Boundary&b,u32 /*unused*/,u32 source)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))==0u)return 0u;const u32 len=b.call_6304_local32?b.call_6304_local32(source):0u;const u32 out=b.call_71c8?b.call_71c8(len+4u):0u;lc14w32(b,out,len);if(b.call_6304)b.call_6304(out+4u,len,source);return out;}static_assert(0xC608u-0xC5B0u==88u);
void function_c608(Machine&m,LowContinuation14Boundary&b,u32 p)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))!=0u&&p!=0u&&b.call_7240)b.call_7240(p);}static_assert(0xC62Au-0xC608u==34u);
void function_c62c(Machine&m,LowContinuation14Boundary&b,u8 enabled)noexcept{*dgroup_ptr(m,dgroup(0x2f78u))=enabled;if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))!=0u&&enabled==0u)function_c57c(m,b);}static_assert(0xC64Du-0xC62Cu==33u);
void function_c650(Machine&m,u32 v)noexcept{store_le32(dgroup_ptr(m,dgroup(0x2f4cu)),v);}static_assert(0xC65Fu-0xC650u==15u);
[[nodiscard]]u32 function_c660(Machine&m,LowContinuation14Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))==0u)return 0u;return (b.call_ec2f?b.call_ec2f(load_le32(dgroup_ptr(m,dgroup(0x2f74u)))):0u)==2u?1u:0u;}static_assert(0xC68Eu-0xC660u==46u);
// C690..C6ED: update one of the 12-byte queue records only when empty or when the existing length is not smaller.
void function_c690(Machine&m,u32 a,u32 bvalue,u32 slot,u32 length)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))==0u)return;const u32 q=0x2f50u+slot*12u;const u32 old=load_le32(dgroup_ptr(m,dgroup(q)));if(old!=0xffffffffu&&static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(q+8u))))<static_cast<s32>(length))return;store_le32(dgroup_ptr(m,dgroup(q)),a);store_le32(dgroup_ptr(m,dgroup(q+4u)),bvalue);store_le32(dgroup_ptr(m,dgroup(q+8u)),length);}static_assert(0xC6EEu-0xC690u==94u);
void function_c6f0()noexcept{}static_assert(0xC6F7u-0xC6F0u==7u);
// C6F8..C79E: submit pending queue records 0/1, choosing the smaller length when both exist, then clear their IDs.
void function_c6f8(Machine&m,LowContinuation14Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))==0u)return;u32 a=load_le32(dgroup_ptr(m,dgroup(0x2f50u))),c=load_le32(dgroup_ptr(m,dgroup(0x2f5cu)));if(a!=0xffffffffu&&c!=0xffffffffu){function_c4e0(m,b,static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x2f58u))))<static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x2f64u))))?a:c);store_le32(dgroup_ptr(m,dgroup(0x2f5cu)),0xffffffffu);store_le32(dgroup_ptr(m,dgroup(0x2f50u)),0xffffffffu);return;}if(a!=0xffffffffu){function_c4e0(m,b,a);store_le32(dgroup_ptr(m,dgroup(0x2f50u)),0xffffffffu);return;}if(c!=0xffffffffu){function_c4e0(m,b,c);store_le32(dgroup_ptr(m,dgroup(0x2f5cu)),0xffffffffu);}}static_assert(0xC79Fu-0xC6F8u==167u);
// C79F..CBD0 contains position-dependent self-modifying VGA blitters and remains uncounted pending a dedicated instruction-level model.
// CBD4..CBEE: copy exactly 0x100 bytes into the translation table at 2F88.
void function_cbd4(Machine&m,LowContinuation14Boundary&b,u32 src)noexcept{for(u32 i=0;i<0x100u;++i)*dgroup_ptr(m,dgroup(0x2f88u+i))=lc14r8(b,src+i);}static_assert(0xCBEFu-0xCBD4u==27u);

// Batch 13: low-level image transforms, VGA timing/address helpers, and audio/resource state machinery.
// External CODE.1 calls remain address-named boundaries; all local control flow and guest-state effects are explicit.
struct LowGraphics13Boundary{
 u8(*read8)(u32)=nullptr;u16(*read16)(u32)=nullptr;u32(*read32)(u32)=nullptr;
 void(*write8)(u32,u8)=nullptr;void(*write16)(u32,u16)=nullptr;void(*write32)(u32,u32)=nullptr;
 u8(*in8)(u16)=nullptr;void(*out8)(u16,u8)=nullptr;void(*out16)(u16,u16)=nullptr;
 void(*clear_video)(u32,u32)=nullptr;
 u32(*call_627c_record)(const u8*)=nullptr;u32(*call_62a8_record)(const u8*)=nullptr;
 u32(*call_627c_guest)(u32)=nullptr;u32(*call_62a8_guest)(u32)=nullptr;void(*call_62f0)(u32)=nullptr;void(*call_6304)(u32,u32,u32)=nullptr;u32(*call_6304_local32)(u32)=nullptr;
 u32(*call_6f10)(u32,u32,u32)=nullptr;void(*call_6f74)(u32)=nullptr;u32(*call_71c8)(u32)=nullptr;void(*call_7240)(u32)=nullptr;
 u32(*call_f8c4)(u32,u32,u32)=nullptr;void(*call_e680)()=nullptr;void(*call_e6ea)(u32)=nullptr;
 u32(*call_ea0b)(u32)=nullptr;u32(*call_eac8)(u32)=nullptr;u32(*call_eae7)(u32,u32,u32,u32,u32)=nullptr;void(*call_eaf1)(u32,u32,u32,u32,u32)=nullptr;
 u32(*call_ec89)(u32)=nullptr;u32(*call_eca7)(u32)=nullptr;void(*call_ecb1)(u32,u32,u32)=nullptr;void(*call_ea9a)(u32)=nullptr;
 u32(*call_ec93)(u32,u32,u32,u32,u32)=nullptr;u16(*call_ecbb)(u32,u32)=nullptr;void(*call_ecc5)(u32,u32,u32,u32)=nullptr;
 void(*call_ec9d)(u32,u32)=nullptr;u32(*call_ed0b)(u32,u32)=nullptr;void(*call_ecf7)(u32,u32)=nullptr;void(*call_ed01)(u32,u32)=nullptr;
 u32(*call_ed15)(u32,u32)=nullptr;void(*call_ed29)(u32,u32,u32,u32)=nullptr;void(*call_ed33)(u32,u32,u32,u32)=nullptr;void(*call_eced)(u32,u32)=nullptr;
 u32(*call_954c)(u32,u32)=nullptr;void(*copy_d1e8)(u32,u32,u32)=nullptr;
 u32(*format_ede7)(u32,u32,u32)=nullptr;
};
static u8 lg13r8(LowGraphics13Boundary&b,u32 a)noexcept{return b.read8?b.read8(a):0u;}static u16 lg13r16(LowGraphics13Boundary&b,u32 a)noexcept{return b.read16?b.read16(a):0u;}static u32 lg13r32(LowGraphics13Boundary&b,u32 a)noexcept{return b.read32?b.read32(a):0u;}
static void lg13w8(LowGraphics13Boundary&b,u32 a,u8 v)noexcept{if(b.write8)b.write8(a,v);}static void lg13w16(LowGraphics13Boundary&b,u32 a,u16 v)noexcept{if(b.write16)b.write16(a,v);}static void lg13w32(LowGraphics13Boundary&b,u32 a,u32 v)noexcept{if(b.write32)b.write32(a,v);}

// AE0C..AE59: translate one frame through a 256-byte XLAT table; transparent key byte is preserved when flag 80h is set.
void function_ae0c(LowGraphics13Boundary&b,u32 frame,u32 table,u32 image)noexcept{const u32 count=lg13r16(b,image);u32 p=image+8u+count*frame;if((lg13r8(b,image+7u)&0x80u)!=0u){const u8 key=lg13r8(b,p++);for(u32 n=count;n;--n,++p){const u8 v=lg13r8(b,p);if(v!=key)lg13w8(b,p,lg13r8(b,table+v));}}else for(u32 n=count;n;--n,++p){const u8 v=lg13r8(b,p);lg13w8(b,p,lg13r8(b,table+v));}}static_assert(0xAE5Au-0xAE0Cu==78u);
// AE5A..AE5B XCHG EBX,EBX alignment.
// AE5C..AEA5: register-input variant; ESI=image and EDX=frame are live on entry, BL is the subtraction delta.
void function_ae5c(LowGraphics13Boundary&b,u32 esi_image,u32 edx_frame,u8 delta)noexcept{const u32 count=lg13r16(b,esi_image);u32 p=esi_image+8u+count*edx_frame;if((lg13r8(b,esi_image+7u)&0x80u)!=0u){const u8 key=lg13r8(b,p++);for(u32 n=count;n;--n,++p){const u8 v=lg13r8(b,p);if(v!=key)lg13w8(b,p,static_cast<u8>(v-delta));}}else for(u32 n=count;n;--n,++p)lg13w8(b,p,static_cast<u8>(lg13r8(b,p)-delta));}static_assert(0xAEA6u-0xAE5Cu==74u);
// AEA8..AEDB: register helper. EDX is row, ECX is x; output is EBX linear address and ECX plane 0..3.
struct PlanarAddress13{u32 ebx=0,ecx=0;};
[[nodiscard]]PlanarAddress13 function_aea8(Machine&m,u32 edx,u32 ecx)noexcept{const u32 shift1=load_le32(dgroup_ptr(m,dgroup(0x2b35u))),shift2=load_le32(dgroup_ptr(m,dgroup(0x2b39u)));const u32 first=edx<<static_cast<u8>(shift1);const u32 second=first<<static_cast<u8>(shift2);return{first+second+(ecx>>2)+load_le32(dgroup_ptr(m,dgroup(0x2b49u))),ecx&3u};}static_assert(0xAEDCu-0xAEA8u==52u);
// AEDC..AF04: change active page; EAX is saved/restored, so only globals change.
void function_aedc(Machine&m,u32 incoming_eax)noexcept{const u32 page=incoming_eax&0xffu;store_le32(dgroup_ptr(m,dgroup(0x2b31u)),page);u32 v=page*load_le32(dgroup_ptr(m,dgroup(0x2b4du)));v=(v+0xa000u)<<4;store_le32(dgroup_ptr(m,dgroup(0x2b49u)),v);}static_assert(0xAF05u-0xAEDCu==41u);
// AF05..AF07 alignment.
void function_af08(Machine&m,LowGraphics13Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b29u)))!=0u)return;if(b.out16)b.out16(0x3c4u,0x0f02u);if(b.clear_video)b.clear_video(0xa0000u,0x10000u);}static_assert(0xAF34u-0xAF08u==44u);
void function_af34(LowGraphics13Boundary&b)noexcept{if(!b.in8)return;while((b.in8(0x3dau)&8u)!=0u){}while((b.in8(0x3dau)&8u)==0u){}}static_assert(0xAF4Bu-0xAF34u==23u);
void function_af4c(LowGraphics13Boundary&b)noexcept{if(!b.in8)return;while((b.in8(0x3dau)&8u)==0u){}while((b.in8(0x3dau)&8u)!=0u){}}static_assert(0xAF63u-0xAF4Cu==23u);
void function_af64(LowGraphics13Boundary&b,u32 ebx)noexcept{if(b.out16){b.out16(0x3d4u,static_cast<u16>(0x0cu|(static_cast<u16>((ebx>>8)&0xffu)<<8)));b.out16(0x3d4u,static_cast<u16>(0x0du|(static_cast<u16>(ebx&0xffu)<<8)));}function_af34(b);}static_assert(0xAF80u-0xAF64u==28u);
void function_af80(Machine&m,LowGraphics13Boundary&b,u32 ecx)noexcept{store_le32(dgroup_ptr(m,dgroup(0x2b3du)),ecx);function_af34(b);u16 ax=static_cast<u16>(ecx);ax=static_cast<u16>(ax<<1);u8 bh=static_cast<u8>(ax>>8),bl=bh;u16 bx=static_cast<u16>((static_cast<u16>(bh)<<8)|bl);bx=static_cast<u16>(bx&0x0201u);bx=static_cast<u16>(bx<<4);bh=static_cast<u8>((bx>>8)<<1);if(b.out16)b.out16(0x3d4u,static_cast<u16>(0x18u|(static_cast<u16>(static_cast<u8>(ax))<<8)));u8 v=0;if(b.out8)b.out8(0x3d4u,7u);if(b.in8)v=b.in8(0x3d5u);if(b.out16)b.out16(0x3d4u,static_cast<u16>(7u|(static_cast<u16>((v&0xefu)|static_cast<u8>(bx))<<8)));if(b.out8)b.out8(0x3d4u,9u);if(b.in8)v=b.in8(0x3d5u);if(b.out16)b.out16(0x3d4u,static_cast<u16>(9u|(static_cast<u16>((v&0xbfu)|bh)<<8)));}static_assert(0xAFD8u-0xAF80u==88u);
// AFD8..AFED: byte sink used by the formatter; record+0 is cursor, record+10h is count.
void function_afd8(LowGraphics13Boundary&b,u32 rec,u8 value)noexcept{const u32 p=lg13r32(b,rec);lg13w32(b,rec,p+1u);lg13w8(b,p,value);lg13w32(b,rec+0x10u,lg13r32(b,rec+0x10u)+1u);}static_assert(0xAFEEu-0xAFD8u==22u);
// AFEE..B019: formatter wrapper. EDE7 returns emitted count; NUL is stored at dst+count.
void function_afee(LowGraphics13Boundary&b,u32 dst,u32 fmt,u32 vararg_cursor)noexcept{const u32 n=b.format_ede7?b.format_ede7(dst,fmt,vararg_cursor):0u;lg13w8(b,dst+n,0u);}static_assert(0xB01Au-0xAFEEu==44u);
// B01A..B054: append NUL-terminated source to destination, copying words exactly but semantically equivalent byte-for-byte.
[[nodiscard]]u32 function_b01a(LowGraphics13Boundary&b,u32 dst,u32 src)noexcept{u32 end=dst;while(lg13r8(b,end)!=0u)++end;for(;;){const u8 lo=lg13r8(b,src++);lg13w8(b,end++,lo);if(lo==0u)break;const u8 hi=lg13r8(b,src++);lg13w8(b,end++,hi);if(hi==0u)break;}return dst;}static_assert(0xB055u-0xB01Au==59u);
// B055..B05F are eleven zero bytes.

// B060..B0F5: load the fixed 10-byte path descriptor at 030Ch into 2B98.
void function_b060(Machine&m,LowGraphics13Boundary&b)noexcept{u8 rec[10];for(u32 i=0;i<10u;++i)rec[i]=*dgroup_ptr(m,dgroup(0x30cu+i));const u32 size=b.call_627c_record?b.call_627c_record(rec):0u;const u32 dst=b.call_71c8?b.call_71c8(size):0u;store_le32(dgroup_ptr(m,dgroup(0x2b98u)),dst);const u32 h=b.call_62a8_record?b.call_62a8_record(rec):0xffffffffu;if(h==0xffffffffu)return;if(b.call_6304)b.call_6304(dst,size,h);if(b.call_62f0)b.call_62f0(h);*dgroup_ptr(m,dgroup(0x2b94u))=1u;}static_assert(0xB0F6u-0xB060u==150u);
// B0F6..B0F7 MOV EAX,EAX alignment.

// B0F8..B42F: initialize the external resource/audio subsystem. Every failure path closes the same selector/guest allocations encoded by the binary.
void function_b0f8(Machine&m,LowGraphics13Boundary&b,u32 path,u32 p2,u32 p3,u32 p4,u32 p5)noexcept{
 if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))!=0u)return;
 const u32 base=b.call_6f10?b.call_6f10(0xc800u,0x2ba0u,0x2ba4u):0u;store_le32(dgroup_ptr(m,dgroup(0x2b9cu)),base);if(base==0u)return;
 const u32 h=b.call_62a8_guest?b.call_62a8_guest(path):0xffffffffu;if(h==0xffffffffu){if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2ba4u))));return;}
 const u32 size=b.call_627c_guest?b.call_627c_guest(path):0u;const u32 tmp=b.call_71c8?b.call_71c8(size):0u;if(tmp==0u){if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2ba4u))));return;}
 if(b.call_6304)b.call_6304(tmp,size,h);if(b.call_62f0)b.call_62f0(h);const u32 image=b.call_f8c4?b.call_f8c4(tmp,5u,0u):0u;if(image==0u){if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2ba4u))));if(b.call_7240)b.call_7240(tmp);return;}if(b.call_7240)b.call_7240(tmp);
 if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))==0u&&b.call_e680)b.call_e680();const u32 sys=b.call_ea0b?b.call_ea0b(image):0xffffffffu;store_le32(dgroup_ptr(m,dgroup(0x2b90u)),sys);
 auto fail=[&](){if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))==0u&&b.call_e6ea)b.call_e6ea(0u);if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2ba4u))));};
 if(sys==0xffffffffu){fail();return;}const u32 info=b.call_eac8?b.call_eac8(sys):0u;if(lg13r32(b,info+4u)!=3u){fail();return;}if((b.call_eae7?b.call_eae7(sys,p2,p3,p4,p5):0u)==0u){fail();return;}if(b.call_eaf1)b.call_eaf1(sys,p2,p3,p4,p5);
 const u32 pool=b.call_ec89?b.call_ec89(sys):0u;store_le32(dgroup_ptr(m,dgroup(0x2c04u)),pool);if(pool!=0u){for(u32 i=0;i<8u;++i){const u32 q=b.call_6f10?b.call_6f10(pool,0x2bc8u+i*4u,0x2be8u+i*2u):0u;store_le32(dgroup_ptr(m,dgroup(0x2ba8u+i*4u)),q);if(q==0u){for(u32 j=0;j<i;++j)if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2be8u+j*2u))));if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2ba4u))));return;}}}
 const u32 aux=b.call_eca7?b.call_eca7(sys):0u;store_le32(dgroup_ptr(m,dgroup(0x2b8cu)),aux);if(aux!=0u){const u32 q=b.call_6f10?b.call_6f10(aux,0x2bfcu,0x2c00u):0u;store_le32(dgroup_ptr(m,dgroup(0x2bf8u)),q);if(q==0u){for(u32 i=0;i<8u;++i)if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2be8u+i*2u))));if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2ba4u))));return;}if(b.call_ecb1)b.call_ecb1(sys,q,aux);}
 const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x2aa0u)));store_le32(dgroup_ptr(m,dgroup(0x8a18u)),actor);const u16 kind=lg13r16(b,actor+0x32u);if(kind!=0u&&kind!=3u)function_b060(m,b);store_le32(dgroup_ptr(m,dgroup(0x2b84u)),1u);
}
static_assert(0xB430u-0xB0F8u==824u);

void function_b6ec(Machine&m,LowGraphics13Boundary&b)noexcept;void function_ba98(Machine&m,LowGraphics13Boundary&b)noexcept;void function_b5e4(Machine&m,LowGraphics13Boundary&b,u32 slot)noexcept;void function_b824(Machine&m,LowGraphics13Boundary&b,u32 slot,u32 value)noexcept;
// B430..B4D1: subsystem shutdown, releasing all selector/guest resources in encoded order.
void function_b430(Machine&m,LowGraphics13Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u)return;function_ba98(m,b);function_b6ec(m,b);if(b.call_ea9a)b.call_ea9a(load_le32(dgroup_ptr(m,dgroup(0x2b90u))));if(load_le32(dgroup_ptr(m,dgroup(0x2b8cu)))!=0u&&b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2c00u))));if(load_le32(dgroup_ptr(m,dgroup(0x2c04u)))!=0u)for(u32 i=0;i<8u;++i)if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2be8u+i*2u))));if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2ba4u))));if(*dgroup_ptr(m,dgroup(0x2b94u))!=0u){if(b.call_7240)b.call_7240(load_le32(dgroup_ptr(m,dgroup(0x2b98u))));*dgroup_ptr(m,dgroup(0x2b94u))=0u;}store_le32(dgroup_ptr(m,dgroup(0x2b84u)),0u);}static_assert(0xB4D2u-0xB430u==162u);
// B4D4..B51A: search six-byte records in 2B98; byte+1 == FFh terminates.
[[nodiscard]]u32 function_b4d4(Machine&m,LowGraphics13Boundary&b,u16 id,u16 sub)noexcept{const u32 base=load_le32(dgroup_ptr(m,dgroup(0x2b98u)));for(u32 i=0;;++i){const u32 rec=base+i*6u;const std::int8_t marker=static_cast<std::int8_t>(lg13r8(b,rec+1u));if(marker==-1)return 0u;if(static_cast<u16>(static_cast<s16>(marker))==id&&static_cast<u16>(static_cast<std::int8_t>(lg13r8(b,rec)))==sub)return base+lg13r32(b,rec+2u);}}static_assert(0xB51Bu-0xB4D4u==71u);
// B51C..B573: load length-prefixed resource only while subsystem is active.
[[nodiscard]]u32 function_b51c(Machine&m,LowGraphics13Boundary&b,u32,u32 source)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u)return 0u;const u32 len=b.call_6304_local32?b.call_6304_local32(source):0u;const u32 out=b.call_71c8?b.call_71c8(len+4u):0u;lg13w32(b,out,len);if(b.call_6304)b.call_6304(out+4u,len,source);return out;}static_assert(0xB574u-0xB51Cu==88u);
void function_b574(Machine&m,LowGraphics13Boundary&b,u32 p)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))!=0u&&p!=0u&&b.call_7240)b.call_7240(p);}static_assert(0xB596u-0xB574u==34u);
[[nodiscard]]s32 function_b598(Machine&m,LowGraphics13Boundary&b)noexcept{const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x2aa0u)));store_le32(dgroup_ptr(m,dgroup(0x8a18u)),actor);const s32 v=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x2c3cu))));if(lg13r16(b,actor+0x32u)!=3u)return v;u32 x=static_cast<u32>(v);x=x*3u;x=x+x;const u32 six=x;x=(x<<4)-six;x+=0x32u;return static_cast<s32>(x)/100;}static_assert(0xB5E1u-0xB598u==73u);
// B5E4..B6BD: lazily create one of eight sound handles and attach all matching subrecords from 2B98.
void function_b5e4(Machine&m,LowGraphics13Boundary&b,u32 slot)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||*dgroup_ptr(m,dgroup(0x2c28u+slot))!=0u)return;const u32 sys=load_le32(dgroup_ptr(m,dgroup(0x2b90u))),pool=load_le32(dgroup_ptr(m,dgroup(0x2b9cu)));const u32 h=b.call_ec93?b.call_ec93(sys,pool,slot,load_le32(dgroup_ptr(m,dgroup(0x2ba8u+slot*4u))),0u):0xffffffffu;store_le32(dgroup_ptr(m,dgroup(0x2c08u+slot*4u)),h);if(h==0xffffffffu)return;if(*dgroup_ptr(m,dgroup(0x2b94u))!=0u&&b.call_ecbb){for(;;){const u16 ax=b.call_ecbb(sys,h);if(ax==0xffffu)break;const u16 high=static_cast<u16>(ax>>8),low=static_cast<u16>(ax&0xffu);const u32 rec=function_b4d4(m,b,high,low);if(rec==0u)break;if(b.call_ecc5)b.call_ecc5(sys,high,low,rec);}}*dgroup_ptr(m,dgroup(0x2c28u+slot))=1u;}static_assert(0xB6BEu-0xB5E4u==218u);
void function_b6c0(Machine&m,LowGraphics13Boundary&b,u32 slot)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))!=0u&&b.call_ec9d)b.call_ec9d(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c08u+slot*4u))));}static_assert(0xB6EBu-0xB6C0u==43u);
void function_b6ec(Machine&m,LowGraphics13Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u)return;const u32 sys=load_le32(dgroup_ptr(m,dgroup(0x2b90u)));for(u32 i=0;i<8u;++i)if(*dgroup_ptr(m,dgroup(0x2c28u+i))!=0u){const u32 h=load_le32(dgroup_ptr(m,dgroup(0x2c08u+i*4u)));const u32 st=b.call_ed0b?b.call_ed0b(sys,h):0u;if((st==0u||st==2u)&&b.call_ec9d)b.call_ec9d(sys,h);*dgroup_ptr(m,dgroup(0x2c28u+i))=0u;}}static_assert(0xB74Cu-0xB6ECu==96u);
void function_b74c(Machine&m,u32 v)noexcept{store_le32(dgroup_ptr(m,dgroup(0x2b80u)),v);}static_assert(0xB75Bu-0xB74Cu==15u);
void function_b75c(Machine&m,u32 v)noexcept{store_le32(dgroup_ptr(m,dgroup(0x2b88u)),v);}static_assert(0xB76Bu-0xB75Cu==15u);
// B76C..B820: switch active slot and start playback with current rate/pan controls.
void function_b76c(Machine&m,LowGraphics13Boundary&b,u32 slot)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||*dgroup_ptr(m,dgroup(0x2c44u))==0u||slot==load_le32(dgroup_ptr(m,dgroup(0x2c38u))))return;function_ba98(m,b);function_b5e4(m,b,slot);const u32 h=load_le32(dgroup_ptr(m,dgroup(0x2c08u+slot*4u)));store_le32(dgroup_ptr(m,dgroup(0x2c30u)),h);store_le32(dgroup_ptr(m,dgroup(0x2c38u)),slot);if(h==0xffffffffu||slot==0xffffffffu)return;const u32 sys=load_le32(dgroup_ptr(m,dgroup(0x2b90u)));if(b.call_ed29)b.call_ed29(sys,h,static_cast<u32>(function_b598(m,b)),0u);if(b.call_ed33)b.call_ed33(sys,h,load_le32(dgroup_ptr(m,dgroup(0x2c40u))),0u);if(b.call_eced)b.call_eced(sys,h);}static_assert(0xB821u-0xB76Cu==181u);
// B821..B823 alignment.

// B824..B8F2: switch slot then issue base playback setup followed by caller-supplied rate.
void function_b824(Machine&m,LowGraphics13Boundary&b,u32 slot,u32 value)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||*dgroup_ptr(m,dgroup(0x2c44u))==0u||slot==load_le32(dgroup_ptr(m,dgroup(0x2c38u))))return;function_ba98(m,b);function_b5e4(m,b,slot);const u32 h=load_le32(dgroup_ptr(m,dgroup(0x2c08u+slot*4u))),sys=load_le32(dgroup_ptr(m,dgroup(0x2b90u)));store_le32(dgroup_ptr(m,dgroup(0x2c30u)),h);store_le32(dgroup_ptr(m,dgroup(0x2c38u)),slot);if(h==0xffffffffu||slot==0xffffffffu)return;if(b.call_ed33)b.call_ed33(sys,h,load_le32(dgroup_ptr(m,dgroup(0x2c40u))),0u);if(b.call_eced)b.call_eced(sys,h);if(b.call_ed29)b.call_ed29(sys,h,0u,0u);if(b.call_ed29)b.call_ed29(sys,h,static_cast<u32>(function_b598(m,b)),value);}static_assert(0xB8F3u-0xB824u==207u);
// B8F3 NOP.
// B8F4..BA00: update active playback; nonzero value chooses stop/reset/restart sequence, zero reapplies pan.
void function_b8f4(Machine&m,LowGraphics13Boundary&b,u32 slot,u32 value)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||*dgroup_ptr(m,dgroup(0x2c44u))==0u||slot==load_le32(dgroup_ptr(m,dgroup(0x2c38u))))return;function_ba98(m,b);const u32 h=load_le32(dgroup_ptr(m,dgroup(0x2c08u+slot*4u))),sys=load_le32(dgroup_ptr(m,dgroup(0x2b90u)));store_le32(dgroup_ptr(m,dgroup(0x2c30u)),h);store_le32(dgroup_ptr(m,dgroup(0x2c38u)),slot);if(h==0xffffffffu||slot==0xffffffffu)return;if(*dgroup_ptr(m,dgroup(0x2c28u+slot))==0u){function_b824(m,b,slot,value);return;}if(b.call_ed29)b.call_ed29(sys,h,static_cast<u32>(function_b598(m,b)),0u);if(b.call_ed01)b.call_ed01(sys,h);if(value!=0u){if(b.call_ed29)b.call_ed29(sys,h,0u,0u);if(b.call_ed29)b.call_ed29(sys,h,static_cast<u32>(function_b598(m,b)),value);}else if(b.call_ed33)b.call_ed33(sys,h,load_le32(dgroup_ptr(m,dgroup(0x2c40u))),0u);}static_assert(0xBA01u-0xB8F4u==269u);
// BA01..BA03 alignment.
// BA04..BA96: optionally refresh 2B9C resource image for selector/id then switch slot through B76C.
void function_ba04(Machine&m,LowGraphics13Boundary&b,u32 id,u32 slot)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||*dgroup_ptr(m,dgroup(0x2c44u))==0u)return;function_ba98(m,b);function_b6ec(m,b);if(load_le32(dgroup_ptr(m,dgroup(0x2c34u)))!=id){const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x2aa0u)));const u16 kind=lg13r16(b,actor+0x32u);const u32 rid=(kind==3u||kind==4u)?id+load_le32(dgroup_ptr(m,dgroup(0x2b88u))):id;const u32 rec=b.call_954c?b.call_954c(rid,load_le32(dgroup_ptr(m,dgroup(0x2b80u)))):0u;if(b.copy_d1e8)b.copy_d1e8(load_le32(dgroup_ptr(m,dgroup(0x2b9cu))),rec+4u,lg13r32(b,rec));store_le32(dgroup_ptr(m,dgroup(0x2c34u)),id);}function_b76c(m,b,slot);}static_assert(0xBA97u-0xBA04u==147u);
// BA97 NOP.
// BA98..BB7F: stop/normalize current active handle and reset slot/handle to FFFFFFFF.
void function_ba98(Machine&m,LowGraphics13Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||load_le32(dgroup_ptr(m,dgroup(0x2c38u)))==0xffffffffu)return;const u32 sys=load_le32(dgroup_ptr(m,dgroup(0x2b90u))),h=load_le32(dgroup_ptr(m,dgroup(0x2c30u)));u32 st=b.call_ed0b?b.call_ed0b(sys,h):0u;if(st!=0u&&st!=2u&&b.call_ecf7)b.call_ecf7(sys,h);function_b5e4(m,b,0u);const u32 h0=load_le32(dgroup_ptr(m,dgroup(0x2c08u)));store_le32(dgroup_ptr(m,dgroup(0x2c30u)),h0);store_le32(dgroup_ptr(m,dgroup(0x2c38u)),0u);if(h0!=0xffffffffu){if(b.call_ed29)b.call_ed29(sys,h0,0u,0u);if(b.call_ed33)b.call_ed33(sys,h0,load_le32(dgroup_ptr(m,dgroup(0x2c40u))),0u);if(b.call_eced)b.call_eced(sys,h0);do{st=b.call_ed0b?b.call_ed0b(sys,h0):2u;}while(st!=2u);}store_le32(dgroup_ptr(m,dgroup(0x2c38u)),0xffffffffu);store_le32(dgroup_ptr(m,dgroup(0x2c30u)),0xffffffffu);}static_assert(0xBB80u-0xBA98u==232u);
// BB80..BBE8: if active state remains live, either mark ended or apply caller value through ED29.
void function_bb80(Machine&m,LowGraphics13Boundary&b,u32 value)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||load_le32(dgroup_ptr(m,dgroup(0x2c38u)))==0xffffffffu)return;const u32 sys=load_le32(dgroup_ptr(m,dgroup(0x2b90u))),h=load_le32(dgroup_ptr(m,dgroup(0x2c30u)));const u32 st=b.call_ed0b?b.call_ed0b(sys,h):0u;if(st==0u||st==2u){store_le32(dgroup_ptr(m,dgroup(0x2c30u)),0xffffffffu);store_le32(dgroup_ptr(m,dgroup(0x2c38u)),0xffffffffu);}else if(b.call_ed29)b.call_ed29(sys,h,0u,value);}static_assert(0xBBE9u-0xBB80u==105u);
// BBE9..BBEB alignment.
void function_bbec(Machine&m,LowGraphics13Boundary&b,u32 slot,u32 value)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))!=0u&&*dgroup_ptr(m,dgroup(0x2c44u))!=0u){function_ba98(m,b);function_b824(m,b,slot,value);}}static_assert(0xBC1Au-0xBBECu==46u);
// BC1A..BC1B alignment.
void function_bc1c(Machine&m,LowGraphics13Boundary&b,u32 id,u32 slot,u32 value)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||*dgroup_ptr(m,dgroup(0x2c44u))==0u)return;function_ba98(m,b);function_b6ec(m,b);if(load_le32(dgroup_ptr(m,dgroup(0x2c34u)))!=id){const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x2aa0u)));const u16 kind=lg13r16(b,actor+0x32u);const u32 rid=(kind==3u||kind==4u)?id+load_le32(dgroup_ptr(m,dgroup(0x2b88u))):id;const u32 rec=b.call_954c?b.call_954c(rid,load_le32(dgroup_ptr(m,dgroup(0x2b80u)))):0u;if(b.copy_d1e8)b.copy_d1e8(load_le32(dgroup_ptr(m,dgroup(0x2b9cu))),rec+4u,lg13r32(b,rec));store_le32(dgroup_ptr(m,dgroup(0x2c34u)),id);}function_b824(m,b,slot,value);}static_assert(0xBCB3u-0xBC1Cu==151u);
void function_bcb4(Machine&m,LowGraphics13Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||load_le32(dgroup_ptr(m,dgroup(0x2c38u)))==0xffffffffu)return;const u32 sys=load_le32(dgroup_ptr(m,dgroup(0x2b90u))),h=load_le32(dgroup_ptr(m,dgroup(0x2c30u)));u32 st=b.call_ed0b?b.call_ed0b(sys,h):0u;if(st==0u||st==2u){store_le32(dgroup_ptr(m,dgroup(0x2c38u)),0xffffffffu);store_le32(dgroup_ptr(m,dgroup(0x2c30u)),0xffffffffu);return;}while((b.call_ed15?b.call_ed15(sys,h):0u)!=0u){}function_ba98(m,b);}static_assert(0xBD20u-0xBCB4u==108u);
void function_bd20(Machine&m,LowGraphics13Boundary&b,u32 value)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2c3cu)))==value)return;store_le32(dgroup_ptr(m,dgroup(0x2c3cu)),value);if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||load_le32(dgroup_ptr(m,dgroup(0x2c38u)))==0xffffffffu)return;const u32 sys=load_le32(dgroup_ptr(m,dgroup(0x2b90u))),h=load_le32(dgroup_ptr(m,dgroup(0x2c30u)));if((b.call_ed15?b.call_ed15(sys,h):0u)==0x64u&&b.call_ed29)b.call_ed29(sys,h,static_cast<u32>(function_b598(m,b)),0u);}static_assert(0xBD83u-0xBD20u==99u);
void function_bd84(Machine&m,LowGraphics13Boundary&b,u32 value)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2c40u)))==value)return;store_le32(dgroup_ptr(m,dgroup(0x2c40u)),value);if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))!=0u&&load_le32(dgroup_ptr(m,dgroup(0x2c38u)))!=0xffffffffu&&b.call_ed33)b.call_ed33(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))),value,0u);}static_assert(0xBDCCu-0xBD84u==72u);
[[nodiscard]]u32 function_bdcc(Machine&m)noexcept{return load_le32(dgroup_ptr(m,dgroup(0x2c38u)));}static_assert(0xBDD8u-0xBDCCu==12u);
[[nodiscard]]u32 function_bdd8(Machine&m)noexcept{return load_le32(dgroup_ptr(m,dgroup(0x2c34u)));}static_assert(0xBDE4u-0xBDD8u==12u);
[[nodiscard]]u32 function_bde4(Machine&m,LowGraphics13Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||load_le32(dgroup_ptr(m,dgroup(0x2c38u)))==0xffffffffu)return 0u;const u32 st=b.call_ed0b?b.call_ed0b(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u)))):0u;return(st==0u||st==2u)?1u:0u;}static_assert(0xBE26u-0xBDE4u==66u);
[[nodiscard]]u32 function_be28(Machine&m)noexcept{return load_le32(dgroup_ptr(m,dgroup(0x2c40u)));}static_assert(0xBE34u-0xBE28u==12u);
[[nodiscard]]u32 function_be34(Machine&m)noexcept{return load_le32(dgroup_ptr(m,dgroup(0x2c3cu)));}static_assert(0xBE40u-0xBE34u==12u);
void function_be40(Machine&m,LowGraphics13Boundary&b,u32 enabled)noexcept{*dgroup_ptr(m,dgroup(0x2c44u))=static_cast<u8>(enabled);if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))!=0u&&static_cast<u8>(enabled)==0u)function_ba98(m,b);}static_assert(0xBE61u-0xBE40u==33u);

constexpr u32 kVerifiedTranslatedBytes=69361u;
constexpr u32 kCoverageDenominator=kCodeObjectVirtualSize;


// Batch 15: remaining actor-choice/animation logic plus independently bounded planar VGA helpers.
// Every range below was re-disassembled directly from immutable CODE.1 before this fold.
struct Batch15EarlyBoundary{
 void(*call_56dc)()=nullptr;void(*call_6588)(u32)=nullptr;
 void(*call_6824)(u32,u32,u32,u32,u32)=nullptr;void(*call_6eb0)(u32,u32,u32,u32,u32,u32)=nullptr;void(*call_6c60)(s32,s32,s32,s32,u32)=nullptr;
 u32(*call_954c)(u32,u32)=nullptr;void(*call_53e4)(u32)=nullptr;void(*call_5454)(u32)=nullptr;
 u32(*call_71c8)(u32)=nullptr;void(*call_66c0)(u32,s32,s32,u32,u32)=nullptr;
 void(*call_6744)(u32,s32,s32,u32,u32)=nullptr;void(*call_67c8)(u32,s32,s32,u32)=nullptr;
 void(*call_7240)(u32)=nullptr;void(*call_86c4)(s32,s32,s32,u32)=nullptr;void(*call_4b0)(u32)=nullptr;
 void(*call_24d4)(u32,s32,s32)=nullptr;void(*call_7e4)(s32,s32)=nullptr;
 void(*call_1464)(u32,const u32*,s32)=nullptr;u32(*call_53b4)()=nullptr;s32(*call_43c)()=nullptr;
 void(*call_15b0)(s32,s32,s32,s32)=nullptr;void(*call_1f24)()=nullptr;
};
static s32 div8_after_plus4(s32 v)noexcept{const s32 w=static_cast<s32>(static_cast<u32>(v)+4u);return w/8;}

// 15B0..1A00: five-frame slot dissolve followed by optional nine-frame movement of the selected slot.
// 7138 is byte-toggled, but the encoded 53E4/5454 calls push the full dword beginning at 7138.
void function_15b0(Machine&m,Batch15EarlyBoundary&b,s32 selected,s32 target_x,s32 target_y,s32 final_mode)noexcept{
 if(b.call_56dc)b.call_56dc();
 for(u32 frame=0;frame<5u;++frame){
  if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));
  for(u32 slot=0;slot<9u;++slot){if(static_cast<s32>(slot)==selected)continue;const s32 id=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08f8u+slot*8u))));if(id==-1)continue;const u32 dst=0x5ff8u+(frame<<8);const s32 x=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+slot*8u)))),y=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+slot*8u))));const s32 mode=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x04d0u))));if(mode==0){if(b.call_6824)b.call_6824(0u,static_cast<u32>(x),static_cast<u32>(y),load_le32(dgroup_ptr(m,dgroup(0x0954u+slot*4u))),dst);continue;}if(slot==7u){if(b.call_6824)b.call_6824(0u,static_cast<u32>(x),static_cast<u32>(y),load_le32(dgroup_ptr(m,dgroup(0x0958u))),dst);continue;}const s32 base=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu))));if(id>=base&&id<base+0x2e){if(b.call_6eb0)b.call_6eb0(0u,static_cast<u32>(x),static_cast<u32>(y),0x14u,7u,dst);const u32 rec=b.call_954c?b.call_954c(static_cast<u32>(id),1u):0u;if(b.call_6824)b.call_6824(0u,static_cast<u32>(x+0x15),static_cast<u32>(y),rec,0x5af8u+(frame<<8));}else if(b.call_6eb0)b.call_6eb0(0u,static_cast<u32>(x),static_cast<u32>(y),static_cast<u32>(id),7u,dst);}
  if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;
 }
 for(s32 frame=4;frame>=0;--frame){if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));for(u32 slot=0;slot<9u;++slot){if(static_cast<s32>(slot)==selected)continue;const s32 id=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08f8u+slot*8u))));if(id==-1)continue;const s32 x=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+slot*8u)))),y=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+slot*8u))));if(b.call_6eb0)b.call_6eb0(0u,static_cast<u32>(x),static_cast<u32>(y),slot+0x1eu,7u,0x5ff8u+(static_cast<u32>(frame)<<8));}if(frame==0){if(b.call_5454)b.call_5454(load_le32(dgroup_ptr(m,dgroup(0x7138u))));}else if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;}
 if(selected==-1)return;
 const u32 scratch[2]={b.call_71c8?b.call_71c8(0x1e80u):0u,b.call_71c8?b.call_71c8(0x1e80u):0u};if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));const s32 x0=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+static_cast<u32>(selected)*8u)))),y0=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+static_cast<u32>(selected)*8u))));if(b.call_6c60)b.call_6c60(0,x0,y0,selected+0x1e,7);if(b.call_66c0){b.call_66c0(scratch[0],x0,y0,0x7au,0x30u);b.call_66c0(scratch[1],x0,y0,0x7au,0x30u);}
 s32 old_x[2]={x0,x0},old_y[2]={y0,y0};s32 x=x0,y=y0;
 for(s32 step=0;step<=8;++step){const u32 page=load_le32(dgroup_ptr(m,dgroup(0x7138u)));if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_6744)b.call_6744(scratch[page],old_x[page],old_y[page],0x7au,0x30u);old_x[page]=x;old_y[page]=y;if(b.call_66c0)b.call_66c0(scratch[page],x,y,0x7au,0x30u);const s32 id=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08f8u+static_cast<u32>(selected)*8u))));const s32 base=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu))));if(id>=base&&id<base+0x2e){const u32 r0=b.call_954c?b.call_954c(0x14u,7u):0u;if(b.call_67c8)b.call_67c8(0u,x,y,r0);const u32 r1=b.call_954c?b.call_954c(static_cast<u32>(id),1u):0u;if(b.call_67c8)b.call_67c8(0u,x+0x15,y,r1);}else{const u32 r=b.call_954c?b.call_954c(static_cast<u32>(id),7u):0u;if(b.call_67c8)b.call_67c8(0u,x,y,r);}const s32 n=step+1;const u32 dxn=(static_cast<u32>(target_x)-static_cast<u32>(x0))*static_cast<u32>(n),dyn=(static_cast<u32>(target_y)-static_cast<u32>(y0))*static_cast<u32>(n);x=static_cast<s32>(static_cast<u32>(x0)+static_cast<u32>(div8_after_plus4(static_cast<s32>(dxn))));y=static_cast<s32>(static_cast<u32>(y0)+static_cast<u32>(div8_after_plus4(static_cast<s32>(dyn))));if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;}
 if(b.call_7240){b.call_7240(scratch[0]);b.call_7240(scratch[1]);}if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_86c4)b.call_86c4(0xc8,0,0x19,7u);if(b.call_4b0)b.call_4b0(static_cast<u32>(final_mode));if(b.call_5454)b.call_5454(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;
}
static_assert(0x1A01u-0x15B0u==1105u);
// 1A01..1A03 is LEA alignment.

// 2198..24D0: generate six availability dwords. Modes 0/1 copy signed table bytes; mode 2 clears the output and applies the encoded class/subclass combination rules.
[[nodiscard]]u32 function_2198(Machine&m,u32 out[6],u32 mode,u32 incoming_edx=0u)noexcept{
 const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x04ccu)))*0x248u;
 if(mode==0u){u32 any=0;const u32 cls=*dgroup_ptr(m,dgroup(actor+0x680cu));for(u32 i=0;i<6u;++i){const s32 v=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(0x06d8u+cls*6u+i)));out[i]=static_cast<u32>(v);if(v!=0)any=1u;}return any;}
 if(mode==1u){u32 any=0;const u32 cls=*dgroup_ptr(m,dgroup(actor+0x680cu));const s32 sub=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(actor+0x680du)));const s32 off=static_cast<s32>(cls*36u)+sub*6;for(u32 i=0;i<6u;++i){const s32 v=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(static_cast<u32>(0x06fcu+off+static_cast<s32>(i)))));out[i]=static_cast<u32>(v);if(v!=0)any=1u;}return any;}
 if(mode!=2u)return incoming_edx;for(u32 i=0;i<6u;++i)out[i]=0u;const u8 c=*dgroup_ptr(m,dgroup(actor+0x680cu)),d=*dgroup_ptr(m,dgroup(actor+0x680du)),e=*dgroup_ptr(m,dgroup(actor+0x680eu));
 if(c==5u){if(d==0u){if(e==4u)out[5]=1u;if(e==5u)out[4]=1u;}if(d==4u){if(e==0u)out[5]=1u;if(e==5u)out[0]=1u;}if(d==5u&&e==4u)out[0]=1u;}
 else if(c==1u){if(d==0u){if(e==4u){out[5]=1u;out[3]=1u;}if(e==5u||e==3u)out[4]=1u;}if(d==4u){if(e==0u){out[5]=1u;out[3]=1u;}if(e==5u||e==3u)out[0]=1u;}if(d==5u){if(e==4u)out[0]=1u;if(e==0u){out[4]=1u;out[3]=1u;}}if(d==3u){if(e==4u)out[0]=1u;if(e==0u)out[4]=1u;}}
 u32 any=0;for(u32 i=0;i<6u;++i)if(out[i]!=0u)any=1u;return any;
}
static_assert(0x24D1u-0x2198u==825u);
// 24D1..24D3 is LEA alignment.

// 266C..28E1: three-stage six-choice driver. Event values are 1..6, 0 cancel, FFh back/reset.
void function_266c(Machine&m,Batch15EarlyBoundary&b)noexcept{
 s32 last_event=0;u32 display[6]={};s32 staged=0,transition=0;for(u32 i=0;i<3u;++i)*dgroup_ptr(m,dgroup(load_le32(dgroup_ptr(m,dgroup(0x04ccu)))*0x248u+0x680du+i))=0xffu;
 s32 state=0;
 for(;;){
  if(state>=3||transition==-1)break;u32 flags[6]={};if(function_2198(m,flags,static_cast<u32>(state),m.cpu.edx)==0u)break;for(u32 i=0;i<6u;++i)display[i]=flags[i]?i+6u:0x15u;
  if(staged!=0){if(state!=0&&b.call_24d4)b.call_24d4(static_cast<u32>(last_event-1),0x114,0x6d);for(u32 i=0;i<6u;++i){const u32 cell=0x08f4u+i*8u;if(display[i]!=0x15u){if(state==0){store_le32(dgroup_ptr(m,dgroup(cell)),load_le32(dgroup_ptr(m,dgroup(cell+4u))));store_le32(dgroup_ptr(m,dgroup(cell+4u)),display[i]);}else{store_le32(dgroup_ptr(m,dgroup(cell)),display[i]);store_le32(dgroup_ptr(m,dgroup(cell+4u)),display[i]);}}else{store_le32(dgroup_ptr(m,dgroup(cell)),load_le32(dgroup_ptr(m,dgroup(cell+4u))));store_le32(dgroup_ptr(m,dgroup(cell+4u)),0x15u);}}if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_86c4)b.call_86c4(0xc8,0,0x19,7u);if(b.call_4b0)b.call_4b0(3u);store_le32(dgroup_ptr(m,dgroup(0x0940u)),0x0bu);store_le32(dgroup_ptr(m,dgroup(0x0948u)),3u);if(b.call_7e4)b.call_7e4(0,6);}
  else{const u32 slots[11]={display[0],display[1],display[2],display[3],display[4],display[5],0xffffffffu,0xffffffffu,0xffffffffu,0x0au,0xffffffffu};if(b.call_1464)b.call_1464(3u,slots,0x28);staged=1;}
  while(b.call_53b4&&b.call_53b4()!=0u){}
  s32 action=0;for(;;){last_event=b.call_43c?b.call_43c():0;if(last_event==0||last_event==0xff){if(last_event==0xff&&state!=0){transition=1;action=2;}else if(last_event==0){if(state!=0){const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x04ccu)))*0x248u;for(u32 i=0;i<3u;++i)*dgroup_ptr(m,dgroup(actor+0x680du+i))=0xffu;state=-1;}else transition=-1;action=1;}}else if(last_event>=1&&last_event<=6&&display[static_cast<u32>(last_event-1)]!=0x15u){u8 v=static_cast<u8>(last_event);--v;const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x04ccu)))*0x248u;*dgroup_ptr(m,dgroup(actor+0x680du+static_cast<u32>(state)))=v;transition=1;action=1;}if(action!=0)break;}
  --action;++state;if(action!=0)break;
 }
 if(last_event!=0&&last_event!=0xff){if(b.call_15b0)b.call_15b0(last_event-1,0x114,0x6d,3);}else if(b.call_15b0)b.call_15b0(-1,-1,-1,-1);
 store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))+static_cast<u32>(transition));if(transition==1&&b.call_1f24)b.call_1f24();
}
static_assert(0x28E2u-0x266Cu==630u);
// 28E2..28E3 is MOV EAX,EAX alignment.

static void batch15_copy(LowGraphics13Boundary&b,u32 dst,u32 src,u32 count,bool backward=false)noexcept{if(!backward){for(u32 i=0;i<count;++i)lg13w8(b,dst+i,lg13r8(b,src+i));}else for(u32 i=0;i<count;++i)lg13w8(b,dst-i,lg13r8(b,src-i));}
static void batch15_vga_copy_mode(LowGraphics13Boundary&b,bool on)noexcept{if(b.out16)b.out16(0x3ceu,on?0x4105u:0x4005u);}

// A21C..A2AD: shift/copy the current planar page horizontally; CX sign selects forward versus backward copy.
void function_a21c(Machine&m,LowGraphics13Boundary&b,u32 raw)noexcept{const u32 pitch=load_le32(dgroup_ptr(m,dgroup(0x2b4du)));if(static_cast<s16>(raw&0xffffu)>=0){const u32 dst=load_le32(dgroup_ptr(m,dgroup(0x2b49u))),src=raw>>2,count=pitch<<4;batch15_vga_copy_mode(b,true);if(b.out16)b.out16(0x3c4u,0x0f02u);batch15_copy(b,dst,src,count);batch15_vga_copy_mode(b,false);}else{raw=0u-raw;const u32 count=pitch<<4,dst=load_le32(dgroup_ptr(m,dgroup(0x2b49u)))+count-1u,src=dst-(raw>>2);batch15_vga_copy_mode(b,true);if(b.out16)b.out16(0x3c4u,0x0f02u);batch15_copy(b,dst,src,count,true);batch15_vga_copy_mode(b,false);}}
static_assert(0xA2AEu-0xA21Cu==146u);
// A2AE..A2AF alignment.

// A2B0..A358: vertical planar copy. DX sign selects forward versus backward direction.
void function_a2b0(Machine&m,LowGraphics13Boundary&b,u32 x,u32 raw_y)noexcept{const u32 pitch=load_le32(dgroup_ptr(m,dgroup(0x2b41u)));if(static_cast<s16>(raw_y&0xffffu)>=0){const PlanarAddress13 s=function_aea8(m,raw_y,x);const u32 count=x*pitch,dst=load_le32(dgroup_ptr(m,dgroup(0x2b49u)));batch15_vga_copy_mode(b,true);if(b.out16)b.out16(0x3c4u,0x0f02u);batch15_copy(b,dst,s.ebx,count);batch15_vga_copy_mode(b,false);}else{const u32 y=0u-raw_y;u32 src=function_aea8(m,y,x).ebx,dst=function_aea8(m,load_le32(dgroup_ptr(m,dgroup(0x2b45u))),0u).ebx;--src;--dst;const u32 count=x*pitch;batch15_vga_copy_mode(b,true);if(b.out16)b.out16(0x3c4u,0x0f02u);batch15_copy(b,dst,src,count,true);batch15_vga_copy_mode(b,false);}}
static_assert(0xA359u-0xA2B0u==169u);
// A359..A35B alignment.
void function_a35c(Machine&m,LowGraphics13Boundary&b,u32 page)noexcept{function_aedc(m,page);}static_assert(0xA36Au-0xA35Cu==14u);
// A36A..A36B alignment.
void function_a36c(Machine&m,LowGraphics13Boundary&b,u32 v)noexcept{const u32 q=v*(load_le32(dgroup_ptr(m,dgroup(0x2b4du)))>>4);function_af64(b,static_cast<u32>(static_cast<u8>(q))<<8);}static_assert(0xA38Du-0xA36Cu==33u);
// A38D..A38F alignment.
void function_a390(Machine&m,LowGraphics13Boundary&b,u32 page)noexcept{const u32 pitch=load_le32(dgroup_ptr(m,dgroup(0x2b4du)));const u32 dst=((page*pitch)+0xa000u)<<4,src=load_le32(dgroup_ptr(m,dgroup(0x2b49u)));const u32 count=(pitch&0xffff0000u)|((pitch<<4)&0xffffu);batch15_vga_copy_mode(b,true);if(b.out16)b.out16(0x3c4u,0x0f02u);batch15_copy(b,dst,src,count);batch15_vga_copy_mode(b,false);}static_assert(0xA3E5u-0xA390u==85u);
// A3E5..A3E7 alignment.
void function_a3e8(Machine&m,LowGraphics13Boundary&b,u32 source_page,u32 sy,u32 sx,u32 dy,u32 dx,u32 height,u32 width)noexcept{const u8 saved=*dgroup_ptr(m,dgroup(0x2b31u));function_aedc(m,(source_page&0xffu)|(static_cast<u32>(saved)<<8));const PlanarAddress13 s=function_aea8(m,sy,sx);width+=s.ecx;const u32 pitch=load_le32(dgroup_ptr(m,dgroup(0x2b41u)));function_aedc(m,saved);const PlanarAddress13 d=function_aea8(m,dy,dx);u32 src=s.ebx,dst=d.ebx;const u32 row=(width+3u)>>2,skip=pitch-row;batch15_vga_copy_mode(b,true);if(b.out16)b.out16(0x3c4u,0x0f02u);for(u32 y=0;y<height;++y){batch15_copy(b,dst,src,row);dst+=row+skip;src+=row+skip;}batch15_vga_copy_mode(b,false);}static_assert(0xA473u-0xA3E8u==139u);
// A473 NOP.
void function_a474(Machine&m,LowGraphics13Boundary&b,u32 v)noexcept{function_af80(m,b,v);}static_assert(0xA484u-0xA474u==16u);
// A484..A4E6: read four VGA planes into consecutive host/guest bytes using GC read-map select 0..3.
void function_a484(Machine&m,LowGraphics13Boundary&b,u32 dst,u32 y,u32 x,u32 height,u32 width)noexcept{const PlanarAddress13 p=function_aea8(m,y,x);const u32 row=(width+p.ecx+3u)>>2,skip=load_le32(dgroup_ptr(m,dgroup(0x2b41u)))-row;u32 out=dst;for(u32 plane=0;plane<4u;++plane){if(b.out16)b.out16(0x3ceu,static_cast<u16>(0x0004u|(plane<<8)));u32 src=p.ebx;for(u32 yy=0;yy<height;++yy){for(u32 i=0;i<row;++i)lg13w8(b,out++,lg13r8(b,src+i));src+=row+skip;}}}static_assert(0xA4E7u-0xA484u==99u);
// A4E7 NOP.
// A4E8..A554: write consecutive source bytes into four VGA planes using sequencer masks 1,2,4,8, then restore FF02h.
void function_a4e8(Machine&m,LowGraphics13Boundary&b,u32 src,u32 y,u32 x,u32 height,u32 width)noexcept{const PlanarAddress13 p=function_aea8(m,y,x);const u32 row=(width+p.ecx+3u)>>2,skip=load_le32(dgroup_ptr(m,dgroup(0x2b41u)))-row;u32 in=src;for(u32 plane=0,mask=1u;plane<4u;++plane,mask<<=1){if(b.out16)b.out16(0x3c4u,static_cast<u16>(0x0002u|(mask<<8)));u32 dst=p.ebx;for(u32 yy=0;yy<height;++yy){for(u32 i=0;i<row;++i)lg13w8(b,dst+i,lg13r8(b,in++));dst+=row+skip;}}if(b.out16)b.out16(0x3c4u,0xff02u);}static_assert(0xA555u-0xA4E8u==109u);
// A555..A557 alignment.

static u8 rol8_1(u8 v)noexcept{return static_cast<u8>((v<<1)|(v>>7));}
static void batch15_sprite_core(Machine&m,LowGraphics13Boundary&b,u32 frame,u32 y,u32 x,u32 image,u8 delta,bool subtract)noexcept{const PlanarAddress13 p=function_aea8(m,y,x);u32 dst0=p.ebx;u8 mask=static_cast<u8>(0x11u<<p.ecx);const u32 width=lg13r16(b,image+4u),rows=lg13r8(b,image+6u),flags=lg13r8(b,image+7u);u32 src=image+8u+lg13r16(b,image)*frame;const u32 rowbytes=width*2u,skip=load_le32(dgroup_ptr(m,dgroup(0x2b41u)))-rowbytes;if((flags&0x80u)==0u){for(u32 plane=0;plane<4u;++plane){if(b.out16)b.out16(0x3c4u,static_cast<u16>(0x0002u|(static_cast<u16>(mask)<<8)));u32 dst=dst0;for(u32 yy=0;yy<rows;++yy){for(u32 i=0;i<rowbytes;++i){u8 v=lg13r8(b,src++);if(subtract)v=static_cast<u8>(v-delta);lg13w8(b,dst++,v);}dst+=skip;}const bool carry=(mask&0x80u)!=0u;mask=rol8_1(mask);if(carry)++dst0;}}else{const u8 key=lg13r8(b,src++);for(u32 plane=0;plane<4u;++plane){if(b.out16)b.out16(0x3c4u,static_cast<u16>(0x0002u|(static_cast<u16>(mask)<<8)));u32 dst=dst0;for(u32 yy=0;yy<rows;++yy){for(u32 i=0;i<rowbytes;++i){u8 v=lg13r8(b,src++);if(v!=key){if(subtract)v=static_cast<u8>(v-delta);lg13w8(b,dst,v);}++dst;}dst+=skip;}const bool carry=(mask&0x80u)!=0u;mask=rol8_1(mask);if(carry)++dst0;}}}
// A6F0..A7E7: planar sprite draw. Transparent images skip key-colored bytes; nontransparent images copy all four planes.
void function_a6f0(Machine&m,LowGraphics13Boundary&b,u32 frame,u32 y,u32 x,u32 image)noexcept{batch15_sprite_core(m,b,frame,y,x,image,0u,false);}static_assert(0xA7E8u-0xA6F0u==248u);
// A7E8..A8F3: same draw with the caller byte subtracted from every emitted non-key pixel. The original writes that byte into two immediates in its own code on every entry.
void function_a7e8(Machine&m,LowGraphics13Boundary&b,u32 frame,u32 y,u32 x,u32 image,u32 delta)noexcept{batch15_sprite_core(m,b,frame,y,x,image,static_cast<u8>(delta),true);}static_assert(0xA8F4u-0xA7E8u==268u);



// Batch 16: 07E4 transition renderer, deferred interrupt handlers, and shared startup exit tail.
struct Batch16Boundary{
 void(*call_8e3c)(u32,u32)=nullptr;void(*call_6588)(u32)=nullptr;void(*call_86c4)(s32,s32,s32,u32)=nullptr;
 void(*call_67c8)(u32,s32,s32,u32)=nullptr;void(*call_6c60)(u32,s32,s32,u32,u32)=nullptr;void(*call_6eb0)(u32,s32,s32,u32,u32,u32)=nullptr;
 u32(*call_954c)(u32,u32)=nullptr;void(*call_9c44)(u32,u32,u32,u32,u32,u32,s32,s32)=nullptr;void(*call_a05c)(s32,s32,s32,s32,u32)=nullptr;
 void(*call_004c)(u32,u32)=nullptr;void(*call_00ec)(s32,s32,u32,u32,u32,u32,u32)=nullptr;void(*call_55dc)(u32,s32,s32,s32,s32)=nullptr;
 void(*call_53e4)(u32)=nullptr;void(*call_064c)()=nullptr;
};
static bool b16_range(Machine&m,s32 id)noexcept{const s32 base=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu))));return id>=base&&id<base+0x2e;}
static void b16_slide(Batch16Boundary&b,u32 dir,u32 step,u32 height,u32 res,s32 x,s32 y)noexcept{if(b.call_9c44)b.call_9c44(dir,step,height,1u,1u,res,x,y);}

// 07E4..10A1: staggered eleven-phase transition of slots [first,last).
void function_07e4(Machine&m,Batch16Boundary&b,s32 first,s32 last)noexcept{
 u8 counter[9]{};
 if(b.call_8e3c)b.call_8e3c(0x14u,7u);
 for(s32 slot=first;slot<last;++slot){const s32 id=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08f8u+static_cast<u32>(slot)*8u))));if(id<0)continue;if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&id==1)continue;if(slot==7&&id!=0x14)continue;if(b.call_8e3c)b.call_8e3c(static_cast<u32>(id),b16_range(m,id)?1u:7u);}
 s32 phase=0;
 while(counter[static_cast<u32>(last-1)]!=0x0bu){
  if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));
  if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&phase<2&&b.call_86c4)b.call_86c4(0xc8,0,0x19,7u);
  for(s32 slot=first;slot<last && (slot-first)<=phase;++slot)++counter[static_cast<u32>(slot)];
  for(s32 slot=first;slot<last;++slot){
   const u32 s=static_cast<u32>(slot),i8=s*8u,i4=s*4u;const u32 k=counter[s];
   s32 x=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+i8))));s32 y=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+i8))));
   const s32 old_id=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08f4u+i8))));const s32 new_id=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08f8u+i8))));
   if(old_id==new_id){if(new_id==-1)continue;if(slot==7&&new_id!=0x14){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0958u))));}else if(b16_range(m,new_id)){if(b.call_6c60)b.call_6c60(0u,x,y,0x14u,7u);if(b.call_6eb0)b.call_6eb0(0u,x+0x15,y,static_cast<u32>(new_id),1u,0x5af8u);}else if(b.call_6eb0)b.call_6eb0(0u,x,y,static_cast<u32>(new_id),7u,0x5ff8u);continue;}
   if(new_id==-1)continue;
   if(k==0u){
    if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&old_id==1){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0954u+i4))));}
    else if(b16_range(m,old_id)){if(b.call_6c60)b.call_6c60(0u,x,y,0x14u,7u);if(b.call_6eb0)b.call_6eb0(0u,x+0x15,y,static_cast<u32>(old_id),1u,0x5af8u);}
    else if(slot==7&&old_id!=0x15){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0954u))));}
    else if(b.call_6c60)b.call_6c60(0u,x,y,static_cast<u32>(old_id),7u);
    continue;
   }
   if(k<6u){
    if(b.call_6c60)b.call_6c60(0u,x,y,s+0x1eu,7u);const u32 q=6u-k;y+=static_cast<s32>(4u*k);
    if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&old_id==1)b16_slide(b,0u,q,10u,load_le32(dgroup_ptr(m,dgroup(0x0954u+i4))),x,y);
    else if(b16_range(m,old_id)){const u32 a=b.call_954c?b.call_954c(0x14u,7u):0u,c=b.call_954c?b.call_954c(static_cast<u32>(old_id),1u):0u;b16_slide(b,0u,q,10u,a,x,y);b16_slide(b,1u,q,10u,c,x+0x15,y);}
    else if(slot==7&&old_id!=0x15)b16_slide(b,0u,q,10u,load_le32(dgroup_ptr(m,dgroup(0x0954u))),x,y);
    else{const u32 r=b.call_954c?b.call_954c(static_cast<u32>(old_id),7u):0u;b16_slide(b,0u,q,10u,r,x,y);}continue;
   }
   if(k==6u){if(b.call_6c60)b.call_6c60(0u,x,y,s+0x1eu,7u);const u32 color=*dgroup_ptr(m,dgroup(0x63f8u));if(b.call_a05c)b.call_a05c(x,y+0x18,x+0x79,y+0x18,color);continue;}
   if(k<12u){
    const u32 q=12u-k;if(b.call_6c60)b.call_6c60(0u,x,y,s+0x1eu,7u);y+=static_cast<s32>(4u*q);
    if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&new_id==1)b16_slide(b,0u,q,6u,load_le32(dgroup_ptr(m,dgroup(0x0954u+i4))),x,y);
    else if(slot==7&&new_id!=0x14)b16_slide(b,0u,q,6u,load_le32(dgroup_ptr(m,dgroup(0x0958u))),x,y);
    else if(b16_range(m,new_id)){const u32 a=b.call_954c?b.call_954c(0x14u,7u):0u,c=b.call_954c?b.call_954c(static_cast<u32>(new_id),1u):0u;b16_slide(b,0u,q,6u,a,x,y);b16_slide(b,1u,q,6u,c,x+0x15,y);}
    else{const u32 r=b.call_954c?b.call_954c(static_cast<u32>(new_id),7u):0u;b16_slide(b,0u,q,6u,r,x,y);}continue;
   }
   if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&new_id==1){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0954u+i4))));}
   else if(slot==7&&new_id!=0x14){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0958u))));}
   else if(b16_range(m,new_id)){if(b.call_6c60)b.call_6c60(0u,x,y,0x14u,7u);if(b.call_6eb0)b.call_6eb0(0u,x+0x15,y,static_cast<u32>(new_id),1u,0x5af8u);}
   else if(b.call_6c60)b.call_6c60(0u,x,y,static_cast<u32>(new_id),7u);
  }
  if(phase<2){if(load_le32(dgroup_ptr(m,dgroup(0x0940u)))!=0xffffffffu){if(b.call_004c)b.call_004c(0x55f8u,load_le32(dgroup_ptr(m,dgroup(0x0940u))));if(b.call_00ec)b.call_00ec(0x114,0x10c,0x7a,0x30,0x55f8u,0u,0x5ff8u);}if(load_le32(dgroup_ptr(m,dgroup(0x0948u)))!=0xffffffffu){if(b.call_004c)b.call_004c(0x55f8u,load_le32(dgroup_ptr(m,dgroup(0x0948u))));if(b.call_00ec)b.call_00ec(0x114,0xd7,0x7a,0x30,0x55f8u,0u,0x5ff8u);}if(b.call_55dc)b.call_55dc(load_le32(dgroup_ptr(m,dgroup(0x7138u))),0xec,0,0xa4,0x140);}
  else{if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;}
  ++phase;
 }
 if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_064c)b.call_064c();if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;
}
static_assert(0x10A2u-0x07E4u==2238u);

struct Batch16InterruptBoundary{
 void(*enter_saved_frame)()=nullptr;void(*leave_iret_frame)()=nullptr;void(*call_cec0)()=nullptr;void(*call_dde0)(u32)=nullptr;void(*chain_292a)()=nullptr;
 u32(*in_port_dde7)(u32)=nullptr;void(*out_port_ddf2)(u32,u32)=nullptr;void(*call_da18)()=nullptr;void(*call_da1a)()=nullptr;void(*call_7fa8)(u32)=nullptr;u32(*call_802c)(u32)=nullptr;
 void(*call_64dc)(u32)=nullptr;void(*call_cf98)()=nullptr;void(*set_vector_ddfe)(u32,u32,u32)=nullptr;void(*chain_2924)()=nullptr;
};
void interrupt_7f5c(Machine&m,Batch16InterruptBoundary&b)noexcept{if(b.enter_saved_frame)b.enter_saved_frame();if(b.call_cec0)b.call_cec0();const u32 tick=load_le32(dgroup_ptr(m,dgroup(0x2934u)));store_le32(dgroup_ptr(m,dgroup(0x2934u)),tick+1u);if(b.call_dde0)b.call_dde0(tick);if(b.chain_292a)b.chain_292a();if(b.leave_iret_frame)b.leave_iret_frame();}static_assert(0x7F8Au-0x7F5Cu==46u);
void interrupt_80bc(Machine&m,Batch16InterruptBoundary&b)noexcept{if(b.enter_saved_frame)b.enter_saved_frame();if(b.call_cec0)b.call_cec0();u32 key=b.in_port_dde7?b.in_port_dde7(0x60u):0u;u8 p61=static_cast<u8>(b.in_port_dde7?b.in_port_dde7(0x61u):0u);*dgroup_ptr(m,dgroup(0x2918u))=p61;if(p61==0x80u){p61=static_cast<u8>(b.in_port_dde7?b.in_port_dde7(0x61u):0u);*dgroup_ptr(m,dgroup(0x2918u))=static_cast<u8>(p61|0x80u);}if(b.out_port_ddf2){b.out_port_ddf2(0x61u,static_cast<u8>(*dgroup_ptr(m,dgroup(0x2918u))|0x80u));b.out_port_ddf2(0x61u,*dgroup_ptr(m,dgroup(0x2918u)));}if(b.call_da18)b.call_da18();u32 scan=key;if(scan>0x80u){scan&=0x7fu;if(scan==0x2au)*dgroup_ptr(m,dgroup(0x291bu))&=0xfeu;else if(scan==0x36u)*dgroup_ptr(m,dgroup(0x291bu))&=0xfdu;else if(scan==0x38u)*dgroup_ptr(m,dgroup(0x291bu))&=0xfbu;else if(scan==0x1du)*dgroup_ptr(m,dgroup(0x291bu))&=0xf7u;else if(scan>=0x47u&&scan<=0x51u){if(b.call_7fa8)b.call_7fa8(scan);}else *dgroup_ptr(m,dgroup(0x291au))=*dgroup_ptr(m,dgroup(0x2919u));}else{if(scan==0x2au)*dgroup_ptr(m,dgroup(0x291bu))|=1u;else if(scan==0x36u)*dgroup_ptr(m,dgroup(0x291bu))|=2u;else if(scan==0x38u)*dgroup_ptr(m,dgroup(0x291bu))|=4u;else if(scan==0x1du)*dgroup_ptr(m,dgroup(0x291bu))|=8u;else{if(scan>=0x47u&&scan<=0x51u){scan=b.call_802c?b.call_802c(scan):scan;*dgroup_ptr(m,dgroup(0x2919u))=static_cast<u8>(scan);}*dgroup_ptr(m,dgroup(0x291au))=static_cast<u8>(scan);}}const u8 code=*dgroup_ptr(m,dgroup(0x291au));if(code==0x10u&&(*dgroup_ptr(m,dgroup(0x291bu))&4u)){*dgroup_ptr(m,dgroup(0x2b01u))|=1u;*dgroup_ptr(m,dgroup(0x291au))=0u;}else if(code==0x43u)*dgroup_ptr(m,dgroup(0x291au))=0u;else if(code==1u)*dgroup_ptr(m,dgroup(0x2b01u))|=2u;else if(code==0x39u)*dgroup_ptr(m,dgroup(0x291cu))=1u;if(b.out_port_ddf2)b.out_port_ddf2(0x20u,0x20u);if(b.call_da1a)b.call_da1a();if(b.leave_iret_frame)b.leave_iret_frame();}static_assert(0x826Eu-0x80BCu==434u);
void interrupt_8338(Machine&m,Batch16InterruptBoundary&b)noexcept{if(b.enter_saved_frame)b.enter_saved_frame();if(b.call_cec0)b.call_cec0();if(b.call_64dc)b.call_64dc(0xffu);if(b.call_cf98)b.call_cf98();if(b.set_vector_ddfe){b.set_vector_ddfe(9u,load_le32(dgroup_ptr(m,dgroup(0x291eu))),load_le16(dgroup_ptr(m,dgroup(0x2922u))));b.set_vector_ddfe(8u,load_le32(dgroup_ptr(m,dgroup(0x292au))),load_le16(dgroup_ptr(m,dgroup(0x292eu))));}if(b.chain_2924)b.chain_2924();if(b.leave_iret_frame)b.leave_iret_frame();}static_assert(0x8398u-0x8338u==96u);

struct StartupExit16Boundary{u32(*open_3d01)(u32)=nullptr;u8(*read8)(u32)=nullptr;void(*write_40)(u32,u32,u32)=nullptr;void(*call_fd2b)(u32)=nullptr;void(*terminate_4c)(u8)=nullptr;};
[[noreturn]] void function_ce8b_exit(StartupExit16Boundary&b,u32 exit_code){if(b.call_fd2b)b.call_fd2b(exit_code);if(b.terminate_4c)b.terminate_4c(static_cast<u8>(exit_code));for(;;){}}
[[noreturn]] void function_ce8e_exit(StartupExit16Boundary&b,u32 message,u32 exit_code){const u32 h=b.open_3d01?b.open_3d01(0xcc92u):0u;u32 p=message;while((b.read8?b.read8(p):0u)!=0u)++p;if(b.write_40)b.write_40(h,message,p-message);function_ce8b_exit(b,exit_code);}
static_assert(0xCEC0u-0xCE8Bu==53u);



// Batch 17: exact line rasterization plus translated/clipped planar sprite families.
static u8 b17_rol1(u8 v)noexcept{return static_cast<u8>((v<<1)|(v>>7));}
static void b17_mask(LowGraphics13Boundary&b,u8 mask)noexcept{if(b.out16)b.out16(0x3c4u,static_cast<u16>(0x0002u|(static_cast<u16>(mask)<<8)));}
static void b17_put(LowGraphics13Boundary&b,u32 addr,u8 value)noexcept{(void)lg13r8(b,addr);lg13w8(b,addr,value);}

void function_a05c(Machine&m,LowGraphics13Boundary&b,s32 a0,s32 b0,s32 a1,s32 b1,u32 color)noexcept{
 if(b.out16)b.out16(0x3ceu,static_cast<u16>(0x0003u|(static_cast<u16>(*dgroup_ptr(m,dgroup(0x2b28u)))<<8)));
 u32 pitch=load_le32(dgroup_ptr(m,dgroup(0x2b41u)));s32 db=b1-b0;const u8 c=static_cast<u8>(color);
 if(db==0){s32 lo=a0,hi=a1;if(hi<lo){const s32 t=lo;lo=hi;hi=t;}const PlanarAddress13 p=function_aea8(m,static_cast<u32>(lo),static_cast<u32>(b0));u32 addr=p.ebx;b17_mask(b,static_cast<u8>(1u<<(p.ecx&3u)));for(s32 n=hi-lo+1;n>0;--n){b17_put(b,addr,c);addr+=pitch;}if(b.out16)b.out16(0x3ceu,0x0003u);return;}
 if(db<0){db=-db;{const s32 t=b0;b0=b1;b1=t;}{const s32 t=a0;a0=a1;a1=t;}}s32 da=a1-a0;if(da==0){const PlanarAddress13 p=function_aea8(m,static_cast<u32>(a0),static_cast<u32>(b0));u32 addr=p.ebx;u8 mask=static_cast<u8>(1u<<(p.ecx&3u));for(s32 n=db+1;n>0;--n){b17_mask(b,mask);b17_put(b,addr,c);const bool carry=(mask&0x80u)!=0u;mask=b17_rol1(mask);if(carry)++addr;}if(b.out16)b.out16(0x3ceu,0x0003u);return;}
 if(da<0){da=-da;pitch=0u-pitch;}const bool a_major=da>db;const s32 major=a_major?da:db,minor=a_major?db:da;const s32 twice_minor=minor*2;const s32 step_diag=twice_minor-major*2; s32 err=twice_minor-major;
 const PlanarAddress13 p=function_aea8(m,static_cast<u32>(a0),static_cast<u32>(b0));u32 addr=p.ebx;u8 mask=static_cast<u8>(1u<<(p.ecx&3u));b17_mask(b,mask);
 for(s32 n=major+1;n>0;--n){b17_put(b,addr,c);if(a_major){addr+=pitch;if(err<0)err+=twice_minor;else{err+=step_diag;const bool carry=(mask&0x80u)!=0u;mask=b17_rol1(mask);if(carry)++addr;b17_mask(b,mask);}}else{const bool carry=(mask&0x80u)!=0u;mask=b17_rol1(mask);if(carry)++addr;b17_mask(b,mask);if(err<0)err+=twice_minor;else{err+=step_diag;addr+=pitch;}}}
 if(b.out16)b.out16(0x3ceu,0x0003u);
}
static_assert(0xA21Au-0xA05Cu==446u);

static void b17_table_sprite(Machine&m,LowGraphics13Boundary&b,u32 frame,u32 y,u32 x,u32 image,u32 table)noexcept{
 const PlanarAddress13 p=function_aea8(m,y,x);u32 dst0=p.ebx;u8 mask=static_cast<u8>(0x11u<<(p.ecx&3u));const u32 width=lg13r16(b,image+4u),rows=lg13r8(b,image+6u),flags=lg13r8(b,image+7u);u32 src=image+8u+lg13r16(b,image)*frame;const u32 rowbytes=width*2u,skip=load_le32(dgroup_ptr(m,dgroup(0x2b41u)))-rowbytes;
 if((flags&0x80u)==0u){for(u32 plane=0;plane<4u;++plane){b17_mask(b,mask);u32 dst=dst0;for(u32 yy=0;yy<rows;++yy){for(u32 i=0;i<rowbytes;++i){const u8 v=lg13r8(b,src++);lg13w8(b,dst++,lg13r8(b,table+v));}dst+=skip;}const bool carry=(mask&0x80u)!=0u;mask=b17_rol1(mask);if(carry)++dst0;}}
 else{const u8 key=lg13r8(b,src++);for(u32 plane=0;plane<4u;++plane){b17_mask(b,mask);u32 dst=dst0;for(u32 yy=0;yy<rows;++yy){for(u32 i=0;i<rowbytes;++i){const u8 v=lg13r8(b,src++);if(v!=key)lg13w8(b,dst,lg13r8(b,table+v));++dst;}dst+=skip;}const bool carry=(mask&0x80u)!=0u;mask=b17_rol1(mask);if(carry)++dst0;}}
}
void function_a8f4(Machine&m,LowGraphics13Boundary&b,u32 frame,u32 y,u32 x,u32 image,u32 table)noexcept{b17_table_sprite(m,b,frame,y,x,image,table);}static_assert(0xAA20u-0xA8F4u==300u);

static void b17_clipped_sprite(Machine&m,LowGraphics13Boundary&b,u32 frame,u32 y,u32 x,u32 clip,u32 image,u8 delta,bool subtract)noexcept{
 auto w=[&](u32 o,u32 v){store_le32(dgroup_ptr(m,dgroup(o)),v);};auto r=[&](u32 o){return load_le32(dgroup_ptr(m,dgroup(o)));};
 w(0x2b71u,0u);w(0x2b75u,0u);w(0x2b55u,lg13r32(b,clip)>>2);w(0x2b59u,lg13r32(b,clip+4u)>>2);w(0x2b5du,lg13r32(b,clip+8u));w(0x2b61u,(lg13r32(b,clip+0x0cu)+3u)>>2);
 u32 q=lg13r32(b,clip+0x10u);w(0x2b6du,q&3u);if(q!=0u){w(0x2b71u,1u);q>>=2;}w(0x2b65u,q);q=(lg13r32(b,clip+0x14u)+r(0x2b6du))>>2;if(q==0u){q=1u;if(r(0x2b71u)!=1u){w(0x2b75u,4u-lg13r32(b,clip+0x14u));w(0x2b61u,r(0x2b61u)-1u);}}w(0x2b69u,q);w(0x2b6du,lg13r32(b,clip+0x0cu)&3u);
 const PlanarAddress13 p=function_aea8(m,y,x);u32 dst0=p.ebx;u8 mask=0x11u;for(u32 i=0;i<((p.ecx+4u-r(0x2b6du))&3u);++i)mask=b17_rol1(mask);u32 src=image+8u+lg13r16(b,image)*frame;u8 key=0xffu;if((lg13r8(b,image+7u)&0x80u)!=0u){key=lg13r8(b,src);++src;}u32 planes=4u;const u32 plane_stride=lg13r16(b,image)>>2;
 for(u32 skip=0;skip<r(0x2b75u)&&planes!=0u;++skip){src+=plane_stride;--planes;mask=b17_rol1(mask);}w(0x2b79u,planes);u32 dst_skip=load_le32(dgroup_ptr(m,dgroup(0x2b41u)))-r(0x2b69u)-r(0x2b65u);b17_mask(b,mask);
 while(planes!=0u){u32 dst=dst0;u32 rows=r(0x2b5du);while(rows--!=0u){src+=r(0x2b55u);src+=r(0x2b61u);u32 n=r(0x2b69u);while(n--!=0u){u8 v=lg13r8(b,src++);if(v!=key){if(subtract)v=static_cast<u8>(v-delta);lg13w8(b,dst,v);}++dst;}const u32 tail=r(0x2b65u);dst+=tail;src+=tail;dst+=dst_skip;}src+=r(0x2b59u);--planes;w(0x2b79u,planes);if(planes==0u)break;const bool carry=(mask&0x80u)!=0u;mask=b17_rol1(mask);if(carry){++dst0;if(r(0x2b61u)!=0u){w(0x2b61u,r(0x2b61u)-1u);--dst_skip;--dst0;if(r(0x2b71u)!=0u)w(0x2b65u,r(0x2b65u)+1u);else w(0x2b69u,r(0x2b69u)+1u);}else if(r(0x2b71u)!=0u){w(0x2b65u,r(0x2b65u)+1u);w(0x2b69u,r(0x2b69u)-1u);if(r(0x2b69u)==0u)break;}}b17_mask(b,mask);}
}
void function_aa20(Machine&m,LowGraphics13Boundary&b,u32 frame,u32 y,u32 x,u32 clip,u32 image)noexcept{b17_clipped_sprite(m,b,frame,y,x,clip,image,0u,false);}static_assert(0xAC0Fu-0xAA20u==495u);
void function_ac10(Machine&m,LowGraphics13Boundary&b,u32 frame,u32 y,u32 x,u32 clip,u32 image,u32 delta)noexcept{b17_clipped_sprite(m,b,frame,y,x,clip,image,static_cast<u8>(delta),true);}static_assert(0xAE0Bu-0xAC10u==507u);



// Batch 18: self-modifying scaled VGA blitters represented with explicit patched runtime fields.
static u8 b18_rol1(u8 v)noexcept{return static_cast<u8>((v<<1)|(v>>7));}
static u8 b18_rol(u8 v,u32 n)noexcept{for(u32 i=0;i<(n&7u);++i)v=b18_rol1(v);return v;}
static void b18_mask(LowGraphics13Boundary&b,u8 mask)noexcept{if(b.out16)b.out16(0x3c4u,static_cast<u16>(0x0002u|(static_cast<u16>(mask)<<8)));}

static void b18_scaled5(Machine&m,LowGraphics13Boundary&b,u32 num,u32 den,u32 image,u32 a,u32 c,bool translate)noexcept{
 const PlanarAddress13 p=function_aea8(m,a,c);u32 dst_base=p.ebx;u8 draw_mask=b18_rol(0x11u,p.ecx),outer_mask=0x11u;u32 src_base=image+9u;const u16 h=lg13r8(b,image+6u),h2=static_cast<u16>(h*2u),outer_total=static_cast<u16>(h2*4u);const u16 h8p1=static_cast<u16>(outer_total+1u),h8m1=static_cast<u16>(outer_total-1u);const u32 src_frac=(static_cast<u32>(h2)<<8)|0x78u;const u32 scale_step=(static_cast<u32>(static_cast<u16>(num))<<8)|0x78u;const u32 inner_total=lg13r16(b,image+2u);const u8 key=lg13r8(b,image+8u);u32 outer_acc=den>>1;
 for(u32 outer=outer_total;outer!=0u;--outer){const u32 outer_src=src_base;outer_acc+=scale_step;while(outer_acc>=den){outer_acc-=den;u32 src=outer_src,dst=dst_base,inner_acc=den>>1;b18_mask(b,draw_mask);for(u32 n=inner_total;n!=0u;--n){inner_acc+=scale_step;if(inner_acc>=den){u8 v=lg13r8(b,src);if(translate)v=*dgroup_ptr(m,dgroup(0x2f88u+v));do{if(v!=key)lg13w8(b,dst,v);dst+=0x50u;inner_acc-=den;}while(inner_acc>=den);}src+=h8p1;}const bool carry=(draw_mask&0x80u)!=0u;draw_mask=b18_rol1(draw_mask);if(carry)++dst_base;}src_base=outer_src+src_frac;const bool carry=(outer_mask&0x80u)!=0u;outer_mask=b18_rol1(outer_mask);if(carry)src_base-=h8m1;}
 if(b.out16)b.out16(0x3c4u,0xff02u);
}
void function_c7a0(Machine&m,LowGraphics13Boundary&b,u32 num,u32 den,u32 image,u32 a,u32 c)noexcept{b18_scaled5(m,b,num,den,image,a,c,true);}static_assert(0xC8C1u-0xC7A0u==289u);
void function_c8c4(Machine&m,LowGraphics13Boundary&b,u32 num,u32 den,u32 image,u32 a,u32 c)noexcept{b18_scaled5(m,b,num,den,image,a,c,false);}static_assert(0xC9D2u-0xC8C4u==270u);

static void b18_scaled7(Machine&m,LowGraphics13Boundary&b,u32 outer_num,u32 outer_den,u32 inner_num,u32 inner_den,u32 image,u32 a,u32 c,bool translate)noexcept{
 const PlanarAddress13 p=function_aea8(m,a,c);u32 dst_base=p.ebx;u8 draw_mask=b18_rol(0x11u,p.ecx),outer_mask=0x11u;const u8 h=lg13r8(b,image+6u);const u16 h2=static_cast<u16>(static_cast<u16>(h)*2u),outer_total=static_cast<u16>(h2*4u);const u16 frame=lg13r16(b,image),frame_m1=static_cast<u16>(frame-1u),plane_step=static_cast<u16>(frame>>2);const u32 inner_total=lg13r16(b,image+4u);const u8 key=lg13r8(b,image+8u);u32 src_base=image+9u,outer_acc=outer_den>>1;
 for(u32 outer=outer_total;outer!=0u;--outer){const u32 outer_src=src_base;outer_acc+=outer_num;while(outer_acc>=outer_den){outer_acc-=outer_den;u32 src=outer_src,dst=dst_base,inner_acc=inner_den>>1;b18_mask(b,draw_mask);for(u32 n=inner_total;n!=0u;--n){inner_acc+=inner_num;if(inner_acc>=inner_den){const u8 raw=lg13r8(b,src);const u8 v=translate?*dgroup_ptr(m,dgroup(0x2f88u+raw)):raw;do{if(translate||v!=key)lg13w8(b,dst,v);dst+=0x50u;inner_acc-=inner_den;}while(inner_acc>=inner_den);}src+=h2;}const bool carry=(draw_mask&0x80u)!=0u;draw_mask=b18_rol1(draw_mask);if(carry)++dst_base;}src_base=outer_src+plane_step;const bool carry=(outer_mask&0x80u)!=0u;outer_mask=b18_rol1(outer_mask);if(carry)src_base-=frame_m1;}
 if(b.out16)b.out16(0x3c4u,0xff02u);
}
void function_c9d4(Machine&m,LowGraphics13Boundary&b,u32 outer_num,u32 outer_den,u32 inner_num,u32 inner_den,u32 image,u32 a,u32 c)noexcept{b18_scaled7(m,b,outer_num,outer_den,inner_num,inner_den,image,a,c,false);}static_assert(0xCACAu-0xC9D4u==246u);
void function_cacc(Machine&m,LowGraphics13Boundary&b,u32 outer_num,u32 outer_den,u32 inner_num,u32 inner_den,u32 image,u32 a,u32 c)noexcept{b18_scaled7(m,b,outer_num,outer_den,inner_num,inner_den,image,a,c,true);}static_assert(0xCBD1u-0xCACCu==261u);



// Batch 19: DE70..DEA0 is the byte-aligning front end to the already integrated DEA7 dword fill core.
// 84A0 constructs EDX as four copies of the caller byte before entering this routine.
// The initial destination-byte read at DE74 is retained as an explicit touch even though TEST AL,3 overwrites its flags.
struct FillDe70Boundary{
 u8(*read8)(u32)=nullptr;
 void(*write8)(u32,u8)=nullptr;
 u32(*call_dea7)(u32,u32,u32)=nullptr;
};
[[nodiscard]]u32 function_de70(FillDe70Boundary&b,u32 dst,u32 pattern,u32 count)noexcept{
 if(count==0u)return dst;
 if(b.read8)(void)b.read8(dst);
 while((dst&3u)!=0u&&count!=0u){
  if(b.write8)b.write8(dst,static_cast<u8>(pattern));
  ++dst;
  pattern=(pattern>>8)|(pattern<<24);
  --count;
 }
 const u32 saved=count;
 const u32 dwords=count>>2;
 if(b.call_dea7)dst=b.call_dea7(dst,pattern,dwords);
 else dst+=dwords*4u;
 count=saved&3u;
 if(count==0u)return dst;
 if(b.write8)b.write8(dst,static_cast<u8>(pattern));
 if(--count==0u)return dst;
 if(b.write8)b.write8(dst+1u,static_cast<u8>(pattern>>8));
 if(--count==0u)return dst;
 if(b.write8)b.write8(dst+2u,static_cast<u8>(pattern));
 return dst;
}
static_assert(0xDEA1u-0xDE70u==49u);
// DE6F is the zero byte immediately preceding the call target; DEA1..DEA6 are six NOP alignment bytes.



// Batch 20: final residual-code/accounting audit.
// 0000..0002 is the encoded INT3; JMP 0000 trap loop. 0003..000F are NOP alignment.
struct TrapStub20Boundary{void(*int3)()=nullptr;};
[[noreturn]] void function_0000(TrapStub20Boundary&b)noexcept{for(;;){if(b.int3)b.int3();}}
static_assert(0x0003u-0x0000u==3u);

// 7F50..7F5B: direct-call tick getter immediately before the INT 8 handler.
[[nodiscard]]u32 function_7f50(Machine&m)noexcept{return load_le32(dgroup_ptr(m,dgroup(0x2934u)));}
static_assert(0x7F5Cu-0x7F50u==12u);

// 95A8..95BA: far callback installed by 95E9/E280. INC/DEC EBP bracket the far frame;
// the dword at encoded [EBP+10h] is copied to 23D0 and EAX is cleared before RETF.
[[nodiscard]]u32 function_95a8(Machine&m,u32 stack_dword_10)noexcept{store_le32(dgroup_ptr(m,dgroup(0x23d0u)),stack_dword_10);return 0u;}
static_assert(0x95BBu-0x95A8u==19u);
// 95BB is alignment NOP.

// 9E10..9E11: two-byte JMP 9E8C tail alias; 9E0F and 9E12..9E13 are alignment.
void function_9e10(Machine&m,u32 index)noexcept{function_9e8c(m,index);}
static_assert(0x9E12u-0x9E10u==2u);
// 9EA7..9EAD is the shared register-pop/LEAVE/RET path used by the nonpositive
// width/height exits of 9EB0; those edges are already represented by its early return.
static_assert(0x9EAEu-0x9EA7u==7u);

// 1058B..1058F is the five-byte JMP 10696 shared early-exit edge from the heap free path.
// The high-level early return in function_10590 already represents the edge; account its bytes here.
static_assert(0x10590u-0x1058Bu==5u);

// CBEF is a one-byte RET callback referenced by three dwords in the original image.
void function_cbef()noexcept{}
static_assert(0xCBF0u-0xCBEFu==1u);

// CBF0..CC18: shared non-returning termination chain reached by JMP from application/runtime exits.
// The exit code remains the caller-pushed dword at [EBP+4] across both encoded frames.
struct Termination20Boundary{void(*call_3088)()=nullptr;void(*call_308c)()=nullptr;void(*call_3090)()=nullptr;void(*tail_ce8b)(u32)=nullptr;};
[[noreturn]] static void function_cc02(Termination20Boundary&b,u32 exit_code)noexcept{if(b.call_308c)b.call_308c();if(b.call_3090)b.call_3090();if(b.tail_ce8b)b.tail_ce8b(exit_code);for(;;){}}
static_assert(0xCC19u-0xCC02u==23u);
[[noreturn]] void function_cbf0(Termination20Boundary&b,u32 exit_code)noexcept{if(b.call_3088)b.call_3088();if(b.call_308c)b.call_308c();function_cc02(b,exit_code);}
static_assert(0xCC02u-0xCBF0u==18u);

} // namespace code1
