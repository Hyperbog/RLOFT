from pathlib import Path
import re

p=Path('Code1/Code1.cpp')
s=p.read_text()

# LowGraphics13 boundaries: 6F74 and E6EA return EAX; B060 has a stack-local fatal string.
s=s.replace("u32(*call_6f10)(u32,u32,u32)=nullptr;void(*call_6f74)(u32)=nullptr;u32(*call_71c8)(u32)=nullptr;void(*call_7240)(u32)=nullptr;",
            "u32(*call_6f10)(u32,u32,u32)=nullptr;u32(*call_6f74)(u32)=nullptr;u32(*call_71c8)(u32)=nullptr;void(*call_7240)(u32)=nullptr;",1)
s=s.replace("u32(*call_f8c4)(u32,u32,u32)=nullptr;void(*call_e680)()=nullptr;void(*call_e6ea)(u32)=nullptr;",
            "u32(*call_f8c4)(u32,u32,u32)=nullptr;void(*call_e680)()=nullptr;u32(*call_e6ea)(u32)=nullptr;",1)
s=s.replace("u32(*format_ede7)(u32,u32,u32)=nullptr;\n};",
            "u32(*format_ede7)(u32,u32,u32)=nullptr;void(*call_9814_local)(const char*)=nullptr;\n};",1)

# AEA8: x86 variable shifts mask CL to five bits in 32-bit mode.
pat=re.compile(r"\[\[nodiscard\]\]PlanarAddress13 function_aea8\(Machine&m,u32 edx,u32 ecx\)noexcept\{.*?\}static_assert\(0xAEDCu-0xAEA8u==52u\);",re.S)
new=r'''[[nodiscard]]PlanarAddress13 function_aea8(Machine&m,u32 edx,u32 ecx)noexcept{const u32 shift1=load_le32(dgroup_ptr(m,dgroup(0x2b35u)))&31u,shift2=load_le32(dgroup_ptr(m,dgroup(0x2b39u)))&31u;const u32 first=edx<<shift1;const u32 second=first<<shift2;return{first+second+(ecx>>2)+load_le32(dgroup_ptr(m,dgroup(0x2b49u))),ecx&3u};}static_assert(0xAEDCu-0xAEA8u==52u);'''
s,n=pat.subn(new,s,1);assert n==1,n

# AF64: CLI and STI are unconditional; final IF is set before the retrace wait.
old="void function_af64(LowGraphics13Boundary&b,u32 ebx)noexcept{if(b.out16){b.out16(0x3d4u,static_cast<u16>(0x0cu|(static_cast<u16>((ebx>>8)&0xffu)<<8)));b.out16(0x3d4u,static_cast<u16>(0x0du|(static_cast<u16>(ebx&0xffu)<<8)));}function_af34(b);}static_assert(0xAF80u-0xAF64u==28u);"
new="void function_af64(Machine&m,LowGraphics13Boundary&b,u32 ebx)noexcept{m.cpu.interrupt_enable=false;if(b.out16){b.out16(0x3d4u,static_cast<u16>(0x0cu|(static_cast<u16>((ebx>>8)&0xffu)<<8)));b.out16(0x3d4u,static_cast<u16>(0x0du|(static_cast<u16>(ebx&0xffu)<<8)));}m.cpu.interrupt_enable=true;function_af34(b);}static_assert(0xAF80u-0xAF64u==28u);"
assert old in s
s=s.replace(old,new,1)
s=s.replace("function_af64(b,static_cast<u32>(static_cast<u8>(q))<<8)","function_af64(m,b,static_cast<u32>(static_cast<u8>(q))<<8)",1)

# B01A: preserve word reads/writes.  A low-byte NUL is read as part of LODSW but is not re-written.
pat=re.compile(r"\[\[nodiscard\]\]u32 function_b01a\(LowGraphics13Boundary&b,u32 dst,u32 src\)noexcept\{.*?\}static_assert\(0xB055u-0xB01Au==59u\);",re.S)
new=r'''[[nodiscard]]u32 function_b01a(LowGraphics13Boundary&b,u32 dst,u32 src)noexcept{u32 end=dst;while(lg13r8(b,end)!=0u)++end;for(;;){const u16 ax=lg13r16(b,src);src+=2u;if(static_cast<u8>(ax)==0u)break;lg13w16(b,end,ax);end+=2u;if(static_cast<u8>(ax>>8)==0u)break;}return dst;}static_assert(0xB055u-0xB01Au==59u);'''
s,n=pat.subn(new,s,1);assert n==1,n

