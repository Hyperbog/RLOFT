from pathlib import Path
import re

p=Path('Code1/Code1.cpp')
s=p.read_text()

# CEC0 is MOV DS,CS:[CC90]; RET, not a value-return helper.
old='[[nodiscard]]u16 function_cec0(u16 saved_ds)noexcept{return saved_ds;} static_assert(0xCEC8u-0xCEC0u==8u);'
new='void function_cec0(Machine&m,u16 cs_cc90_ds)noexcept{m.cpu.ds=cs_cc90_ds;} static_assert(0xCEC8u-0xCEC0u==8u);'
assert old in s
s=s.replace(old,new,1)

# CF98 returns EAX=8A5Ah on its normal mouse-present return path.
pat=re.compile(r'void function_cf98\(.*?\}\s*static_assert\(0xD08Cu-0xCF98u==244u\);',re.S)
m=pat.search(s); assert m
blk=m.group(0)
assert blk.startswith('void function_cf98(')
assert blk.endswith('} static_assert(0xD08Cu-0xCF98u==244u);')
blk=blk.replace('void function_cf98(','[[nodiscard]]u32 function_cf98(',1)
blk=blk[:-len('} static_assert(0xD08Cu-0xCF98u==244u);')]+'return 0x8a5au;} static_assert(0xD08Cu-0xCF98u==244u);'
s=s[:m.start()]+blk+s[m.end():]

# D4F2's two IOCTL calls return a full DX error word plus carry.
old='struct DosHandleResult{u16 ax=0;bool carry=false;};\nstruct FileBoundaries{'
new='struct DosHandleResult{u16 ax=0;bool carry=false;};\nstruct DosIoctlResult{u16 dx=0;bool carry=false;};\nstruct FileBoundaries{'
assert old in s
s=s.replace(old,new,1)
old='bool(*ioctl_get_mode)(u16,u8&)=nullptr;bool(*ioctl_set_mode)(u16,u8)=nullptr;'
new='DosIoctlResult(*ioctl_get_mode)(u16)=nullptr;DosIoctlResult(*ioctl_set_mode)(u16,u8)=nullptr;'
assert old in s
s=s.replace(old,new,1)
pat=re.compile(r'\[\[nodiscard\]\]s32 function_d4f2\(.*?static_assert\(0xD55Eu-0xD4F2u==108u\);',re.S)
m=pat.search(s); assert m
new=r'''[[nodiscard]]s32 function_d4f2(FileBoundaries&b,u32 h)noexcept{
 u32 state=b.call_103c3?b.call_103c3(h):0u;state=(state&~0xffu)|static_cast<u8>(state|0x40u);if(b.set_fd_state_10415)b.set_fd_state_10415(h,state);if((state&0x2000u)==0u)return 0;
 const DosIoctlResult got=b.ioctl_get_mode?b.ioctl_get_mode(static_cast<u16>(h)):DosIoctlResult{0u,true};if(got.carry)return b.call_10314?b.call_10314(got.dx):-1;
 const u8 mode=static_cast<u8>(got.dx)|0x20u;const DosIoctlResult set=b.ioctl_set_mode?b.ioctl_set_mode(static_cast<u16>(h),mode):DosIoctlResult{0u,true};if(set.carry)return b.call_10314?b.call_10314(set.dx):-1;return 0;
}
static_assert(0xD55Eu-0xD4F2u==108u);'''
s=s[:m.start()]+new+s[m.end():]

# D5AD's Ctrl-Z ends compaction of the current DOS-read chunk; it does not
# return immediately. Common post-chunk accounting still executes.
start=s.index('[[nodiscard]]s32 function_d5ad(')
end=s.index('[[nodiscard]]s32 function_d686(',start)
blk=s[start:end]
old='if(ch==0x1a){total+=kept;return static_cast<s32>(total);}'
assert old in blk
blk=blk.replace(old,'if(ch==0x1a)break;',1)
s=s[:start]+blk+s[end:]

# D906 accepts (destination,count), builds drive:\\cwd as one stack string,
# allocates strlen+1 only for a null destination, and uses 1047E semantics.
pat=re.compile(r'struct CurrentDirBoundaries:DirectoryBoundaries\{.*?\};\n\[\[nodiscard\]\]char\* function_d906\(.*?return dst;\}',re.S)
m=pat.search(s); assert m
new=r'''struct CurrentDirBoundaries:DirectoryBoundaries{char*(*allocate)(u32)=nullptr;u32(*string_length)(const char*)=nullptr;void(*copy_string)(char*,const char*)=nullptr;u32*(*call_103b7)()=nullptr;};
static void set_current_dir_error(CurrentDirBoundaries&b,u32 v)noexcept{if(b.call_103b7)if(u32*p=b.call_103b7())*p=v;}
[[nodiscard]]char* function_d906(CurrentDirBoundaries&b,char*out,u32 count)noexcept{
 char full[115]{};if(!b.get_current_directory||b.get_current_directory(0,full+3)<0){set_current_dir_error(b,1u);return nullptr;}full[0]=static_cast<char>((b.get_current_drive?b.get_current_drive():0)+'A');full[1]=':';full[2]=static_cast<char>(0x5cu);
 const u32 need=(b.string_length?b.string_length(full):static_cast<u32>(std::strlen(full)))+1u;char*dst=out;if(!dst){dst=b.allocate?b.allocate(need):nullptr;if(!dst){set_current_dir_error(b,5u);return nullptr;}count=need;}
 u32 src=0;bool padding=false;for(u32 i=0;i<count;++i){if(padding){dst[i]=0;continue;}const char ch=full[src];if(ch==0){dst[i]=0;padding=true;}else{dst[i]=ch;++src;}}return dst;
}'''
s=s[:m.start()]+new+s[m.end():]

