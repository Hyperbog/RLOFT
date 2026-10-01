from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
if 'function_dad9(AllocBoundary&b' not in s:
    marker='void function_dde0()noexcept{}'
    block=r'''// DAD9..DB7F: verified allocator/retry/free runtime block restored after an integration regression.
struct AllocBoundary{
 u32(*call_104e0)(u32 request,u16 ds,u32 arena)=nullptr;
 u32(*call_10aa9)(u32 request)=nullptr;
 u32(*call_10c46)(u32 request)=nullptr;
 void(*call_10590)(u32 pointer,u16 ds,u32 arena)=nullptr;
 void(*write_linear8)(u32,u8)=nullptr;
 u16 ds=0;
};
[[nodiscard]]u32 function_dad9(AllocBoundary&b,u32 request)noexcept{
 if(request==0)return 0;bool retried_10aa9=false;for(;;){const u32 result=b.call_104e0?b.call_104e0(request,b.ds,0x30b8u):0;if(result!=0){if(b.write_linear8)b.write_linear8(0x8e78u,0);return result;}if(!retried_10aa9){if(b.call_10aa9&&b.call_10aa9(request)!=0){retried_10aa9=true;continue;}}if(!b.call_10c46||b.call_10c46(request)==0){if(b.write_linear8)b.write_linear8(0x8e78u,0);return 0;}}}
void function_db5b(AllocBoundary&b,u32 pointer)noexcept{if(b.call_10590)b.call_10590(pointer,b.ds,0x30b8u);if(b.write_linear8)b.write_linear8(0x8e78u,0);}
void function_db4b(AllocBoundary&b,u32 pointer)noexcept{function_db5b(b,pointer);}

// DB80..DC8C: exact 4096-byte ring dictionary walk.
struct DecodeBoundary{u8(*input_7360)()=nullptr;void(*output_738c)(u8)=nullptr;u8(*read_linear8)(u32)=nullptr;};
void function_db80(DecodeBoundary&b)noexcept{u8 dict[4096];std::memset(dict,0xfe,sizeof(dict));u16 write_index=0x0feeu;u16 si=0;for(;;){si=static_cast<u16>(si>>1);if((si&0x0100u)==0){if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 v=b.input_7360?b.input_7360():0;si=static_cast<u16>(0xff00u|v);}if(si&1u){if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 v=b.input_7360?b.input_7360():0;if(b.output_738c)b.output_738c(v);dict[write_index]=v;write_index=static_cast<u16>((write_index+1u)&0x0fffu);}else{if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 first=b.input_7360?b.input_7360():0;if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 second=b.input_7360?b.input_7360():0;const u16 source=static_cast<u16>((static_cast<u16>(second&0xf0u)<<4)|first);const u16 last=static_cast<u16>((second&0x0fu)+2u);for(u16 i=0;i<=last;++i){const u8 v=dict[(source+i)&0x0fffu];if(b.output_738c)b.output_738c(v);dict[write_index]=v;write_index=static_cast<u16>((write_index+1u)&0x0fffu);}}}}

// DC90..DDD7: verified permutation/transform routine; REPNE SCASW semantics retained.
struct TransformBoundary{u16(*read16)(u32)=nullptr;u8(*read8)(u32)=nullptr;void(*write16)(u32,u16)=nullptr;void(*write8)(u32,u8)=nullptr;};
static u16 tr16(TransformBoundary&b,u32 a)noexcept{return b.read16?b.read16(a):0;}static u8 tr8(TransformBoundary&b,u32 a)noexcept{return b.read8?b.read8(a):0;}static void tw16(TransformBoundary&b,u32 a,u16 v)noexcept{if(b.write16)b.write16(a,v);}static void tw8(TransformBoundary&b,u32 a,u8 v)noexcept{if(b.write8)b.write8(a,v);}
void function_dc90(TransformBoundary&b,u32 base)noexcept{if(tr16(b,base+4)!=1)return;u32 dst=base+8u;u32 source_base=base+8u;if(tr8(b,base+7u)&0x80u){++dst;++source_base;}const u32 row=tr16(b,base);const u32 stride=static_cast<u32>(tr8(b,base+6u))<<1;u32 remaining=4u*tr16(b,base+2u);const u32 table_words=remaining;const u32 wrap=remaining<<1;u32 bx=0,eax=0;for(u32 ecx=table_words;ecx!=0;--ecx){tw16(b,0x30e8u+bx,static_cast<u16>(eax));bx+=8u;++eax;if(bx>=wrap){bx-=wrap;bx+=2u;}}--remaining;dst+=stride;u32 index=1;while(remaining!=0){const u32 table_entry=0x30e8u+(index<<1);const u16 saved_table_value=tr16(b,table_entry);u32 scan=remaining;u32 after=table_entry;if(scan!=0){do{const u16 examined=tr16(b,after);after+=2u;--scan;if(examined==static_cast<u16>(index))break;}while(scan!=0);}const u32 match=after-2u;const u16 displaced=tr16(b,match);tw16(b,match,saved_table_value);const u32 mapped=(match-0x30e8u)>>1;tw16(b,table_entry,displaced);u32 a=source_base+(index>>2)*row+(index&3u)*stride;u32 q=source_base+(mapped>>2)*row+(mapped&3u)*stride;const u32 saved=a;for(u32 n=stride;n!=0;--n){const u8 av=tr8(b,a),sv=tr8(b,q);tw8(b,a,sv);tw8(b,q,av);++a;++q;}for(u32 n=0;n<stride;++n)tw8(b,dst+n,tr8(b,saved+n));++index;dst+=stride;--remaining;}const u16 old_count=tr16(b,base+2u);tw16(b,base+4u,old_count);tw16(b,base+2u,1);const u16 product=static_cast<u16>(static_cast<u16>(old_count)*static_cast<u16>(static_cast<u16>(tr8(b,base+6u))<<3));tw16(b,base,product);}

static_assert(0xDB80u-0xDAD9u==167u);static_assert(0xDC8Du-0xDB80u==269u);static_assert(0xDDD8u-0xDC90u==328u);
// DAC9..DAD8: exact frame wrapper around DAD9.
[[nodiscard]]u32 function_dac9(AllocBoundary&b,u32 request)noexcept{return function_dad9(b,request);}

'''
    if marker not in s: raise SystemExit('DDE0 marker missing')
    s=s.replace(marker,block+marker,1)
