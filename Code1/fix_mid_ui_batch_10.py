from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
# This script runs after integrate_mid_ui_batch_10.py in CI.
s=s.replace('static_cast<s8>(ch)','static_cast<std::int8_t>(ch)')

old='[[nodiscard]]u32 function_767c(MidUi10Boundaries&b,u32 desc,u32 source)noexcept{\n const u32 h=mu32(b,desc+5u);'
new='[[nodiscard]]u32 function_767c(Machine&m,MidUi10Boundaries&b,u32 desc,u32 source)noexcept{\n store_le32(dgroup_ptr(m,dgroup(0x2904u)),source);const u32 h=mu32(b,desc+5u);'
assert old in s
s=s.replace(old,new,1)

start=s.index('// 7944..79C5: initialize an existing one-row image descriptor and capture into its payload.')
end=s.index('// 79C6..79C7 MOV EAX,EAX alignment.',start)
replacement='''// 7944..79C5: initialize an existing one-row image descriptor and capture into its payload.\n[[nodiscard]]u32 function_7944(MidUi10Boundaries&b,u32 out,u32 first_word,s32 x,s32 y,s32 w,s32 pixel_bits,u32 key)noexcept{mw16(b,out,static_cast<u16>(first_word));mw16(b,out+4u,static_cast<u16>(w));mw8(b,out+6u,static_cast<u8>(div8_zero(pixel_bits)));mw16(b,out+2u,1u);mw8(b,out+7u,key==0xffu?0u:0x80u);if(b.call_66c0)b.call_66c0(out+8u+(key==0xffu?0u:1u),x,y,w,pixel_bits);if(key!=0xffu)mw8(b,out+8u,static_cast<u8>(key));return out;} static_assert(0x79C6u-0x7944u==130u);\n'''
s=s[:start]+replacement+s[end:]

start=s.index('void function_85d4(')
end=s.index('void function_862c(',start)
replacement='''void function_85d4(MidUi10Boundaries&b,s32 x,s32 y,s32 start,s32 stop,u32 object)noexcept{if(stop==-1)stop=static_cast<s32>(mu16(b,object+2u));else stop+=start;const s32 stride=static_cast<s32>(mu16(b,object+4u));for(s32 i=start;i<stop;++i){if(b.call_6d54)b.call_6d54(i,x,y,object);++i;--i;x+=stride;}} static_assert(0x862Cu-0x85D4u==88u);\n'''
s=s[:start]+replacement+s[end:]
# Remove no-op ++i/--i pair: it was only used while checking loop correspondence.
s=s.replace('if(b.call_6d54)b.call_6d54(i,x,y,object);++i;--i;x+=stride;','if(b.call_6d54)b.call_6d54(i,x,y,object);x+=stride;',1)

start=s.index('void function_864c(')
end=s.index('void function_86a4(',start)
replacement='''void function_864c(MidUi10Boundaries&b,s32 x,s32 y,s32 start,s32 stop,u32 object)noexcept{if(stop==-1)stop=static_cast<s32>(mu16(b,object+2u));else stop+=start;const s32 stride=static_cast<s32>(mu16(b,object+4u));for(s32 i=start;i<stop;++i){if(b.call_67c8)b.call_67c8(i,static_cast<u32>(x),static_cast<u32>(y),object);x+=stride;}} static_assert(0x86A4u-0x864Cu==88u);\n'''
s=s[:start]+replacement+s[end:]
p.write_text(s)
