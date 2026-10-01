from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
st=Path('Code1/RuntimeNext.staging.cpp').read_text()
start=st.index('// DAD9..DB7F')
end=st.index('static_assert(0xDB80u-0xDAD9u')
block=st[start:end].rstrip()+"\n\n"
needle='static_assert(0xE4C7u-0xE3B4u==275u);'
assert needle in s
assert 'void function_db80(DecodeBoundary&b)' not in s
s=s.replace(needle,block+'static_assert(0xDB80u-0xDAD9u==167u); static_assert(0xDC8Du-0xDB80u==269u); static_assert(0xDDD8u-0xDC90u==328u);\n'+needle,1)
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=7325u;','constexpr u32 kVerifiedTranslatedBytes=8089u;',1)
p.write_text(s)

pr=Path('Code1/PROGRESS.md')
t=pr.read_text()
t=t.replace('verified%20conversion-10.40%25-red','verified%20conversion-11.48%25-red')
t=t.replace('**Verified conversion:** 10.40%','**Verified conversion:** 11.48%').replace('**Remaining:** 89.60%','**Remaining:** 88.52%')
t=t.replace('Current verified translated bytes: **7,325**.','Current verified translated bytes: **8,089**.')
t=t.replace('Raw coverage: **10.4045%**, displayed conservatively as **10.40%**.','Raw coverage: **11.4897%**, displayed conservatively as **11.48%**.')
t=t.replace('Total integrated verified translated bytes: **7,325**.','Total integrated verified translated bytes: **8,089**.')
marker='- Complete formatter driver/parser/materializer `EDE7..F741`: **2,395 bytes**.'
t=t.replace(marker,marker+'\n- Runtime allocator/decoder/transform `DAD9..DDD7`: **764 bytes**.',1)
start=t.find('## Next runtime staging batch')
end=t.find('## Formatter fully integrated')
if start!=-1 and end!=-1:
 t=t[:start]+'''## Runtime batch integrated\n\n`DAD9..DDD7` (**764 verified bytes**) has been physically folded into `Code1.cpp`. The integrated code preserves the allocator retry/free wrappers, 4096-byte ring-dictionary decoder, and the corrected `REPNE SCASW` permutation search in `DC90`.\n\n'''+t[end:]
t=t.replace('Reconcile the staged `DAD9..DDD7` ranges instruction-by-instruction, especially `DC90`\'s `REPNE SCASW` state and allocator retry contracts; fold only proven ranges into `Code1.cpp`. Continue past any isolated dependency into startup successors and remaining address-named CRT helpers to approach the larger-batch target without relaxing fidelity.','Continue with startup successor `CD97..`, remaining address-named CRT helpers, and the unresolved `DE6F` boundary without relaxing fidelity.')
pr.write_text(t)
