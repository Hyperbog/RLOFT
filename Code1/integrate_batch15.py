from pathlib import Path

cpp=Path('Code1/Code1.cpp')
text=cpp.read_text()
assert 'void function_15b0(' not in text
assert 'void function_a21c(' not in text
assert 'void function_a6f0(' not in text
marker='} // namespace code1'
assert marker in text
block=r'''

// Batch 15: remaining actor-choice/animation logic plus independently bounded planar VGA helpers.
// Every range below was re-disassembled directly from immutable CODE.1 before this fold.
struct Batch15Pair{u32 ebx=0,esi=0;};
struct Batch15EarlyBoundary{
 void(*call_56dc)()=nullptr;void(*call_6588)(u32)=nullptr;
 void(*call_6824)(u32,u32,u32,u32,u32)=nullptr;void(*call_6eb0)(u32,u32,u32,u32,u32,u32)=nullptr;
 u32(*call_954c)(u32,u32)=nullptr;void(*call_53e4)(u32)=nullptr;void(*call_5454)(u32)=nullptr;
 u32(*call_71c8)(u32)=nullptr;void(*call_66c0)(u32,s32,s32,u32,u32)=nullptr;
 Batch15Pair(*call_6744)(u32,s32,s32,u32,u32)=nullptr;void(*call_67c8)(u32,s32,s32,u32)=nullptr;
 void(*call_7240)(u32)=nullptr;void(*call_86c4)(s32,s32,s32,u32)=nullptr;void(*call_4b0)(u32)=nullptr;
 void(*call_24d4)(u32,s32,s32)=nullptr;void(*call_7e4)(s32,s32)=nullptr;
 void(*call_1464)(u32,const u32*,s32)=nullptr;u32(*call_53b4)()=nullptr;s32(*call_43c)()=nullptr;
 void(*call_15b0)(s32,s32,s32,s32)=nullptr;void(*call_1f24)()=nullptr;
};
static s32 div8_after_plus4(s32 v)noexcept{return static_cast<s32>(static_cast<s64>(v)+4)/8;}

// 15B0..1A00: five-frame slot dissolve followed by optional nine-frame movement of the selected slot.
// 7138 is byte-toggled, but the encoded 53E4/5454 calls push the full dword beginning at 7138.
void function_15b0(Machine&m,Batch15EarlyBoundary&b,s32 selected,s32 target_x,s32 target_y,s32 final_mode)noexcept{
 if(b.call_56dc)b.call_56dc();
 for(u32 frame=0;frame<5u;++frame){
  if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));
  for(u32 slot=0;slot<9u;++slot){
   if(static_cast<s32>(slot)==selected)continue;
   const s32 id=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08f8u+slot*8u))));if(id==-1)continue;
   const u32 dst=0x5ff8u+(frame<<8);const s32 x=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+slot*8u))));const s32 y=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+slot*8u))));
   const s32 mode=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x04d0u))));
   if(mode==0){if(b.call_6824)b.call_6824(0u,static_cast<u32>(x),static_cast<u32>(y),load_le32(dgroup_ptr(m,dgroup(0x0954u+slot*4u))),dst);continue;}
   if(slot==7u){if(b.call_6824)b.call_6824(0u,static_cast<u32>(x),static_cast<u32>(y),load_le32(dgroup_ptr(m,dgroup(0x0958u))),dst);continue;}
   const s32 base=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu))));
   if(id>=base&&id<base+0x2e){
    if(b.call_6eb0)b.call_6eb0(0u,static_cast<u32>(x),static_cast<u32>(y),0x14u,7u,dst);
    const u32 rec=b.call_954c?b.call_954c(static_cast<u32>(id),1u):0u;
    if(b.call_6824)b.call_6824(0u,static_cast<u32>(x+0x15),static_cast<u32>(y),rec,0x5af8u+(frame<<8));
   }else if(b.call_6eb0)b.call_6eb0(0u,static_cast<u32>(x),static_cast<u32>(y),static_cast<u32>(id),7u,dst);
  }
  if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;
 }
 for(s32 frame=4;frame>=0;--frame){
  if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));
  for(u32 slot=0;slot<9u;++slot){if(static_cast<s32>(slot)==selected)continue;const s32 id=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08f8u+slot*8u))));if(id==-1)continue;const s32 x=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+slot*8u))));const s32 y=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+slot*8u))));if(b.call_6eb0)b.call_6eb0(0u,static_cast<u32>(x),static_cast<u32>(y),slot+0x1eu,7u,0x5ff8u+(static_cast<u32>(frame)<<8));}
  if(frame==0){if(b.call_5454)b.call_5454(load_le32(dgroup_ptr(m,dgroup(0x7138u))));}else if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));
  *dgroup_ptr(m,dgroup(0x7138u))^=1u;
 }
 if(selected==-1)return;
 const u32 scratch0=b.call_71c8?b.call_71c8(0x1e80u):0u,scratch1=b.call_71c8?b.call_71c8(0x1e80u):0u;
 if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));
 const s32 x0=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+static_cast<u32>(selected)*8u))));const s32 y0=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+static_cast<u32>(selected)*8u))));
 if(b.call_6eb0)b.call_6eb0(0u,static_cast<u32>(x0),static_cast<u32>(y0),static_cast<u32>(selected+0x1e),7u,0u);
 if(b.call_66c0){b.call_66c0(scratch0,x0,y0,0x7au,0x30u);b.call_66c0(scratch1,x0,y0,0x7au,0x30u);}
 s32 px[2]={x0,x0},py[2]={y0,y0};const u32 scratch[2]={scratch0,scratch1};
 for(s32 step=0;step<=8;++step){
  const u32 page=*dgroup_ptr(m,dgroup(0x7138u))&1u;if(b.call_6588)b.call_6588(page);
  Batch15Pair q{static_cast<u32>(px[page]),static_cast<u32>(py[page])};if(b.call_6744)q=b.call_6744(scratch[page],px[page],py[page],0x7au,0x30u);px[page]=static_cast<s32>(q.ebx);py[page]=static_cast<s32>(q.esi);
  if(b.call_66c0)b.call_66c0(scratch[page],px[page],py[page],0x7au,0x30u);
  const s32 id=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08f8u+static_cast<u32>(selected)*8u))));const s32 base=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu))));
  if(id>=base&&id<base+0x2e){const u32 r0=b.call_954c?b.call_954c(0x14u,7u):0u;if(b.call_67c8)b.call_67c8(r0,px[page],py[page],0u);const u32 r1=b.call_954c?b.call_954c(static_cast<u32>(id),1u):0u;if(b.call_67c8)b.call_67c8(r1,px[page]+0x15,py[page],0u);}else{const u32 r=b.call_954c?b.call_954c(static_cast<u32>(id),7u):0u;if(b.call_67c8)b.call_67c8(r,px[page],py[page],0u);}
  const s32 n=step+1;px[page]=x0+div8_after_plus4((target_x-x0)*n);py[page]=y0+div8_after_plus4((target_y-y0)*n);
  if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;
 }
 if(b.call_7240){b.call_7240(scratch0);b.call_7240(scratch1);}if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_86c4)b.call_86c4(0xc8,0,0x19,7u);if(b.call_4b0)b.call_4b0(static_cast<u32>(final_mode));if(b.call_5454)b.call_5454(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;
}
static_assert(0x1A01u-0x15B0u==1105u);
// 1A01..1A03 is LEA alignment.

// 2198..24D0: generate six availability dwords. Modes 0/1 copy signed table bytes; mode 2 clears the output and applies the encoded class/subclass combination rules.
[[nodiscard]]u32 function_2198(Machine&m,u32 out[6],u32 mode,u32 incoming_edx=0u)noexcept{
 const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x04ccu)))*0x248u;
 if(mode==0u){u32 any=0;const u32 cls=*dgroup_ptr(m,dgroup(actor+0x680cu));for(u32 i=0;i<6u;++i){const s32 v=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(0x06d8u+cls*6u+i)));out[i]=static_cast<u32>(v);if(v!=0)any=1u;}return any;}
 if(mode==1u){u32 any=0;const u32 cls=*dgroup_ptr(m,dgroup(actor+0x680cu));const s32 sub=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(actor+0x680du)));const s32 off=static_cast<s32>(cls*36u)+sub*6;for(u32 i=0;i<6u;++i){const s32 v=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(static_cast<u32>(0x06fcu+off+static_cast<s32>(i)))));out[i]=static_cast<u32>(v);if(v!=0)any=1u;}return any;}
 if(mode!=2u)return incoming_edx;for(u32&i:*reinterpret_cast<u32(*)[6]>(out))i=0u;const u8 c=*dgroup_ptr(m,dgroup(actor+0x680cu)),d=*dgroup_ptr(m,dgroup(actor+0x680du)),e=*dgroup_ptr(m,dgroup(actor+0x680eu));
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

'''
text=text.replace(marker,block+marker)
cpp.write_text(text)

