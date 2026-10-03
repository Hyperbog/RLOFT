from pathlib import Path

cpp_p=Path('Code1/Code1.cpp')
audit_p=Path('Code1/FINAL_BINARY_AUDIT.md')
cpp=cpp_p.read_text()

def replace_once(old,new):
    global cpp
    if old in cpp:
        assert cpp.count(old)==1,(old[:100],cpp.count(old))
        cpp=cpp.replace(old,new,1)
    else:
        assert new in cpp,(old[:100],'neither old nor new found')

replace_once(
"[[nodiscard]]u32 function_64b0(Machine&m,u32 arg)noexcept{if(static_cast<u16>(arg)==0u)arg=static_cast<u32>(load_le16(dgroup_ptr(m,dgroup(0x46cu))));function_61ac(m,static_cast<u16>(arg));return arg;} static_assert(0x64D9u-0x64B0u==41u);",
"[[nodiscard]]u32 function_64b0(Machine&m,u32 arg)noexcept{if(arg==0u)arg=static_cast<u32>(load_le16(dgroup_ptr(m,dgroup(0x46cu))));function_61ac(m,static_cast<u16>(arg));return arg;} static_assert(0x64D9u-0x64B0u==41u);"
)

start=cpp.index('static u32 clip_prepare(')
end=cpp.index('[[nodiscard]]u32 function_6854',start)
old_clip=cpp[start:end]
new_clip='''static u32 clip_prepare(Machine&m,MidGraphicsBoundaries&b,s32&x,s32&y,u32 image,u32 clip)noexcept{\n if(image==0u)return 0u;u32 w=mg16(b,image+4u);const u32 stride=static_cast<u32>(mg8(b,image+6u))*8u;mgw32(b,clip+8u,w);mgw32(b,clip,0u);mgw32(b,clip+4u,0u);\n s32 d=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x23edu)))-static_cast<u32>(x));if(d>0){w-=static_cast<u32>(d);mgw32(b,clip+8u,w);mgw32(b,clip,static_cast<u32>(d)*stride);x=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x23edu))));}\n d=static_cast<s32>(static_cast<u32>(x)+w-1u-load_le32(dgroup_ptr(m,dgroup(0x23f1u))));if(d>0){w-=static_cast<u32>(d);mgw32(b,clip+8u,w);mgw32(b,clip+4u,static_cast<u32>(d)*stride);}\n u32 h=stride;mgw32(b,clip+0x14u,h);mgw32(b,clip+0x0cu,0u);mgw32(b,clip+0x10u,0u);d=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x23f5u)))-static_cast<u32>(y));if(d>0){h-=static_cast<u32>(d);mgw32(b,clip+0x14u,h);mgw32(b,clip+0x0cu,static_cast<u32>(d));y=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x23f5u))));}d=static_cast<s32>(static_cast<u32>(y)+h-1u-load_le32(dgroup_ptr(m,dgroup(0x23f9u))));if(d>0){h-=static_cast<u32>(d);mgw32(b,clip+0x14u,h);mgw32(b,clip+0x10u,static_cast<u32>(d));}\n store_le16(dgroup_ptr(m,dgroup(0x2401u)),static_cast<u16>(x));store_le16(dgroup_ptr(m,dgroup(0x2403u)),static_cast<u16>(y));store_le16(dgroup_ptr(m,dgroup(0x2405u)),static_cast<u16>(w));store_le16(dgroup_ptr(m,dgroup(0x2407u)),static_cast<u16>(h));return (w==mg16(b,image+4u)&&h==stride)?2u:((static_cast<s32>(w)>0&&static_cast<s32>(h)>0)?1u:0u);\n}\n'''
if old_clip!=new_clip:
    cpp=cpp[:start]+new_clip+cpp[end:]

