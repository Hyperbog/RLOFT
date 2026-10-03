from pathlib import Path

cpp_path=Path('Code1/Code1.cpp')
audit_path=Path('Code1/FINAL_BINARY_AUDIT.md')
cpp=cpp_path.read_text()

old="""[[nodiscard]]u32 function_103c3(Machine&m,RuntimeIoctlBoundary&b,u32 h)noexcept{const u32 n=load_le32(dgroup_ptr(m,dgroup(0x5344u)));if(h>=n)return 0u;const u32 table=load_le32(dgroup_ptr(m,dgroup(0x5398u)));if(h<=5u){u8*entry=dgroup_ptr(m,dgroup(table+h*4u));if((entry[1]&0x40u)==0){entry[1]|=0x40u;if(function_10397(b,static_cast<u16>(h)))entry[1]|=0x20u;}}return load_le32(dgroup_ptr(m,dgroup(table+h*4u)));}
"""
new="""[[nodiscard]]u32 function_103c3(Machine&m,RuntimeIoctlBoundary&b,u32 h)noexcept{const u32 n=load_le32(dgroup_ptr(m,dgroup(0x5344u)));if(h>=n)return 0u;const u32 table=load_le32(dgroup_ptr(m,dgroup(0x5398u)));const bool signed_le_five=(h&0x80000000u)!=0u||h<=5u;if(signed_le_five){u8*entry=dgroup_ptr(m,dgroup(table+h*4u));if((entry[1]&0x40u)==0){entry[1]|=0x40u;if(function_10397(b,static_cast<u16>(h)))entry[1]|=0x20u;}}return load_le32(dgroup_ptr(m,dgroup(table+h*4u)));}
"""
assert old in cpp
cpp=cpp.replace(old,new,1)

old="""void function_10452(Machine&m,StackGuardBoundary&b,u32 probe)noexcept{bool ok=false;if(probe<m.cpu.esp){const u32 distance=m.cpu.esp-probe;if(distance<=load_le32(dgroup_ptr(m,dgroup(0x5300u))))ok=true;}if(!ok&&m.cpu.ss==load_le16(dgroup_ptr(m,dgroup(0x55b0u)))){if(b.fatal_ce8e)b.fatal_ce8e(0x55b2u,1u);}}
void function_1043f(Machine&m,StackGuardBoundary&b,u32 argument)noexcept{const u32 incoming_eax=m.cpu.eax;m.cpu.eax=argument;function_10452(m,b,incoming_eax);m.cpu.eax=incoming_eax;}
"""
new="""void function_10452(Machine&m,StackGuardBoundary&b,u32 probe)noexcept{bool check_fatal=probe>=m.cpu.esp;if(!check_fatal){const u32 distance=m.cpu.esp-probe;if(distance<=load_le32(dgroup_ptr(m,dgroup(0x5300u))))check_fatal=true;}if(check_fatal&&m.cpu.ss==load_le16(dgroup_ptr(m,dgroup(0x55b0u)))){if(b.fatal_ce8e)b.fatal_ce8e(0x55b2u,1u);}}
void function_1043f(Machine&m,StackGuardBoundary&b,u32 argument)noexcept{const u32 incoming_eax=m.cpu.eax;m.cpu.eax=argument;function_10452(m,b,argument);m.cpu.eax=incoming_eax;}
"""
assert old in cpp
cpp=cpp.replace(old,new,1)

