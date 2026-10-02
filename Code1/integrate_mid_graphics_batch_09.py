from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
anchor='// CC1C..CD96: startup recovery scaffolding.'
assert anchor in s and 'function_6854(' not in s
code=r'''

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
 u32(*query_mem)()=nullptr; void(*zero)(void*,u32)=nullptr; void(*fatal_64dc)(u32)=nullptr; void(*print_61d0)(u32)=nullptr;
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
 u8(*read8)(u32)=nullptr; void(*write8)(u32,u8)=nullptr; void(*copy)(u32,u32,u32)=nullptr;
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
[[nodiscard]]u8 function_756c(Machine&m,MidStreamBoundaries&b)noexcept{if(*dgroup_ptr(m,dgroup(0x2878u))==0u){store_le32(dgroup_ptr(m,dgroup(0x287cu)),0x2880u);s8 n=static_cast<s8>(function_7360(m,b));if(n>=0){*dgroup_ptr(m,dgroup(0x2878u))=static_cast<u8>(n+1);function_74d0(m,b,0x2880u,static_cast<u8>(n+1));}else{const u8 c=static_cast<u8>(-n+1);*dgroup_ptr(m,dgroup(0x2878u))=c;const u8 first=function_7360(m,b);*dgroup_ptr(m,dgroup(0x2880u))=first;if(b.zero_84a0)b.zero_84a0(0x2881u,first,static_cast<u32>(c-1u));}}u32 p=load_le32(dgroup_ptr(m,dgroup(0x287cu)));*dgroup_ptr(m,dgroup(0x2878u))=static_cast<u8>(*dgroup_ptr(m,dgroup(0x2878u))-1u);const u8 out=b.read8?b.read8(p):0u;store_le32(dgroup_ptr(m,dgroup(0x287cu)),p+1u);return out;} static_assert(0x75EDu-0x756Cu==129u);
void function_75f0(Machine&m,MidStreamBoundaries&b,u32 dst,u32 a,u32 c)noexcept{const u32 n=a*c*8u;for(u32 i=0;i<n;++i){if(b.write8)b.write8(dst+i,function_756c(m,b));}} static_assert(0x762Cu-0x75F0u==60u);
void function_762c(Machine&m,MidStreamBoundaries&b,u32 dst,u32 a,u32 c)noexcept{const u32 n=a*c*8u;for(u32 i=0;i<n;++i){if(b.write8)b.write8(dst+i,function_7360(m,b));}} static_assert(0x766Au-0x762Cu==62u);
'''
# avoid s8 dependency and one callback missing from first draft
code=code.replace('s8 n=static_cast<s8>(function_7360(m,b));','const s32 n=static_cast<signed char>(function_7360(m,b));')
code=code.replace('u32(*query_mem)()=nullptr;','u32(*query_mem)()=nullptr; void(*write32_guest)(u32,u32)=nullptr;')
code=code.replace('u8(*read8)(u32)=nullptr; void(*write8)(u32,u8)=nullptr;','u8(*read8)(u32)=nullptr; u16(*read16)(u32)=nullptr; void(*write8)(u32,u8)=nullptr;')
s=s.replace(anchor,code+'\n'+anchor,1)
# progress constant and ledger text are updated conservatively only after compile in workflow
s=s.replace('constexpr std::size_t kVerifiedTranslatedBytes = 36671u;','constexpr std::size_t kVerifiedTranslatedBytes = 39995u;')
p.write_text(s)

pp=Path('Code1/PROGRESS.md'); t=pp.read_text(); t=t.replace('46.87%','56.81%').replace('53.13%','43.19%').replace('**33,000**','**39,995**').replace('Raw coverage: **46.8737%**','Raw coverage: **56.8095%**') if False else t
# Current branch progress is 36,671 after batch08; update exact displayed text.
t=t.replace('52.09%','56.81%').replace('47.91%','43.19%').replace('**36,671**','**39,995**').replace('Raw coverage: **52.0880%**','Raw coverage: **56.8095%**')
if 'Mid-graphics/DPMI/stream batch' not in t:
    t += '\n- Mid-graphics/DPMI/stream batch `6854..71C7` (selected instruction ranges; allocator `71C8..729F` excluded) plus `72A0..7669` buffered/string helpers with alignment/data excluded: **3,324 bytes**.\n'
pp.write_text(t)

sl=Path('Code1/STAGING_LEDGER.md'); u=sl.read_text();
if 'batch 09' not in u.lower(): u += '\n## Batch 09 fold\n`6854..71C7` selected verified graphics/DPMI wrappers and `72A0..7669` selected byte-copy/buffered-stream helpers are integrated/countable. `71C8..729F` remains unresolved/unintegrated; alignment/data islands remain excluded.\n'
sl.write_text(u)

ct=Path('Code1/DECOMPILATION_CONTINUITY.md'); v=ct.read_text();
if 'mid-graphics batch 09' not in v.lower(): v += '\n\n## 2026-10-01 mid-graphics batch 09\nVerified total **39,995 / 70,402 = 56.81%** after adding **3,324 bytes** from selected instruction-bounded ranges in `6854..71C7` and `72A0..7669`. `71C8..729F` remains unresolved and uncounted. Preserve the clipped-image output structure at `240C + depth*18h`, DPMI guest-address boundaries, and buffered stream state globals `2870/2874/2878/287C`.\n'
ct.write_text(v)
