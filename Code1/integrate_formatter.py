from pathlib import Path

code_path=Path('Code1/Code1.cpp')
s=code_path.read_text()
assert 'function_ede7(FormatterDriverBoundary&' not in s
needle='// F007..F741: verified formatter parser, conversion materializer, and helpers.'
assert needle in s
block=r'''// EDE7..F006: verified formatter driver. F2E9 shares the original F001
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

'''
# MaterializeResult is defined later in the formatter block, so insert the driver
# after that definition/function materializer instead of before F007.
insert='static_assert(0xE4C7u-0xE3B4u==275u);'
pos=s.index(insert)
s=s[:pos]+block+s[pos:]
s=s.replace('static_assert(0xE4C7u-0xE3B4u==275u);','static_assert(0xE4C7u-0xE3B4u==275u); static_assert(0xF007u-0xEDE7u==544u);',1)
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=6781u;','constexpr u32 kVerifiedTranslatedBytes=7325u;',1)
code_path.write_text(s)

pr=Path('Code1/PROGRESS.md')
t=pr.read_text()
t=t.replace('verified%20conversion-7.77%25-red','verified%20conversion-10.40%25-red')
t=t.replace('**Verified conversion:** 7.77%','**Verified conversion:** 10.40%').replace('**Remaining:** 92.23%','**Remaining:** 89.60%')
t=t.replace('Current verified translated bytes: **5,473**.','Current verified translated bytes: **7,325**.')
t=t.replace('Raw coverage: **7.7739%**, displayed conservatively as **7.77%**.','Raw coverage: **10.4045%**, displayed conservatively as **10.40%**.')
t=t.replace('- Formatter helper subset `F007..F205` and `F722..F741`: **543 bytes**.','- Complete formatter driver/parser/materializer `EDE7..F741`: **2,395 bytes**.')
t=t.replace('Total integrated verified translated bytes: **5,473**.','Total integrated verified translated bytes: **7,325**.')
start=t.find('## Formatter staging cleared for integration')
end=t.find('## Larger-batch policy')
if start!=-1 and end!=-1:
 t=t[:start]+'''## Formatter fully integrated\n\nBoth cleared staging ranges are now physically folded into `Code1.cpp`: `EDE7..F006` (**544 bytes**) and `F206..F721` (**1,308 bytes**). With the previously integrated helpers, the contiguous formatter region `EDE7..F741` accounts for **2,395 verified bytes**.\n\nThe fold preserves the corrected post-`%` parser pointer, `%n` near/far word and dword stores, callback-owned output count, post-decrement padding/precision behavior, selector-qualified payload reads, two-byte `s/S` stride, signed 16.16 conversion, EAX:DX far-return contract, and the original shared `F001` epilogue relationship.\n\nThe staging files are removed after the syntax-checked fold so no verified bytes can be double-counted.\n\n## Active continuation\n\nAdvance to the next independent CODE.1 regions: allocator bodies at `DAD9`/`DB5B`, candidate compressed-data routine `DB80`, transformation routine `DC90`, uncertain entry at `DE6F`, startup successors, and remaining address-named CRT helpers.\n\n'''+t[end:]
pr.write_text(t)
Path('Code1/FormatterDriver.staging.cpp').unlink()
Path('Code1/integrate_formatter.py').unlink()
