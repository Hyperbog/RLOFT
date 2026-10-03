from pathlib import Path
import re

cpp=Path('Code2/Code2.cpp')
s=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 24697u;' in s
assert 'constexpr u32 kVerifiedNonCodeBytes = 190u;' in s
assert 'void function_0461c' not in s
frag=Path('Code2/batch07.inc').read_text()
marker='\n} // namespace code2\n'
assert s.count(marker)==1
s=s.replace('constexpr u32 kVerifiedTranslatedBytes = 24697u;','constexpr u32 kVerifiedTranslatedBytes = 27622u;')
s=s.replace('constexpr u32 kVerifiedNonCodeBytes = 190u;','constexpr u32 kVerifiedNonCodeBytes = 193u;')
pat=re.compile(r'static_assert\(kVerifiedTranslatedBytes ==.*?\);',re.S)
m=pat.search(s);assert m
new='''static_assert(kVerifiedTranslatedBytes == 24697u +\n              (0x050D9u-0x0461Cu) + (0x0518Cu-0x050DCu));'''
s=s[:m.start()]+new+s[m.end():]
s=s.replace(marker,'\n'+frag+marker)
cpp.write_text(s)

p=Path('Code2/PROGRESS.md');t=p.read_text()
t=t.replace('object%20accounting-10.38%25-red','object%20accounting-11.60%25-red')
t=t.replace('**Verified translated instruction bytes:** **24,697**','**Verified translated instruction bytes:** **27,622**')
t=t.replace('**Proven non-code/data bytes:** **190**','**Proven non-code/data bytes:** **193**')
t=t.replace('**Reconciled object bytes:** **24,887 / 239,698 = 10.3826% (10.38%)**','**Reconciled object bytes:** **27,815 / 239,698 = 11.6042% (11.60%)**')
t=t.replace('**Unresolved object bytes:** **214,811**','**Unresolved object bytes:** **211,883**')
append='''## Batch 07 — four-party simulation/update loop and slot classifier\n\nSource identity and object-1 SHA-256 were reverified before this batch. Added **2,925 translated instruction bytes** and **3 directly proven alignment bytes**, bringing cumulative accounting to **27,622 translated + 193 non-code = 27,815 / 239,698 = 11.6042% (11.60%)**.\n\n### Newly translated ranges\n\n- `0461C..050D8` — **2,749 bytes**: four-party state reconciliation/update loop, including nibble normalization, pending/deficit accounting, timed progression, special-resource checks, escape polling, message construction, and exact normal-vs-hard cleanup paths.\n- `050DC..0518B` — **176 bytes**: slot classifier with exact `F240h` exclusion mask, mode-dependent signed byte tests, and the signed `>= 9` word gate.\n\n### Newly proven non-code/data\n\n`050D9..050DB` is the explicit three-byte `LEA EAX,[EAX]` compiler separator between the two routines. A full object-1 direct branch/call target scan found no incoming target to these bytes.\n\n### Fidelity notes\n\nThe `04100` early-message path returns directly after output and does **not** execute the `1E74C` cleanup. By contrast, the random `04228` abort and the two party-zero `7D44` aborts jump directly to `1E74C` and skip the normal key-drain, elapsed-time formatting, and D681/D682 publication epilogue. The odd initial special-resource predicate intentionally reuses the `2199B` low nibble while testing the `219B8/219C2/219CC` alternatives.\n\nThe zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n\n### Next high-value targets\n\n1. Continue the sequential application path at `0518C`.\n2. Reconcile startup at `353A0` as a CODE.2-specific path.\n3. Continue harvesting independently proved relocated CODE.1 runtime families.\n'''
assert '## Batch 07 —' not in t
t=t.rstrip()+'\n\n'+append
p.write_text(t.rstrip()+'\n')

led=Path('Code2/RESIDUAL_LEDGER.md');u=led.read_text()
ins='050D9..050DB | alignment | 3\n'
assert '050D9..050DB' not in u
u=u.replace('<!-- RESIDUAL-RANGES-END -->',ins+'<!-- RESIDUAL-RANGES-END -->')
u=u.replace('**Total proven non-code/data:** **190 bytes**.','**Total proven non-code/data:** **193 bytes**.')
u=u.rstrip()+'''\n\nBatch 07 evidence: `050D9..050DB` decodes as `LEA EAX,[EAX]` between the `0461C` RET and the `050DC` entry. A full object-1 direct relative branch/call target scan found no incoming target to the separator. The known zero islands remain unresolved.\n'''
led.write_text(u.rstrip()+'\n')

cont=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md');c=cont.read_text()
assert 'Batch 07 checkpoint' not in c
c=c.rstrip()+'''\n\n## CODE.2 Batch 07 checkpoint\n\nBatch 07 added **2,925 translated bytes** plus **3 proven non-code/data bytes**. Cumulative accounting is **27,622 translated + 193 non-code = 27,815 / 239,698 = 11.6042% (11.60%)**; unresolved is **211,883 bytes**.\n\nThe sequential application frontier now reaches `0518B`; `050D9..050DB` is proven alignment and the next routine starts at `0518C`. Preserve the three cleanup distinctions from `0461C`: the `04100` message path skips `1E74C`; random/party hard aborts execute only `1E74C`; normal escape/no-work completion performs the key drain/reporting epilogue before `1E74C`.\n'''
cont.write_text(c.rstrip()+'\n')
