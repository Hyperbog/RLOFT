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
        raise SystemExit(f'expected {count} occurrence(s), found {n}: {old[:120]!r}')
    cpp=cpp.replace(old,new,count)

# 103C3 uses signed JG against 5 after the unsigned descriptor-count bound.
repl('if(h<=5u){u8*entry=', 'if(static_cast<s32>(h)<=5){u8*entry=')

# 10415 OR DH,40h sets bit 14, not bit 22.
repl('state|=0x00400000u;', 'state|=0x00004000u;')

# 10452: only probe<ESP with distance > [5300] takes the safe RET.
old='void function_10452(Machine&m,StackGuardBoundary&b,u32 probe)noexcept{bool ok=false;if(probe<m.cpu.esp){const u32 distance=m.cpu.esp-probe;if(distance<=load_le32(dgroup_ptr(m,dgroup(0x5300u))))ok=true;}if(!ok&&m.cpu.ss==load_le16(dgroup_ptr(m,dgroup(0x55b0u)))){if(b.fatal_ce8e)b.fatal_ce8e(0x55b2u,1u);}}'
new='void function_10452(Machine&m,StackGuardBoundary&b,u32 probe)noexcept{bool check=true;if(probe<m.cpu.esp){const u32 distance=m.cpu.esp-probe;if(distance>load_le32(dgroup_ptr(m,dgroup(0x5300u))))check=false;}if(check&&m.cpu.ss==load_le16(dgroup_ptr(m,dgroup(0x55b0u)))){if(b.fatal_ce8e)b.fatal_ce8e(0x55b2u,1u);}}'
repl(old,new)

# 10974 needs the encoded LSL DS side effect for extender modes 2..8.
repl(' u32(*read32)(u32)=nullptr; void(*write32)(u32,u32)=nullptr;\n void(*dpmi_free_0502)(u32)=nullptr; void(*dos_free_49)(u16)=nullptr;',
     ' u32(*read32)(u32)=nullptr; void(*write32)(u32,u32)=nullptr; u32(*lsl_limit)(u16)=nullptr;\n void(*dpmi_free_0502)(u32)=nullptr; void(*dos_free_49)(u16)=nullptr;')
repl('[[nodiscard]]u32 function_10974(Machine&,HeapCoreBoundaries&,u32)noexcept;', '[[nodiscard]]u32 function_10974(Machine&,HeapCoreBoundaries&)noexcept;')
repl('function_10974(m,hb,load_le32(dgroup_ptr(m,dgroup(0x52f0))))', 'function_10974(m,hb)')
old='[[nodiscard]]u32 function_10974(Machine&m,HeapCoreBoundaries&b,u32 selector_limit)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x30b0)))==0u)return 0u;u32 a=load_le32(dgroup_ptr(m,dgroup(0x30d8)));u32 extent=hread(b,a);u32 end=a+extent+8u;if(end!=selector_limit)return 0u;return extent;}'
new='[[nodiscard]]u32 function_10974(Machine&m,HeapCoreBoundaries&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x30b0)))==0u)return 0u;const u32 a=load_le32(dgroup_ptr(m,dgroup(0x30d8)));const u32 extent=hread(b,a);const u32 end=a+extent+8u;const u8 mode=*dgroup_ptr(m,dgroup(0x531eu));if(mode>=2u&&mode<=8u&&*dgroup_ptr(m,dgroup(0x531fu))==0u){const u32 limit=b.lsl_limit?b.lsl_limit(m.cpu.ds):static_cast<u32>(m.cpu.ds);store_le32(dgroup_ptr(m,dgroup(0x52f0u)),limit+1u);}return end==load_le32(dgroup_ptr(m,dgroup(0x52f0u)))?extent:0u;}'
repl(old,new)

# 1083F returns ESI = new_arena+24h on successful 0503 resize, not 1069B's header pointer.
repl('u32 freep=function_1069b(m,hb,new_arena+0x14u);', '(void)function_1069b(m,hb,new_arena+0x14u);')
repl('return r.carry?0u:freep;', 'return r.carry?0u:new_arena+0x24u;')

