from pathlib import Path
import re

# Prove the batch-08 correction from preserved repository evidence before editing.
b8=Path('Code1/integrate_mid_runtime_batch_08.py').read_text()
sa=re.compile(r'static_assert\(0x([0-9A-Fa-f]+)u-0x([0-9A-Fa-f]+)u==([0-9]+)u\)')
ranges=[]
for end,start,n in sa.findall(b8):
    a,b,n=int(start,16),int(end,16),int(n)
    assert b-a==n,(hex(a),hex(b),n)
    ranges.append((a,b))
ranges.sort()
assert all(ranges[i][1] <= ranges[i+1][0] for i in range(len(ranges)-1))
exact=sum(b-a for a,b in ranges)
assert exact==3384,exact
historical_batch08=3671
correction=historical_batch08-exact
assert correction==287

old_verified=69652
new_verified=old_verified-correction
den=70402
assert new_verified==69365
assert den-new_verified==1037

# Correct the compiled coverage constant only; translated semantics are unchanged.
cpp=Path('Code1/Code1.cpp')
s=cpp.read_text()
assert s.count('constexpr u32 kVerifiedTranslatedBytes=69652u;')==1
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=69652u;','constexpr u32 kVerifiedTranslatedBytes=69365u;',1)
cpp.write_text(s)

# Correct current progress accounting and the current batch-08 inventory entry.
p=Path('Code1/PROGRESS.md');t=p.read_text()
repls={
    'verified%20conversion-98.93%25-red':'verified%20conversion-98.53%25-red',
    '**Verified conversion:** 98.93%':'**Verified conversion:** 98.53%',
    '**Remaining:** 1.07%':'**Remaining:** 1.47%',
    'Current verified translated bytes: **69,652**.':'Current verified translated bytes: **69,365**.',
    'Raw coverage: **98.9347%**, displayed as **98.93%**.':'Raw coverage: **98.5270%**, displayed as **98.53%**.',
    'Palette/tree, actor-table, RNG/DOS/display wrapper routines `59AC..6053`, `6184..6653`, and clipped display wrappers `6654..6851` (alignment excluded): **3,671 bytes**.':'Palette/tree, actor-table, RNG/DOS/display wrapper routines `59AC..6053`, `6184..6653`, and clipped display wrappers `6654..6851` (alignment excluded): **3,384 bytes**.',
    'Total integrated verified translated bytes: **69,652**.':'Total integrated verified translated bytes: **69,365**.',
}
for a,b in repls.items():
    assert a in t,a
    t=t.replace(a,b,1)
section='''\n\n## 2026-10-02 accounting correction pass 30\n\nA preserved-integrator audit found a historical batch-08 scalar overcount. `integrate_mid_runtime_batch_08.py` contains **47 exact non-overlapping translated ranges totaling 3,384 bytes**, while its progress update added **3,671 bytes**. Its own checkpoint text also states that `6060..6183` remained deliberately uncounted, so no hidden translated range justifies the extra 287 bytes. Batches 02 through 07 reconcile exactly to their recorded increments, and batches 09 through 12 were separately reconciled in the residual audit. The verified counter is therefore corrected by **-287**, from **69,652** to **69,365 / 70,402 = 98.5270% (98.53%)**. This pass changes accounting only; no C++ translation semantics or residual classifications are altered.\n'''
if '## 2026-10-02 accounting correction pass 30' not in t:t+=section
p.write_text(t)

# Rebase residual arithmetic onto the corrected verified-code total.
p=Path('Code1/RESIDUAL_LEDGER.md');t=p.read_text()
for a,b in {
    'verified translated code: **69,652 bytes**':'verified translated code: **69,365 bytes**',
    'conservative residual: **750 bytes**':'conservative residual: **1,037 bytes**',
    'residual still requiring classification: **64 bytes**':'residual still requiring classification: **351 bytes**',
    'Completion requires the remaining 64 bytes':'Completion requires the remaining 351 bytes',
}.items():
    assert a in t,a
    t=t.replace(a,b,1)
assert 'explicitly classified residual below: **686 bytes**' in t
p.write_text(t)

# Preserve historical notes but add an explicit correction checkpoint.
p=Path('Code1/DECOMPILATION_CONTINUITY.md');t=p.read_text()
section='''\n\n## 2026-10-02 accounting pass 30 checkpoint\n\nBatch-08 accounting was re-audited from `integrate_mid_runtime_batch_08.py`. Its 47 exact asserted translated intervals total **3,384 bytes**, not the **3,671** historically added to the progress scalar. The integrator explicitly leaves `6060..6183` uncounted, so the **287-byte difference is an overcount**, not omitted batch-08 code. Batches 02-07 reconcile exactly to their historical deltas; batches 09-12 have independent exact reconciliations. Current verified translated coverage is corrected to **69,365 / 70,402 = 98.53%**. The conservative residual is now **1,037 bytes**; with **686 bytes classified non-code**, **351 bytes remain unresolved**. No translation semantics changed in this accounting-only pass.\n'''
if '## 2026-10-02 accounting pass 30 checkpoint' not in t:t+=section
p.write_text(t)

p=Path('Code1/STAGING_LEDGER.md');t=p.read_text()
# Correct the current batch-08 ledger statement if present, while retaining the audit note below.
t=t.replace('Batch 08 integrated **3,671 bytes**: `59AC..6053`, `6184..6653`, `6654..6851`, excluding documented alignment.','Batch 08 contains **3,384 verified translated bytes** across `59AC..6053`, `6184..6653`, `6654..6851`, excluding documented alignment. The earlier 3,671 scalar was corrected in accounting pass 30.')
section='''\n\n## Accounting pass 30\nBatch-08 exact-range reconciliation corrected the global verified counter by **-287 bytes**. Verified translated code is **69,365**; classified residual remains **686 bytes**; corrected conservative unresolved residual is **351 bytes**. Active staging remains zero.\n'''
if '## Accounting pass 30' not in t:t+=section
p.write_text(t)

# Final self-checks for the corrected accounting.
assert 69365+1037==70402
assert 686+351==1037