progress=Path('Code1/PROGRESS.md')
p=progress.read_text()
p=p.replace('verified%20conversion-85.04%25-red','verified%20conversion-90.56%25-red')
p=p.replace('**Verified conversion:** 85.04%','**Verified conversion:** 90.56%').replace('**Remaining:** 14.96%','**Remaining:** 9.44%')
p=p.replace('Current verified translated bytes: **59,869**.','Current verified translated bytes: **63,755**.')
p=p.replace('Raw coverage: **85.0388%**, displayed as **85.04%**.','Raw coverage: **90.5585%**, displayed as **90.56%**.')
anchor='- Batch 14 early text/presentation plus palette/resource continuation: `00EC..035B`, `10A4..1461`, selected `BE70..C79E`, and `CBD4..CBEE`, with alignment/data and self-modifying `C79F..CBD0` excluded: **3,921 bytes**.\n'
assert anchor in p
p=p.replace(anchor,anchor+'- Batch 15 actor animation/choice and planar VGA helpers: `15B0..1A00`, `2198..24D0`, `266C..28E1`, selected `A21C..A554`, `A6F0..A8F3`, with documented alignment excluded: **3,886 bytes**.\n')
# Repair the old stale roll-up if still present.
p=p.replace('Total integrated verified translated bytes: **39,995**.','Total integrated verified translated bytes: **63,755**.')
progress.write_text(p)