# 109C1 accepts the linear base returned by DPMI 0006 without an encoded zero test.
old='u32 linear=gb.segment_base_0006?gb.segment_base_0006(seg):0u;if(!linear){if(gb.dos_free_49)gb.dos_free_49(seg);return 0u;}hwrite(hb,linear+4u,bytes);'
new='u32 linear=gb.segment_base_0006?gb.segment_base_0006(seg):0u;hwrite(hb,linear+4u,bytes);'
repl(old,new)

# 10DD7 nonzero-mode resize passes rounded 4 KiB pages in EBX.
repl('else ok=b.resize_break_other?b.resize_break_other(requested):false;', 'else{u32 pages=(requested+0xfffu)>>12;if(pages==0u)pages=0x000fffffu;ok=b.resize_break_other?b.resize_break_other(pages):false;}')

# 10E81 has no allocation-failure branch after DAD9(8).
repl('u32 node=b.alloc_node8?b.alloc_node8():0u;if(!node)break;tw32(b,node+4u,stream);', 'u32 node=b.alloc_node8?b.alloc_node8():0u;tw32(b,node+4u,stream);')

# 10EC6 tail-jumps into 10FFC after close-all.
repl('[[nodiscard]]s32 function_10ee0(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept;', '[[nodiscard]]s32 function_10ee0(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; void function_10ffc(Machine&,RuntimeIoBoundaries&)noexcept;')
repl('[[nodiscard]]s32 function_10ec6(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b)noexcept{(void)function_10ee0(m,f,b,0u);return 0;}', '[[nodiscard]]s32 function_10ec6(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b)noexcept{(void)function_10ee0(m,f,b,0u);function_10ffc(m,b);return 0;}')

# 10EE0: bit 40h forces close even below the threshold.
old='[[nodiscard]]s32 function_10ee0(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b,u32 index)noexcept{u32 threshold=0x539cu+index*0x1au,count=0;for(u32 node=load_le32(dgroup_ptr(m,dgroup(0x8e84)));node;){u32 next=tr32(b,node),stream=tr32(b,node+4u);if(stream>=threshold){u32 close_handle=((tr8(b,stream+0x0du)&0x40u)!=0u||stream>=0x541eu)?1u:0u;++count;(void)function_1104a(m,f,b,stream,close_handle);}node=next;}return static_cast<s32>(count);}'
new='[[nodiscard]]s32 function_10ee0(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b,u32 index)noexcept{u32 threshold=0x539cu+index*0x1au,count=0;for(u32 node=load_le32(dgroup_ptr(m,dgroup(0x8e84)));node;){u32 next=tr32(b,node),stream=tr32(b,node+4u);const bool forced=(tr8(b,stream+0x0du)&0x40u)!=0u;if(forced||stream>=threshold){u32 close_handle=(forced||stream>=0x541eu)?1u:0u;++count;(void)function_1104a(m,f,b,stream,close_handle);}node=next;}return static_cast<s32>(count);}'
repl(old,new)

# 1106E's JLE is signed.
repl('if(c>0x39u)c+=0x27u;', 'if(static_cast<s32>(c)>0x39)c+=0x27u;')

# Stream layer: restore the encoded D85F/D8AA/11285 wrappers and modulo arithmetic.
repl('[[nodiscard]]s32 function_111a4(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11241(FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept;',
     '[[nodiscard]]s32 function_111a4(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11241(FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11285(FileBoundaries&,RuntimeIoBoundaries&,u32,u32,u32)noexcept;')
