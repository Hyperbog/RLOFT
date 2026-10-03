from pathlib import Path

cpp_path=Path('Code1/Code1.cpp')
audit_path=Path('Code1/FINAL_BINARY_AUDIT.md')
progress_path=Path('Code1/PROGRESS.md')
continuity_path=Path('Code1/DECOMPILATION_CONTINUITY.md')
cpp=cpp_path.read_text()

def repl(old,new,count=1):
    global cpp
    n=cpp.count(old)
    if n!=count:
        raise SystemExit(f'expected {count} occurrence(s), found {n}: {old[:140]!r}')
    cpp=cpp.replace(old,new,count)

# 10415: OR DH,40h sets state bit 14.
repl('state|=0x00400000u;', 'state|=0x00004000u;')

# 1083F: successful DPMI 0503 path returns ESI = relocated arena + 24h.
repl('return r.carry?0u:r.linear;}previous=arena;', 'return r.carry?0u:new_arena+0x24u;}previous=arena;')

# 109C1: INT 31h AX=0006 result is consumed directly; no encoded zero-result branch.
repl('u32 linear=gb.segment_base_0006?gb.segment_base_0006(seg):0u;if(!linear){if(gb.dos_free_49)gb.dos_free_49(seg);return 0u;}hwrite(hb,linear+4u,bytes);',
     'u32 linear=gb.segment_base_0006?gb.segment_base_0006(seg):0u;hwrite(hb,linear+4u,bytes);')

# 10DD7: nonzero extender modes pass rounded 4 KiB page count, not raw break address.
repl('else ok=b.resize_break_other?b.resize_break_other(requested):false;',
     'else{u32 pages=(requested+0xfffu)>>12;if(pages==0u)pages=0x000fffffu;ok=b.resize_break_other?b.resize_break_other(pages):false;}')

# 10EE0: stream flag 40h forces close independent of threshold.
old='[[nodiscard]]s32 function_10ee0(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b,u32 index)noexcept{u32 threshold=0x539cu+index*0x1au,count=0;for(u32 node=load_le32(dgroup_ptr(m,dgroup(0x8e84)));node;){u32 next=tr32(b,node),stream=tr32(b,node+4u);if(stream>=threshold){u32 close_handle=((tr8(b,stream+0x0du)&0x40u)!=0u||stream>=0x541eu)?1u:0u;++count;(void)function_1104a(m,f,b,stream,close_handle);}node=next;}return static_cast<s32>(count);}'
new='[[nodiscard]]s32 function_10ee0(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b,u32 index)noexcept{u32 threshold=0x539cu+index*0x1au,count=0;for(u32 node=load_le32(dgroup_ptr(m,dgroup(0x8e84)));node;){u32 next=tr32(b,node),stream=tr32(b,node+4u);const bool forced=(tr8(b,stream+0x0du)&0x40u)!=0u;if(forced||stream>=threshold){u32 close_handle=(forced||stream>=0x541eu)?1u:0u;++count;(void)function_1104a(m,f,b,stream,close_handle);}node=next;}return static_cast<s32>(count);}'
repl(old,new)

# 1106E: JLE is signed.
repl('if(c>0x39u)c+=0x27u;', 'if(static_cast<s32>(c)>0x39)c+=0x27u;')

# Stream call graph: 11100 calls D85F and 11275; 11275 wraps D8AA.
repl('[[nodiscard]]s32 function_10c59(FileBoundaries&,u32)noexcept; [[nodiscard]]s32 function_111a4(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11241(FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11285(FileBoundaries&,RuntimeIoBoundaries&,u32,u32,u32)noexcept;',
     '[[nodiscard]]s32 function_10c59(FileBoundaries&,u32)noexcept; [[nodiscard]]s32 function_111a4(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11241(FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11275(FileBoundaries&,const char*)noexcept; [[nodiscard]]s32 function_11285(FileBoundaries&,RuntimeIoBoundaries&,u32,u32,u32)noexcept;')
old='[[nodiscard]]s32 function_11100(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream,u32 close_handle)noexcept{if(tr32(b,stream+0x0cu)==0u)return -1;s32 status=0;if(tr8(b,stream+0x0du)&0x10u)status=function_111a4(m,f,b,stream);s32 pos=function_11241(f,b,stream);if(pos!=-1&&f.dos_seek)(void)f.dos_seek(static_cast<u16>(tr32(b,stream+0x10u)),pos,0u);if(close_handle&&function_d573(f,tr32(b,stream+0x10u))!=0)status=-1;if(tr8(b,stream+0x0cu)&8u){if(b.free_node)b.free_node(tr32(b,stream+8u));tw32(b,stream+8u,0u);}if(tr8(b,stream+0x0du)&8u){char name[16]{};function_11081(b,name,tr8(b,stream+0x19u));if(b.unlink_path)(void)b.unlink_path(name);}return status;}'
new='[[nodiscard]]s32 function_11100(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream,u32 close_handle)noexcept{if(tr32(b,stream+0x0cu)==0u)return -1;s32 status=0;if(tr8(b,stream+0x0du)&0x10u)status=function_111a4(m,f,b,stream);s32 pos=function_11241(f,b,stream);if(pos!=-1)(void)function_d85f(f,static_cast<u16>(tr32(b,stream+0x10u)),pos,0u);if(close_handle&&function_d573(f,tr32(b,stream+0x10u))!=0)status=-1;if(tr8(b,stream+0x0cu)&8u){if(b.free_node)b.free_node(tr32(b,stream+8u));tw32(b,stream+8u,0u);}if(tr8(b,stream+0x0du)&8u){char name[16]{};function_11081(b,name,tr8(b,stream+0x19u));(void)function_11275(f,name);}return status;}'
repl(old,new)
repl('[[nodiscard]]s32 function_11275(RuntimeIoBoundaries&b,const char*path)noexcept{return b.unlink_path?b.unlink_path(path):-1;}',
     '[[nodiscard]]s32 function_11275(FileBoundaries&f,const char*path)noexcept{return function_d8aa(f,path);}')

