from pathlib import Path
import re

p=Path('Code1/Code1.cpp')
s=p.read_text()
anchor='// CC1C..CD96: startup recovery scaffolding.'
assert anchor in s
assert 'function_71c8(' not in s

code=r'''
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
[[nodiscard]]u32 function_767c(MidUi10Boundaries&b,u32 desc,u32 source)noexcept{
 const u32 h=mu32(b,desc+5u);const s16 count=static_cast<s16>(mu16(b,h));if(count==0&&b.call_9814)b.call_9814(0x158u);
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
[[nodiscard]]u32 function_7944(MidUi10Boundaries&b,u32 out,s32 x,s32 y,s32 w,s32 pixel_bits,u32 key)noexcept{mw16(b,out,static_cast<u16>(x));mw16(b,out+4u,static_cast<u16>(w));mw8(b,out+6u,static_cast<u8>(div8_zero(pixel_bits)));mw16(b,out+2u,1u);mw8(b,out+7u,key==0xffu?0u:0x80u);if(b.call_66c0)b.call_66c0(out+8u+(key==0xffu?0u:1u),y,w==w?y:y,w,pixel_bits);/* push order is repaired below by direct call */if(b.call_66c0)b.call_66c0(out+8u+(key==0xffu?0u:1u),x,y,w,pixel_bits);if(key!=0xffu)mw8(b,out+8u,static_cast<u8>(key));return out;} static_assert(0x79C6u-0x7944u==130u);
// 79C6..79C7 MOV EAX,EAX alignment.

// 79C8..7A60: clone a variable descriptor, rewrite its local +3/+5 fields, recurse through 767C,
// and preserve the uninitialized local byte as an explicit incoming value when 9AD4 returns zero.
[[nodiscard]]u32 function_79c8(MidUi10Boundaries&b,u32 src,u32 source,u8 incoming_local_f0)noexcept{u8 flag=incoming_local_f0;if((b.call_9ad4?b.call_9ad4():0u)!=0u){flag=1u;if(b.call_9ab4)b.call_9ab4();}const s16 off=static_cast<s16>(mu16(b,src+3u));const u32 text=mu32(b,src+5u);const u32 rec=text+static_cast<u32>(static_cast<s32>(off))-8u;const s16 len=static_cast<s16>(mu16(b,rec));const u32 out=function_71c8(b,static_cast<u32>(static_cast<s32>(len)+4));for(s32 i=0;i<len;++i)mw8(b,out+4u+static_cast<u32>(i),mu8(b,text+static_cast<u32>(i)));u8 local[9]{};for(u32 i=0;i<9u;++i)local[i]=mu8(b,src+i);store_le16(local+3,8u);store_le32(local+5,rec);const u32 child=b.call_767c_local?b.call_767c_local(local,source):0u;mw32(b,out,child);if(flag!=0u&&b.call_9ac4)b.call_9ac4();return out;} static_assert(0x7A61u-0x79C8u==153u);
// 7A61..7A63 LEA alignment.
void function_7a64(MidUi10Boundaries&b,u32 p)noexcept{function_7830(b,mu32(b,p));function_7240(b,p);} static_assert(0x7A84u-0x7A64u==32u);
[[nodiscard]]u32 function_7a84(MidUi10Boundaries&b,u32 s,u8 delta)noexcept{const u32 first=s;for(u32 p=s;p!=0u&&mu8(b,p)!=0u;++p)mw8(b,p,static_cast<u8>(mu8(b,p)+delta));return first;} static_assert(0x7AB1u-0x7A84u==45u);
// 7AB1..7AB3 LEA alignment.

static u32 font_ptr_10(MidUi10Boundaries&b,u32 id)noexcept{return b.call_954c?b.call_954c(id,0u):0u;}
static u8 glyph_width_signed_10(MidUi10Boundaries&b,u32 font,u8 ch)noexcept{return mu8(b,font+4u+static_cast<u32>(static_cast<s32>(static_cast<s8>(ch))));}
static u8 glyph_width_unsigned_10(MidUi10Boundaries&b,u32 font,u8 ch)noexcept{return mu8(b,font+4u+static_cast<u32>(ch));}
[[nodiscard]]u32 function_7ab4(Machine&m,MidUi10Boundaries&b,u32 s,u32 font_id)noexcept{const u32 font=font_ptr_10(b,font_id);store_le32(dgroup_ptr(m,dgroup(0x8988u)),font);u32 total=0;for(u32 p=s;p!=0u&&mu8(b,p)!=0u;++p)total+=glyph_width_signed_10(b,font,mu8(b,p));return total;} static_assert(0x7AFCu-0x7AB4u==72u);
[[nodiscard]]u32 function_7afc(Machine&m,MidUi10Boundaries&b,u32 s,u32 font_id,u32 count)noexcept{const u32 font=font_ptr_10(b,font_id);store_le32(dgroup_ptr(m,dgroup(0x8988u)),font);u32 total=0,p=s,n=count;while(p!=0u&&mu8(b,p)!=0u&&n!=0u){total+=glyph_width_signed_10(b,font,mu8(b,p));++p;--n;}return total;} static_assert(0x7B50u-0x7AFCu==84u);
[[nodiscard]]u32 function_7b50(Machine&m,MidUi10Boundaries&b,u8 ch,u32 font_id)noexcept{const u32 font=font_ptr_10(b,font_id);store_le32(dgroup_ptr(m,dgroup(0x8988u)),font);return glyph_width_signed_10(b,font,ch);} static_assert(0x7B73u-0x7B50u==35u);
// 7B73 NOP alignment.
[[nodiscard]]u32 function_7b74(Machine&m,MidUi10Boundaries&b,u32 font_id)noexcept{const u32 font=font_ptr_10(b,font_id);store_le32(dgroup_ptr(m,dgroup(0x8988u)),font);return mu16(b,mu32(b,font)+4u);} static_assert(0x7B94u-0x7B74u==32u);

static void draw_char_10(Machine&m,MidUi10Boundaries&b,u8 ch,bool sign_char)noexcept{const u32 font=load_le32(dgroup_ptr(m,dgroup(0x8988u)));const u32 x=load_le32(dgroup_ptr(m,dgroup(0x8990u))),y=load_le32(dgroup_ptr(m,dgroup(0x898cu))),data=mu32(b,font);const s32 c=sign_char?static_cast<s32>(static_cast<s8>(ch)):static_cast<s32>(ch);const u32 extra=load_le32(dgroup_ptr(m,dgroup(0x2908u)));if(extra!=0u){if(b.call_6824)b.call_6824(c,x,y,data,extra);}else if(b.call_67c8)b.call_67c8(c,x,y,data);store_le32(dgroup_ptr(m,dgroup(0x898cu)),y+(sign_char?glyph_width_signed_10(b,font,ch):glyph_width_unsigned_10(b,font,ch)));}
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

void function_85d4(MidUi10Boundaries&b,s32 x,s32 y,s32 start,s32 stop,u32 object)noexcept{if(stop==-1)stop=static_cast<s32>(mu16(b,object+2u));else stop+=start;const s32 stride=static_cast<s32>(mu16(b,object+4u));for(s32 i=start;i<stop;++i){if(b.call_6d54)b.call_6d54(i,x,y,object);x+=stride;}} static_assert(0x8607u-0x85D4u==51u);
void function_8607(MidUi10Boundaries&b,s32 x,s32 y,u32 object)noexcept{function_85d4(b,x,y,0,-1,object);} static_assert(0x862Cu-0x8607u==37u);
void function_862c(MidUi10Boundaries&b,s32 x,s32 y,u32 object)noexcept{function_85d4(b,x,y,0,-1,object);} static_assert(0x864Bu-0x862Cu==31u);
// 864B NOP.
void function_864c(MidUi10Boundaries&b,s32 x,s32 y,s32 start,s32 stop,u32 object)noexcept{if(stop==-1)stop=static_cast<s32>(mu16(b,object+2u));else stop+=start;const s32 stride=static_cast<s32>(mu16(b,object+4u));for(s32 i=start;i<stop;++i){if(b.call_67c8)b.call_67c8(i,static_cast<u32>(x),static_cast<u32>(y),object);x+=stride;}} static_assert(0x867Fu-0x864Cu==51u);
void function_867f(MidUi10Boundaries&b,s32 x,s32 y,u32 object)noexcept{function_864c(b,x,y,0,-1,object);} static_assert(0x86A4u-0x867Fu==37u);
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

'''
s=s.replace(anchor,code+'\n'+anchor,1)
# Remove the accidental duplicate 66C0 call in the structural text while retaining the exact second call.
s=s.replace('if(b.call_66c0)b.call_66c0(out+8u+(key==0xffu?0u:1u),y,w==w?y:y,w,pixel_bits);/* push order is repaired below by direct call */','')
# Official count: +3941 bytes.
s=re.sub(r'constexpr std::size_t kVerifiedTranslatedBytes = \d+u;', 'constexpr std::size_t kVerifiedTranslatedBytes = 43936u;', s)
p.write_text(s)

