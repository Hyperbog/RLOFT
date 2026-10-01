from pathlib import Path

cpp=Path('Code1/Code1.cpp')
p=Path('Code1/PROGRESS.md')
ledger=Path('Code1/STAGING_LEDGER.md')
s=cpp.read_text()
assert 'function_e174' not in s
assert 'function_e360' in s
assert 'constexpr u32 kVerifiedTranslatedBytes=8659u;' in s
needle='// E360..E3B2:'
idx=s.index(needle)
block='''// E174..E186, E1F4..E206, E26C..E27E: three address-distinct wrappers with identical encoded bodies.\n// Each establishes EBP, forwards its sole dword argument to 7240, discards the pushed argument, and returns EAX unchanged.\nstruct Runtime7240Boundary{u32(*call_7240)(u32)=nullptr;};\n[[nodiscard]]u32 function_e174(Runtime7240Boundary&b,u32 a)noexcept{return b.call_7240?b.call_7240(a):0u;}\n[[nodiscard]]u32 function_e1f4(Runtime7240Boundary&b,u32 a)noexcept{return b.call_7240?b.call_7240(a):0u;}\n[[nodiscard]]u32 function_e26c(Runtime7240Boundary&b,u32 a)noexcept{return b.call_7240?b.call_7240(a):0u;}\nstatic_assert(0xE187u-0xE174u==19u); static_assert(0xE207u-0xE1F4u==19u); static_assert(0xE27Fu-0xE26Cu==19u);\n\n'''
s=s[:idx]+block+s[idx:]
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=8659u;','constexpr u32 kVerifiedTranslatedBytes=8716u;')
for sentinel in ['function_d313','function_d4f2','function_d573','function_dad9','function_e360','function_f206']:
    assert sentinel in s
cpp.write_text(s)

ps=p.read_text()
ps=ps.replace('12.29%25-red','12.38%25-red').replace('12.29%  ','12.38%  ').replace('87.71%  ','87.62%  ').replace('**8,659**','**8,716**').replace('12.2994%','12.3803%')
p.write_text(ps)

ls=ledger.read_text()
ls=ls.replace('`E0D0..E173`, `E190..E1F0`, `E208..E268`, `E280..E35F` excluding padding | 639 |','`E0D0..E173`, `E190..E1F0`, `E208..E268`, `E280..E35F` excluding padding and integrated wrappers | 582 |')
ls=ls.replace('`E360..E3B2` is integrated and removed from active count.','`E174..E186`, `E1F4..E206`, `E26C..E27E`, and `E360..E3B2` are integrated and removed from active count.')
ls=ls.replace('**Active unique staged bytes before the next wrapper fold: 1,981.**','**Active unique staged bytes: 1,924.**')
ledger.write_text(ls)
print('integrated 57 verified wrapper bytes; total 8716')
