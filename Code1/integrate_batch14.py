from pathlib import Path

cpp=Path('Code1/Code1.cpp')
text=cpp.read_text()
assert 'void function_00ec(' not in text
assert 'void function_10a4(' not in text
assert 'void function_be70(' not in text
anchor='// Batch 13: low-level image transforms, VGA timing/address helpers, and audio/resource state machinery.\n'
assert anchor in text

block=r'''
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
void function_c4a0(Machine&m,LowContinuation14Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))==0u)return;function_c57c(m,b);if(b.call_ea9a)b.call_ea9a(load_le32(dgroup_ptr(m,dgroup(0x2f74u)));if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2f70u))));store_le32(dgroup_ptr(m,dgroup(0x2f48u)),0u);}static_assert(0xC4E0u-0xC4A0u==64u);
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

'''
text=text.replace(anchor,block+anchor)
cpp.write_text(text)

progress=Path('Code1/PROGRESS.md')
p=progress.read_text()
p=p.replace('verified%20conversion-79.47%25-red','verified%20conversion-85.04%25-red')
p=p.replace('**Verified conversion:** 79.47%','**Verified conversion:** 85.04%')
p=p.replace('**Remaining:** 20.53%','**Remaining:** 14.96%')
p=p.replace('Current verified translated bytes: **55,948**.','Current verified translated bytes: **59,869**.')
p=p.replace('Raw coverage: **79.4693%**, displayed as **79.47%**.','Raw coverage: **85.0388%**, displayed as **85.04%**.')
needle='- Batch 13 low graphics/resource/audio state routines selected from `AE0C..BE60`, excluding eleven zero bytes and documented alignment islands: **4,129 bytes**.\n'
assert needle in p
p=p.replace(needle,needle+'- Batch 14 early text/presentation plus palette/resource continuation: `00EC..035B`, `10A4..1461`, selected `BE70..C79E`, and `CBD4..CBEE`, with alignment/data and self-modifying `C79F..CBD0` excluded: **3,921 bytes**.\n')
progress.write_text(p)

ledger=Path('Code1/STAGING_LEDGER.md')
l=ledger.read_text()
l+='''\n\n## Batch 14 integration checkpoint\nIntegrated **3,921 verified instruction bytes** from `00EC..035B`, `10A4..1461`, selected `BE70..C79E`, and `CBD4..CBEE`. Alignment/data islands are excluded. `C79F..CBD0` is explicitly retained on the unresolved queue because it is position-dependent self-modifying VGA code; it is not staging and is not counted. No previously staged startup/interrupt bytes are double-counted.\n'''
ledger.write_text(l)

continuity=Path('Code1/DECOMPILATION_CONTINUITY.md')
c=continuity.read_text()
c+='''\n\n## Batch 14 checkpoint\n- Verified total after this fold: **59,869 / 70,402 = 85.0388% (85.04%)**.\n- Newly integrated: **3,921 bytes**: `00EC..035B` (624), `10A4..1461` (958), selected `BE70..C79E` plus `CBD4..CBEE` (2,339).\n- `C79F..CBD0` is a self-modifying VGA blitter cluster: encoded immediates are patched through stores into instruction bytes. Do **not** lift it as ordinary immutable C++ control flow. Re-disassemble and model its patch points explicitly before counting.\n- Batch 14 preserves guest pointers through callback boundaries; the `C5B0` four-byte stack-local read remains an explicit `call_6304_local32` boundary rather than an invented guest address.\n- Next high-output candidates: the unresolved `C79F..CBD0` cluster only after a self-modifying-code model, plus remaining early-game gaps (`07E2..10A1`, `15B0..1A03`, `2197..24D3`, `266C..28E3`) that can be harvested independently.\n'''
continuity.write_text(c)
print('integrated batch14')