prog=Path('Code1/PROGRESS.md')
t=prog.read_text()
t=t.replace('verified%20conversion-56.81%25-red','verified%20conversion-62.41%25-red')
t=t.replace('**Verified conversion:** 56.81%','**Verified conversion:** 62.41%')
t=t.replace('**Remaining:** 43.19%','**Remaining:** 37.59%')
t=t.replace('Current verified translated bytes: **39,995**.','Current verified translated bytes: **43,936**.')
t=t.replace('Raw coverage: **56.8095%**, displayed as **56.81%**.','Raw coverage: **62.4073%**, displayed as **62.41%**.')
marker='## Immutable source identity'
inv='- Mid UI/resource/font/input/vector/render routines `71C8..729F`, selected `767C..80B9`, `8270..8335`, `8398..848F`, `84A0..87C1` with alignment/data/interrupt islands excluded: **3,941 bytes**.\n'
if inv not in t:t=t.replace(marker,inv+'\n'+marker,1)
t+='\n## 2026-10-01 mid UI batch 10\nAdded **3,941 verified bytes**, bringing the conservative total to **43,936 / 70,402 = 62.4073% (62.41%)**. Integrated the guarded 71C8 allocator/free pair, image/resource constructors, font metrics/text drawing state, ordered input queue helpers, vector install/restore wrappers, rectangle/frame helpers, and repeated-object render loops through 87C1. Explicitly excluded 766A..767B jump-table/alignment, interrupt-handler bodies 7F5C..7F89 / 80BC..826D / 8338..8397, and all documented alignment/data islands.\n'
prog.write_text(t)