old='[[nodiscard]]s32 function_11100(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream,u32 close_handle)noexcept{if(tr32(b,stream+0x0cu)==0u)return -1;s32 status=0;if(tr8(b,stream+0x0du)&0x10u)status=function_111a4(m,f,b,stream);s32 pos=function_11241(f,b,stream);if(pos!=-1&&f.dos_seek)(void)f.dos_seek(static_cast<u16>(tr32(b,stream+0x10u)),pos,0u);if(close_handle&&function_d573(f,tr32(b,stream+0x10u))!=0)status=-1;if(tr8(b,stream+0x0cu)&8u){if(b.free_node)b.free_node(tr32(b,stream+8u));tw32(b,stream+8u,0u);}if(tr8(b,stream+0x0du)&8u){char name[16]{};function_11081(b,name,tr8(b,stream+0x19u));if(b.unlink_path)(void)b.unlink_path(name);}return status;}'
new='[[nodiscard]]s32 function_11100(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream,u32 close_handle)noexcept{if(tr32(b,stream+0x0cu)==0u)return -1;s32 status=0;if(tr8(b,stream+0x0du)&0x10u)status=function_111a4(m,f,b,stream);s32 pos=function_11241(f,b,stream);if(pos!=-1)(void)function_d85f(f,static_cast<u16>(tr32(b,stream+0x10u)),pos,0u);if(close_handle&&function_d573(f,tr32(b,stream+0x10u))!=0)status=-1;if(tr8(b,stream+0x0cu)&8u){if(b.free_node)b.free_node(tr32(b,stream+8u));tw32(b,stream+8u,0u);}if(tr8(b,stream+0x0du)&8u){char name[16]{};function_11081(b,name,tr8(b,stream+0x19u));(void)function_d8aa(f,name);}return status;}'
repl(old,new)
old='[[nodiscard]]s32 function_111a4(Machine&,FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream)noexcept{s32 status=0;if(!(tr8(b,stream+0x0du)&0x10u))goto finish;tw8(b,stream+0x0du,static_cast<u8>(tr8(b,stream+0x0du)&0xefu));if(tr8(b,stream+0x0cu)&2u){if(tr32(b,stream+8u)){s32 wrote=b.write_handle?b.write_handle(tr32(b,stream+0x10u),tr32(b,stream+8u),tr32(b,stream+4u)):-1;if(wrote==-1)status=-1;else if(static_cast<u32>(wrote)!=tr32(b,stream+4u))status=-1;}}else if(tr32(b,stream+8u)){tw8(b,stream+0x0cu,static_cast<u8>(tr8(b,stream+0x0cu)&0xefu));if(!(tr8(b,stream+0x0du)&0x20u)){s32 n=static_cast<s32>(tr32(b,stream+4u));if(n!=0&&f.dos_seek&&f.dos_seek(static_cast<u16>(tr32(b,stream+0x10u)),-n,1u)==-1)status=-1;}}if(status==-1)tw8(b,stream+0x0cu,static_cast<u8>(tr8(b,stream+0x0cu)|0x20u));finish:tw32(b,stream,tr32(b,stream+8u));tw32(b,stream+4u,0u);return status;}'
new='[[nodiscard]]s32 function_111a4(Machine&,FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream)noexcept{s32 status=0;if(!(tr8(b,stream+0x0du)&0x10u))goto finish;tw8(b,stream+0x0du,static_cast<u8>(tr8(b,stream+0x0du)&0xefu));if(tr8(b,stream+0x0cu)&2u){if(tr32(b,stream+8u)){const u32 count=tr32(b,stream+4u);const s32 wrote=function_11285(f,b,tr32(b,stream+0x10u),tr32(b,stream+8u),count);if(wrote==-1)status=-1;else if(static_cast<u32>(wrote)!=count){set_runtime_error(f,12u);status=-1;}}}else if(tr32(b,stream+8u)){tw8(b,stream+0x0cu,static_cast<u8>(tr8(b,stream+0x0cu)&0xefu));if(!(tr8(b,stream+0x0du)&0x20u)){const u32 unread=tr32(b,stream+4u);if(unread!=0u&&function_d85f(f,static_cast<u16>(tr32(b,stream+0x10u)),static_cast<s32>(0u-unread),1u)==-1)status=-1;}}if(status==-1)tw8(b,stream+0x0cu,static_cast<u8>(tr8(b,stream+0x0cu)|0x20u));finish:tw32(b,stream,tr32(b,stream+8u));tw32(b,stream+4u,0u);return status;}'
repl(old,new)
old='[[nodiscard]]s32 function_11241(FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream)noexcept{s32 p=f.dos_seek?f.dos_seek(static_cast<u16>(tr32(b,stream+0x10u)),0,1u):-1;if(p==-1)return -1;u32 buffered=tr32(b,stream+4u);if(buffered)p+=(tr8(b,stream+0x0du)&0x10u)?static_cast<s32>(buffered):-static_cast<s32>(buffered);return p;}'
new='[[nodiscard]]s32 function_11241(FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream)noexcept{const s32 raw=function_10c59(f,tr32(b,stream+0x10u));if(raw==-1)return -1;u32 p=static_cast<u32>(raw);const u32 buffered=tr32(b,stream+4u);if(buffered)p=(tr8(b,stream+0x0du)&0x10u)?p+buffered:p-buffered;return static_cast<s32>(p);}'
repl(old,new)
repl('[[nodiscard]]s32 function_11275(RuntimeIoBoundaries&b,const char*path)noexcept{return b.unlink_path?b.unlink_path(path):-1;}', '[[nodiscard]]s32 function_11275(FileBoundaries&f,const char*path)noexcept{return function_d8aa(f,path);}')
old='[[nodiscard]]s32 function_11285(FileBoundaries&f,RuntimeIoBoundaries&b,u32 handle,u32 buffer,u32 count)noexcept{if(b.fd_flags&&(b.fd_flags(handle)&0x80u)){if(f.dos_seek&&f.dos_seek(static_cast<u16>(handle),0,2u)==-1)return -1;}s32 wrote=b.write_handle?b.write_handle(handle,buffer,count):-1;if(wrote<0)return -1;if(static_cast<u32>(wrote)!=count&&f.call_103b7)if(u32*p=f.call_103b7())*p=12u;return wrote;}'
new='[[nodiscard]]s32 function_11285(FileBoundaries&f,RuntimeIoBoundaries&b,u32 handle,u32 buffer,u32 count)noexcept{const u32 flags=f.call_103c3?f.call_103c3(handle):0u;if((flags&0x80u)!=0u&&function_d85f(f,static_cast<u16>(handle),0,2u)==-1)return -1;const s32 wrote=b.write_handle?b.write_handle(handle,buffer,count):-1;if(wrote<0)return map_dos_error(f,wrote);if(static_cast<u32>(wrote)!=count)set_runtime_error(f,12u);return wrote;}'
repl(old,new)

