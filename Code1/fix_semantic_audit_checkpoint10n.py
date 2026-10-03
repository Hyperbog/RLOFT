from pathlib import Path

cpp_path=Path('Code1/Code1.cpp')
audit_path=Path('Code1/FINAL_BINARY_AUDIT.md')
cpp=cpp_path.read_text()

# The LSL DS result is a runtime boundary: callers need the inclusive selector limit.
old="""struct HeapCoreBoundaries{
 u32(*read32)(u32)=nullptr; void(*write32)(u32,u32)=nullptr;
 void(*dpmi_free_0502)(u32)=nullptr; void(*dos_free_49)(u16)=nullptr;
};
"""
new="""struct HeapCoreBoundaries{
 u32(*read32)(u32)=nullptr; void(*write32)(u32,u32)=nullptr;
 void(*dpmi_free_0502)(u32)=nullptr; void(*dos_free_49)(u16)=nullptr;
 u32(*selector_limit)(u16)=nullptr;
};
"""
assert old in cpp
cpp=cpp.replace(old,new,1)

# 1083F returns the relocated DPMI linear arena base (ESI), not the 1069B free-block pointer.
old="u32 freep=function_1069b(m,hb,new_arena+0x14u);"
new="(void)function_1069b(m,hb,new_arena+0x14u);"
assert old in cpp
cpp=cpp.replace(old,new,1)
old="return r.carry?0u:freep;}previous=arena;"
new="return r.carry?0u:r.linear;}previous=arena;"
assert old in cpp
cpp=cpp.replace(old,new,1)

# 10974 has no caller argument. In extender modes 2..8 with 531F==0 it refreshes 52F0 from LSL DS + 1.
old="[[nodiscard]]u32 function_10974(Machine&,HeapCoreBoundaries&,u32)noexcept;"
new="[[nodiscard]]u32 function_10974(Machine&,HeapCoreBoundaries&)noexcept;"
assert old in cpp
cpp=cpp.replace(old,new,1)
old="""[[nodiscard]]u32 function_10974(Machine&m,HeapCoreBoundaries&b,u32 selector_limit)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x30b0)))==0u)return 0u;u32 a=load_le32(dgroup_ptr(m,dgroup(0x30d8)));u32 extent=hread(b,a);u32 end=a+extent+8u;if(end!=selector_limit)return 0u;return extent;}
"""
new="""[[nodiscard]]u32 function_10974(Machine&m,HeapCoreBoundaries&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x30b0)))==0u)return 0u;const u32 a=load_le32(dgroup_ptr(m,dgroup(0x30d8)));const u32 extent=hread(b,a);const u32 end=a+extent+8u;const u8 mode=*dgroup_ptr(m,dgroup(0x531eu));if(mode>=2u&&mode<=8u&&*dgroup_ptr(m,dgroup(0x531fu))==0u&&b.selector_limit)store_le32(dgroup_ptr(m,dgroup(0x52f0u)),b.selector_limit(m.cpu.ds)+1u);if(end!=load_le32(dgroup_ptr(m,dgroup(0x52f0u))))return 0u;return extent;}
"""
assert old in cpp
cpp=cpp.replace(old,new,1)

# 109C1 keeps the DOS probe block allocated during the requested AH=48 allocation, then frees the probe.
old="""u16 probe=0;const u16 probe_par=static_cast<u16>((load_le32(dgroup_ptr(m,dgroup(0x8e88)))>>4)|1u);if(!gb.dos_alloc_48||!gb.dos_alloc_48(probe_par,probe))return 0u;if(gb.dos_free_49)gb.dos_free_49(probe);u16 seg=0;if(!gb.dos_alloc_48(static_cast<u16>(bytes>>4),seg))return 0u;u32 linear=gb.segment_base_0006?gb.segment_base_0006(seg):0u;"""
new="""u16 probe=0;const u16 probe_par=static_cast<u16>((load_le32(dgroup_ptr(m,dgroup(0x8e88)))>>4)|1u);if(!gb.dos_alloc_48||!gb.dos_alloc_48(probe_par,probe))return 0u;u16 seg=0;const bool got_requested=gb.dos_alloc_48(static_cast<u16>(bytes>>4),seg);if(gb.dos_free_49)gb.dos_free_49(probe);if(!got_requested)return 0u;u32 linear=gb.segment_base_0006?gb.segment_base_0006(seg):0u;"""
assert old in cpp
cpp=cpp.replace(old,new,1)

