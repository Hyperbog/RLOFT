from pathlib import Path
import sys
r=Path.cwd(); cpp=r/'Code2/Code2.cpp'; pth=r/'Code2/PROGRESS.md'; led=r/'Code2/RESIDUAL_LEDGER.md'; add=r/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; frag=r/'Code2/.batch128.fragment.cpp'
def once(s,a,b):
    if a not in s: raise SystemExit('missing '+a[:60])
    return s.replace(a,b,1)
def integrate():
    s=cpp.read_text(); s=once(s,'226302u','226595u'); s=once(s,'4665u','4672u')
    s=once(s,'+ 505u + 433u);','+ 505u + 433u + 293u);')
    close='} // namespace code2'; f=frag.read_text(); s=s.rsplit(close,1)[0]+f+'\n'+close+s.rsplit(close,1)[1]; cpp.write_text(s)
    p=pth.read_text(); p=once(p,'object%20accounting-96.36%25-red','object%20accounting-96.48%25-red'); p=once(p,'**Verified translated instruction bytes:** **226,302**','**Verified translated instruction bytes:** **226,595**'); p=once(p,'**Proven non-code/data bytes:** **4,665**','**Proven non-code/data bytes:** **4,672**'); p=once(p,'**Reconciled object bytes:** **230,967 / 239,698 = 96.3575% (96.36%)**','**Reconciled object bytes:** **231,267 / 239,698 = 96.4827% (96.48%)**'); p=once(p,'**Unresolved object bytes:** **8,731**','**Unresolved object bytes:** **8,431**'); p+='''\n\n## Batch 128 — optimized fills, selector snapshot, and DOS memory wrappers\n\nAdded **293 translated instruction bytes** and **7 proven alignment/separator bytes** across `379D0..37AFB`. `379D0..37A00` is the byte-count fill entry with byte alignment, EDX rotation, aligned dword helper call, and 0..3 byte tail; `37A07..37A72` is its unrolled dword-count fill helper. `37A73..37A9C` snapshots ES/CS/SS/DS/FS/GS. `37A9E..37AFB` comprises DOS AH=48h allocation, AH=49h free, and AH=4Ah resize wrappers, preserving success/failure output conventions. Proven separators are six NOPs at `37A01..37A06` and one zero byte at `37A9D`. Next boundary: **`37AFC`**. Accounting: **226,595 translated + 4,672 non-code/data = 231,267 / 239,698 = 96.4827% (96.48%)**, leaving **8,431 unresolved bytes**.\n'''; pth.write_text(p)
    led.write_text(led.read_text()+'''\n\n## Batch 128 additions\n- `37A01..37A06` — six NOP alignment bytes between the byte-count and dword-count fill entries.\n- `37A9D` — one zero separator byte before the DOS allocation wrapper.\n\nBatch 128 adds **7 proven non-code/alignment bytes**, cumulative **4,672**.\n''')
    add.write_text(add.read_text()+'''\n\n## CODE.2 Batch 128 checkpoint\nBatch 128 reconciles `379D0..37AFB`: **293 translated + 7 alignment/separator bytes**. Coverage is **231,267 / 239,698 = 96.4827% (96.48%)**, with **8,431 unresolved bytes**. Preserve the fill entry's EDX byte rotation/alignment and the DOS memory wrappers' failure outputs. Next boundary is `37AFC`; `36CEC..372F5` remains unresolved.\n''')
def verify():
    s=cpp.read_text()
    for x in ('226595u','4672u','function_379d0','function_37a07','function_37a73','function_37a9e','function_37ac2','function_37ad4','+ 433u + 293u);'):
        if x not in s: raise SystemExit('verify '+x)
    if '231,267 / 239,698 = 96.4827% (96.48%)' not in pth.read_text(): raise SystemExit('progress')
if __name__=='__main__': {'integrate':integrate,'verify':verify}[sys.argv[1]]()
