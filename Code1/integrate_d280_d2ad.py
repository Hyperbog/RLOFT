from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
assert 'kVerifiedTranslatedBytes=9589u' in s
assert 'function_d280(' not in s and 'function_d2ad(' not in s
# Extend the existing DOS boundary without flattening the unresolved mode-9 DTA selector shuttle.
needle='struct FileBoundaries{'
a=s.index(needle); b=s.index('};',a)
body=s[a:b]
extra='DosAttrResult(*dos_find_first)(u32,u16,u32)=nullptr;DosAttrResult(*dos_find_next)(u32)=nullptr;void(*dta_pre_find_next)(u32)=nullptr;void(*dta_post_find)(u32)=nullptr;'
assert 'dos_find_first' not in body
s=s[:b]+extra+s[b:]
anchor='// D313..D331: variadic front wrapper.'
assert anchor in s
code='''// D280..D2AC: set DOS DTA, execute AH=4Eh find-first, map AX+CF through 10384 semantics, then enter the exact D2CD mode-9 post-find shuttle boundary.\n[[nodiscard]]s32 function_d280(FileBoundaries&b,u32 path,u16 attr,u32 dta)noexcept{DosAttrResult r=b.dos_find_first?b.dos_find_first(path,attr,dta):DosAttrResult{1,0,true};const s32 out=r.carry?map_dos_error(b,r.ax):0;if(b.dta_post_find)b.dta_post_find(dta);return out;}\nstatic_assert(0xD2ADu-0xD280u==45u);\n// D2AD..D2CC: set DTA, execute the D2F0 mode-9 pre-find shuttle, AH=4Fh find-next, 10384 mapping, then D2CD post-find shuttle. D2CD/D2F0 remain machine/selector boundaries.\n[[nodiscard]]s32 function_d2ad(FileBoundaries&b,u32 dta)noexcept{if(b.dta_pre_find_next)b.dta_pre_find_next(dta);DosAttrResult r=b.dos_find_next?b.dos_find_next(dta):DosAttrResult{1,0,true};const s32 out=r.carry?map_dos_error(b,r.ax):0;if(b.dta_post_find)b.dta_post_find(dta);return out;}\nstatic_assert(0xD2CDu-0xD2ADu==32u);\n'''
s=s.replace(anchor,code+anchor,1)
s=s.replace('kVerifiedTranslatedBytes=9589u','kVerifiedTranslatedBytes=9666u',1)
p.write_text(s)

q=Path('Code1/PROGRESS.md'); t=q.read_text()
t=t.replace('13.62%25-red','13.72%25-red').replace('**Verified conversion:** 13.62%','**Verified conversion:** 13.72%').replace('**Remaining:** 86.38%','**Remaining:** 86.28%').replace('**9,589**','**9,666**').replace('**13.6204%**','**13.7297%**')
t=t.replace('- Open front/fixed wrappers `D313..D331` and `D55E..D572`: **52 bytes**.','- Find-first/find-next wrappers `D280..D2CC`: **77 bytes**.\n- Open front/fixed wrappers `D313..D331` and `D55E..D572`: **52 bytes**.')
t=t.replace('Total integrated verified translated bytes: **9,589**.','Total integrated verified translated bytes: **9,666**.')
t=t.replace('leaving **147 unique unintegrated bytes**','leaving **70 unique unintegrated bytes**')
t=t.replace('`D332..D4F1` remains structural staging until descriptor-table representation is reconciled against `103C3..1042C`.','`D332..D4F1` is integrated. The remaining DOS/CRT staging is the 70-byte `D2CD..D312` mode-9 DTA shuttle, retained because its DS/ES selector identity must not be flattened.')
t=t.replace('The unique unintegrated instruction-bounded staging pool is **1,499 bytes**: 747 startup + 595 remaining DOS/CRT + 157 remaining E2C3..E35F.','The unique unintegrated instruction-bounded staging pool is **974 bytes**: 747 startup + 70 remaining DOS/CRT + 157 remaining E2C3..E35F. `D280..D2CC` is integrated while its calls into `D2CD/D2F0` remain explicit selector-sensitive boundaries.')
q.write_text(t)

l=Path('Code1/STAGING_LEDGER.md'); u=l.read_text()
u=u.replace('595','70').replace('1,051','974')
# Append an explicit state note rather than relying on older prose.
u+='\n\n## D280/D2AD fold update\n`D280..D2CC` (77 bytes) is now integrated. `D2CD..D312` (70 bytes) remains staged because AH=2F returns a DTA address whose DS/ES selector use in the two REP MOVSB directions must be represented exactly. The integrated wrappers retain pre/post shuttle calls as explicit machine-boundary callbacks; no flat-memory assumption was introduced.\n'
l.write_text(u)
