from pathlib import Path
p=Path('Code1/Code1.cpp');s=p.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes=12645u;' in s
assert 'function_104e0' not in s
marker='static_assert(0x102D4u-0xFFD4u==256u*3u);\n'
block=r'''

// 104E0..1083E: Watcom-style guest heap core. All heap addresses remain 32-bit guest addresses; no host malloc/free or native-pointer aliasing is substituted.
struct HeapCoreBoundaries{
 u32(*read32)(u32)=nullptr; void(*write32)(u32,u32)=nullptr;
 void(*dpmi_free_0502)(u32)=nullptr; void(*dos_free_49)(u16)=nullptr;
};
static u32 hread(HeapCoreBoundaries&b,u32 a)noexcept{return b.read32?b.read32(a):0u;} static void hwrite(HeapCoreBoundaries&b,u32 a,u32 v)noexcept{if(b.write32)b.write32(a,v);}
// 104E0..1058A: allocate from one arena descriptor. Size is rounded exactly as encoded; free-list links are split or unlinked in place.
[[nodiscard]]u32 function_104e0(HeapCoreBoundaries&b,u32 request,u32 arena)noexcept{
 if(request==0u)return 0u;u32 n=request+7u;if(n<request)return 0u;n&=0xfffffffcu;const u32 overhead=12u;n=n<overhead?overhead:n;
 if(n>hread(b,arena+0x10u))return 0u;u32 node=hread(b,arena+8u),scan=hread(b,arena+0x0cu);if(n<=scan){node=hread(b,arena+0x24u);scan=0u;}
 const u32 sentinel=arena+0x1cu;for(;;){u32 size=hread(b,node);if(n<=size){u32 remain=size-n;hwrite(b,arena+0x0cu,scan);hwrite(b,arena+0x14u,hread(b,arena+0x14u)+1u);u32 next=hread(b,node+8u);if(remain>=12u){u32 tail=node+n;hwrite(b,arena+8u,tail);hwrite(b,tail,remain);hwrite(b,node,n);u32 prev=hread(b,node+4u);hwrite(b,tail+4u,prev);hwrite(b,tail+8u,next);hwrite(b,prev+8u,tail);hwrite(b,next+4u,tail);}else{hwrite(b,arena+0x18u,hread(b,arena+0x18u)-1u);u32 prev=hread(b,node+4u);hwrite(b,prev+8u,next);hwrite(b,next+4u,prev);hwrite(b,arena+8u,prev);}hwrite(b,node,hread(b,node)|1u);return node+4u;}scan=scan-size;node=hread(b,node+8u);if(node==sentinel){hwrite(b,arena+0x10u,scan);return 0u;}}
}
static_assert(0x1058Bu-0x104E0u==171u);
// 1058B is the shared free epilogue target. 10590..1069A returns a block to the address-ordered free list and coalesces adjacent blocks.
void function_10590(HeapCoreBoundaries&b,u32 ptr,u32 arena)noexcept{
 if(ptr==0u)return;u32 block=ptr-4u,size=hread(b,block);if((size&1u)==0u)return;size&=~1u;u32 after=block+size;
 if((hread(b,after)&1u)==0u){if(after==hread(b,arena+8u))hwrite(b,arena+8u,block);size+=hread(b,after);hwrite(b,block,size);u32 p=hread(b,after+4u),n=hread(b,after+8u);hwrite(b,p+8u,n);hwrite(b,n+4u,p);hwrite(b,arena+0x18u,hread(b,arena+0x18u)-1u);}else hwrite(b,block,size);
 u32 cur=hread(b,arena+8u);if(block<cur){if(block>hread(b,cur+4u))cur=hread(b,arena+0x24u);}else{cur=hread(b,cur+8u);while(block>=cur){cur=hread(b,cur+8u);if(cur==arena+0x1cu)break;}}
 u32 prev=hread(b,cur+4u);u32 merged_size=hread(b,block);if(prev+hread(b,prev)==block){merged_size+=hread(b,prev);hwrite(b,prev,merged_size);if(block==hread(b,arena+8u))hwrite(b,arena+8u,prev);block=prev;}else{hwrite(b,arena+0x18u,hread(b,arena+0x18u)+1u);hwrite(b,block+8u,cur);hwrite(b,block+4u,prev);hwrite(b,prev+8u,block);hwrite(b,cur+4u,block);}hwrite(b,arena+0x14u,hread(b,arena+0x14u)-1u);if(block<hread(b,arena+8u)&&merged_size>hread(b,arena+0x0cu))hwrite(b,arena+0x0cu,merged_size);if(merged_size>hread(b,arena+0x10u))hwrite(b,arena+0x10u,merged_size);
}
static_assert(0x1069Bu-0x10590u==267u);
// 1069B..10714: insert a newly acquired arena into the global address-ordered arena list and construct its initial terminal/free block metadata.
[[nodiscard]]u32 function_1069b(Machine&m,HeapCoreBoundaries&b,u32 raw)noexcept{
 u32 next=load_le32(dgroup_ptr(m,dgroup(0x30b4))),prev=0;while(next&&raw>=next){prev=next;next=hread(b,next+8u);}hwrite(b,raw+4u,prev);hwrite(b,raw+8u,next);if(prev){hwrite(b,prev+8u,raw);hwrite(b,prev+hread(b,prev)+4u,raw+0x0cu);}else{store_le32(dgroup_ptr(m,dgroup(0x30b0)),raw);store_le32(dgroup_ptr(m,dgroup(0x30b4)),raw);}if(next){hwrite(b,next+4u,raw);next+=0x0cu;}u32 size=hread(b,raw);u32 freep=raw+0x0cu,free_size=size-0x0cu;hwrite(b,freep,free_size);if(freep<=load_le32(dgroup_ptr(m,dgroup(0x30c0)))&&free_size>load_le32(dgroup_ptr(m,dgroup(0x30c4))))store_le32(dgroup_ptr(m,dgroup(0x30c4)),free_size);hwrite(b,freep+free_size,0xffffffffu);hwrite(b,freep+free_size+4u,next);return freep;
}
static_assert(0x10715u-0x1069Bu==122u);
// 10715..10730: unlink one arena-list node.
void function_10715(HeapCoreBoundaries&b,u32 node)noexcept{u32 p=hread(b,node+4u),n=hread(b,node+8u);if(p)hwrite(b,p+8u,n);if(n)hwrite(b,n+4u,p);}
static_assert(0x10731u-0x10715u==28u);
// 10731..107CA: detach an arena descriptor and its embedded free-list nodes from global lists.
void function_10731(Machine&m,HeapCoreBoundaries&b,u32 previous,u32 arena)noexcept{
 if(hread(b,arena+0x18u)!=0u){u32 x=hread(b,arena+0x18u),end=x+hread(b,x)+4u,src=arena+0x14u+hread(b,arena+0x14u)+4u;hwrite(b,end,hread(b,src));}
 if(previous==0u)store_le32(dgroup_ptr(m,dgroup(0x8e74)),hread(b,arena+8u));else hwrite(b,previous+8u,hread(b,arena+8u));u32 n=arena+0x14u;if(n==load_le32(dgroup_ptr(m,dgroup(0x30b0))))store_le32(dgroup_ptr(m,dgroup(0x30b0)),hread(b,arena+0x1cu));if(n==load_le32(dgroup_ptr(m,dgroup(0x30b4))))store_le32(dgroup_ptr(m,dgroup(0x30b4)),hread(b,arena+0x1cu));function_10715(b,n);if((hread(b,arena+0x20u)&1u)==0u){u32 q=arena+0x20u;function_10715(b,q);store_le32(dgroup_ptr(m,dgroup(0x30d0)),load_le32(dgroup_ptr(m,dgroup(0x30d0)))-1u);if(q==load_le32(dgroup_ptr(m,dgroup(0x30c0))))store_le32(dgroup_ptr(m,dgroup(0x30c0)),hread(b,arena+0x28u));}
}
static_assert(0x107CBu-0x10731u==154u);
// 107CB..1083E: reclaim arenas whose terminal free block spans the whole arena; use DPMI 0502h or DOS AH=49h according to the stored arena kind.
void function_107cb(Machine&m,HeapCoreBoundaries&b)noexcept{u32 prev=0,arena=load_le32(dgroup_ptr(m,dgroup(0x8e74)));while(arena){if(hread(b,arena+0x20u)+0x0cu==hread(b,arena+0x14u)){u32 old=arena,next=hread(b,arena+8u);function_10731(m,b,prev,arena);if(hread(b,old+0x10u)==0u){if(b.dpmi_free_0502)b.dpmi_free_0502(hread(b,old));}else if(b.dos_free_49)b.dos_free_49(static_cast<u16>(hread(b,old+0x10u)));arena=next;}else{prev=arena;arena=hread(b,arena+8u);}}}
static_assert(0x1083Fu-0x107CBu==116u);
// 10974..109C0: report the top arena's terminal extent only when it exactly reaches the current selector/stack limit.
[[nodiscard]]u32 function_10974(Machine&m,HeapCoreBoundaries&b,u32 selector_limit)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x30b0)))==0u)return 0u;u32 a=load_le32(dgroup_ptr(m,dgroup(0x30d8)));u32 extent=hread(b,a);u32 end=a+extent+8u;if(end!=selector_limit)return 0u;return extent;}
static_assert(0x109C1u-0x10974u==77u);
// 10C46..10C4C: constant-success/zero helper.
[[nodiscard]]u32 function_10c46()noexcept{return 0u;} static_assert(0x10C4Du-0x10C46u==7u);
// 10C4D..10C58: fatal runtime path, encoded as CE8E(0x390,1).
struct HeapTailBoundaries{void(*fatal_ce8e)(u32,u32)=nullptr;}; void function_10c4d(HeapTailBoundaries&b)noexcept{if(b.fatal_ce8e)b.fatal_ce8e(0x390u,1u);} static_assert(0x10C59u-0x10C4Du==12u);
// 10C59..10C6C: seek wrapper equivalent to D85F(handle,0,SEEK_CUR).
[[nodiscard]]s32 function_10c59(FileBoundaries&b,u32 h)noexcept{return function_d85f(b,static_cast<u16>(h),0,1u);} static_assert(0x10C6Du-0x10C59u==20u);
'''
s=s.replace(marker,marker+block)
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=12645u;','constexpr u32 kVerifiedTranslatedBytes=13624u;')
p.write_text(s)
q=Path('Code1/PROGRESS.md');t=q.read_text();t=t.replace('17.96%25-red','19.35%25-red').replace('**Verified conversion:** 17.96%','**Verified conversion:** 19.35%').replace('**Remaining:** 82.04%','**Remaining:** 80.65%').replace('**12,645**','**13,624**').replace('**17.9611%**, displayed as **17.96%**','**19.3517%**, displayed as **19.35%**').replace('- CRT error/descriptor/stack/selector helpers `102D4..104DD` excluding four zero alignment bytes: **520 bytes**.','- CRT error/descriptor/stack/selector helpers `102D4..104DD` excluding four zero alignment bytes: **520 bytes**.\n- Guest heap allocator/free-list core `104E0..1083E`: **863 bytes**.\n- Heap extent query `10974..109C0`: **77 bytes**.\n- Runtime tail helpers `10C46..10C6C`: **39 bytes**.').replace('Total integrated verified translated bytes: **12,645**.','Total integrated verified translated bytes: **13,624**.')
q.write_text(t)
r=Path('Code1/STAGING_LEDGER.md');u=r.read_text();u+='\n- `104E0..1083E`, `10974..109C0`, `10C46..10C6C` integrated: 979 verified heap/runtime bytes. Guest heap addresses and DPMI/DOS release operations remain explicit boundaries; no host allocator substitution.\n';r.write_text(u)
