from pathlib import Path
p=Path('Code1/Code1.cpp');s=p.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes=13624u;' in s
assert 'function_1083f' not in s and 'function_109c1' not in s and 'function_10aa9' not in s
marker='static_assert(0x1083Fu-0x107CBu==116u);\n'
block=r'''

// 1083F..10973: resize the arena whose embedded terminal block is supplied by the caller. The INT 31h/0503 result remains an explicit DPMI boundary: linear address and SI:DI handle are distinct values.
struct DpmiBlockResult{bool carry=false;u32 linear=0;u32 handle=0;};
struct HeapGrowthBoundaries{
 DpmiBlockResult(*resize_0503)(u32,u32)=nullptr; DpmiBlockResult(*alloc_0501)(u32)=nullptr;
 bool(*dos_alloc_48)(u16,u16&)=nullptr; void(*dos_free_49)(u16)=nullptr; u32(*segment_base_0006)(u16)=nullptr;
 u32(*legacy_alloc_80004800)(u32)=nullptr; u32(*extend_10dd7)(u32)=nullptr; void(*free_db5b)(u32)=nullptr;
};
[[nodiscard]]u32 function_1083f(Machine&m,HeapCoreBoundaries&hb,HeapGrowthBoundaries&gb,u32 terminal,u32 requested)noexcept{
 function_107cb(m,hb);u32 previous=0,arena=load_le32(dgroup_ptr(m,dgroup(0x8e74)));
 while(arena){if(terminal==arena+0x20u){u32 terminal_size=hread(hb,terminal)&0xfffffffeu;if(terminal_size+0x0cu!=hread(hb,arena+0x14u)||hread(hb,arena+0x10u)!=0u)return 0u;function_10731(m,hb,previous,arena);const u32 bytes=(requested+0x2cu+0xfffu)&0xfffff000u;const u32 old_handle=hread(hb,arena);DpmiBlockResult r=gb.resize_0503?gb.resize_0503(old_handle,bytes):DpmiBlockResult{true,0,0};u32 new_arena=r.carry?0u:r.linear;if(new_arena==0u)new_arena=arena;else{hwrite(hb,new_arena,r.handle);hwrite(hb,new_arena+4u,bytes);hwrite(hb,new_arena+0x14u,bytes-0x1cu);}hwrite(hb,new_arena+8u,load_le32(dgroup_ptr(m,dgroup(0x8e74))));store_le32(dgroup_ptr(m,dgroup(0x8e74)),new_arena);hwrite(hb,new_arena+0x0cu,0u);hwrite(hb,new_arena+0x10u,0u);u32 freep=function_1069b(m,hb,new_arena+0x14u);if(!r.carry){const u32 split=hread(hb,new_arena+0x20u)-requested;if(split>=0x0cu){hwrite(hb,new_arena+0x20u,requested|1u);u32 tail=new_arena+0x20u+requested;hwrite(hb,tail,split|1u);store_le32(dgroup_ptr(m,dgroup(0x30c8)),0xffffffffu);store_le32(dgroup_ptr(m,dgroup(0x30cc)),load_le32(dgroup_ptr(m,dgroup(0x30cc)))+1u);if(gb.free_db5b)gb.free_db5b(tail+4u);}}hwrite(hb,new_arena+0x20u,hread(hb,new_arena+0x20u)|1u);return r.carry?0u:freep;}previous=arena;arena=hread(hb,arena+8u);}return 0u;
}
static_assert(0x10974u-0x1083Fu==309u);

// 109C1..10AA8: acquire a raw arena. First try DPMI 0501h; on failure and small requests, probe/free DOS memory, allocate the requested paragraphs, then convert the DOS segment through INT 31h/0006h.
[[nodiscard]]u32 function_109c1(Machine&m,HeapCoreBoundaries&hb,HeapGrowthBoundaries&gb,u32 bytes)noexcept{
 function_107cb(m,hb);DpmiBlockResult r=gb.alloc_0501?gb.alloc_0501(bytes):DpmiBlockResult{true,0,0};if(!r.carry&&r.linear){hwrite(hb,r.linear,r.handle);hwrite(hb,r.linear+4u,bytes);hwrite(hb,r.linear+8u,load_le32(dgroup_ptr(m,dgroup(0x8e74))));store_le32(dgroup_ptr(m,dgroup(0x8e74)),r.linear);hwrite(hb,r.linear+0x10u,0u);return r.linear+0x0cu;}
 if((load_le16(dgroup_ptr(m,dgroup(0x8e8a)))&0xfff0u)!=0u)store_le32(dgroup_ptr(m,dgroup(0x8e88)),0x000fffffu);if(bytes>0x10000u)return 0u;u16 probe=0;const u16 probe_par=static_cast<u16>((load_le32(dgroup_ptr(m,dgroup(0x8e88)))>>4)|1u);if(!gb.dos_alloc_48||!gb.dos_alloc_48(probe_par,probe))return 0u;if(gb.dos_free_49)gb.dos_free_49(probe);u16 seg=0;if(!gb.dos_alloc_48(static_cast<u16>(bytes>>4),seg))return 0u;u32 linear=gb.segment_base_0006?gb.segment_base_0006(seg):0u;if(!linear){if(gb.dos_free_49)gb.dos_free_49(seg);return 0u;}hwrite(hb,linear+4u,bytes);hwrite(hb,linear+8u,load_le32(dgroup_ptr(m,dgroup(0x8e74))));store_le32(dgroup_ptr(m,dgroup(0x8e74)),linear);hwrite(hb,linear+0x10u,seg);return linear+0x0cu;
}
static_assert(0x10AA9u-0x109C1u==232u);

// 10AA9..10C45: grow the runtime heap. The mode-1 DPMI/DOS arena path, mode-9 AX=80004800 path, and break-extension path remain separate exactly as encoded.
[[nodiscard]]u32 function_10aa9(Machine&m,HeapCoreBoundaries&hb,HeapGrowthBoundaries&gb,u32 request)noexcept{
 if(load_le32(dgroup_ptr(m,dgroup(0x55cc)))==0u||load_le32(dgroup_ptr(m,dgroup(0x52f0)))==0xfffffffeu)return 0u;u32 size=(request+3u)&0xfffffffcu;if(size==0u)return 0u;const u8 mode=*dgroup_ptr(m,dgroup(0x531e));if(mode==1u||mode==9u)size+=0x2cu;else size-=function_10974(m,hb,load_le32(dgroup_ptr(m,dgroup(0x52f0)));if(size+0x20u<size)return 0u;size+=0x20u;u32 minimum=load_le32(dgroup_ptr(m,dgroup(0x55d0)));if(size<minimum)size=minimum&0xfffffffeu;size=(size+0xfffu)&0xfffff000u;
 u32 raw=0,usable=size;if(mode==1u){raw=function_109c1(m,hb,gb,size);usable-=0x14u;}else if(mode==9u){raw=gb.legacy_alloc_80004800?gb.legacy_alloc_80004800(size):0u;usable-=8u;if(raw)hwrite(hb,raw,0u),raw+=8u;}else{u32 limit=load_le32(dgroup_ptr(m,dgroup(0x52f0)));u32 desired=size+limit;if(desired<limit)desired=0xfffffffeu;u32 old=gb.extend_10dd7?gb.extend_10dd7(desired):0xffffffffu;if(old==0xffffffffu||old>0xfffffff8u||size<=old)return 0u;raw=old;usable=size-old;}
 if(raw==0u)return 0u;u32 block_size=usable-8u;if(block_size>usable)return 0u;u32 block=raw;if(load_le32(dgroup_ptr(m,dgroup(0x30b0)))!=0u){u32 top=load_le32(dgroup_ptr(m,dgroup(0x30b0)));u32 end=top+hread(hb,top);if(raw-8u==end){hwrite(hb,top,hread(hb,top)+block_size+8u);hwrite(hb,end+block_size+8u,0xffffffffu);hwrite(hb,end+block_size+0x0cu,0u);block=end;goto finish;}}
 if(block_size<0x18u)return 0u;hwrite(hb,raw,block_size);block=function_1069b(m,hb,raw);
finish:hwrite(hb,block,hread(hb,block)|1u);block+=4u;store_le32(dgroup_ptr(m,dgroup(0x30c8)),0xffffffffu);store_le32(dgroup_ptr(m,dgroup(0x30cc)),load_le32(dgroup_ptr(m,dgroup(0x30cc)))+1u);if(gb.free_db5b)gb.free_db5b(block);return 1u;
}
static_assert(0x10C46u-0x10AA9u==413u);
'''
s=s.replace(marker,marker+block)
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=13624u;','constexpr u32 kVerifiedTranslatedBytes=14578u;')
p.write_text(s)
q=Path('Code1/PROGRESS.md');t=q.read_text();t=t.replace('19.35%25-red','20.70%25-red').replace('**Verified conversion:** 19.35%','**Verified conversion:** 20.70%').replace('**Remaining:** 80.65%','**Remaining:** 79.30%').replace('Current verified translated bytes: **13,624**.','Current verified translated bytes: **14,578**.').replace('Raw coverage: **19.3517%**, displayed as **19.35%**.','Raw coverage: **20.7068%**, displayed as **20.70%**.').replace('- Heap extent query `10974..109C0`: **77 bytes**.','- Heap resize/growth core `1083F..10973`: **309 bytes**.\n- Heap extent query `10974..109C0`: **77 bytes**.\n- Raw arena acquisition `109C1..10AA8`: **232 bytes**.\n- Runtime heap-extension driver `10AA9..10C45`: **413 bytes**.').replace('Total integrated verified translated bytes: **13,624**.','Total integrated verified translated bytes: **14,578**.')
q.write_text(t)
r=Path('Code1/STAGING_LEDGER.md');u=r.read_text();u+='\n- `1083F..10973`, `109C1..10C45` integrated: 954 verified heap-growth bytes. DPMI 0501/0503, DOS AH=48/49, segment-base conversion, mode-9 allocation, and break-extension remain explicit boundaries; no host allocator substitution.\n';r.write_text(u)
c=Path('Code1/DECOMPILATION_CONTINUITY.md');v=c.read_text();v+='\n\n## Latest heap-growth checkpoint\nVerified total is 14,578 bytes (20.70%). `1083F..10973` and `109C1..10C45` are integrated after direct disassembly. Preserve the separation between DPMI 0501/0503 linear-address+handle results, DOS segment allocation/free, mode-9 AX=80004800 allocation, and the 10DD7 break-extension boundary. Active staging remains 974 bytes (747 startup + 70 DTA + 157 INT24 trampoline).\n';c.write_text(v)