# 10AA9 calls the no-argument 10974, fixes mode-1 header placement, and refreshes 52F0 from LSL DS in modes 2..8.
old="else size-=function_10974(m,hb,load_le32(dgroup_ptr(m,dgroup(0x52f0))));"
new="else size-=function_10974(m,hb);"
assert old in cpp
cpp=cpp.replace(old,new,1)
old="""if(mode==1u){raw=function_109c1(m,hb,gb,size);usable-=0x14u;}else if(mode==9u){raw=gb.legacy_alloc_80004800?gb.legacy_alloc_80004800(size):0u;usable-=8u;if(raw){hwrite(hb,raw,0u);raw+=8u;}}else{u32 limit=load_le32(dgroup_ptr(m,dgroup(0x52f0)));"""
new="""if(mode==1u){const u32 acquired=function_109c1(m,hb,gb,size);usable-=0x14u;if(acquired!=0u){hwrite(hb,acquired,0u);raw=acquired+8u;}}else if(mode==9u){raw=gb.legacy_alloc_80004800?gb.legacy_alloc_80004800(size):0u;usable-=8u;if(raw){hwrite(hb,raw,0u);raw+=8u;}}else{if(mode>=2u&&mode<=8u&&hb.selector_limit)store_le32(dgroup_ptr(m,dgroup(0x52f0u)),hb.selector_limit(m.cpu.ds)+1u);u32 limit=load_le32(dgroup_ptr(m,dgroup(0x52f0)));"""
assert old in cpp
cpp=cpp.replace(old,new,1)

# The fixed-stream registration loop dereferences the DAC9 result unconditionally; no null-safety branch is encoded.
old="u32 node=b.alloc_node8?b.alloc_node8():0u;if(!node)break;tw32(b,node+4u,stream);"
new="u32 node=b.alloc_node8?b.alloc_node8():0u;tw32(b,node+4u,stream);"
assert old in cpp
cpp=cpp.replace(old,new,1)

# 10EC6 tail-jumps to 10FFC after closing; expose it as side-effect-only rather than inventing EAX=0.
old="[[nodiscard]]s32 function_1104a(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32,u32)noexcept; void function_10fc9(Machine&,RuntimeIoBoundaries&,u32)noexcept;"
new="[[nodiscard]]s32 function_1104a(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32,u32)noexcept; void function_10fc9(Machine&,RuntimeIoBoundaries&,u32)noexcept; void function_10ffc(Machine&,RuntimeIoBoundaries&)noexcept;"
assert old in cpp
cpp=cpp.replace(old,new,1)
old="[[nodiscard]]s32 function_10ec6(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b)noexcept{(void)function_10ee0(m,f,b,0u);return 0;}"
new="void function_10ec6(Machine&m,FileBoundaries&f,RuntimeIoBoundaries&b)noexcept{(void)function_10ee0(m,f,b,0u);function_10ffc(m,b);}"
assert old in cpp
cpp=cpp.replace(old,new,1)

# Both DAC9 failures in 10F2E set runtime errno 5 before returning null.
old="node=b.alloc_node8?b.alloc_node8():0u;if(!node)return 0u;flags=3u;"
new="node=b.alloc_node8?b.alloc_node8():0u;if(!node){store_le32(dgroup_ptr(m,dgroup(0x8e68u)),5u);return 0u;}flags=3u;"
assert old in cpp
cpp=cpp.replace(old,new,1)
old="node=b.alloc_stream34?b.alloc_stream34():0u;if(!node)return 0u;flags=0x4003u;stream=node+8u;"
new="node=b.alloc_stream34?b.alloc_stream34():0u;if(!node){store_le32(dgroup_ptr(m,dgroup(0x8e68u)),5u);return 0u;}flags=0x4003u;stream=node+8u;"
assert old in cpp
cpp=cpp.replace(old,new,1)