old6bc0='void function_6bc0(Machine&m,s32 left,s32 top,s32 right,s32 bottom)noexcept{if(left<0)left=0;if(top<0)top=0;const s32 maxx=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d7u))));const s32 maxy=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d9u))));if(maxx<=right)right=maxx-1;if(maxy<=bottom)bottom=maxy-1;top&=~3;const s32 q=(bottom+((bottom<0)?15:0))/4;bottom=q*4+3;store_le32(dgroup_ptr(m,dgroup(0x23edu)),static_cast<u32>(left));store_le32(dgroup_ptr(m,dgroup(0x23f5u)),static_cast<u32>(top));store_le32(dgroup_ptr(m,dgroup(0x23f1u)),static_cast<u32>(right));store_le32(dgroup_ptr(m,dgroup(0x23f9u)),static_cast<u32>(bottom));} static_assert(0x6C43u-0x6BC0u==131u);'
new6bc0='void function_6bc0(Machine&m,s32 left,s32 top,s32 right,s32 bottom)noexcept{if(left<0)left=0;if(top<0)top=0;const s32 maxx=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d7u))));const s32 maxy=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d9u))));if(maxx<=right)right=maxx-1;if(maxy<=bottom)bottom=maxy-1;top&=~3;const s32 q=bottom/4;bottom=q*4+3;store_le32(dgroup_ptr(m,dgroup(0x23edu)),static_cast<u32>(left));store_le32(dgroup_ptr(m,dgroup(0x23f5u)),static_cast<u32>(top));store_le32(dgroup_ptr(m,dgroup(0x23f1u)),static_cast<u32>(right));store_le32(dgroup_ptr(m,dgroup(0x23f9u)),static_cast<u32>(bottom));} static_assert(0x6C43u-0x6BC0u==131u);'
replace_once(old6bc0,new6bc0)

start=cpp.index('struct MidDpmiBoundaries{')
end=cpp.index('// 72A0..7669:',start)
old_dpmi=cpp[start:end]
new_dpmi='''struct MidDpmiBoundaries{\n u32(*alloc_dos)(u32 paragraphs,u16*out_sel)=nullptr; s32(*free_dos)(u16)=nullptr; u32(*get_base)(u16)=nullptr;\n u32(*alloc_linear)(u32 bytes,u32*handle)=nullptr; s32(*free_linear)(u32 handle)=nullptr; u32(*query_largest_dos_bytes)()=nullptr;\n u32(*query_mem)()=nullptr; void(*write16_guest)(u32,u16)=nullptr; void(*write32_guest)(u32,u32)=nullptr; void(*zero)(void*,u32)=nullptr; void(*fatal_64dc)(u32)=nullptr; void(*print_61d0)(u32)=nullptr;\n void(*call_cf98)()=nullptr; void(*call_8424)()=nullptr; void(*call_97d4)(u32)=nullptr; void(*call_daa6)()=nullptr;\n};\n[[nodiscard]]u32 function_6f10(MidDpmiBoundaries&b,s32 bytes,u32*out_linear,u16*out_sel)noexcept{const s32 rounded=static_cast<s32>(static_cast<u32>(bytes)+0x10u);const s32 paragraphs=rounded/16;u16 sel=0;const u32 seg=b.alloc_dos?b.alloc_dos(static_cast<u32>(paragraphs),&sel):0u;if(seg==0u)return 0u;*out_sel=sel;*out_linear=seg<<16;return seg<<4;} static_assert(0x6F72u-0x6F10u==98u);\n[[nodiscard]]s32 function_6f74(MidDpmiBoundaries&b,u32 sel_slot)noexcept{return b.free_dos?b.free_dos(static_cast<u16>(sel_slot)):-1;} static_assert(0x6FACu-0x6F74u==56u);\nvoid function_6fac(MidDpmiBoundaries&b,u32 sel_slot)noexcept{(void)(b.free_dos?b.free_dos(static_cast<u16>(sel_slot)):0);} static_assert(0x6FC0u-0x6FACu==20u);\n[[nodiscard]]u32 function_6fc0(MidDpmiBoundaries&b,u32 sel_slot)noexcept{return b.get_base?b.get_base(static_cast<u16>(sel_slot)):0u;} static_assert(0x6FF2u-0x6FC0u==50u);\n[[nodiscard]]u32 function_6ff4(MidDpmiBoundaries&b,s32 bytes)noexcept{const s32 rounded=static_cast<s32>(static_cast<u32>(bytes)+15u);const s32 paras=rounded/16;if(paras>0xffff)return 0u;u16 sel=0;const u32 seg=b.alloc_dos?b.alloc_dos(static_cast<u32>(paras),&sel):0u;if(!seg)return 0u;const u32 p=b.get_base?b.get_base(sel):0u;if(b.write16_guest)b.write16_guest(p,sel);if(b.write32_guest)b.write32_guest(p+2u,0u);return p;} static_assert(0x705Bu-0x6FF4u==103u);\n[[nodiscard]]s32 function_705c(MidDpmiBoundaries&b,u32 handle)noexcept{return b.free_linear?b.free_linear(handle):-1;} static_assert(0x7099u-0x705Cu==61u);\n[[nodiscard]]u32 function_709c(MidDpmiBoundaries&b,u32 bytes)noexcept{u32 h=0;const u32 p=b.alloc_linear?b.alloc_linear(bytes,&h):0u;if(p){if(b.write16_guest)b.write16_guest(p,0u);if(b.write32_guest)b.write32_guest(p+2u,h);}return p;} static_assert(0x70FCu-0x709Cu==96u);\n[[nodiscard]]u32 function_70fc(MidDpmiBoundaries&b)noexcept{return b.query_largest_dos_bytes?b.query_largest_dos_bytes():0u;} static_assert(0x713Bu-0x70FCu==63u);\n[[nodiscard]]u32 function_713c(MidDpmiBoundaries&b)noexcept{return b.query_mem?b.query_mem():0u;} static_assert(0x7182u-0x713Cu==70u);\nvoid function_7184(MidDpmiBoundaries&b)noexcept{if(b.fatal_64dc)b.fatal_64dc(0xffu);if(b.print_61d0)b.print_61d0(0xb8u);if(b.call_cf98)b.call_cf98();if(b.call_8424)b.call_8424();if(b.call_97d4)b.call_97d4(0x0au);} static_assert(0x71B9u-0x7184u==53u);\nvoid function_71bc(MidDpmiBoundaries&b)noexcept{if(b.call_daa6)b.call_daa6();} static_assert(0x71C8u-0x71BCu==12u);\n\n'''
if old_dpmi!=new_dpmi:
    cpp=cpp[:start]+new_dpmi+cpp[end:]