# D9EE is an explicit forward copy. Avoid memcpy overlap UB because callers do
# use overlapping regions and the binary's forward-copy result is defined.
old='void* function_d9ee(void*dst,const void*src,u32 count)noexcept{return std::memcpy(dst,src,static_cast<std::size_t>(count));}'
new='void* function_d9ee(void*dst,const void*src,u32 count)noexcept{auto*d=static_cast<u8*>(dst);auto*sp=static_cast<const u8*>(src);for(u32 i=0;i<count;++i)d[i]=sp[i];return dst;}'
assert old in s
s=s.replace(old,new,1)

# A successful 10C46 retry jumps back through DAF1, which resets the one-shot
# 10AA9 flag before the next allocation attempt.
old='  if(!b.call_10c46||b.call_10c46(request)==0){if(b.write_linear8)b.write_linear8(0x8e78u,0);return 0;}\n'
new=old+'  retried_10aa9=false;\n'
assert old in s
s=s.replace(old,new,1)

# DC90 enters these ECX loops before DEC/JNE. A zero initial count therefore
# wraps through the full dword counter rather than becoming zero iterations.
old=' u32 bx=0,eax=0;for(u32 ecx=table_words;ecx!=0;--ecx){tw16(b,0x30e8u+bx,static_cast<u16>(eax));bx+=8u;++eax;if(bx>=wrap){bx-=wrap;bx+=2u;}}'
new=' u32 bx=0,eax=0,ecx=table_words;do{tw16(b,0x30e8u+bx,static_cast<u16>(eax));bx+=8u;++eax;if(bx>=wrap){bx-=wrap;bx+=2u;}}while(--ecx!=0u);'
assert old in s
s=s.replace(old,new,1)
old='  for(u32 n=stride;n!=0;--n){const u8 av=tr8(b,a),sv=tr8(b,s);tw8(b,a,sv);tw8(b,s,av);++a;++s;}'
new='  u32 n=stride;do{const u8 av=tr8(b,a),sv=tr8(b,s);tw8(b,a,sv);tw8(b,s,av);++a;++s;}while(--n!=0u);'
assert old in s
s=s.replace(old,new,1)

p.write_text(s)

a=Path('Code1/FINAL_BINARY_AUDIT.md')
t=a.read_text()
section=r'''

## Semantic audit checkpoint 10i — `CEC0..DDD7`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the runtime register adapter, mouse/DOS file wrappers, directory helpers, allocator retry wrapper, dictionary decoder, and image permutation transform through `DDD7`. `CEC0` is now modeled for its actual sole side effect (`MOV DS,CS:[CC90h]`) rather than as a fictitious value-return helper. The normal `CF98` return path also preserves its encoded EAX value `8A5Ah`.

`D4F2` now carries the full 16-bit DX result and carry state across both `AX=4400h` and `AX=4401h` IOCTL boundaries. Error mapping therefore receives the encoded DX word instead of the previous low-byte-only mode value. In text-mode `D5AD`, Ctrl-Z ends compaction of the current DOS-read chunk but still reaches the common kept-byte accounting and possible next read; the prior lift returned immediately at Ctrl-Z and changed the stream-consumption semantics.

`D906` was restored to its two-argument ABI `(destination,count)`: it builds the contiguous `drive:\\current-directory` stack string, records runtime error 1 on the DOS current-directory failure, allocates exactly `strlen+1` bytes and records error 5 on allocation failure only when destination is null, and otherwise passes the caller's exact count through the encoded `1047E` bounded-copy/zero-pad behavior. `D9EE` now performs an explicit forward copy instead of invoking C++ `memcpy` on overlapping regions, preserving the deterministic machine result used by existing overlapping callers.

`DAD9` now resets its one-shot `10AA9` retry flag when a successful `10C46` retry returns through `DAF1`, so subsequent failed allocation attempts can invoke `10AA9` again exactly as encoded. Finally, both `DC90` loops driven by `DEC ECX/JNE` are entry-before-decrement loops; zero initial table/stride counts now retain their full dword wrap behavior rather than being normalized to zero iterations. The intervening mouse, DTA, open/create, seek/delete, DOS-memory, and `DB80` dictionary paths were rechecked directly and required no additional semantic change. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `DDD7`; `DDD8..DDDF` is the known eight-byte zero-alignment island and the next code begins at `DDE0`.
'''
if '## Semantic audit checkpoint 10i' not in t:
    a.write_text(t+section)
