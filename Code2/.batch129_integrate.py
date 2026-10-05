from pathlib import Path
import sys
r=Path.cwd(); cpp=r/'Code2/Code2.cpp'; pth=r/'Code2/PROGRESS.md'; add=r/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; frag=r/'Code2/.batch129.fragment.cpp'
def once(s,a,b):
    if a not in s: raise SystemExit('missing '+a[:80])
    return s.replace(a,b,1)
def integrate():
    s=cpp.read_text(); s=once(s,'226595u','227018u'); s=once(s,'+ 505u + 433u + 293u);','+ 505u + 433u + 293u + 423u);')
    close='} // namespace code2'; f=frag.read_text(); s=s.rsplit(close,1)[0]+f+'\n'+close+s.rsplit(close,1)[1]; cpp.write_text(s)
    p=pth.read_text(); p=once(p,'object%20accounting-96.48%25-red','object%20accounting-96.66%25-red'); p=once(p,'**Verified translated instruction bytes:** **226,595**','**Verified translated instruction bytes:** **227,018**'); p=once(p,'**Reconciled object bytes:** **231,267 / 239,698 = 96.4827% (96.48%)**','**Reconciled object bytes:** **231,690 / 239,698 = 96.6591% (96.66%)**'); p=once(p,'**Unresolved object bytes:** **8,431**','**Unresolved object bytes:** **8,008**'); p+='''\n\n## Batch 129 — heap arena query/allocation/free family\n\nAdded **423 translated instruction bytes** across `37AFC..37CA2`. `37AFC..37B33` scans the runtime arena chain and computes the largest free payload with the encoded -4 adjustment. `37B34..37B41` forwards to the core allocator at `37B42..37C36`, which rejects zero and sizes above `FFFFFFD4h`, rounds to four bytes, enforces a 12-byte minimum, searches arenas, and retries through the two encoded heap-growth paths. `37C37..37C44` forwards to `37C45..37CA2`, which locates the owning arena, releases the pointer, updates cached largest-free state, and clears the heap-error byte. No non-code bytes are claimed in this batch. Next boundary: **`37CA3`**. Accounting: **227,018 translated + 4,672 non-code/data = 231,690 / 239,698 = 96.6591% (96.66%)**, leaving **8,008 unresolved bytes**.\n'''; pth.write_text(p)
    add.write_text(add.read_text()+'''\n\n## CODE.2 Batch 129 checkpoint\nBatch 129 reconciles `37AFC..37CA2`: **423 translated instruction bytes**, no newly classified non-code. Coverage is **231,690 / 239,698 = 96.6591% (96.66%)**, with **8,008 unresolved bytes**. Preserve the allocator's unsigned upper-bound test, four-byte rounding, 12-byte minimum, two distinct growth/retry paths, arena-chain traversal, and address-distinct forwarding wrappers. Next boundary is `37CA3`; `36CEC..372F5` remains unresolved.\n''')
def verify():
    s=cpp.read_text(); p=pth.read_text()
    assert 'kVerifiedTranslatedBytes = 227018u' in s and 'kVerifiedNonCodeBytes = 4672u' in s
    assert '+ 505u + 433u + 293u + 423u);' in s
    assert 'static_assert(0x37CA3u-0x37C45u==94u);' in s
    assert '231,690 / 239,698 = 96.6591% (96.66%)' in p
if __name__=='__main__':
    {'integrate':integrate,'verify':verify}[sys.argv[1]]()
