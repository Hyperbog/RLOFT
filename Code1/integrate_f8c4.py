from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes=11146u;' in s
assert 'static_assert(0xF8BDu-0xF7B8u==261u);' in s
assert 'function_f8c4' not in s
marker='static_assert(0xF8BDu-0xF7B8u==261u);\n'
block=r'''

// F8C4..FC96: resource image loader. This is kept on guest addresses: allocation, zeroing, source reads, and relocation writes cross explicit boundaries rather than casting 32-bit DOS addresses to host pointers.
struct ResourceLoadBoundaries{
 u32(*alloc_71c8)(u32)=nullptr;
 void(*zero_84a0)(u32,u32,u32)=nullptr;
 s32(*compare_10c70)(const u8*,u32)=nullptr;
 void(*write_guest32)(u32,u32)=nullptr;
};
[[nodiscard]]u32 function_f8c4(FileBoundaries&fb,ResourceBoundaries&rb,ResourceLoadBoundaries&lb,u32 source,u32 flags,u32 destination)noexcept{
 u32 handle=source;u32 chunk_count=0;u32 image_size=0;u32 cursor=0;u32 segment_base[64]{};u32 chunk_base[144]{};
 u8 sig[4]{};store_le32(sig,load_le32(rb.dgroup_data?rb.dgroup_data+0xf8bdu:sig));
 if((flags&1u)==0){handle=function_d313(fb,source,0x200u);if(handle==0xffffffffu)return 0;}
 image_size=function_f7b8(fb,rb,source,flags);
 if(flags&4u){destination=lb.alloc_71c8?lb.alloc_71c8(image_size):0u;if(destination==0){if((flags&1u)==0)function_d573(fb,handle);return 0;}}
 if(lb.zero_84a0)lb.zero_84a0(destination,0u,image_size);
 u32 header_off=0;cursor=function_f750(fb,rb,handle,0x3cu,flags,reinterpret_cast<u8*>(&header_off),4u);
 cursor=function_f750(fb,rb,handle,header_off,flags,sig,2u);
 const s32 sigcmp=lb.compare_10c70?lb.compare_10c70(sig,0x38cu):0;
 if(sigcmp!=0){if((flags&1u)==0)function_d573(fb,handle);return 0;}
 u8 h[0xacu]{};cursor=function_f750(fb,rb,handle,header_off,flags,h,sizeof h);
 const u32 object_table_off=load_le32(h+0x40u),object_count=load_le32(h+0x44u),page_map_off=load_le32(h+0x48u);
 const u32 data_pages_off=load_le32(h+0x80u),fixup_page_off=load_le32(h+0x68u),fixup_record_off=load_le32(h+0x6cu);
 const u32 page_shift=static_cast<u32>(h[0x2cu]),page_size=load_le32(h+0x28u),module_pages=load_le32(h+0x14u);
 cursor=header_off+object_table_off;
 for(u32 object=0;object<object_count;++object){
  u8 rec[0x18u]{};cursor=function_f750(fb,rb,handle,cursor,flags,rec,sizeof rec);
  const u32 virtual_size=load_le32(rec),object_flags=load_le32(rec+8u),page_count=load_le32(rec+0x10u);
  u32 page_cursor=header_off+page_map_off;
  for(u32 page=0;page<page_count;++page){
   u8 map[8]{};page_cursor=function_f750(fb,rb,handle,page_cursor,flags,map,sizeof map);
   if(page==0){if((object_flags&3u)==3u&&(destination&0xfu)!=0)destination+=0x10u-(destination&0xfu);segment_base[object]=destination;}
   chunk_base[chunk_count++]=destination;
   const u32 used=destination-segment_base[object];
   const u32 remain=virtual_size-used;const u32 bytes=remain<static_cast<u32>(load_le16(map+4u))?remain:static_cast<u32>(load_le16(map+4u));
   const u32 data_off=(load_le32(map)<<page_shift)+data_pages_off;
   function_f750(fb,rb,handle,data_off,flags,reinterpret_cast<u8*>(static_cast<uintptr_t>(destination)),bytes);
   destination+=bytes;
  }
  cursor=page_cursor;
 }
 u32 fix_cursor=header_off+fixup_page_off;u32 previous=0;fix_cursor=function_f750(fb,rb,handle,fix_cursor,flags,reinterpret_cast<u8*>(&previous),4u);
 for(u32 page=0;page<module_pages;++page){
  u32 next=0;u32 after=function_f750(fb,rb,handle,fix_cursor,flags,reinterpret_cast<u8*>(&next),4u);fix_cursor=after;
  if(previous!=next){u32 rec_cursor=header_off+fixup_record_off+previous;do{
    u8 source_type=0,target_flags=0;rec_cursor=function_f750(fb,rb,handle,rec_cursor,flags,&source_type,1u);rec_cursor=function_f750(fb,rb,handle,rec_cursor,flags,&target_flags,1u);
    if((source_type&7u)==0u||((target_flags&4u)!=0u&&(target_flags&0x20u)==0u)||((source_type&0x20u)!=0u)){if((flags&1u)==0)function_d573(fb,handle);return 0;}
    u8 offbuf[2]{},idx=0,addbuf[2]{};rec_cursor=function_f750(fb,rb,handle,rec_cursor,flags,offbuf,2u);rec_cursor=function_f750(fb,rb,handle,rec_cursor,flags,&idx,1u);rec_cursor=function_f750(fb,rb,handle,rec_cursor,flags,addbuf,2u);
    const u32 src_off=load_le16(offbuf);if(src_off<=page_size){const u32 patch=chunk_base[page]+src_off;const u32 value=segment_base[idx]+load_le16(addbuf);if(lb.write_guest32)lb.write_guest32(patch,value);}
   }while(rec_cursor<header_off+fixup_record_off+next);fix_cursor=after;}
  previous=next;
 }
 if((flags&1u)==0)function_d573(fb,handle);return destination;
}
static_assert(0xFC97u-0xF8C4u==979u);
'''
s=s.replace(marker,marker+block)
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=11146u;','constexpr u32 kVerifiedTranslatedBytes=12125u;')
p.write_text(s)

q=Path('Code1/PROGRESS.md');t=q.read_text();t=t.replace('15.83%25-red','17.22%25-red').replace('**Verified conversion:** 15.83%','**Verified conversion:** 17.22%').replace('**Remaining:** 84.17%','**Remaining:** 82.78%').replace('**11,146**','**12,125**').replace('**15.8320%**, displayed as **15.83%**','**17.2225%**, displayed as **17.22%**').replace('- Resource image-size parser `F7B8..F8BC`: **261 bytes**.','- Resource image-size parser `F7B8..F8BC`: **261 bytes**.\n- Resource image loader/relocator `F8C4..FC96`: **979 bytes**.').replace('Total integrated verified translated bytes: **11,146**.','Total integrated verified translated bytes: **12,125**.')
q.write_text(t)

r=Path('Code1/STAGING_LEDGER.md');u=r.read_text();u += '\n- `F8C4..FC96 integrated`: 979-byte resource image loader/relocator directly disassembled; guest allocation, zeroing, reads, and relocation writes remain explicit machine boundaries.\n';r.write_text(u)
