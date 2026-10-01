from pathlib import Path
p=Path('Code1/integrate_heap_growth.py');s=p.read_text()
s=s.replace('[[nodiscard]]u32 function_10dd7(Machine&,RuntimeTailBoundaries&,u32)noexcept;','[[nodiscard]]u32 function_10dd7(Machine&,FileBoundaries&,RuntimeTailBoundaries&,u32)noexcept;')
old='[[nodiscard]]u32 function_10d3a(Machine&m,HeapGrowthBoundaries&gb,RuntimeTailBoundaries&tb,s32 request)noexcept{const u8 mode=*dgroup_ptr(m,dgroup(0x531e));if(mode!=1u&&mode!=9u)return function_10dd7(m,tb,static_cast<u32>(request)+load_le32(dgroup_ptr(m,dgroup(0x52f0))));if(request<=0){if(auto*p=gb.free_db5b?nullptr:nullptr;(void)p,false){}return 0xffffffffu;}u32 n=(static_cast<u32>(request)+0xfffu)&0xfffff000u;u32 out=0;if(mode==1u){DpmiBlockResult r=gb.alloc_0501?gb.alloc_0501(n):DpmiBlockResult{true,0,0};out=r.carry?0u:r.linear;}else out=gb.legacy_alloc_80004800?gb.legacy_alloc_80004800(n):0u;return out?out:0xffffffffu;}'
new='[[nodiscard]]u32 function_10d3a(Machine&m,FileBoundaries&f,HeapGrowthBoundaries&gb,RuntimeTailBoundaries&tb,s32 request)noexcept{const u8 mode=*dgroup_ptr(m,dgroup(0x531e));if(mode!=1u&&mode!=9u)return function_10dd7(m,f,tb,static_cast<u32>(request)+load_le32(dgroup_ptr(m,dgroup(0x52f0))));if(request<=0){if(f.call_103b7)if(u32*p=f.call_103b7())*p=9u;return 0xffffffffu;}u32 n=(static_cast<u32>(request)+0xfffu)&0xfffff000u;u32 out=0;if(mode==1u){DpmiBlockResult r=gb.alloc_0501?gb.alloc_0501(n):DpmiBlockResult{true,0,0};out=r.carry?0u:r.linear;}else out=gb.legacy_alloc_80004800?gb.legacy_alloc_80004800(n):0u;if(!out&&f.call_103b7)if(u32*p=f.call_103b7())*p=5u;return out?out:0xffffffffu;}'
assert old in s;s=s.replace(old,new)
old='[[nodiscard]]u32 function_10dd7(Machine&m,RuntimeTailBoundaries&b,u32 requested)noexcept{if(requested<load_le32(dgroup_ptr(m,dgroup(0x5304))))return 0xffffffffu;bool ok=false;if(*dgroup_ptr(m,dgroup(0x531e))==0u){u32 paragraphs=(requested+0x0fu)>>4;if(paragraphs==0u)paragraphs=0x0fffffffu;ok=b.resize_break_mode0?b.resize_break_mode0(paragraphs):false;}else ok=b.resize_break_other?b.resize_break_other(requested):false;if(!ok)return 0xffffffffu;u32 old=load_le32(dgroup_ptr(m,dgroup(0x52f0)));store_le32(dgroup_ptr(m,dgroup(0x52f0)),requested);return old;}'
new='[[nodiscard]]u32 function_10dd7(Machine&m,FileBoundaries&f,RuntimeTailBoundaries&b,u32 requested)noexcept{bool ok=false;if(requested>=load_le32(dgroup_ptr(m,dgroup(0x5304)))){if(*dgroup_ptr(m,dgroup(0x531e))==0u){u32 paragraphs=(requested+0x0fu)>>4;if(paragraphs==0u)paragraphs=0x0fffffffu;ok=b.resize_break_mode0?b.resize_break_mode0(paragraphs):false;}else ok=b.resize_break_other?b.resize_break_other(requested):false;}if(!ok){if(f.call_103b7)if(u32*p=f.call_103b7())*p=5u;return 0xffffffffu;}u32 old=load_le32(dgroup_ptr(m,dgroup(0x52f0)));store_le32(dgroup_ptr(m,dgroup(0x52f0)),requested);return old;}'
assert old in s;s=s.replace(old,new)
old='bool close=(tr8(b,stream+0x0du)&0x40u)!=0u;if(!close)close=!(stream<threshold)&&(stream>=0x541eu);if(close){++count;(void)function_1104a(m,f,b,stream,1u);}'
new='if(stream>=threshold){u32 close_handle=((tr8(b,stream+0x0du)&0x40u)!=0u||stream>=0x541eu)?1u:0u;++count;(void)function_1104a(m,f,b,stream,close_handle);}'
assert old in s;s=s.replace(old,new)
old='if(close_handle&&f.dos_close&&!f.dos_close(static_cast<u16>(tr32(b,stream+0x10u))))status=-1;'
new='if(close_handle&&function_d573(f,tr32(b,stream+0x10u))!=0)status=-1;'
assert old in s;s=s.replace(old,new)
old='s32 wrote=b.write_handle?b.write_handle(handle,buffer,count):-1;if(wrote<0)return -1;return wrote;'
new='s32 wrote=b.write_handle?b.write_handle(handle,buffer,count):-1;if(wrote<0)return -1;if(static_cast<u32>(wrote)!=count&&f.call_103b7)if(u32*p=f.call_103b7())*p=12u;return wrote;'
assert old in s;s=s.replace(old,new)
p.write_text(s)