# 111A4 calls the encoded 11285 low-level writer and D85F seek wrapper. 11241 calls 10C59.
old="[[nodiscard]]s32 function_111a4(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11241(FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept;"
new="[[nodiscard]]s32 function_111a4(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11241(FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11285(FileBoundaries&,RuntimeIoBoundaries&,u32,u32,u32)noexcept;"
assert old in cpp
cpp=cpp.replace(old,new,1)
old="""if(tr8(b,stream+0x0cu)&2u){if(tr32(b,stream+8u)){s32 wrote=b.write_handle?b.write_handle(tr32(b,stream+0x10u),tr32(b,stream+8u),tr32(b,stream+4u)):-1;if(wrote==-1)status=-1;else if(static_cast<u32>(wrote)!=tr32(b,stream+4u))status=-1;}}else if(tr32(b,stream+8u)){tw8(b,stream+0x0cu,static_cast<u8>(tr8(b,stream+0x0cu)&0xefu));if(!(tr8(b,stream+0x0du)&0x20u)){s32 n=static_cast<s32>(tr32(b,stream+4u));if(n!=0&&f.dos_seek&&f.dos_seek(static_cast<u16>(tr32(b,stream+0x10u)),-n,1u)==-1)status=-1;}}"""
new="""if(tr8(b,stream+0x0cu)&2u){if(tr32(b,stream+8u)){const u32 count=tr32(b,stream+4u);const s32 wrote=function_11285(f,b,tr32(b,stream+0x10u),tr32(b,stream+8u),count);if(wrote==-1)status=-1;else if(static_cast<u32>(wrote)!=count){if(f.call_103b7)if(u32*p=f.call_103b7())*p=12u;status=-1;}}}else if(tr32(b,stream+8u)){tw8(b,stream+0x0cu,static_cast<u8>(tr8(b,stream+0x0cu)&0xefu));if(!(tr8(b,stream+0x0du)&0x20u)){const u32 n=tr32(b,stream+4u);if(n!=0u&&function_d85f(f,static_cast<u16>(tr32(b,stream+0x10u)),static_cast<s32>(0u-n),1u)==-1)status=-1;}}"""
assert old in cpp
cpp=cpp.replace(old,new,1)
old="""[[nodiscard]]s32 function_11241(FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream)noexcept{s32 p=f.dos_seek?f.dos_seek(static_cast<u16>(tr32(b,stream+0x10u)),0,1u):-1;if(p==-1)return -1;u32 buffered=tr32(b,stream+4u);if(buffered)p+=(tr8(b,stream+0x0du)&0x10u)?static_cast<s32>(buffered):-static_cast<s32>(buffered);return p;}"""
new="""[[nodiscard]]s32 function_11241(FileBoundaries&f,RuntimeIoBoundaries&b,u32 stream)noexcept{const s32 p=function_10c59(f,tr32(b,stream+0x10u));if(p==-1)return -1;const u32 buffered=tr32(b,stream+4u);u32 out=static_cast<u32>(p);if(buffered!=0u)out=(tr8(b,stream+0x0du)&0x10u)?out+buffered:out-buffered;return static_cast<s32>(out);}"""
assert old in cpp
cpp=cpp.replace(old,new,1)