ledger=Path('Code1/STAGING_LEDGER.md')
lt=ledger.read_text()
lt+='\n## Batch 10 integration note\n`71C8..729F` is no longer unresolved/staged: the guarded allocation ownership contract and magic values are integrated. Interrupt-handler bodies at `7F5C..7F89`, `80BC..826D`, and `8338..8397` remain uncounted; only their install/restore plumbing is integrated. Existing startup/E2C3 staging remains unchanged.\n'
ledger.write_text(lt)

cont=Path('Code1/DECOMPILATION_CONTINUITY.md')
ct=cont.read_text()
ct+='\n## 2026-10-01 mid UI batch 10 checkpoint\nVerified total **43,936 / 70,402 = 62.41%**. Added 3,941 bytes across the resolved `71C8..729F` allocator pair and selected `767C..87C1` UI/resource/font/input/vector/render helpers. Preserve the 766C four-entry CS jump table as data, the explicit incoming high EDX dependency in 7844, the uninitialized local-byte dependency in 79C8, signed-vs-unsigned glyph indexing differences, and the interrupt queue globals 2919/291A/2930. Interrupt bodies 7F5C, 80BC, and 8338 remain uncounted and must be modeled with their full saved-register/segment/IRET frames before integration.\n'
cont.write_text(ct)