cpp_path.write_text(cpp)

audit=audit_path.read_text()
marker='## Semantic audit checkpoint 10m — `FC98..11301`'
if marker not in audit:
    audit += '''\n\n## Semantic audit checkpoint 10m — `FC98..11301`\n\nFresh direct disassembly of immutable `CODE.1` reconciled the complete remaining object tail: the launch and runtime-record dispatchers, argv tokenizer/build path, FEFC/FF57 register bridges and all 256 fixed interrupt veneers, CRT descriptor/stack helpers, the Watcom guest heap core and growth paths, optimized string compare, break-management helpers, stream registry/close/flush routines, and the terminal low-level write wrapper through the final byte at `11301`. The source and extracted-object SHA-256 values were reverified before this pass.\n\nThe CRT/descriptor block required three direct corrections. `103C3` uses signed `JG` for its handle-versus-5 test after the unsigned descriptor-count bound. `10415` executes `OR DH,40h`, which sets descriptor-state bit `00004000h`; the previous lift incorrectly set `00400000h`. `10452` also had its stack-guard branch inverted: when the probe is below ESP, only a distance strictly greater than `[5300]` takes the safe return; the other cases proceed to the saved-SS check and fatal path.\n\nThe heap-growth audit corrected several machine-visible details. `1083F` returns successful resize payload pointer `new_arena+24h` (live ESI), not the free-block header returned by `1069B`. `10974` now performs the encoded mode-2..8 `LSL DS` limit refresh into `52F0` before comparing the terminal extent, and no longer accepts a fictitious selector-limit argument. `109C1` no longer invents a zero-linear-address failure after DPMI function 0006. `10DD7` now passes the encoded rounded 4-KiB page count to the nonzero-mode resize boundary.\n\nThe stream-runtime tail also required corrections. `10E81` has no allocation-failure test after its eight-byte node allocation. `10EC6` tail-enters the recycled-node cleanup at `10FFC`. In `10EE0`, stream flag `40h` forces closing even when the stream record lies below the caller threshold. `1106E` uses signed `JLE`. `11100`, `111A4`, `11241`, `11275`, and `11285` now retain the encoded `D85F`/`D8AA`/`11285` wrapper calls and DOS-error mapping instead of bypassing them through raw host-style boundaries; the backward unread-count seek is formed with modulo-32-bit NEG semantics, and logical-position adjustment is also wrap-preserving.\n\nThe remaining tail routines, including `FC98..FEFB`, the FEFC/FF57 bridges, `FFD4..102D3`, `104E0..1083E`, `109C1..10D39`, and the unaffected stream/list paths, were instruction-reconciled without further changes. Known non-code/alignment bytes inside the span (`FCE7`, `10436..10437`, `104DE..104DF`, and `10C6D..10C6E`) remain classified exactly as before.\n\nThis closes the sequential semantic audit at the end of object 1: **`00000..11301` = 70,402 / 70,402 address bytes = 100.00%**. Object classification remains independently closed at **69,361 translated instruction bytes + 1,041 proven non-code/data bytes = 70,402 / 70,402**, with **0 unresolved bytes**.\n'''
    audit_path.write_text(audit)

