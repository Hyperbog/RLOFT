from pathlib import Path

cpp_p=Path('Code1/Code1.cpp')
audit_p=Path('Code1/FINAL_BINARY_AUDIT.md')
cpp=cpp_p.read_text()

old='return static_cast<s16>(tree_word(m,best_node,3u));'
new='u32 result=static_cast<u32>(static_cast<s32>(best_node));set_low16(result,load_le16(dgroup_ptr(m,dgroup(0x7ff0u+static_cast<u32>(static_cast<s32>(best_node)*9)+3u))));return static_cast<s32>(result);'
if old in cpp:
    assert cpp.count(old)==1
    cpp=cpp.replace(old,new,1)
else:
    assert new in cpp

old='if(function_5d90(m,0,0u)==function_5d90(m,1,1u)&&*dgroup_ptr(m,dgroup(0x680cu))==*dgroup_ptr(m,dgroup(0x6a54u))){for(u32 i=0;i<10u;++i){const s16 v=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x6a80u+i*2u))));if(v<0)continue;const u32 q=load_le32(dgroup_ptr(m,dgroup(0x7128u)))+static_cast<u32>(v)*65u+0x2fu;if(b.read_guest8&&b.read_guest8(q)==1u)store_le16(dgroup_ptr(m,dgroup(0x6a80u+i*2u)),0xffffu);}}'
new='{const u8 family0=function_5d90(m,0,2u);const u8 family1=function_5d90(m,1,family0);if(family0==family1&&*dgroup_ptr(m,dgroup(0x680cu))==*dgroup_ptr(m,dgroup(0x6a54u))){for(u32 i=0;i<10u;++i){const s16 v=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x6a80u+i*2u))));const u32 q=load_le32(dgroup_ptr(m,dgroup(0x7128u)))+static_cast<u32>(v)*65u+0x2fu;if(b.read_guest8&&b.read_guest8(q)==1u)store_le16(dgroup_ptr(m,dgroup(0x6a80u+i*2u)),0xffffu);}}}'
if old in cpp:
    assert cpp.count(old)==1
    cpp=cpp.replace(old,new,1)
else:
    assert new in cpp

needle='// 5B64..5D8E: exact iterative nearest-tree search. The explicit 16-bit stack/count globals and 0x101 sentinel are retained.\n'
comment='// Final audit: the return deliberately overwrites AX only, preserving EAX upper bits from the sign-extended node index.\n'
assert needle in cpp
if comment not in cpp:
    cpp=cpp.replace(needle,needle+comment,1)
needle2='// 5F4C..6053: populate both actor runtime tables from class-family/class-index records and invalidate conflicting 6A80 entries.\n'
comment2='// Final audit: the second family probe inherits BL=2, then the first family result; negative 6A80 words are not prefiltered.\n'
assert needle2 in cpp
if comment2 not in cpp:
    cpp=cpp.replace(needle2,needle2+comment2,1)

assert 'constexpr u32 kVerifiedTranslatedBytes=69361u;' in cpp
cpp_p.write_text(cpp)

audit=audit_p.read_text()
marker='## Semantic audit checkpoint 4 — `4978..6053`'
if marker not in audit:
    audit += '''\n\n## Semantic audit checkpoint 4 — `4978..6053`\n\nFresh disassembly from the immutable original `CODE.1` reconciled the post-generation controller, palette transforms/tree builder/search, cursor state routines, class-family mapper, and runtime-table population through `6053`. Three semantic corrections were required. In `5B64`, the final return is a partial-register operation: the binary sign-extends the selected node into EAX and then overwrites only AX with the palette index; the initialized-tree no-candidate sentinel therefore returns `0000FFFFh` (65535), not sign-extended `FFFFFFFFh`. In the final `5F4C` family comparison, BL enters the actor-0 `5D90` call as `2` from the completed actor loop, then BL is replaced by actor 0's returned family before the actor-1 call; the previous C++ incorrectly supplied fallback values 0 and 1. Finally, the encoded `6A80` invalidation loop does not skip negative words: it sign-extends the word, multiplies it by 65, and probes the resulting address exactly as encoded.\n\nThe cursor/page routines `50C0..5878`, tree insertion/build routines `5880..5B63`, the class-family decision tree `5D90`, and the remaining resource/palette routines in this checkpoint were rechecked. The unusual `MOVZX r16` sequences in `59AC`/`5B64` were retained or shown equivalent at the low-word comparison points rather than normalized by assumption. Object accounting remains **69,361 translated instruction bytes + 1,041 proven non-code/data bytes = 70,402 / 70,402**, with **0 unresolved bytes**.\n'''
    audit_p.write_text(audit)

print('semantic checkpoint 4 applied')
