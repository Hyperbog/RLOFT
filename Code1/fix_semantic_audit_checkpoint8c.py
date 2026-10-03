from pathlib import Path
import re
p=Path('Code1/Code1.cpp')
s=p.read_text()

old=' void(*release_type)(u32,u32,u32)=nullptr;u32(*modal_9874)(u32,u32)=nullptr;void(*exit_97d4)(u32)=nullptr;'
new=' void(*release_type)(u32,u32,u32)=nullptr;void(*call_8e3c)(s32,u32)=nullptr;u32(*modal_9874)(u32,u32)=nullptr;void(*exit_97d4)(u32)=nullptr;'
if old in s:
    assert s.count(old)==1
    s=s.replace(old,new)
else:
    assert new in s

def replace_fn(name, end_assert, replacement):
    global s
    pat=re.compile(r'(?:\[\[nodiscard\]\])?(?:void|u32) '+re.escape(name)+r'\(.*?'+re.escape(end_assert), re.S)
    m=pat.search(s)
    assert m, name
    if m.group(0)==replacement:
        return
    s=s[:m.start()]+replacement+s[m.end():]

replace_fn('function_8e3c','static_assert(0x91BCu-0x8E3Cu==896u);',r'''void function_8e3c(Machine&m,Batch12Boundaries&b,s32 index,u32 group)noexcept{u32 groups=load_le32(dgroup_ptr(m,dgroup(0x2b08u)));u32 gr=groups+(group<<4);if(index<0||index>=static_cast<s32>(b12r32(b,gr+8u))){if(b.asset_bad_index)b.asset_bad_index(index,group);}u32 ent=b12r32(b,gr+0x0cu)+static_cast<u32>(index)*9u;if(b12r8(b,ent+2u)!=0u){u32 t=b.tick_7f50?b.tick_7f50():0u;b12w16(b,ent+3u,static_cast<u16>(t>>5));return;}u32 cb=load_le32(dgroup_ptr(m,dgroup(0x2b1cu)));if(cb&&b.asset_preload)b.asset_preload(index,group);u8 local[0x18]{};if(b.asset_header_8d0c)b.asset_header_8d0c(local,index,b12r32(b,gr),group);u8 type=local[2];u32 result=0;if(type>=1u&&type<=7u){auto fn=b.asset_load[type-1u];if(fn)result=fn(local,b12r32(b,gr));}else if(b.fatal_9814)b.fatal_9814(0x2d4u);b12w32(b,ent+5u,result);b12w8(b,ent+2u,1u);const u16 typed=static_cast<u16>((b12r8(b,ent)&0xf0u)|(type&0x0fu));b12w16(b,ent,typed);b12w8(b,ent,static_cast<u8>(b12r8(b,ent)&0xefu));u32 temp=load_le32(local+5);if(temp&&b.free_7240)b.free_7240(temp);b12w32(b,gr+4u,b12r32(b,gr+4u)+1u);u32 t=b.tick_7f50?b.tick_7f50():0u;b12w16(b,ent+3u,static_cast<u16>(t>>5));} static_assert(0x91BCu-0x8E3Cu==896u);''')
replace_fn('function_9378','static_assert(0x943Eu-0x9378u==198u);',r'''[[nodiscard]]u32 function_9378(Machine&m,MidAssetBoundaries&b,u32 kind)noexcept{u32 bg=0,bi=0,best=0xffffffffu;bool found=false;const u32 table=load_le32(dgroup_ptr(m,dgroup(0x2b08u)));for(s32 g=0;g<static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x2b04u))));++g){const u32 gu=static_cast<u32>(g),rec=table+gu*16u;if(ma32(b,rec+4u)==0u)continue;const u32 slots=ma32(b,rec+12u),n=ma32(b,rec+8u);for(s32 i=0;i<static_cast<s32>(n);++i){const u32 iu=static_cast<u32>(i),e=slots+iu*9u;if(static_cast<u32>(ma8(b,e+2u))!=kind||(ma8(b,e)&0x10u))continue;const u32 age=ma16(b,e+3u);if(!found||age<best){found=true;best=age;bg=gu;bi=iu;}}}if(!found)return 0u;function_91d8(m,b,bi,bg);return 1u;}static_assert(0x943Eu-0x9378u==198u);''')
replace_fn('function_94d0','static_assert(0x951Du-0x94D0u==77u);',r'''void function_94d0(Machine&m,MidAssetBoundaries&b)noexcept{for(s32 g=0;g<static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x2b04u))));++g){const u32 gu=static_cast<u32>(g),n=function_8e08(m,b,gu);for(s32 i=0;i<static_cast<s32>(n);++i)if(static_cast<u32>(i)!=load_le32(dgroup_ptr(m,dgroup(0x2b10u))))function_9488(m,b,static_cast<u32>(i),gu);}}static_assert(0x951Du-0x94D0u==77u);''')
replace_fn('function_9520','static_assert(0x954Cu-0x9520u==44u);',r'''void function_9520(Machine&m,MidAssetBoundaries&b)noexcept{function_8b94(m,b);const u32 i=load_le32(dgroup_ptr(m,dgroup(0x2b10u)));if(b.call_8e3c)b.call_8e3c(static_cast<s32>(i),0u);function_9440(m,b,i,0u);}static_assert(0x954Cu-0x9520u==44u);''')
replace_fn('function_954c','static_assert(0x95A8u-0x954Cu==92u);',r'''[[nodiscard]]u32 function_954c(Machine&m,MidAssetBoundaries&b,u32 index,u32 group)noexcept{if(b.call_8e3c)b.call_8e3c(static_cast<s32>(index),group);const u32 e=ma32(b,load_le32(dgroup_ptr(m,dgroup(0x2b08u)))+group*16u+12u)+index*9u;return ma8(b,e+2u)==1u?ma32(b,e+5u):0u;}static_assert(0x95A8u-0x954Cu==92u);''')

assert 'constexpr u32 kVerifiedTranslatedBytes=69361u;' in s
p.write_text(s)

a=Path('Code1/FINAL_BINARY_AUDIT.md')
audit=a.read_text()
marker='## Semantic audit checkpoint 8c — `8E3C..95A7`'
if marker not in audit:
    audit += '''\n\n## Semantic audit checkpoint 8c — `8E3C..95A7`\n\nFresh direct disassembly of the immutable original `CODE.1` reconciled the asset loader/cache lifecycle through `95A7`. `8E3C` now uses the encoded signed index-vs-count check, computes the nine-byte entry address with modulo-32-bit arithmetic, and preserves the `OR WORD PTR [entry],DX` side effect that clears entry byte `+1` while installing the type nibble before clearing the lock bit. The eviction scan `9378` and unlock sweep `94D0` now use signed `JL` loop termination, matching negative/high-bit group or entry counts rather than treating them as huge unsigned counts.\n\nThe two public cache entry points were also corrected to call the real internal loader. `9520` always invokes `8E3C(selected,0)` after `8B94`—including when `selected` is `FFFFFFFFh`, allowing `8E3C`'s bad-index path to own that case—then locks the selected entry with `9440`. `954C` likewise invokes `8E3C(index,group)` before returning the object pointer only when the entry state byte is exactly one. The previous generic release-boundary substitutions were not equivalent to these encoded loader calls. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.\n'''
    a.write_text(audit)
print('semantic checkpoint 8c applied')