old=""" if(n>hread(b,arena+0x10u))return 0u;u32 node=hread(b,arena+8u),scan=hread(b,arena+0x0cu);if(n<=scan){node=hread(b,arena+0x24u);scan=0u;}
 const u32 sentinel=arena+0x1cu;for(;;){u32 size=hread(b,node);if(n<=size){u32 remain=size-n;hwrite(b,arena+0x0cu,scan);hwrite(b,arena+0x14u,hread(b,arena+0x14u)+1u);u32 next=hread(b,node+8u);if(remain>=12u){u32 tail=node+n;hwrite(b,arena+8u,tail);hwrite(b,tail,remain);hwrite(b,node,n);u32 prev=hread(b,node+4u);hwrite(b,tail+4u,prev);hwrite(b,tail+8u,next);hwrite(b,prev+8u,tail);hwrite(b,next+4u,tail);}else{hwrite(b,arena+0x18u,hread(b,arena+0x18u)-1u);u32 prev=hread(b,node+4u);hwrite(b,prev+8u,next);hwrite(b,next+4u,prev);hwrite(b,arena+8u,prev);}hwrite(b,node,hread(b,node)|1u);return node+4u;}scan=scan-size;node=hread(b,node+8u);if(node==sentinel){hwrite(b,arena+0x10u,scan);return 0u;}}
"""
new=""" if(n>hread(b,arena+0x10u))return 0u;u32 node=hread(b,arena+8u),largest=hread(b,arena+0x0cu);if(n<=largest){node=hread(b,arena+0x24u);largest=0u;}
 const u32 sentinel=arena+0x1cu;for(;;){u32 size=hread(b,node);if(n<=size){u32 remain=size-n;hwrite(b,arena+0x0cu,largest);hwrite(b,arena+0x14u,hread(b,arena+0x14u)+1u);u32 next=hread(b,node+8u);if(remain>=12u){u32 tail=node+n;hwrite(b,arena+8u,tail);hwrite(b,tail,remain);hwrite(b,node,n);u32 prev=hread(b,node+4u);hwrite(b,tail+4u,prev);hwrite(b,tail+8u,next);hwrite(b,prev+8u,tail);hwrite(b,next+4u,tail);}else{hwrite(b,arena+0x18u,hread(b,arena+0x18u)-1u);u32 prev=hread(b,node+4u);hwrite(b,prev+8u,next);hwrite(b,next+4u,prev);hwrite(b,arena+8u,prev);}hwrite(b,node,hread(b,node)|1u);return node+4u;}if(size>largest)largest=size;node=hread(b,node+8u);if(node==sentinel){hwrite(b,arena+0x10u,largest);return 0u;}}
"""
assert old in cpp
cpp=cpp.replace(old,new,1)

cpp_path.write_text(cpp)

audit=audit_path.read_text()
entry="""

## Semantic audit checkpoint 10m — `FC98..1083E`

Fresh direct disassembly of immutable `CODE.1` reconciled the runtime launch/record dispatchers, argv builder/tokenizer, register adapters and full interrupt veneer table, CRT descriptor/error/stack helpers, and the guest heap core through `1083E`. The original file SHA-256 and extracted object SHA-256 were reverified before comparison.

Four semantic defects were corrected. `103C3` first performs the encoded unsigned descriptor-count bound, but its subsequent `CMP ESI,5 / JG` test is signed; high-bit descriptor values that survive the first bound therefore enter the lazy device-query path. The previous C++ used an unsigned `h<=5` test. The reconstruction now preserves the signed gate without relying on implementation-defined unsigned-to-signed conversion.

`1043F` exchanges incoming EAX with the caller's stack argument before calling `10452`; consequently the stack guard receives the caller argument as its probe while the original EAX is restored only after the call. The previous lift incorrectly passed the pre-exchange EAX. `10452` itself was also inverted: the binary reaches the SS/fatal check when `probe >= ESP` or, for a lower probe, when `ESP-probe <= [5300]`; only a lower probe whose distance is greater than `[5300]` returns before the selector check. Both edges now match the encoded `CMP/JAE` and `CMP/JBE` flow.

Finally, `104E0`'s failed-free-block scan uses `SUB / CMC / SBB / AND / ADD` to retain the maximum of the prior candidate and the current free-block size. That maximum is stored in arena fields `+0C` on allocation and `+10` on exhaustion. The former C++ modeled the sequence as `scan -= size`, producing unrelated wrapped values. The heap allocation split/unlink paths, `10590` free/coalesce result semantics, arena insertion/removal, and `107CB` arena reclamation were otherwise rechecked directly through `1083E`.

The launch wrapper, record-priority dispatchers, `FD6E` argv sizing/layout, `FE21` quoted/unquoted token scanner, `FEFC/FF57` register adapters, the 256 three-byte interrupt veneers, and `102D4..104DD` helpers were otherwise reconciled without further changes. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, unresolved **0**. The sequential semantic frontier is now `1083E`; the next code begins at `1083F`.
"""
assert 'Semantic audit checkpoint 10m' not in audit
audit_path.write_text(audit+entry)
print('applied semantic audit checkpoint 10m')