elif 'function_dac9(AllocBoundary&b' not in s:
    marker='void function_dde0()noexcept{}'
    s=s.replace(marker,'// DAC9..DAD8: exact frame wrapper around DAD9.\n[[nodiscard]]u32 function_dac9(AllocBoundary&b,u32 request)noexcept{return function_dad9(b,request);}\n'+marker,1)
p.write_text(s)

q=Path('Code1/PROGRESS.md')
t=q.read_text()
t=t.replace('verified%20conversion-11.69%25-red','verified%20conversion-11.71%25-red').replace('**Verified conversion:** 11.69%','**Verified conversion:** 11.71%').replace('**Remaining:** 88.31%','**Remaining:** 88.29%').replace('Current verified translated bytes: **8,234**.','Current verified translated bytes: **8,250**.').replace('Raw coverage: **11.6957%**, displayed conservatively as **11.69%**.','Raw coverage: **11.7184%**, displayed conservatively as **11.71%**.').replace('Total integrated verified translated bytes: **8,234**.','Total integrated verified translated bytes: **8,250**.')
t=t.replace('- Runtime allocator/decoder/transform `DAD9..DDD7`: **764 bytes**.','- Runtime allocator/decoder/transform `DAD9..DDD7`: **764 bytes**.\n- Allocator wrapper `DAC9..DAD8`: **16 bytes**.')
t=t.replace('`DosMemoryNext.staging.cpp` contains **186 functional instruction-bounded bytes** from `DA1C..DAD8`; `DA45..DA47` is three zero bytes and excluded. AH=48/AH=4A failure outputs use DOS-returned BX, and `10384` consumes live AX+CF.','`DosMemoryNext.staging.cpp` originally covered 186 functional instruction-bounded bytes from `DA1C..DAD8`; `DA45..DA47` is three zero bytes and excluded. `DA48..DAA6` was already integrated, and `DAC9..DAD8` is now physically integrated. The remaining unique staged bridge is `DA1C..DA44` (**41 bytes**), whose call to `104B1` is now known to capture ES/CS/SS/DS/FS/GS selector words rather than clear a 12-byte cell.')
t=t.replace('unique uncounted staged code is **2,406 bytes** (747 startup + 57 genuinely missing DA1C/DAC9 bridge bytes + 880 D23A..D5AC + 722 E0D0..E3B2).','unique uncounted staged code is **2,390 bytes** (747 startup + 41 remaining DA1C bridge bytes + 880 D23A..D5AC + 722 E0D0..E3B2).')
t += '\n## Integration regression audit\n\nA post-integration audit found that the previously verified `DAD9..DDD7` block had been accidentally dropped from the physical `Code1.cpp` by a later one-shot integration based on stale content. The exact previously verified translation from commit `fa12d5fee0a53334682c9a6b6033c8a7e0be447c` has been restored before adding new coverage. This restoration does **not** increase the percentage because those 764 bytes were already counted. `DAC9..DAD8` adds 16 newly integrated verified bytes. Future integration scripts must modify the checked-out branch head in place and assert preservation of existing verified region sentinels before committing.\n'
q.write_text(t)
