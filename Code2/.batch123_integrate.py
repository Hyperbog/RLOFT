from pathlib import Path
import os, sys
root=Path(__file__).resolve().parents[1]
cpp=root/'Code2/Code2.cpp'; progress=root/'Code2/PROGRESS.md'; ledger=root/'Code2/RESIDUAL_LEDGER.md'; add=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; checkpoint=root/'Code2/RESUME_CHECKPOINT.md'; frag=root/'Code2/.batch123.fragment.cpp'

def once(s,a,b):
    if s.count(a)!=1: raise SystemExit(f'anchor count {s.count(a)} for {a!r}')
    return s.replace(a,b,1)

def integrate():
    s=cpp.read_text()
    s=once(s,'constexpr u32 kVerifiedTranslatedBytes = 224685u;','constexpr u32 kVerifiedTranslatedBytes = 224767u;')
    s=once(s,'constexpr u32 kVerifiedNonCodeBytes = 4528u;','constexpr u32 kVerifiedNonCodeBytes = 4565u;')
    s=once(s,'+ 448u + 328u);','+ 448u + 328u + 82u);')
    marker='} // namespace code2'
    if s.count(marker)!=1: raise SystemExit('namespace marker')
    s=s.replace(marker,'\n'+frag.read_text().rstrip()+'\n\n'+marker)
    cpp.write_text(s)
    p=progress.read_text()
    p=once(p,'object%20accounting-95.63%25-red','object%20accounting-95.68%25-red')
    p=once(p,'**Verified translated instruction bytes:** **224,685**','**Verified translated instruction bytes:** **224,767**')
    p=once(p,'**Proven non-code/data bytes:** **4,528**','**Proven non-code/data bytes:** **4,565**')
    p=once(p,'**Reconciled object bytes:** **229,213 / 239,698 = 95.6257% (95.63%)**','**Reconciled object bytes:** **229,332 / 239,698 = 95.6754% (95.68%)**')
    p=once(p,'**Unresolved object bytes:** **10,485**','**Unresolved object bytes:** **10,366**')
    p+='''\n\n## Batch 123 — PC speaker PIT helpers and radix digit table\n\nAdded **82 translated instruction bytes** and **37 directly proven data bytes**.\n\n- `372F6..37338` — **67 bytes**: programs PIT channel 2 from `0x1234DC / divisor` when the requested divisor exceeds `0x12`; preserves the conditional port-61 gate enable and conditional `43h=B6h` control write before emitting the low/high counter bytes to port `42h`.\n- `37339..37347` — **15 bytes**: clears speaker gate bits 0..1 at port `61h`.\n- `37348..3736C` — **37 bytes data**: NUL-terminated lowercase radix digit alphabet `0123456789abcdefghijklmnopqrstuvwxyz`; directly referenced with a CS override by the independently bounded conversion helper beginning at `3736D`.\n\nFresh overlap window `[372B6,373C8)` SHA-256: `cccc69f3ea9f01660fc5a5fcb3022d991e7243bb42ca2d26199ca71f989bc417`. The pre-`372F6` bytes belong to the still-unresolved large helper ending at `372F5` and were verification only.\n'''
    progress.write_text(p)
    ledger.write_text(ledger.read_text()+'''\n\n## Batch 123 additions\n\n- `37348..3736C` — **37 bytes data**: NUL-terminated lowercase radix digit alphabet. The bytes are exactly `0123456789abcdefghijklmnopqrstuvwxyz\\0`; `37398` loads from `CS:[EDX+37348h]` in the next independently bounded conversion routine, proving the range is data rather than linear code.\n\nBatch 123 adds **37** proven non-code/data bytes; cumulative proven non-code/data is **4,565 bytes**.\n''')
    add.write_text(add.read_text()+'''\n\n## CODE.2 Batch 123 checkpoint\n\n- Integrated `372F6..37347`: PC-speaker PIT channel-2 start/stop helpers, **82 translated bytes**.\n- Classified `37348..3736C`: 37-byte NUL-terminated lowercase radix alphabet, directly indexed by `37398` with a CS override.\n- Accounting: **224,767 translated + 4,565 non-code = 229,332 / 239,698 = 95.6754% (95.68%)**; **10,366 unresolved**.\n- Fresh overlap `[372B6,373C8)` SHA-256 `cccc69f3ea9f01660fc5a5fcb3022d991e7243bb42ca2d26199ca71f989bc417`.\n- `3736D` is the next independently decoded instruction boundary; the large `36CEC..372F5` helper remains unresolved.\n''')

def verify():
    s=cpp.read_text(); p=progress.read_text(); l=ledger.read_text()
    for x in ['function_372f6','function_37339','kVerifiedTranslatedBytes = 224767u','kVerifiedNonCodeBytes = 4565u','+ 448u + 328u + 82u);']:
        if x not in s: raise SystemExit(x)
    if '229,332 / 239,698 = 95.6754% (95.68%)' not in p: raise SystemExit('progress')
    if '`37348..3736C`' not in l: raise SystemExit('ledger')

def checkpoint_update():
    sha=os.environ['BATCH_SHA']
    checkpoint.write_text(f'''# CODE.2 resume checkpoint\n\nDurable handoff anchor; live repository/binary evidence wins.\n\n## Last committed substantive decompilation state\n- Last substantive batch commit: `{sha}` — `Integrate CODE.2 decompilation batch 123`.\n- Verified translated instruction bytes: **224,767**.\n- Proven non-code/data bytes: **4,565**.\n- Reconciled object bytes: **229,332 / 239,698 = 95.6754% (95.68%)**.\n- Unresolved object bytes: **10,366**.\n- Last independently integrated island: `372F6..3736C`.\n- Next independently decoded instruction boundary: **`3736D`**.\n- The earlier large helper `36CEC..372F5` remains unresolved and must not be counted through overlap.\n\n## Immutable identity\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n\n## Mandatory overlap anchor for next resume\n- Window `[0x372B6,0x373C8)` — 274 bytes; SHA-256 `cccc69f3ea9f01660fc5a5fcb3022d991e7243bb42ca2d26199ca71f989bc417`.\n- Batch-owned narrow `[0x372F6,0x3736D)` — 119 bytes; SHA-256 `6bd1174aca75fe65210f87fb0ebe484e9cb6060f82f597e6251a6bf66403ad67`.\n- Overlap bytes are verification only and must not be recounted.\n\n## Batch 123 interpretation\n- `372F6..37338`: PC speaker/PIT channel-2 start helper; divisor threshold `0x12`, counter `0x1234DC/divisor`, conditional port-61 enable and PIT mode write, then low/high bytes to port 42h.\n- `37339..37347`: speaker gate disable helper.\n- `37348..3736C`: 37-byte NUL-terminated lowercase radix digit alphabet, directly referenced by `37398`.\n- `3736D` begins the next conversion helper and remains uncounted.\n''')

if __name__=='__main__':
    {'integrate':integrate,'verify':verify,'checkpoint':checkpoint_update}[sys.argv[1]]()
