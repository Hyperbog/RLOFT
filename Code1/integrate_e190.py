from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
needle='// E360..E3B2: signed decimal parser.'
assert needle in s
assert 'function_e190' not in s and 'function_e208' not in s
block=r'''// E190..E1F0 and E208..E268: address-distinct constructor-like readers. The first stack dword is encoded but unused.\n// 6304 is the read wrapper: these bodies request exactly two bytes from source into [EBP-8], zero-extend that word, allocate len+2, store len, set 2904 through 72D0, run 72E0, then call 746C(0,0,dst+2,len,0).\nstruct RuntimeConstructBoundary{u16(*read_word_6304)(u32)=nullptr;u32(*alloc_71c8)(u32)=nullptr;void(*write16)(u32,u16)=nullptr;void(*call_72d0)(u32)=nullptr;void(*call_72e0)()=nullptr;void(*call_746c)(u32,u32,u32,u32,u32)=nullptr;};\n[[nodiscard]]u32 runtime_construct_e190_body(RuntimeConstructBoundary&b,u32 source)noexcept{const u16 len=b.read_word_6304?b.read_word_6304(source):0u;const u32 dst=b.alloc_71c8?b.alloc_71c8(static_cast<u32>(len)+2u):0u;if(b.write16)b.write16(dst,len);if(b.call_72d0)b.call_72d0(source);if(b.call_72e0)b.call_72e0();if(b.call_746c)b.call_746c(0u,0u,dst+2u,static_cast<u32>(len),0u);return dst;}\n[[nodiscard]]u32 function_e190(RuntimeConstructBoundary&b,u32 /*unused_arg1*/,u32 source)noexcept{return runtime_construct_e190_body(b,source);}\n[[nodiscard]]u32 function_e208(RuntimeConstructBoundary&b,u32 /*unused_arg1*/,u32 source)noexcept{return runtime_construct_e190_body(b,source);}\nstatic_assert(0xE1F1u-0xE190u==97u); static_assert(0xE269u-0xE208u==97u);\n\n'''.replace('\\n','\n')
s=s.replace(needle,block+needle)
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=8716u;','constexpr u32 kVerifiedTranslatedBytes=8910u;')
assert 'kVerifiedTranslatedBytes=8910u' in s
p.write_text(s)

q=Path('Code1/PROGRESS.md'); t=q.read_text()
t=t.replace('12.38%25-red','12.65%25-red').replace('**Verified conversion:** 12.38%','**Verified conversion:** 12.65%').replace('**Remaining:** 87.62%','**Remaining:** 87.35%')
t=t.replace('Current verified translated bytes: **8,716**.','Current verified translated bytes: **8,910**.')
t=t.replace('Raw coverage: **12.2993%**, displayed conservatively as **12.29%**.','Raw coverage: **12.6559%**, displayed as **12.65%**.')
t=t.replace('Total integrated verified translated bytes: **8,716**.','Total integrated verified translated bytes: **8,910**.')
t=t.replace('- Signed decimal parser `E360..E3B2`: **83 bytes**.','- Runtime constructor readers `E190..E1F0` and `E208..E268`: **194 bytes**.\n- Signed decimal parser `E360..E3B2`: **83 bytes**.')
t=t.replace('leaving **639 unique unintegrated bytes**','leaving **445 unique unintegrated bytes**').replace('unique unintegrated instruction-bounded staging pool is **1,981 bytes**','unique unintegrated instruction-bounded staging pool is **1,730 bytes**')
# Current branch already integrated the three 19-byte wrappers after the older 1,981 wording; normalize either current wording.
t=t.replace('unique unintegrated instruction-bounded staging pool is **1,924 bytes**','unique unintegrated instruction-bounded staging pool is **1,730 bytes**')
t=t.replace('747 startup + 595 remaining DOS/CRT + 639 remaining E0D0..E35F','747 startup + 595 remaining DOS/CRT + 388 remaining E0D0..E35F')
q.write_text(t)

r=Path('Code1/STAGING_LEDGER.md'); u=r.read_text()
u=u.replace('`E0D0..E173`, `E190..E1F0`, `E208..E268`, `E280..E35F` excluding padding and integrated wrappers | 582 | `E0D0` object-copy semantics need exact guest-memory/call contracts. The two 97-byte constructors currently use a schematic local-memory representation and must be reconciled. `E280` installs INT 24h with extender-specific behavior. `E2C3..E35F` is an IRET trampoline and must remain a machine/interrupt boundary, never an ordinary host callback. `E174..E186`, `E1F4..E206`, `E26C..E27E`, and `E360..E3B2` are integrated and removed from active count.','`E0D0..E173`, `E280..E35F` excluding padding and integrated ranges | 388 | `E0D0` object-copy semantics still need exact guest-memory/call contracts. `E280` installs INT 24h with extender-specific behavior. `E2C3..E35F` is an IRET trampoline and must remain a machine/interrupt boundary, never an ordinary host callback. The two 97-byte constructors `E190..E1F0` and `E208..E268` are now directly reconciled and integrated, along with the three 7240 wrappers and `E360..E3B2`.')
u=u.replace('**Active unique staged bytes: 1,924.**','**Active unique staged bytes: 1,730.**')
r.write_text(u)
