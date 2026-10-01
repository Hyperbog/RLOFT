from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
# DAD9..DDD7 is physically present later in the unified file. Insert DAC9 only
# after AllocBoundary/function_dad9 are defined; do not duplicate the runtime.
if 'function_dad9(AllocBoundary&b' not in s:
    raise SystemExit('verified DAD9 runtime sentinel missing')
if 'function_dac9(AllocBoundary&b' not in s:
    marker='void function_db5b(AllocBoundary&b,u32 pointer)noexcept'
    if marker not in s: raise SystemExit('DB5B insertion marker missing')
    insert='// DAC9..DAD8: exact frame wrapper around DAD9.\n[[nodiscard]]u32 function_dac9(AllocBoundary&b,u32 request)noexcept{return function_dad9(b,request);}\n'
    s=s.replace(marker,insert+marker,1)
# Keep the in-source verified-byte constant synchronized with the ledger.
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=8089u;','constexpr u32 kVerifiedTranslatedBytes=8250u;')
p.write_text(s)

q=Path('Code1/PROGRESS.md')
t=q.read_text()
t=t.replace('verified%20conversion-11.69%25-red','verified%20conversion-11.71%25-red').replace('**Verified conversion:** 11.69%','**Verified conversion:** 11.71%').replace('**Remaining:** 88.31%','**Remaining:** 88.29%').replace('Current verified translated bytes: **8,234**.','Current verified translated bytes: **8,250**.').replace('Raw coverage: **11.6957%**, displayed conservatively as **11.69%**.','Raw coverage: **11.7184%**, displayed conservatively as **11.71%**.').replace('Total integrated verified translated bytes: **8,234**.','Total integrated verified translated bytes: **8,250**.')
if '- Allocator wrapper `DAC9..DAD8`: **16 bytes**.' not in t:
    t=t.replace('- Runtime allocator/decoder/transform `DAD9..DDD7`: **764 bytes**.','- Allocator wrapper `DAC9..DAD8`: **16 bytes**.\n- Runtime allocator/decoder/transform `DAD9..DDD7`: **764 bytes**.')
t=t.replace('`DosMemoryNext.staging.cpp` contains **186 functional instruction-bounded bytes** from `DA1C..DAD8`; `DA45..DA47` is three zero bytes and excluded. AH=48/AH=4A failure outputs use DOS-returned BX, and `10384` consumes live AX+CF.','`DosMemoryNext.staging.cpp` originally covered 186 functional instruction-bounded bytes from `DA1C..DAD8`; `DA45..DA47` is three zero bytes and excluded. `DA48..DAA6` was already integrated, and `DAC9..DAD8` is now physically integrated. The remaining unique staged bridge is `DA1C..DA44` (**41 bytes**). Direct disassembly of `104B1..104DD` proves that `104B1` captures ES/CS/SS/DS/FS/GS selector words into the 12-byte cell; it does not clear that cell.')
t=t.replace('unique uncounted staged code is **2,406 bytes** (747 startup + 57 genuinely missing DA1C/DAC9 bridge bytes + 880 D23A..D5AC + 722 E0D0..E3B2).','unique uncounted staged code is **2,390 bytes** (747 startup + 41 remaining DA1C bridge bytes + 880 D23A..D5AC + 722 E0D0..E3B2).')
# Record the actual audit result: DAD9 was present, but physically appended near
# the footer rather than at address order, which caused the false initial alarm.
if '## Integration placement audit' not in t:
    t += '\n## Integration placement audit\n\nA physical-file audit confirmed that `DAD9..DDD7` was still present in `Code1.cpp`, but appended near the footer rather than in address order. The first attempted DAC9 fold therefore placed the wrapper before `AllocBoundary` and correctly failed syntax checking without committing. The integration was corrected to insert `DAC9..DAD8` adjacent to the existing DAD9 runtime definition. Preservation assertions now check existing verified-region sentinels before every one-shot fold.\n'
q.write_text(t)
