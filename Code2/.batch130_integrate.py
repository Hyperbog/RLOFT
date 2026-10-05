from pathlib import Path
import sys
r=Path.cwd(); cpp=r/'Code2/Code2.cpp'; pth=r/'Code2/PROGRESS.md'; add=r/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; frag=r/'Code2/.batch130.fragment.cpp'
def once(s,a,b):
    if a not in s: raise SystemExit('missing '+a[:80])
    return s.replace(a,b,1)
def integrate():
    s=cpp.read_text(); s=once(s,'227018u','227486u'); s=once(s,'+ 505u + 433u + 293u + 423u);','+ 505u + 433u + 293u + 423u + 468u);')
    close='} // namespace code2'; f=frag.read_text(); s=s.rsplit(close,1)[0]+f+'\n'+close+s.rsplit(close,1)[1]; cpp.write_text(s)
    p=pth.read_text(); p=once(p,'object%20accounting-96.66%25-red','object%20accounting-96.85%25-red'); p=once(p,'**Verified translated instruction bytes:** **227,018**','**Verified translated instruction bytes:** **227,486**'); p=once(p,'**Reconciled object bytes:** **231,690 / 239,698 = 96.6591% (96.66%)**','**Reconciled object bytes:** **232,158 / 239,698 = 96.8544% (96.85%)**'); p=once(p,'**Unresolved object bytes:** **8,008**','**Unresolved object bytes:** **7,540**'); p+='''\n\n## Batch 130 — errno, stream-buffer, flush, and DOS attribute helpers\n\nAdded **468 translated instruction bytes** across `37CA3..37E76`. `37CA3..37CDF` contains constant/common errno setters and the alternate errno-domain/indirect callback path. `37CE0..37D5F` initializes stream buffering with flag-selected capacities and the embedded one-byte fallback. `37D60..37E23` synchronizes buffered output/input, preserving write-length/error behavior and backward seek of unread input. `37E24..37E76` wraps DOS AH=43h attribute query and its writable/read-only rejection/error translation. No non-code bytes claimed. Next boundary: **`37E77`**. Accounting: **227,486 translated + 4,672 non-code/data = 232,158 / 239,698 = 96.8544% (96.85%)**, leaving **7,540 unresolved bytes**.\n'''; pth.write_text(p)
    add.write_text(add.read_text()+'''\n\n## CODE.2 Batch 130 checkpoint\nBatch 130 reconciles `37CA3..37E76`: **468 translated instruction bytes**, no new non-code. Coverage is **232,158 / 239,698 = 96.8544% (96.85%)**, with **7,540 unresolved bytes**. Preserve distinct errno domains, stream flag bits, capacity choices `86h`/`1`/`1000h`, embedded one-byte fallback, partial-write failure, unread-input backward seek, and DOS AH=43h attribute semantics. Next boundary `37E77`; `36CEC..372F5` remains unresolved.\n''')
def verify():
    s=cpp.read_text(); p=pth.read_text(); assert 'kVerifiedTranslatedBytes = 227486u' in s and 'kVerifiedNonCodeBytes = 4672u' in s; assert '+ 423u + 468u);' in s; assert 'static_assert(0x37E77u-0x37E24u==83u);' in s; assert '232,158 / 239,698 = 96.8544% (96.85%)' in p
if __name__=='__main__': {'integrate':integrate,'verify':verify}[sys.argv[1]]()
