from pathlib import Path

cpp_path = Path('Code1/Code1.cpp')
s = cpp_path.read_text()

old = " u32(*open_62a8)(u32)=nullptr;u32(*close_62f0)(u32)=nullptr;s32(*read_6304)(u32,u32,u32)=nullptr;s32(*read_local_6304)(void*,u32,u32)=nullptr;void(*seek_6358)(u32,u32)=nullptr;"
new = " u32(*open_62a8)(u32)=nullptr;u32(*open_local_62a8)(const void*)=nullptr;u32(*close_62f0)(u32)=nullptr;s32(*read_6304)(u32,u32,u32)=nullptr;s32(*read_local_6304)(void*,u32,u32)=nullptr;void(*seek_6358)(u32,u32)=nullptr;"
assert old in s
s = s.replace(old, new, 1)

old = " void(*concat_b01a)(u32,u32)=nullptr;void(*fatal_9814)(u32)=nullptr;"
new = " void(*concat_b01a)(u32,u32)=nullptr;void(*fatal_9814)(u32)=nullptr;void(*fatal_local_9814)(const void*)=nullptr;"
assert old in s
s = s.replace(old, new, 1)

marker = "static u8 ma8(MidAssetBoundaries&b,u32 a)noexcept{return b.read8?b.read8(a):0u;}static u16 ma16(MidAssetBoundaries&b,u32 a)noexcept{return b.read16?b.read16(a):0u;}static u32 ma32(MidAssetBoundaries&b,u32 a)noexcept{return b.read32?b.read32(a):0u;}static void maw8(MidAssetBoundaries&b,u32 a,u8 v)noexcept{if(b.write8)b.write8(a,v);}static void maw16(MidAssetBoundaries&b,u32 a,u16 v)noexcept{if(b.write16)b.write16(a,v);}static void maw32(MidAssetBoundaries&b,u32 a,u32 v)noexcept{if(b.write32)b.write32(a,v);}\n"
assert marker in s
helpers = marker + """
static void ma_local_copy_guest_string(MidAssetBoundaries&b,u8*dst,u32 src)noexcept{
 for(;;){u8 c=ma8(b,src);*dst++=c;if(c==0u)break;c=ma8(b,src+1u);*dst++=c;src+=2u;if(c==0u)break;}
}
static void ma_local_append_guest_string(MidAssetBoundaries&b,u8*dst,u32 src)noexcept{while(*dst!=0u)++dst;ma_local_copy_guest_string(b,dst,src);}
void function_91d8(Machine&,MidAssetBoundaries&,u32,u32)noexcept;
void function_9488(Machine&,MidAssetBoundaries&,u32,u32)noexcept;
"""
s = s.replace(marker, helpers, 1)

