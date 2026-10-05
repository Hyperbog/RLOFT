from pathlib import Path
import os,re
R=Path('Code2');C=R/'Code2.cpp';P=R/'PROGRESS.md';L=R/'RESIDUAL_LEDGER.md';A=R/'CODE2_CONTINUITY_ADDENDUM.md';K=R/'RESUME_CHECKPOINT.md';F=R/'.batch115.fragment.cpp'

def norm(s): return s.rstrip()+"\n"
def once(s,a,b):
    assert a in s,a
    return s.replace(a,b,1)

def integrate():
    c=C.read_text();f=F.read_text().rstrip()+"\n"
    assert 'constexpr u32 kVerifiedTranslatedBytes = 221731u;' in c
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4465u;' in c
    assert '// Batch 115:' not in c
    c=once(c,'constexpr u32 kVerifiedTranslatedBytes = 221731u;','constexpr u32 kVerifiedTranslatedBytes = 221820u;')
    c=once(c,'constexpr u32 kVerifiedNonCodeBytes = 4465u;','constexpr u32 kVerifiedNonCodeBytes = 4468u;')
    c=once(c,'+ 535u + 70u + 488u);','+ 535u + 70u + 488u + 89u);')
    c=once(c,'\n} // namespace code2\n','\n'+f+'\n} // namespace code2\n')
    C.write_text(norm(c))

    p=P.read_text()
    p=once(p,'object%20accounting-94.37%25-red','object%20accounting-94.41%25-red')
    p=once(p,'**Verified translated instruction bytes:** **221,731**','**Verified translated instruction bytes:** **221,820**')
    p=once(p,'**Proven non-code/data bytes:** **4,465**','**Proven non-code/data bytes:** **4,468**')
    p=once(p,'**Reconciled object bytes:** **226,196 / 239,698 = 94.3671% (94.37%)**','**Reconciled object bytes:** **226,288 / 239,698 = 94.4055% (94.41%)**')
    p=once(p,'**Unresolved object bytes:** **13,502**','**Unresolved object bytes:** **13,410**')
    p+=('\n\n## CODE.2 Batch 115 checkpoint\n\n'
        'Batch 115 reconciles `35878..358D3`: **89 translated instruction bytes + 3 proven alignment bytes**. '
        'Cumulative accounting is **221,820 translated + 4,468 non-code = 226,288 / 239,698 = 94.4055% (94.41%)**; '
        '**13,410 bytes** remain unresolved.\n\n'
        '`35878` preserves the selector-qualified `DS:source -> ES:destination` REP MOVS path when flag bit 0 is set; '
        'the alternate branch retains the encoded `38407(source,offset,0) -> 38106(source,destination,length) -> 3967C(source)` chain and returns the final close result. '
        '`358D1..358D3` is an unreferenced `LEA EAX,[EAX+0]` separator. `358D4` has an independent direct caller and begins the next loader routine.\n')
    P.write_text(norm(p))

    l=L.read_text()
    assert '358D1..358D3 | alignment | 3' not in l
    l=once(l,'<!-- RESIDUAL-RANGES-END -->','358D1..358D3 | alignment | 3\n<!-- RESIDUAL-RANGES-END -->')
    l=once(l,'**Total proven non-code/data:** **4,465 bytes**.','**Total proven non-code/data:** **4,468 bytes**.')
    l+=('\nBatch 115 evidence: `358D1..358D3` decodes exactly as `LEA EAX,[EAX+0]`; fresh whole-object direct-control-flow and raw-address scans find no reference into those three bytes, '
        'while `358D4` has an independent direct caller from `35A65`.\n')
    L.write_text(norm(l))

    a=A.read_text()
    a+=('\n## CODE.2 Batch 115 checkpoint\n\n'
        'Batch 115 reconciles `35878..358D3`: **89 translated + 3 proven alignment bytes**, bringing cumulative accounting to '
        '**226,288 / 239,698 = 94.4055% (94.41%)** with **13,410 unresolved bytes**. Preserve the direct path as selector-aware REP MOVS and the alternate '
        '`38407/38106/3967C` seek-read-close ordering. Next frontier: `358D4`.\n')
    A.write_text(norm(a))

def verify():
    c=C.read_text();p=P.read_text();l=L.read_text();a=A.read_text()
    for x in ['Runtime115Boundary','function_35878','copy_ds_to_es','static_assert(0x358D1u-0x35878u==89u);','static_assert(0x358D4u-0x358D1u==3u);']:
        assert x in c,x
    assert 'constexpr u32 kVerifiedTranslatedBytes = 221820u;' in c
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4468u;' in c
    assert '226,288 / 239,698 = 94.4055% (94.41%)' in p
    assert '**13,410 bytes** remain unresolved' in p
    assert '358D1..358D3 | alignment | 3' in l
    assert '**Total proven non-code/data:** **4,468 bytes**.' in l
    assert 'CODE.2 Batch 115 checkpoint' in a
    body=l.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split('<!-- RESIDUAL-RANGES-END -->',1)[0]
    total=0
    for line in body.splitlines():
        m=re.match(r'^([0-9A-F]+)\.\.([0-9A-F]+) \| [^|]+ \| (\d+)$',line.strip())
        if m:
            lo,hi,n=int(m.group(1),16),int(m.group(2),16),int(m.group(3))
            assert hi-lo+1==n,(line,hi-lo+1,n)
            total+=n
    assert total==4468,total

def checkpoint():
    sha=os.environ['BATCH_SHA']
    K.write_text(norm(f'''# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `{sha}` — `Integrate CODE.2 decompilation batch 115`.
- Verified translated instruction bytes: **221,820**.
- Proven non-code/data bytes: **4,468**.
- Reconciled object bytes: **226,288 / 239,698 = 94.4055% (94.41%)**.
- Unresolved object bytes: **13,410**.
- Last integrated ownership fold: `35878..358D3`.
- Next independently decoded instruction boundary: **`358D4`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x35870,0x35950)` — 224 bytes; SHA-256 `47bb1fa97187d5a3b11f020a3c0567e35ab9a9ff135ca8780fc9587e42213a0b`.
- Narrow `[0x358C0,0x35920)` — 96 bytes; SHA-256 `eada8f8c1b1484303fc1478ed3bb2f86b5d497154163e3310838d790b78efc25`.
- Narrow bytes: `000083c40c56e8b13d000083c4045f5ec38d400056575581eccc000000a17058030031ff898424c8000000f68424e00000000174098bac24dc000000eb226800020000ffb424e0000000e86825000083c40889c583f8ff750731c0e9df000000`.

Interpretation: `35878..358D0` is the translated selector-aware copy/file-I/O helper and `358D1..358D3` is proven alignment. `358D4` begins an independently called loader routine.
'''))

if __name__=='__main__':
    import sys
    {'integrate':integrate,'verify':verify,'checkpoint':checkpoint}[sys.argv[1]]()
