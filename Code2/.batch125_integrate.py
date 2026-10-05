from pathlib import Path
import sys
root=Path.cwd(); cpp=root/'Code2/Code2.cpp'; progress=root/'Code2/PROGRESS.md'; ledger=root/'Code2/RESIDUAL_LEDGER.md'; add=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; frag=root/'Code2/.batch125.fragment.cpp'
OLD_T='225087u'; NEW_T='225364u'; OLD_N='4602u'; NEW_N='4624u'

def once(s,a,b):
    if a not in s: raise SystemExit('missing anchor: '+a[:80])
    return s.replace(a,b,1)

def integrate():
    s=cpp.read_text(); f=frag.read_text()
    s=once(s,OLD_T,NEW_T); s=once(s,OLD_N,NEW_N)
    anchor='} // namespace code2'
    if anchor not in s: anchor='} // namespace Code2'
    if anchor not in s: raise SystemExit('namespace close anchor missing')
    s=s.rsplit(anchor,1)[0]+f+'\n'+anchor+s.rsplit(anchor,1)[1]
    cpp.write_text(s)
    p=progress.read_text()
    p=once(p,'object%20accounting-95.82%25-red','object%20accounting-95.95%25-red')
    p=once(p,'**Verified translated instruction bytes:** **225,087**','**Verified translated instruction bytes:** **225,364**')
    p=once(p,'**Proven non-code/data bytes:** **4,602**','**Proven non-code/data bytes:** **4,624**')
    p=once(p,'**Reconciled object bytes:** **229,689 / 239,698 = 95.8243% (95.82%)**','**Reconciled object bytes:** **229,988 / 239,698 = 95.9491% (95.95%)**')
    p=once(p,'**Unresolved object bytes:** **10,009**','**Unresolved object bytes:** **9,710**')
    p+='''\n\n## Batch 125 — allocation/read helper family\n\nAdded **277 translated instruction bytes** and **22 directly proven alignment bytes** across `374D2..375FD`. `374E0..3757B` clones a 28-byte header from the descriptor-selected source, allocates `length+0x1B`, then reads `length-1` bytes into offset `+0x1C`; `3757C..37588` is its free wrapper. `37590..375EE` reads a two-byte length, allocates `length+2`, stores the length prefix, performs the `2F960`/`2F96C` setup, and fills the payload through `2FB04`; `375F0..375FC` is the second free wrapper. Zero/alignment islands `374D2..374DF` and `37589..3758F`, plus NOP `375EF`, are directly target-scanned and have no incoming direct relative control-flow targets. Next independent instruction boundary: **`375FE`**. Accounting: **225,364 translated + 4,624 non-code/data = 229,988 / 239,698 = 95.9491% (95.95%)**, leaving **9,710 unresolved bytes**.\n'''
    progress.write_text(p)
    l=ledger.read_text()+'''\n\n## Batch 125 additions\n\n- `374D2..374DF` — 14 zero alignment bytes between the radix family and `374E0`; no incoming direct relative control-flow target in a full object-1 scan.\n- `37589..3758F` — 7 zero alignment bytes between the first free wrapper and `37590`; no incoming direct relative control-flow target.\n- `375EF` — 1 explicit NOP separator before `375F0`; no incoming direct relative control-flow target.\n\nBatch 125 adds **22 proven non-code/alignment bytes**, bringing the cumulative non-code/data total to **4,624**.\n'''; ledger.write_text(l)
    a=add.read_text()+'''\n\n## CODE.2 Batch 125 checkpoint\n\nBatch 125 reconciles `374D2..375FD`: **277 translated + 22 alignment bytes**. Coverage is **229,988 / 239,698 = 95.9491% (95.95%)**, with **9,710 unresolved bytes**. `374E0` clones the fixed 28-byte header and reads the variable tail; `37590` is the length-prefixed allocation/fill path. Preserve the encoded `count+0x1B` allocation versus `count-1` tail read and the distinct two-byte length prefix. Next boundary is `375FE`; the earlier `36CEC..372F5` helper remains unresolved.\n'''; add.write_text(a)

def verify():
    s=cpp.read_text()
    for x in [NEW_T,NEW_N,'function_374e0','function_37590','function_375f0']:
        if x not in s: raise SystemExit('verify missing '+x)
    p=progress.read_text()
    if '229,988 / 239,698 = 95.9491% (95.95%)' not in p: raise SystemExit('progress total mismatch')

if __name__=='__main__': {'integrate':integrate,'verify':verify}[sys.argv[1]]()
