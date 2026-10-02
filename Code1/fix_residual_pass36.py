from pathlib import Path

# Residual pass 36: reconcile the original structured-mouse fold.
# Historical commit e825575... explicitly counted these five machine ranges:
# D08C..D0B0 (37), D0B4..D108 (85), D10C..D12E (35),
# D130..D152 (35), D1E8..D239 (82) = 274 translated bytes.
# The four complementary islands inside D08C..D153 therefore total eight
# proven non-translated bytes. Their exact subtype is not preserved, so use
# the conservative generic `noncode` category rather than guessing padding.

cpp_path=Path('Code1/Code1.cpp')
cpp=cpp_path.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes=69365u;' in cpp

repls={
"void function_d08c(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=4;r.dx=static_cast<u16>(a);r.cx=static_cast<u16>(c<<1);if(b.mouse_int33)b.mouse_int33(r);}\n":
"void function_d08c(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=4;r.dx=static_cast<u16>(a);r.cx=static_cast<u16>(c<<1);if(b.mouse_int33)b.mouse_int33(r);}\nstatic_assert(0xD0B1u-0xD08Cu==37u);\n// D0B1..D0B3: excluded by the original 274-byte structured-mouse fold; exact non-code subtype is not preserved.\n",
"[[nodiscard]]MouseSnapshot*function_d0b4(NativeBoundaries&b,u8*d)noexcept{auto*out=reinterpret_cast<MouseSnapshot*>(d+0x8a60);if(!b.call_84d0||!b.call_84d0())return out;MouseRegs r{};r.ax=3;if(b.mouse_int33)b.mouse_int33(r);out->buttons=r.bx;out->y=r.dx;out->x_half=static_cast<s16>(static_cast<s16>(r.cx)>>1);return out;}\n":
"[[nodiscard]]MouseSnapshot*function_d0b4(NativeBoundaries&b,u8*d)noexcept{auto*out=reinterpret_cast<MouseSnapshot*>(d+0x8a60);if(!b.call_84d0||!b.call_84d0())return out;MouseRegs r{};r.ax=3;if(b.mouse_int33)b.mouse_int33(r);out->buttons=r.bx;out->y=r.dx;out->x_half=static_cast<s16>(static_cast<s16>(r.cx)>>1);return out;}\nstatic_assert(0xD109u-0xD0B4u==85u);\n// D109..D10B: excluded by the original 274-byte structured-mouse fold; exact non-code subtype is not preserved.\n",
"void function_d10c(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=7;r.cx=static_cast<u16>(a);r.dx=static_cast<u16>(c);if(b.mouse_int33)b.mouse_int33(r);} void function_d130(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=8;r.cx=static_cast<u16>(a);r.dx=static_cast<u16>(c);if(b.mouse_int33)b.mouse_int33(r);}\n":
"void function_d10c(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=7;r.cx=static_cast<u16>(a);r.dx=static_cast<u16>(c);if(b.mouse_int33)b.mouse_int33(r);}\nstatic_assert(0xD12Fu-0xD10Cu==35u);\n// D12F: excluded by the original 274-byte structured-mouse fold; exact non-code subtype is not preserved.\nvoid function_d130(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=8;r.cx=static_cast<u16>(a);r.dx=static_cast<u16>(c);if(b.mouse_int33)b.mouse_int33(r);}\nstatic_assert(0xD153u-0xD130u==35u);\n// D153: excluded by the original 274-byte structured-mouse fold; exact non-code subtype is not preserved.\n",
"void*function_d1e8(void*dst,const void*src,u32 n)noexcept{return std::memmove(dst,src,static_cast<std::size_t>(n));}\n":
"void*function_d1e8(void*dst,const void*src,u32 n)noexcept{return std::memmove(dst,src,static_cast<std::size_t>(n));}\nstatic_assert(0xD23Au-0xD1E8u==82u);\n"
}
for old,new in repls.items():
    assert old in cpp
    cpp=cpp.replace(old,new,1)
cpp_path.write_text(cpp)

ledger_path=Path('Code1/RESIDUAL_LEDGER.md')
ledger=ledger_path.read_text()
assert '- explicitly classified residual below: **713 bytes**' in ledger
assert '  - alignment/zero/padding: **479 bytes**' in ledger
assert '  - embedded tables: **122 bytes**' in ledger
assert '  - proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **112 bytes**' in ledger
assert '- residual still requiring classification: **324 bytes**' in ledger
for r in ('0D0B1..0D0B3','0D109..0D10B','0D12F..0D12F','0D153..0D153'):
    assert r not in ledger
anchor='0D179..0D17B | alignment | 3\n'
assert anchor in ledger
rows=(
'0D0B1..0D0B3 | noncode | 3\n'
'0D109..0D10B | noncode | 3\n'
'0D12F..0D12F | noncode | 1\n'
'0D153..0D153 | noncode | 1\n'
)
ledger=ledger.replace(anchor,rows+anchor,1)
ledger=ledger.replace('- explicitly classified residual below: **713 bytes**','- explicitly classified residual below: **721 bytes**',1)
ledger=ledger.replace('  - proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **112 bytes**','  - proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **120 bytes**',1)
ledger=ledger.replace('- residual still requiring classification: **324 bytes**','- residual still requiring classification: **316 bytes**',1)
ledger_path.write_text(ledger)

section='''\n\n## 2026-10-02 residual classification pass 36\n\nHistorical commit `e825575221e2c2af5489201e18bfab21dc7f8c9d` introduced the structured mouse/copy fold with five explicit translated ranges: `D08C..D0B0` (**37**), `D0B4..D108` (**85**), `D10C..D12E` (**35**), `D130..D152` (**35**), and `D1E8..D239` (**82**), totaling the preserved **274 translated bytes**. The complementary bytes inside `D08C..D153` are therefore exactly `D0B1..D0B3`, `D109..D10B`, `D12F`, and `D153` (**8 bytes**). Because preserved evidence does not identify their narrower padding/data subtype, they are conservatively classified as generic non-code. Exact `static_assert` boundaries are now restored beside the existing C++ functions. Classified non-code rises from **713** to **721 bytes** and unresolved residual falls from **324** to **316 bytes**. Verified translated code remains **69,365 / 70,402 = 98.53%**; no already-counted code is credited twice.\n'''
for name in ('Code1/PROGRESS.md','Code1/STAGING_LEDGER.md','Code1/DECOMPILATION_CONTINUITY.md'):
    p=Path(name); text=p.read_text()
    assert 'residual classification pass 36' not in text.lower()
    p.write_text(text+section)

print('residual pass36: +8 generic non-code bytes; unresolved 316; verified scalar unchanged')