progress=progress_path.read_text()
if 'semantic%20audit-100.00%25-brightgreen' not in progress:
    progress=progress.replace('![Progress](https://img.shields.io/badge/object%20accounting-100.00%25-brightgreen)\n', '![Progress](https://img.shields.io/badge/object%20accounting-100.00%25-brightgreen)\n![Semantic audit](https://img.shields.io/badge/semantic%20audit-100.00%25-brightgreen)\n')
    progress=progress.replace('**Status color:** GREEN — all object-1 bytes are now translated instructions or directly proven non-code/data.\n', '**Status color:** GREEN — all object-1 bytes are translated instructions or directly proven non-code/data, and the end-to-end semantic audit is complete through `11301`.\n\n**Sequential semantic audit:** **70,402 / 70,402 = 100.00%**  \n**Semantic bytes remaining:** **0**  \n')
progress=progress.replace('Total integrated verified translated bytes: **69,360**.', 'Total integrated verified translated bytes: **69,361**.')
progress_path.write_text(progress)

continuity=continuity_path.read_text()
if 'Semantic completion checkpoint 10m' not in continuity:
    continuity += '''\n\n## Semantic completion checkpoint 10m\n\nThe end-to-end CODE.1 semantic audit is complete through logical object offset `11301`, the final byte of the `0x11302`-byte code object. Sequential semantic coverage is **70,402 / 70,402 = 100.00%**, with zero semantic frontier bytes remaining. Byte classification remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402**, unresolved zero. The final tail checkpoint corrected `103C3`, `10415`, `10452`, `1083F`, `10974`, `109C1`, `10DD7`, `10E81`, `10EC6`, `10EE0`, `1106E`, and the `11100..11285` stream/error-mapping paths.\n\nFor any future CODE.1 work, treat `FINAL_BINARY_AUDIT.md` as the completed sequential audit record and only reopen a range when new byte-level evidence demonstrates a discrepancy. The next project phase is CODE.2 decompilation/audit when requested.\n'''
    continuity_path.write_text(continuity)

print('applied final semantic audit checkpoint 10m')
