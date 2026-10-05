from pathlib import Path
import os,re
R=Path('Code2');C=R/'Code2.cpp';P=R/'PROGRESS.md';L=R/'RESIDUAL_LEDGER.md';A=R/'CODE2_CONTINUITY_ADDENDUM.md';K=R/'RESUME_CHECKPOINT.md';F=R/'.batch114.fragment.cpp'

def norm(s): return s.rstrip()+"\n"
def once(s,a,b):
    assert a in s,a
    return s.replace(a,b,1)

def integrate():
    c=C.read_text();f=F.read_text().rstrip()+"\n"
    assert 'constexpr u32 kVerifiedTranslatedBytes = 221243u;' in c
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4449u;' in c
    assert '// Batch 114:' not in c
    c=once(c,'constexpr u32 kVerifiedTranslatedBytes = 221243u;','constexpr u32 kVerifiedTranslatedBytes = 221731u;')
    c=once(c,'constexpr u32 kVerifiedNonCodeBytes = 4449u;','constexpr u32 kVerifiedNonCodeBytes = 4465u;')
    c=once(c,'+ 1182u + 284u + 535u + 70u);','+ 1182u + 284u + 535u + 70u + 488u);')
    c=once(c,'\n} // namespace code2\n','\n'+f+'\n} // namespace code2\n')
    C.write_text(norm(c))

    p=P.read_text()
    p=once(p,'object%20accounting-94.16%25-red','object%20accounting-94.37%25-red')
    p=once(p,'**Verified translated instruction bytes:** **221,243**','**Verified translated instruction bytes:** **221,731**')
    p=once(p,'**Proven non-code/data bytes:** **4,449**','**Proven non-code/data bytes:** **4,465**')
    p=once(p,'**Reconciled object bytes:** **225,692 / 239,698 = 94.1568% (94.16%)**','**Reconciled object bytes:** **226,196 / 239,698 = 94.3671% (94.37%)**')
    p=once(p,'**Unresolved object bytes:** **14,006**','**Unresolved object bytes:** **13,502**')
    p+=('\n\n## CODE.2 Batch 114 checkpoint\n\n'
        'Batch 114 reconciles `35680..35877`: **488 translated instruction bytes + 16 proven non-code/data bytes**. '
        'Cumulative accounting is **221,731 translated + 4,465 non-code = 226,196 / 239,698 = 94.3671% (94.37%)**; '
        '**13,502 bytes** remain unresolved.\n\n'
        '`35680` and `356D8` preserve the encoded 16-bit multiply/carry/sign machinery; `35730` builds the bounded divisor used by the projection path; '
        '`35798` preserves PUSHA/POPA register ownership and only publishes its two word outputs. `35797` is an unreferenced NOP separator. '
        '`35869..3586F` is zero alignment and `35870..35877` holds two referenced `00002020h` constants. `35878` is independently called from seventeen sites and starts the next runtime copy/I/O family.\n')
    P.write_text(norm(p))

    l=L.read_text()
    assert '35797..35797 | alignment | 1' not in l
    l=once(l,'<!-- RESIDUAL-RANGES-END -->','35797..35797 | alignment | 1\n35869..3586F | alignment | 7\n35870..35877 | data | 8\n<!-- RESIDUAL-RANGES-END -->')
    l=once(l,'**Total proven non-code/data:** **4,449 bytes**.','**Total proven non-code/data:** **4,465 bytes**.')
    l+=('\nBatch 114 evidence: `35797` is a standalone NOP with no incoming direct/raw reference between independently bounded functions. '
        '`35869..3586F` is a seven-byte zero alignment island; `35870` and `35874` are separately referenced as dword constants by `358DD` and `35A1C`, '
        'proving `35870..35877` as data. `35878` has seventeen independent direct callers.\n')
    L.write_text(norm(l))

    a=A.read_text()
    a+=('\n## CODE.2 Batch 114 checkpoint\n\n'
        'Batch 114 reconciles `35680..35877`: **488 translated + 16 proven non-code/data bytes**, bringing cumulative accounting to '
        '**226,196 / 239,698 = 94.3671% (94.37%)** with **13,502 unresolved bytes**. The two multiply helpers retain their asymmetric factor-sign handling; '
        '`35730` retains the signed clamp followed by unsigned DIV; `35798` retains PUSHA/POPA register restoration and its two word-only outputs. '
        'Next frontier: `35878`.\n')
    A.write_text(norm(a))

def verify():
    c=C.read_text();p=P.read_text();l=L.read_text();a=A.read_text()
    for x in ['app114_mul_fraction','function_35680','function_356d8','function_35730','function_35798','static_assert(0x35869u-0x35798u==209u);']:
        assert x in c,x
    assert 'constexpr u32 kVerifiedTranslatedBytes = 221731u;' in c
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4465u;' in c
    assert '226,196 / 239,698 = 94.3671% (94.37%)' in p
    assert '**13,502 bytes** remain unresolved' in p
    for x in ['35797..35797 | alignment | 1','35869..3586F | alignment | 7','35870..35877 | data | 8']:
        assert x in l,x
    assert '**Total proven non-code/data:** **4,465 bytes**.' in l
    assert 'CODE.2 Batch 114 checkpoint' in a
    body=l.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split('<!-- RESIDUAL-RANGES-END -->',1)[0]
    total=0
    for line in body.splitlines():
        m=re.match(r'^([0-9A-F]+)\.\.([0-9A-F]+) \| [^|]+ \| (\d+)$',line.strip())
        if m:
            lo,hi,n=int(m.group(1),16),int(m.group(2),16),int(m.group(3))
            assert hi-lo+1==n,(line,hi-lo+1,n)
            total+=n
    assert total==4465,total

def checkpoint():
    sha=os.environ['BATCH_SHA']
    K.write_text(norm(f'''# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `{sha}` — `Integrate CODE.2 decompilation batch 114`.
- Verified translated instruction bytes: **221,731**.
- Proven non-code/data bytes: **4,465**.
- Reconciled object bytes: **226,196 / 239,698 = 94.3671% (94.37%)**.
- Unresolved object bytes: **13,502**.
- Last integrated ownership fold: `35680..35877`.
- Next independently decoded instruction boundary: **`35878`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x357F0,0x358D0)` — 224 bytes; SHA-256 `346b8672efe32399aaec41fd3c473d478cdec6f91d732835a675b61099a23b8a`.
- Narrow `[0x35860,0x358C0)` — 96 bytes; SHA-256 `7b3b9d08d56d4f000e36725fdf51a74623cf6e48dc7e85594aa756e3eb134648`.
- Narrow bytes: `07b80100000061c9c300000000000000202000002020000056578b74240c8b5424108b5c241cf644241401741e8b7c241889d901d65789c8c1e902f2a58ac880e103f2a45f8d041a5f5ec36a005256e8532b000083c40c53ff74241c56e84428`.

Interpretation: `35680..35868` is the translated fixed-point/projection helper family. `35797` is proven NOP alignment; `35869..3586F` is zero alignment; `35870..35877` contains two referenced `00002020h` dwords. `35878` begins the next independently called runtime copy/I/O helper.
'''))

if __name__=='__main__':
    import sys
    {'integrate':integrate,'verify':verify,'checkpoint':checkpoint}[sys.argv[1]]()