assert 'constexpr u32 kVerifiedTranslatedBytes=69361u;' in cpp
assert 'mgw32(b,clip,0u)' in cpp
assert 'const s32 q=bottom/4' in cpp
assert 'b.write16_guest(p,sel)' in cpp
assert 'b.write32_guest(p+2u,h)' in cpp
assert 'query_largest_dos_bytes' in cpp
cpp_p.write_text(cpp)

audit=audit_p.read_text()
marker='## Semantic audit checkpoint 5b — `6854..71C7`'
if marker not in audit:
    audit += '''\n\n## Semantic audit checkpoint 5b — `6854..71C7`\n\nA direct byte-level follow-up closes the remainder of checkpoint 5. In both clipped-image paths (`6854` and `6A04`), the binary initializes the source-offset dword at clip `+0` to zero, not image width, and all ADD/SUB/IMUL address arithmetic is preserved modulo 2^32. `6BC0` uses the compiler's signed divide-by-four idiom, which is exactly C/C++ truncation toward zero; the prior extra negative bias therefore double-adjusted negative values and was removed. `64B0` was also corrected to test the full 32-bit argument slot for zero before replacing it from word `046C`; only the later call to `61AC` consumes the low word.\n\nThe DPMI wrappers were reconciled directly from their register frames. `6F10` and `6FF4` form their rounded signed dividend with 32-bit wrap and then divide by 16 toward zero. The promoted selector arguments to `6F74`, `6FAC`, and `6FC0` remain 32-bit cdecl slots with low-word consumption. A successful `6FF4` allocation writes a six-byte ownership header (`WORD selector`, then zero `DWORD`) at the returned linear base. A successful `709C` allocation writes `WORD 0` followed by the 32-bit DPMI handle at `+2`. `70FC` is not a selector allocator: it issues the DOS-memory query pattern and returns the largest available DOS block in bytes on the expected failure path, so its boundary is named accordingly. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.\n'''
    audit_p.write_text(audit)
print('semantic checkpoint 5b applied')
