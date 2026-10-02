from pathlib import Path

# Accounting pass 39: remove the five-byte 1058B..1058F double count.
# integrate_heap_core.py credited the full 104E0..1083E span as 863 bytes even though
# its explicit asserts split at 1058B/10590; the shared 5-byte JMP is part of that span.
# Batch 20 later credited those five bytes again. No C++ semantics are removed.

cpp=Path('Code1/Code1.cpp')
s=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes=69365u;' in s
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=69365u;','constexpr u32 kVerifiedTranslatedBytes=69360u;',1)
cpp.write_text(s)

p=Path('Code1/PROGRESS.md'); t=p.read_text()
assert 'verified%20conversion-98.53%25-red' in t
assert '**Verified conversion:** 98.53%' in t
assert '**Remaining:** 1.47%' in t
assert 'Current verified translated bytes: **69,365**.' in t
assert 'Raw coverage: **98.5270%**, displayed as **98.53%**.' in t
assert 'Total integrated verified translated bytes: **69,365**.' in t
t=t.replace('verified%20conversion-98.53%25-red','verified%20conversion-98.52%25-red',1)
t=t.replace('**Verified conversion:** 98.53%','**Verified conversion:** 98.52%',1)
t=t.replace('**Remaining:** 1.47%','**Remaining:** 1.48%',1)
t=t.replace('Current verified translated bytes: **69,365**.','Current verified translated bytes: **69,360**.',1)
t=t.replace('Raw coverage: **98.5270%**, displayed as **98.53%**.','Raw coverage: **98.5199%**, displayed as **98.52%**.',1)
t=t.replace('Total integrated verified translated bytes: **69,365**.','Total integrated verified translated bytes: **69,360**.',1)
old='- Batch 20 residual/accounting audit: newly represented/accounted `0000..0002`, `7F50..7F5B`, `95A8..95BA`, `9E10..9E11`, shared `9EA7..9EAD`, `1058B..1058F`, `CBEF`, `CBF0..CC18`, plus previously translated-but-omitted `DF13..DF72`: **186 unique bytes**; corrected the historical duplicate count of `10384..10396` (**-19**), net **+167 bytes**.'
new='- Batch 20 residual/accounting audit: its listed ranges cover **186 address bytes**, but `1058B..1058F` had already been included in the earlier **863-byte** heap-core scalar for `104E0..1083E`; therefore only **181 bytes** were newly unique. After the `10384..10396` duplicate correction (**-19**), the corrected net increase is **+162 bytes**.'
assert old in t
t=t.replace(old,new,1)
p.write_text(t)

ledger_path=Path('Code1/RESIDUAL_LEDGER.md'); ledger=ledger_path.read_text()
assert '- verified translated code: **69,365 bytes**' in ledger
assert '- conservative residual: **1,037 bytes**' in ledger
assert '- residual still requiring classification: **116 bytes**' in ledger
assert 'Completion requires the remaining 116 bytes' in ledger
ledger=ledger.replace('- verified translated code: **69,365 bytes**','- verified translated code: **69,360 bytes**',1)
ledger=ledger.replace('- conservative residual: **1,037 bytes**','- conservative residual: **1,042 bytes**',1)
ledger=ledger.replace('- residual still requiring classification: **116 bytes**','- residual still requiring classification: **121 bytes**',1)
ledger=ledger.replace('Completion requires the remaining 116 bytes','Completion requires the remaining 121 bytes',1)
ledger_path.write_text(ledger)

section='''\n\n## 2026-10-02 accounting correction pass 39\n\nA full address-union audit exposed a second historical duplicate: `integrate_heap_core.py` had already credited the entire `104E0..1083E` heap core as **863 bytes**. Its explicit subrange asserts total 858 bytes because the five-byte shared jump `1058B..1058F` is represented by the early-exit semantics around `function_10590`, but those five bytes are part of the original 863-byte scalar increment. Batch 20 later credited `1058B..1058F` again as if newly unique.\n\nThe verified scalar is corrected by **-5 bytes**, from **69,365** to **69,360 / 70,402 = 98.5199% (98.52%)**. Batch 20's corrected contribution is **181 newly unique bytes - 19 duplicate bytes = +162**, not +167. Classified non-code remains **921 bytes**. The conservative residual becomes **1,042 bytes**, leaving **121 bytes unresolved**. No translated semantics are removed; this is accounting-only. The badge remains red.\n'''
for name in ('Code1/PROGRESS.md','Code1/STAGING_LEDGER.md','Code1/DECOMPILATION_CONTINUITY.md'):
    q=Path(name); u=q.read_text()
    assert 'accounting correction pass 39' not in u.lower()
    q.write_text(u+section)

print('accounting pass39: removed duplicate 1058B..1058F count; verified=69360; unresolved=121')
