from pathlib import Path
import re
p=Path('Code1/Code1.cpp');s=p.read_text()
needle='struct Batch12Boundaries{\n u8(*read8)(u32)=nullptr;u16(*read16)(u32)=nullptr;u32(*read32)(u32)=nullptr;\n'
rep='struct Batch12Boundaries{\n u8(*read8)(u32)=nullptr;u16(*read16)(u32)=nullptr;u32(*read32)(u32)=nullptr;u8(*read_code8)(u32)=nullptr;\n'
if needle in s:s=s.replace(needle,rep,1)
else: assert rep in s

def repl(name,end_assert,newtext):
    global s
    pat=re.compile(r'(?:static u8 sar8_exact_9eb0.*?\n)?(?:\[\[nodiscard\]\])?(?:void|u8|u32) '+re.escape(name)+r'\(.*?'+re.escape(end_assert),re.S)
    m=pat.search(s);assert m,name
    s=s[:m.start()]+newtext+s[m.end():]
repl('function_9e14','static_assert(0x9E60u-0x9E14u==76u);',r'''void function_9e14(Machine&m,Batch12Boundaries&b,u32 x,u32 y,u8 value)noexcept{u32 p=0;u8 plane=0;if(b.translate_aea8)b.translate_aea8(x,y,&p,&plane);if(b.out16){b.out16(0x3ce,static_cast<u16>(4u|(u16(plane)<<8)));b.out16(0x3c4,static_cast<u16>(2u|(u16(1u<<plane)<<8)));b.out16(0x3ce,static_cast<u16>(3u|(u16(*dgroup_ptr(m,dgroup(0x2b28u)))<<8)));}(void)b12r8(b,p);b12w8(b,p,value);if(b.out16)b.out16(0x3ce,3u);} static_assert(0x9E60u-0x9E14u==76u);''')
repl('function_9e8c','static_assert(0x9EA3u-0x9E8Cu==23u);',r'''void function_9e8c(Machine&m,Batch12Boundaries&b,u32 index)noexcept{const u8 v=b.read_code8?b.read_code8(0x9ea3u+index):0u;*dgroup_ptr(m,dgroup(0x2b28u))=v;} static_assert(0x9EA3u-0x9E8Cu==23u);''')
repl('function_9eb0','static_assert(0xA00Eu-0x9EB0u==350u);',r'''static u8 sar8_exact_9eb0(u8 v,u32 count)noexcept{count&=0x1fu;if(count==0u)return v;const bool neg=(v&0x80u)!=0u;if(count>=8u)return neg?0xffu:0u;u8 r=static_cast<u8>(v>>count);if(neg)r=static_cast<u8>(r|static_cast<u8>(0xffu<<(8u-count)));return r;}
void function_9eb0(Machine&m,Batch12Boundaries&b,s32 x,s32 y,s32 w,s32 h,u8 color)noexcept{if(w<=0||h<=0)return;u32 base=0;u8 plane=0;if(b.translate_aea8)b.translate_aea8(static_cast<u32>(x),static_cast<u32>(y),&base,&plane);u8 left=static_cast<u8>(0x0fu<<plane),right=0u;u32 work=static_cast<u32>(h-1),phase=static_cast<u32>(plane^3u),middle=0u;const bool single=work<=phase;work-=phase;if(!single){--work;const u32 rem=work&3u;work-=rem;const u32 shift=rem^3u;right=static_cast<u8>(~sar8_exact_9eb0(0xf0u,shift));middle=work>>2;}else{work+=3u;const u32 shift=static_cast<u32>(low8(work)^3u);right=static_cast<u8>(~sar8_exact_9eb0(0xf0u,shift));left=static_cast<u8>(left&right);right=0u;middle=0u;}u32 stride=load_le32(dgroup_ptr(m,dgroup(0x2b41u)))-middle-1u;if(right!=0u)set_low16(stride,static_cast<u16>(low16(stride)-1u));if(b.planar_fill)b.planar_fill(base,static_cast<u32>(w),middle,left,right,color,stride,*dgroup_ptr(m,dgroup(0x2b28u)));} static_assert(0xA00Eu-0x9EB0u==350u);''')
repl('function_a010','static_assert(0xA059u-0xA010u==73u);',r'''void function_a010(Batch12Boundaries&b,u32 src,u32 first,u32 count)noexcept{if(b.out8)b.out8(0x3c8,static_cast<u8>(first));u32 p=src,n=count;for(;;){if(b.sync_af34)b.sync_af34(n);if(static_cast<s32>(n)>0xc0){if(b.out8)for(u32 i=0;i<0xc0u;++i)b.out8(0x3c9,b12r8(b,p+i));p+=0xc0u;n-=0xc0u;continue;}if(n!=0u&&b.out8)for(u32 i=0;i<n;++i)b.out8(0x3c9,b12r8(b,p+i));break;}} static_assert(0xA059u-0xA010u==73u);''')
old='void function_9e10(Machine&m,u32 index)noexcept{function_9e8c(m,index);}'
new='void function_9e10(Machine&m,Batch12Boundaries&b,u32 index)noexcept{function_9e8c(m,b,index);}'
if old in s:s=s.replace(old,new)
else:assert new in s
assert 'constexpr u32 kVerifiedTranslatedBytes=69361u;' in s
p.write_text(s)
a=Path('Code1/FINAL_BINARY_AUDIT.md');audit=a.read_text();marker='## Semantic audit checkpoint 10a — `9BE0..A058`'
if marker not in audit:
    audit+='''\n\n## Semantic audit checkpoint 10a — `9BE0..A058`\n\nFresh direct disassembly of the immutable original `CODE.1` reconciled the first planar-VGA block through `A058`. The dispatch wrappers `9BE0/9C44`, mode setup `9CA0`, mode restore `9DF4`, and planar read `9E60` were rechecked without structural change. `9E14` now performs the encoded dummy video-memory read that loads VGA latches before the write. `9E8C` now preserves its CS-relative byte lookup at `9EA3h+index` rather than masking the index to the four-byte nominal table; a code-segment read boundary makes that alias explicit, and the `9E10` tail alias forwards the same boundary.\n\n`9EB0` was corrected at the mask/arithmetic level: the right-edge mask derives from `(h-1)-(plane^3)`, uses signed byte `SAR` on `F0h`, preserves the unsigned `JBE` single-byte case, rounds the middle span exactly as encoded, and applies the final stride decrement to `DI` only rather than the full dword. `A010` now calls `AF34` with the remaining byte count before each burst (including the zero-length case), uses the encoded signed `JLE` decision against `C0h`, and only then emits a `C0h` or final remainder burst. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.\n''';a.write_text(audit)
print('semantic checkpoint 10a applied')
