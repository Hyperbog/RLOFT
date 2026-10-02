from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()

# Signed byte type: preserve the encoded signed class bytes without adding a project-wide alias.
s=s.replace('const s8 primary=static_cast<s8>', 'const std::int8_t primary=static_cast<std::int8_t>')
s=s.replace('const s8 second=static_cast<s8>', 'const std::int8_t second=static_cast<std::int8_t>')
s=s.replace('const s8 third=static_cast<s8>', 'const std::int8_t third=static_cast<std::int8_t>')

# 59AC/5A34 operate on guest offsets, not host pointers. Preserve AX-only MOVZX by retaining the
# upper 16 bits of the incoming guest EAX while replacing only AX with each byte value.
a=s.index('// 59AC..5A33:')
b=s.index('static_assert(0x5B64u-0x5A34u==304u);', a)+len('static_assert(0x5B64u-0x5A34u==304u);')
old=s[a:b]
new=r'''// 59AC..5A33: scan 256 RGB triples and return the triple index nearest to (31,31,31).
// MOVZX targets AX, not EAX: the upper 16 bits of the incoming guest address survive each byte load.
[[nodiscard]]s32 function_59ac(PaletteTreeBoundaries&b,u32 src)noexcept{
 s32 best_index=0; s16 best=0x200; s32 bx=0; const u32 high=src&0xffff0000u;
 while(static_cast<s16>(bx)<0x300){const s32 off=static_cast<s16>(bx);const u32 a=high|(b.read_guest8?b.read_guest8(src+static_cast<u32>(off)):0u);const u32 c1=high|(b.read_guest8?b.read_guest8(src+static_cast<u32>(off+1)):0u);const u32 c2=high|(b.read_guest8?b.read_guest8(src+static_cast<u32>(off+2)):0u);const s32 da=static_cast<s32>(a)-31,db=static_cast<s32>(c1)-31,dc=static_cast<s32>(c2)-31;const s32 dist=da*da+db*db+dc*dc;if(static_cast<s16>(dist)<best){best=static_cast<s16>(dist);best_index=off/3;}bx+=3;}return best_index;
}
static_assert(0x5A34u-0x59ACu==136u);

// 5A34..5B63: initialize sentinels, choose the closest palette entry, then insert downward and upward.
void function_5a34(Machine&m,PaletteTreeBoundaries&b,u32 src)noexcept{
 store_le16(dgroup_ptr(m,dgroup(0x7ff3u)),0xffffu);store_le16(dgroup_ptr(m,dgroup(0x7ff5u)),0x0101u);store_le16(dgroup_ptr(m,dgroup(0x7ff7u)),0x0101u);store_le16(dgroup_ptr(m,dgroup(0x88fcu)),0xffffu);store_le16(dgroup_ptr(m,dgroup(0x88feu)),0x0101u);store_le16(dgroup_ptr(m,dgroup(0x8900u)),0x0101u);s32 serial=1;const s32 lo=function_59ac(b,src);s32 hi=lo+1;auto ins=[&](s32 idx){const u32 o=static_cast<u32>(idx*3);if(b.insert_5880)b.insert_5880(b.read_guest8?b.read_guest8(src+o):0u,b.read_guest8?b.read_guest8(src+o+1u):0u,b.read_guest8?b.read_guest8(src+o+2u):0u,static_cast<u16>(idx),serial);++serial;};for(s32 i=lo;i>-1;--i)ins(i);while(static_cast<s16>(hi)<0x100){ins(hi);++hi;}store_le16(dgroup_ptr(m,dgroup(0x982u)),1u);
}
static_assert(0x5B64u-0x5A34u==304u);'''
s=s[:a]+new+s[b:]

# The first four class-record words are encoded in the exact order 98C,98E,98A,988.
needle='for(u32 j=0;j<4u;++j)store_le16(dgroup_ptr(m,dgroup(abase+0x6854u+j*2u)),load_le16(dgroup_ptr(m,dgroup(rec+0x988u+((3u-j)&3u)*2u))));'
repl='const u32 q4[4]={0x98cu,0x98eu,0x98au,0x988u};for(u32 j=0;j<4u;++j)store_le16(dgroup_ptr(m,dgroup(abase+0x6854u+j*2u)),load_le16(dgroup_ptr(m,dgroup(rec+q4[j]))));'
assert needle in s
s=s.replace(needle,repl)

# 64B0 receives a 32-bit ABI slot, tests only AX, and returns the original full slot when nonzero.
old='[[nodiscard]]u32 function_64b0(Machine&m,u16 seed)noexcept{u32 v=seed;if(seed==0u)v=load_le16(dgroup_ptr(m,dgroup(0x46cu)));function_61ac(m,static_cast<u16>(v));return v;}'
new='[[nodiscard]]u32 function_64b0(Machine&m,u32 arg)noexcept{if(static_cast<u16>(arg)==0u)arg=static_cast<u32>(load_le16(dgroup_ptr(m,dgroup(0x46cu))));function_61ac(m,static_cast<u16>(arg));return arg;}'
assert old in s
s=s.replace(old,new)

# Keep the constant 892F current-directory buffer as a guest address instead of manufacturing a host pointer.
s=s.replace('char*(*call_d906)(char*)=nullptr;', 'u32(*call_d906_guest)(u32)=nullptr;')
old='[[nodiscard]]char* function_640c(PaletteTreeBoundaries&b)noexcept{return b.call_d906?b.call_d906(reinterpret_cast<char*>(0x892fu)):nullptr;}'
new='[[nodiscard]]u32 function_640c(PaletteTreeBoundaries&b)noexcept{return b.call_d906_guest?b.call_d906_guest(0x892fu):0u;}'
assert old in s
s=s.replace(old,new)

# 61D0 mutates a guest string in place around DOS AH=09. Do not cast guest addresses to native char pointers.
s=s.replace('u8(*read_guest8)(u32)=nullptr;\n void(*dos_print_dollar)(char*)=nullptr;', 'u8(*read_guest8)(u32)=nullptr; void(*write_guest8)(u32,u8)=nullptr;\n void(*dos_print_dollar_guest)(u32)=nullptr;')
old="void function_61d0(PaletteTreeBoundaries&b,char*s)noexcept{char*p=s;while(*p){if(*p=='$')*p='*';++p;}*p='$';if(b.dos_print_dollar)b.dos_print_dollar(s);*p=0;}"
new="void function_61d0(PaletteTreeBoundaries&b,u32 s)noexcept{u32 p=s;for(;;){const u8 ch=b.read_guest8?b.read_guest8(p):0u;if(ch==0u)break;if(ch=='$'&&b.write_guest8)b.write_guest8(p,'*');++p;}if(b.write_guest8)b.write_guest8(p,'$');if(b.dos_print_dollar_guest)b.dos_print_dollar_guest(s);if(b.write_guest8)b.write_guest8(p,0u);}"
assert old in s
s=s.replace(old,new)

p.write_text(s)