# 11285 explicitly calls 103C3 and maps its seek-to-end through the same D85F error semantics.
old="""[[nodiscard]]s32 function_11285(FileBoundaries&f,RuntimeIoBoundaries&b,u32 handle,u32 buffer,u32 count)noexcept{if(b.fd_flags&&(b.fd_flags(handle)&0x80u)){if(f.dos_seek&&f.dos_seek(static_cast<u16>(handle),0,2u)==-1)return -1;}s32 wrote=b.write_handle?b.write_handle(handle,buffer,count):-1;if(wrote<0)return -1;if(static_cast<u32>(wrote)!=count&&f.call_103b7)if(u32*p=f.call_103b7())*p=12u;return wrote;}"""
new="""[[nodiscard]]s32 function_11285(FileBoundaries&f,RuntimeIoBoundaries&b,u32 handle,u32 buffer,u32 count)noexcept{const u32 flags=f.call_103c3?f.call_103c3(handle):(b.fd_flags?b.fd_flags(handle):0u);if((flags&0x80u)!=0u&&function_d85f(f,static_cast<u16>(handle),0,2u)==-1)return -1;const s32 wrote=b.write_handle?b.write_handle(handle,buffer,count):-1;if(wrote<0)return -1;if(static_cast<u32>(wrote)!=count&&f.call_103b7)if(u32*p=f.call_103b7())*p=12u;return wrote;}"""
assert old in cpp
cpp=cpp.replace(old,new,1)

cpp_path.write_text(cpp)

audit=audit_path.read_text()
entry="""

## Semantic audit checkpoint 10n — `1083F..11301`

Fresh direct disassembly of immutable `CODE.1` reconciled the final arena-growth, program-break, stream-registration/cleanup, buffered-I/O, temporary-file, seek/write, and runtime-tail routines through the final object byte at `11301`. The original `CODE.1` SHA-256 and extracted object SHA-256 were reverified before this pass.

`1083F` was corrected to return the successful DPMI 0503 relocated **linear arena base** carried in ESI, not the free-block pointer returned by `1069B`; failure still rebuilds the old arena and returns zero. `10974` was restored to its actual no-argument ABI and mode-sensitive side effect: in extender modes 2..8 while byte `531F` is zero, it executes the semantic equivalent of `LSL DS`, increments the inclusive limit, and writes global `52F0` before comparing the top arena end against that global. The `LSL` operation is now an explicit selector-limit boundary.

`109C1` now preserves the first DOS AH=48 probe allocation while attempting the requested second allocation, then frees the probe and only afterward tests the second result. The former reconstruction freed the probe first, changing DOS allocation availability. `10AA9` now performs the separate unconditional modes-2..8 `LSL DS` refresh encoded at `10B67`, and its mode-1 path correctly zeroes the word-owner/header dword at the address returned by `109C1` and uses **returned+8** as the arena block base; the previous lift was eight bytes early.

In the stream tail, `10E81` no longer invents a null-allocation escape before the encoded node dereference. `10EC6` now preserves its tail jump into `10FFC`, releasing every recycled registration node after closing streams. Both allocation-failure exits of `10F2E` now set runtime errno 5 through the `103B7` cell before returning null.

The buffered-I/O paths were also corrected to follow their encoded call graph. `111A4` calls `11285` for write-mode flushes and `D85F` for read-mode seek-back, including modulo-32-bit negation of the unread count; short writes set errno 12 before the error flag is latched. `11241` now obtains the base handle position through the `10C59 -> D85F` wrapper and adjusts buffered counts with modulo-32-bit add/subtract. `11285` queries descriptor flags through the `103C3` boundary and maps its optional seek-to-end through the same D85F error semantics before the low-level write.

The remaining allocator/break paths (`10C46..10E80`), stream-list walks and close/unregister routines, temporary-name construction, close driver, unlink wrapper, and final low-level write return behavior were rechecked directly and require no additional semantic changes in this pass. This reaches the end of object 1: the sequential semantic audit frontier is **`11301`**, covering the complete **70,402-byte (`0x11302`) object address span**. Object classification remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, unresolved **0**.
"""
assert 'Semantic audit checkpoint 10n' not in audit
audit_path.write_text(audit+entry)
print('applied semantic audit checkpoint 10n')