# B060: on open failure the binary formats "Timber file \"%s\" not found\n" in a 0x100-byte stack buffer,
# calls 9814, and then continues if that fatal boundary happens to return.
pat=re.compile(r"void function_b060\(Machine&m,LowGraphics13Boundary&b\)noexcept\{.*?\}static_assert\(0xB0F6u-0xB060u==150u\);",re.S)
new=r'''void function_b060(Machine&m,LowGraphics13Boundary&b)noexcept{
 u8 rec[10];for(u32 i=0;i<10u;++i)rec[i]=*dgroup_ptr(m,dgroup(0x30cu+i));
 const u32 size=b.call_627c_record?b.call_627c_record(rec):0u;const u32 dst=b.call_71c8?b.call_71c8(size):0u;store_le32(dgroup_ptr(m,dgroup(0x2b98u)),dst);
 const u32 h=b.call_62a8_record?b.call_62a8_record(rec):0xffffffffu;
 if(h==0xffffffffu){char msg[0x100]{};u32 n=0;const char prefix[]="Timber file \"";for(char c:prefix){if(c==0)break;msg[n++]=c;}for(u32 i=0;i<10u&&rec[i]!=0u;++i)msg[n++]=static_cast<char>(rec[i]);const char suffix[]="\" not found\n";for(char c:suffix){if(c==0)break;msg[n++]=c;}msg[n]=0;if(b.call_9814_local)b.call_9814_local(msg);}
 if(b.call_6304)b.call_6304(dst,size,h);if(b.call_62f0)b.call_62f0(h);*dgroup_ptr(m,dgroup(0x2b94u))=1u;
}
static_assert(0xB0F6u-0xB060u==150u);'''
s,n=pat.subn(new,s,1);assert n==1,n

# B0F8 auxiliary-selector failure path: the encoded loop indexes 2BE8 through live EAX,
# not EBX.  EAX can be replaced by E6EA and by each 6F74 return, so retain that odd alias exactly.
old="if(q==0u){for(u32 i=0;i<8u;++i)if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2be8u+i*2u))));if(b.call_6f74)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2ba4u))));return;}"
new="if(q==0u){u32 eax=q;if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))==0u&&b.call_e6ea)eax=b.call_e6ea(eax);for(u32 ebx=0;ebx<8u;++ebx){const u16 sel=load_le16(dgroup_ptr(m,dgroup(0x2be8u+eax*2u)));if(b.call_6f74)eax=b.call_6f74(sel);}if(b.call_6f74)(void)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2ba4u))));return;}"
assert old in s
s=s.replace(old,new,1)

# B4D4: MOVSX byte compared to MOVZX word is a 32-bit comparison; negative bytes cannot match 0xFFxx IDs.
pat=re.compile(r"\[\[nodiscard\]\]u32 function_b4d4\(Machine&m,LowGraphics13Boundary&b,u16 id,u16 sub\)noexcept\{.*?\}static_assert\(0xB51Bu-0xB4D4u==71u\);",re.S)
new=r'''[[nodiscard]]u32 function_b4d4(Machine&m,LowGraphics13Boundary&b,u16 id,u16 sub)noexcept{const u32 base=load_le32(dgroup_ptr(m,dgroup(0x2b98u)));for(u32 i=0;;++i){const u32 rec=base+i*6u;const u32 marker=static_cast<u32>(static_cast<s32>(static_cast<std::int8_t>(lg13r8(b,rec+1u))));if(marker==0xffffffffu)return 0u;const u32 kind=static_cast<u32>(static_cast<s32>(static_cast<std::int8_t>(lg13r8(b,rec))));if(marker==static_cast<u32>(id)&&kind==static_cast<u32>(sub))return base+lg13r32(b,rec+2u);}}static_assert(0xB51Bu-0xB4D4u==71u);'''
s,n=pat.subn(new,s,1);assert n==1,n

p.write_text(s)

a=Path('Code1/FINAL_BINARY_AUDIT.md')
t=a.read_text()
head='## Semantic audit checkpoint 10e — `AEA8..B5E0`'
if head not in t:
    t += r'''

## Semantic audit checkpoint 10e — `AEA8..B5E0`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the planar-address/page helpers, retrace/CRTC controls, formatter append helper, and external resource/audio bootstrap through `B5E0`. `AEA8` now masks both variable shift counts to x86's low five CL bits before each 32-bit `SHL`, avoiding host C++ shift undefined behavior for counts >= 32. `AF64` now preserves its unconditional `CLI`/two CRTC writes/`STI` sequence before the `AF34` retrace wait, including the encoded final interrupt-enable state.

`B01A` now performs the encoded word-granular `LODSW`/`STOSW` append: a source word whose low byte is NUL is still read but is not written over the destination terminator. `B060` restores the open-failure path that formats the data-object literal at `0318h` (`Timber file "%s" not found\n`) with the fixed `030Ch` descriptor, calls `9814` on the 0x100-byte local message, and continues to the encoded read/close path if that fatal boundary returns. In `B0F8`, the auxiliary-allocation cleanup no longer substitutes a normalized eight-selector loop: the machine code indexes `2BE8h` through live EAX, with EAX replaced by the optional `E6EA` return and by each `6F74` return, while EBX only counts eight iterations. Finally, `B4D4` now retains the 32-bit `MOVSX byte` versus `MOVZX word` comparisons, so negative record bytes cannot spuriously match `FFxxh` caller IDs. `B430`, `B51C`, `B574`, and the signed-rate helper `B598` were rechecked directly and required no further semantic change. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `B5E0`; the next code begins at `B5E4` after the `B5E1..B5E3` LEA alignment.
'''
    a.write_text(t)
