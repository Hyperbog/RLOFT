from pathlib import Path
import sys
r=Path.cwd(); cpp=r/'Code2/Code2.cpp'; pth=r/'Code2/PROGRESS.md'; led=r/'Code2/RESIDUAL_LEDGER.md'; add=r/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; frag=r/'Code2/.batch126.fragment.cpp'
def once(s,a,b):
 if a not in s: raise SystemExit('missing '+a[:60])
 return s.replace(a,b,1)
def integrate():
 s=cpp.read_text(); s=once(s,'225364u','225869u'); s=once(s,'4624u','4650u')
 # cumulative assertion: append this batch to the existing final summand.
 s=once(s,'+ 82u + 320u + 277u);','+ 82u + 320u + 277u + 505u);')
 close='} // namespace code2'; f=frag.read_text(); s=s.rsplit(close,1)[0]+f+'\n'+close+s.rsplit(close,1)[1]; cpp.write_text(s)
 p=pth.read_text(); p=once(p,'object%20accounting-95.95%25-red','object%20accounting-96.17%25-red'); p=once(p,'**Verified translated instruction bytes:** **225,364**','**Verified translated instruction bytes:** **225,869**'); p=once(p,'**Proven non-code/data bytes:** **4,624**','**Proven non-code/data bytes:** **4,650**'); p=once(p,'**Reconciled object bytes:** **229,988 / 239,698 = 95.9491% (95.95%)**','**Reconciled object bytes:** **230,519 / 239,698 = 96.1706% (96.17%)**'); p=once(p,'**Unresolved object bytes:** **9,710**','**Unresolved object bytes:** **9,179**'); p+='''\n\n## Batch 126 — INT 24h bridge and dictionary decoder\n\nAdded **505 translated instruction bytes** and **26 proven alignment bytes** across `375FD..3780F`. `375FE..37641` stores/installs the critical-error handler bridge, preserving the extender-mode split; `37642..376E8` is the full interrupt bridge with saved SS:ESP, private stack switch, BP:SI linearization, far callback, LSS restoration, and IRET. `376F0` is an address-taken one-byte RET stub. `37700..3780C` is a 4 KiB ring-dictionary decoder with 16-bit flag state, 0FFFh wrapping, literal and back-reference paths, and inclusive `length_nibble+2` copy bound. Proven separators are `375FD`, `376E9..376EF`, `376F1..376FF`, and `3780D..3780F`; direct-target scanning confirms `376F0`, `37700`, and `37810` are real entry points. Accounting: **225,869 translated + 4,650 non-code/data = 230,519 / 239,698 = 96.1706% (96.17%)**, leaving **9,179 unresolved bytes**. Next boundary: **`37810`**.\n'''; pth.write_text(p)
 led.write_text(led.read_text()+'''\n\n## Batch 126 additions\n- `375FD` — 1 zero separator byte before the addressable `375FE` routine.\n- `376E9..376EF` — 7 zero separator bytes before the address-taken `376F0` RET stub.\n- `376F1..376FF` — 15 zero separator bytes before the directly called `37700` decoder.\n- `3780D..3780F` — 3-byte `XCHG EBX,EBX; NOP` alignment before directly called `37810`.\n\nBatch 126 adds **26 proven non-code/alignment bytes**, cumulative **4,650**.\n''')
 add.write_text(add.read_text()+'''\n\n## CODE.2 Batch 126 checkpoint\nBatch 126 reconciles `375FD..3780F`: **505 translated + 26 alignment bytes**. Coverage is **230,519 / 239,698 = 96.1706% (96.17%)**, with **9,179 unresolved bytes**. Preserve the INT 24h bridge's private-stack/LSS/IRET behavior and the decoder's 16-bit flag/dictionary arithmetic. Next boundary is `37810`; `36CEC..372F5` remains unresolved.\n''')
def verify():
 s=cpp.read_text()
 for x in ('225869u','4650u','function_375fe','interrupt_bridge_37642','function_376f0','function_37700','+ 277u + 505u);'):
  if x not in s: raise SystemExit('verify '+x)
 if '230,519 / 239,698 = 96.1706% (96.17%)' not in pth.read_text(): raise SystemExit('progress')
if __name__=='__main__': {'integrate':integrate,'verify':verify}[sys.argv[1]]()