ledger=Path('Code1/STAGING_LEDGER.md')
l=ledger.read_text()
# Remove the stale D280/D2AD active row: those ranges were integrated long ago.
old='| `DosCrtNext.staging.cpp` | `D280..D312` | 147 | `D280..D312` performs DOS DTA get/set and 0x2B-byte copies whose DS/ES direction must be represented exactly. `D332..D4F1` is the open/create/truncate descriptor-state machine; its DOS carry/error encoding and `10397/103C3/10415` interactions are mapped but the placeholder staged C++ is not integration quality. `D313..D331`, `D4F2..D55D`, `D55E..D572`, and `D573..D5AC` are already integrated and must not be counted here. |\n'
if old in l:l=l.replace(old,'')
l+='\n## Batch 15 integration\nIntegrated **3,886 verified bytes** from `15B0..1A00`, `2198..24D0`, `266C..28E1`, functional `A21C..A554`, and `A6F0..A8F3`. Alignment `1A01..1A03`, `24D1..24D3`, `28E2..28E3`, `A2AE..A2AF`, `A359..A35B`, `A36A..A36B`, `A38D..A38F`, `A3E5..A3E7`, `A473`, `A4E7`, `A555..A557` is excluded. The self-modifying subtract-byte behavior of `A7E8` is represented as an explicit per-call byte because both encoded immediates are overwritten on every entry. Active staging remains only `CE8B..CEBF` (53 bytes).\n'
ledger.write_text(l)

cont=Path('Code1/DECOMPILATION_CONTINUITY.md')
c=cont.read_text()
c+='\n## Batch 15 checkpoint\nVerified coverage is **63,755 / 70,402 = 90.56%**. Batch 15 integrated `15B0..1A00`, `2198..24D0`, `266C..28E1`, functional `A21C..A554`, and `A6F0..A8F3` for **3,886 bytes** after direct CODE.1 disassembly. Preserve these specific facts: `2198` invalid modes return the incoming EDX value; `15B0` byte-toggles 7138 but pushes the full dword at 7138 to 53E4/5454; `A21C/A2B0` select copy direction from signed CX/DX while negating the full 32-bit register; `A7E8` patches two subtraction immediates on every entry, so its C++ delta is per-call rather than persistent hidden state. `07E4..10A1`, `C79F..CBD0`, interrupt-handler islands, `CE8B..CEBF`, and `DE6F..DEA0` remain high-priority unresolved/accounting candidates.\n'
cont.write_text(c)
print('integrated batch15')