start = s.index('void function_8a04')
end = s.index('[[nodiscard]]u32 function_8df0', start)
block = r'''void function_8a04(Machine&m,MidAssetBoundaries&b,u32 prefix,u32 groups)noexcept{
 function_8978(m,b);
 const u32 table=b.alloc_71c8?b.alloc_71c8(groups*16u):0u;
 store_le32(dgroup_ptr(m,dgroup(0x2b08u)),table);store_le32(dgroup_ptr(m,dgroup(0x2b04u)),groups);
 u8 path[0x50]{};
 for(s32 g=0;g<static_cast<s32>(groups);++g){
  const u32 gu=static_cast<u32>(g);
  if(*dgroup_ptr(m,dgroup(0x2aa4u+gu))!=0u){ma_local_copy_guest_string(b,path,0x2ab1u);ma_local_append_guest_string(b,path,0x268u);ma_local_append_guest_string(b,path,prefix);}else ma_local_copy_guest_string(b,path,prefix);
  ma_local_append_guest_string(b,path,function_88d8(b,gu));
  const u32 h=b.open_local_62a8?b.open_local_62a8(path):0xffffffffu;
  if(h==0xffffffffu&&b.fatal_9814)b.fatal_9814(0x26cu);
  u32 rec=load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+gu*16u;maw32(b,rec,h);
  if(b.read_6304)b.read_6304(rec+8u,4u,h);
  rec=load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+gu*16u;maw32(b,rec+4u,0u);
  rec=load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+gu*16u;const u32 count=ma32(b,rec+8u);const u32 slots=b.alloc_71c8?b.alloc_71c8(count*9u):0u;
  rec=load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+gu*16u;maw32(b,rec+12u,slots);
  for(s32 i=0;;++i){rec=load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+gu*16u;if(i>=static_cast<s32>(ma32(b,rec+8u)))break;const u32 e=ma32(b,rec+12u)+static_cast<u32>(i)*9u;maw8(b,e,static_cast<u8>((ma8(b,e)&0xf0u)|0x0fu));maw8(b,e,static_cast<u8>(ma8(b,e)&0xefu));maw8(b,e+2u,0u);maw16(b,e+3u,0u);maw32(b,e+5u,0u);}
 }
}static_assert(0x8B93u-0x8A04u==399u);

void function_8b94(Machine&m,MidAssetBoundaries&b)noexcept{
 for(s32 g=0;g<static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x2b04u))));++g){const u32 gu=static_cast<u32>(g);for(s32 i=0;;++i){const u32 rec=load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+gu*16u;if(i>=static_cast<s32>(ma32(b,rec+8u)))break;function_9488(m,b,static_cast<u32>(i),gu);function_91d8(m,b,static_cast<u32>(i),gu);}}
}static_assert(0x8BE3u-0x8B94u==79u);

void function_8be4(Machine&m,MidAssetBoundaries&b,u32 ticks)noexcept{
 ticks>>=5;
 for(s32 g=0;g<static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x2b04u))));++g){const u32 gu=static_cast<u32>(g);for(s32 i=0;;++i){const u32 rec=load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+gu*16u;if(i>=static_cast<s32>(ma32(b,rec+8u)))break;const u32 e=ma32(b,rec+12u)+static_cast<u32>(i)*9u;if(ma8(b,e+2u)!=0u)maw16(b,e+3u,static_cast<u16>(ticks));}}
}static_assert(0x8C3Du-0x8BE4u==89u);

void function_8c40(Machine&m,MidAssetBoundaries&b)noexcept{
 function_8b94(m,b);
 for(s32 g=0;g<static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x2b04u))));++g){const u32 gu=static_cast<u32>(g);u32 rec=load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+gu*16u;const u32 h=ma32(b,rec);if(h!=0u){if(b.close_62f0)(void)b.close_62f0(h);rec=load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+gu*16u;maw32(b,rec,0u);maw32(b,rec+8u,0u);}rec=load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+gu*16u;const u32 slots=ma32(b,rec+12u);if(slots!=0u){if(b.free_7240)b.free_7240(slots);rec=load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+gu*16u;maw32(b,rec+12u,0u);}}
 const u32 table=load_le32(dgroup_ptr(m,dgroup(0x2b08u)));if(b.free_7240)b.free_7240(table);store_le32(dgroup_ptr(m,dgroup(0x2b08u)),0u);
}static_assert(0x8CD4u-0x8C40u==148u);

[[nodiscard]]u32 function_8cd4(Machine&m,MidAssetBoundaries&b)noexcept{
 u32 total=0u;for(s32 g=0;g<static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x2b04u))));++g){const u32 table=load_le32(dgroup_ptr(m,dgroup(0x2b08u)));total+=ma32(b,table+static_cast<u32>(g)*16u+4u);}return total;
}static_assert(0x8D0Cu-0x8CD4u==56u);

void function_8d0c(MidAssetBoundaries&b,u32 dst,u32 index,u32 h,u32 kind)noexcept{
 if(b.seek_6358)b.seek_6358(index,h);if(b.read_6304)b.read_6304(dst,5u,h);
 const s32 n=static_cast<s16>(ma16(b,dst+3u));
 if(n!=0){const u32 p=b.alloc_71c8?b.alloc_71c8(static_cast<u32>(n)):0u;maw32(b,dst+5u,p);if(b.read_6304)b.read_6304(p,static_cast<u32>(n),h);}else maw32(b,dst+5u,0u);
 if(ma16(b,dst)!=0x4845u){u8 msg[0x50]{};for(u32 i=0;i<36u;++i)msg[i]=ma8(b,0x28cu+i);ma_local_append_guest_string(b,msg,function_88d8(b,index));ma_local_append_guest_string(b,msg,0x2b0u);ma_local_append_guest_string(b,msg,function_88f8(b,kind));if(b.fatal_local_9814)b.fatal_local_9814(msg);}
}static_assert(0x8DEDu-0x8D0Cu==225u);
'''
s = s[:start] + block + s[end:]

cpp_path.write_text(s)

audit = Path('Code1/FINAL_BINARY_AUDIT.md').read_text()
assert '## Semantic audit checkpoint 8b — `8A04..8E1E`' in audit
print('checkpoint 8b patch applied')
