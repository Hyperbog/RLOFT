from pathlib import Path
import re

cppp=Path('Code2/Code2.cpp')
progp=Path('Code2/PROGRESS.md')
ledp=Path('Code2/RESIDUAL_LEDGER.md')
contp=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md')
incp=Path('Code2/batch14.inc')
cpp=cppp.read_text(); prog=progp.read_text(); led=ledp.read_text(); cont=contp.read_text(); inc=incp.read_text().rstrip()+"\n"

assert 'constexpr u32 kVerifiedTranslatedBytes = 44604u;' in cpp
assert 'constexpr u32 kVerifiedNonCodeBytes = 448u;' in cpp
assert 'function_094e0' not in cpp
cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 44604u;','constexpr u32 kVerifiedTranslatedBytes = 46240u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 448u;','constexpr u32 kVerifiedNonCodeBytes = 456u;',1)
pat=r'static_assert\(kVerifiedTranslatedBytes == 42612u \+\n.*?\);\n'
m=re.search(pat,cpp,re.S); assert m, 'batch13 cumulative assert not found'
new_assert='''static_assert(kVerifiedTranslatedBytes == 44604u +\n              (0x095BDu-0x094E0u) + (0x096E0u-0x095C0u) +\n              (0x0978Du-0x096E0u) + (0x09B4Au-0x09790u));\n'''
cpp=cpp[:m.start()]+new_assert+cpp[m.end():]
marker='\n} // namespace code2\n'
pos=cpp.rfind(marker); assert pos>=0
cpp=cpp[:pos]+'\n'+inc+cpp[pos:]
cppp.write_text(cpp)

repls={
'object%20accounting-18.80%25-red':'object%20accounting-19.48%25-red',
'**Verified translated instruction bytes:** **44,604**':'**Verified translated instruction bytes:** **46,240**',
'**Proven non-code/data bytes:** **448**':'**Proven non-code/data bytes:** **456**',
'**Reconciled object bytes:** **45,052 / 239,698 = 18.7953% (18.80%)**':'**Reconciled object bytes:** **46,696 / 239,698 = 19.4812% (19.48%)**',
'**Unresolved object bytes:** **194,646**':'**Unresolved object bytes:** **193,002**',
}
for a,b in repls.items(): assert a in prog,a; prog=prog.replace(a,b,1)
prog += '''\n\n## Batch 14 — opposite-buffer selection and party effect insertion\n\nAdded **1,636 verified translated instruction bytes**:\n\n- `094E0..095BC` — **221 bytes**: reverse-order opposite-buffer rectangle hit-test, including the encoded fall-through to record[count] when the order search misses.\n- `095C0..096DF` — **288 bytes**: interactive target picker with two distinct `1A1F4` polling phases, ESC handling, pointer bounds, and `094E0` hit-testing.\n- `096E0..0978C` — **173 bytes**: 12-word effect-slot insertion/fallback wrapper, preserving the mode-selected source pair and `2195C` argument order.\n- `09790..09B49` — **954 bytes**: party status/equipment application, including `A5h`/`80h` special cases, `F218`/`F238` sequencing, paired UI refresh, optional sound, and timed-event installation.\n\nAdded **8 directly proven alignment bytes** at `095BD..095BF`, `0978D..0978F`, and `09B4A..09B4B`. Full-object direct relative branch/call-target scanning found no incoming target to any separator byte.\n\nCumulative accounting: **46,240 translated + 456 proven non-code/data = 46,696 / 239,698 = 19.4812% (19.48%)**. Next sequential frontier: `09B4C`.\n'''
progp.write_text(prog.rstrip()+"\n")

ins='''095BD..095BF | alignment | 3\n0978D..0978F | alignment | 3\n09B4A..09B4B | alignment | 2\n'''
mark='<!-- RESIDUAL-RANGES-END -->'
assert mark in led and '095BD..095BF' not in led
led=led.replace(mark,ins+mark,1)
assert '**Total proven non-code/data:** **448 bytes**.' in led
led=led.replace('**Total proven non-code/data:** **448 bytes**.','**Total proven non-code/data:** **456 bytes**.',1)
led += '''\n\nBatch 14 evidence: `095BD..095BF` and `0978D..0978F` decode as `LEA EAX,[EAX+0]`; `09B4A..09B4B` decodes as `MOV EAX,EAX`. Each lies between directly reconstructed routine boundaries, and a full object-1 direct relative branch/call-target scan found no incoming target to any byte in these ranges. The known zero islands remain unresolved.\n'''
ledp.write_text(led.rstrip()+"\n")

cont += '''\n\n## CODE.2 Batch 14 checkpoint\n\nBatch 14 added **1,636 translated instruction bytes** plus **8 proven alignment bytes**. Cumulative accounting is **46,240 translated + 456 non-code = 46,696 / 239,698 = 19.4812% (19.48%)**; unresolved is **193,002 bytes**.\n\nThe sequential frontier now reaches `09B49`; the next routine begins at `09B4C`. Preserve these details: `094E0` may inspect record[count] when its order-word search misses; `095C0` has two address-distinct input polling loops; `096E0` chooses different word/coordinate pairs according to its mode; and `09790` keeps old slot words distinct from the two `F218` result bytes, with separate `A5h` and `80h` paths and partial-register side toggles. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
contp.write_text(cont.rstrip()+"\n")
