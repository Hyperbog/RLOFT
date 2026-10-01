from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
needle='// DAD9..DB7F. The encoded routine calls the allocator core at 104E0 with\n'
assert needle in s
insert='''// DA1C..DA44: 41-byte wrapper. 104B1 initializes the 12-byte local adapter\n// cell; CEC8 receives the original three dword arguments followed by that cell.\n// The boundary stays address-named because CEC8/FEFC carries selector/register\n// state that is not yet representable as an ordinary host call.\nstruct DosAdapterBoundary{void(*call_104b1)(u32 guest_cell)=nullptr;u32(*call_cec8)(u32,u32,u32,u32)=nullptr;};\n[[nodiscard]]u32 function_da1c(DosAdapterBoundary&b,u32 a1,u32 a2,u32 a3,u32 local_guest)noexcept{if(b.call_104b1)b.call_104b1(local_guest);return b.call_cec8?b.call_cec8(a1,a2,a3,local_guest):0;}\n\n'''
s=s.replace(needle,insert+needle,1)
needle2='void function_db4b(AllocBoundary&b,u32 pointer)noexcept{function_db5b(b,pointer);}\n'
assert needle2 in s
s=s.replace(needle2,needle2+'// DAC9..DAD8: exact 16-byte frame wrapper around DAD9.\n[[nodiscard]]u32 function_dac9(AllocBoundary&b,u32 request)noexcept{return function_dad9(b,request);}\n',1)
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=8089u;','constexpr u32 kVerifiedTranslatedBytes=8291u;')
assert 'kVerifiedTranslatedBytes=8291u' in s
p.write_text(s)

q=Path('Code1/PROGRESS.md')
t=q.read_text()
t=t.replace('verified%20conversion-11.69%25-red','verified%20conversion-11.77%25-red')
t=t.replace('**Verified conversion:** 11.69%  ','**Verified conversion:** 11.77%  ')
t=t.replace('**Remaining:** 88.31%  ','**Remaining:** 88.23%  ')
t=t.replace('Current verified translated bytes: **8,234**.  ','Current verified translated bytes: **8,291**.  ')
t=t.replace('Raw coverage: **11.6957%**, displayed conservatively as **11.69%**.','Raw coverage: **11.7766%**, displayed conservatively as **11.77%**.')
t=t.replace('Total integrated verified translated bytes: **8,234**.','Total integrated verified translated bytes: **8,291**.')
t=t.replace('`DosMemoryNext.staging.cpp` contains **186 functional instruction-bounded bytes** from `DA1C..DAD8`; `DA45..DA47` is three zero bytes and excluded. AH=48/AH=4A failure outputs use DOS-returned BX, and `10384` consumes live AX+CF.','`DA1C..DA44` and `DAC9..DAD8` are now physically integrated, adding **57 verified bytes**. `DA45..DA47` is three zero bytes. The already-integrated `DA48..DAC8` retains the reconciled AH=48/AH=4A DOS-returned BX and live AX+CF behavior.')
t=t.replace('unique uncounted staged code is **2,406 bytes** (747 startup + 57 genuinely missing DA1C/DAC9 bridge bytes + 880 D23A..D5AC + 722 E0D0..E3B2).','unique uncounted staged code is now **2,349 bytes** (747 startup + 880 D23A..D5AC + 722 E0D0..E3B2).')
q.write_text(t)