# 11285: descriptor query is specifically 103C3 and raw write errors map through 10314.
old='[[nodiscard]]s32 function_11285(FileBoundaries&f,RuntimeIoBoundaries&b,u32 handle,u32 buffer,u32 count)noexcept{const u32 flags=f.call_103c3?f.call_103c3(handle):(b.fd_flags?b.fd_flags(handle):0u);if((flags&0x80u)!=0u&&function_d85f(f,static_cast<u16>(handle),0,2u)==-1)return -1;const s32 wrote=b.write_handle?b.write_handle(handle,buffer,count):-1;if(wrote<0)return -1;if(static_cast<u32>(wrote)!=count&&f.call_103b7)if(u32*p=f.call_103b7())*p=12u;return wrote;}'
new='[[nodiscard]]s32 function_11285(FileBoundaries&f,RuntimeIoBoundaries&b,u32 handle,u32 buffer,u32 count)noexcept{const u32 flags=f.call_103c3?f.call_103c3(handle):0u;if((flags&0x80u)!=0u&&function_d85f(f,static_cast<u16>(handle),0,2u)==-1)return -1;const s32 wrote=b.write_handle?b.write_handle(handle,buffer,count):-1;if(wrote<0)return map_dos_error(f,wrote);if(static_cast<u32>(wrote)!=count)set_runtime_error(f,12u);return wrote;}'
repl(old,new)

cpp_path.write_text(cpp)

audit=audit_path.read_text()
marker='## Post-closure semantic hardening checkpoint 10o — final runtime tail'
if marker not in audit:
    audit += '''\
\n\n## Post-closure semantic hardening checkpoint 10o — final runtime tail\n\nAfter the sequential audit had already reached object end, a deeper byte-level recheck of the final Watcom CRT/heap/stream tail found nine remaining normalization errors. This checkpoint does not extend the address frontier; it hardens the already-covered `103C3..11301` semantics against fresh direct disassembly of immutable `CODE.1`.\n\n`10415` now preserves `OR DH,40h` as descriptor-state bit `00004000h`. `1083F` now returns the successful DPMI-0503 relocated pointer carried in ESI, which is `new_arena+24h`, not the arena base. `109C1` no longer invents a null-result branch after the encoded INT 31h/0006 segment-base query. `10DD7` now passes the extender resize boundary the encoded rounded 4 KiB page count rather than the raw requested break address.\n\nThe stream tail received the remaining call-graph/control-flow fixes. In `10EE0`, stream flag 40h forces the close path even when the stream address is below the caller threshold. `1106E` uses the encoded signed `JLE` comparison. `11100` now reaches its seek adjustment through `D85F` and its temporary-file removal through the address-distinct `11275` wrapper; `11275` itself forwards to `D8AA`. Finally, `11285` always obtains descriptor state through `103C3` and maps low-level write errors through the DOS/runtime error mapper instead of collapsing them directly to -1.\n\nThis checkpoint supersedes the affected details in checkpoint 10n, in particular its earlier statement that `1083F` returned the relocated arena base. The stale post-10n `fix_semantic_audit_checkpoint10m.py` staging script was deliberately not executed because it mixed valid discoveries with replacements that had already been superseded by checkpoint 10n. Object classification and the sequential semantic frontier remain closed at **70,402 / 70,402 address bytes = 100.00%**, with **69,361 translated instructions + 1,041 proven non-code/data**, unresolved **0**.\n'''
    audit_path.write_text(audit)

progress=progress_path.read_text()
if 'semantic%20audit-100.00%25-brightgreen' not in progress:
    progress=progress.replace('![Progress](https://img.shields.io/badge/object%20accounting-100.00%25-brightgreen)\n',
        '![Progress](https://img.shields.io/badge/object%20accounting-100.00%25-brightgreen)\n![Semantic audit](https://img.shields.io/badge/semantic%20audit-100.00%25-brightgreen)\n')
    progress=progress.replace('**Unresolved object bytes:** 0  \n',
        '**Unresolved object bytes:** 0  \n**Sequential semantic audit:** **70,402 / 70,402 address bytes = 100.00%**  \n**Semantic frontier:** **`11301` (object end)**  \n')
progress=progress.replace('Total integrated verified translated bytes: **69,360**.', 'Total integrated verified translated bytes: **69,361**.')
progress_path.write_text(progress)

continuity=continuity_path.read_text()
cm='## CODE.1 semantic closure / next phase'
if cm not in continuity:
    continuity += '''\
\n## CODE.1 semantic closure / next phase\n\nCODE.1 object-1 byte classification is closed at 70,402 / 70,402 bytes, and the sequential semantic audit has reconciled the complete logical address span `00000..11301`. Post-closure checkpoint 10o performed an additional hardening pass over the final CRT/heap/stream runtime and corrected residual normalized behavior without changing byte accounting. There is no remaining CODE.1 sequential audit frontier.\n\nFuture CODE.1 work should therefore be evidence-driven backtracking only if new contradictory binary evidence appears. The next major decompilation phase is CODE.2, using the same immutable-source, exact-width/signedness, selector/far-pointer, register-alias, stack-slot, shared-tail, and syntax-verification rules documented above.\n'''
    continuity_path.write_text(continuity)

print('applied CODE.1 post-closure semantic hardening checkpoint 10o')
