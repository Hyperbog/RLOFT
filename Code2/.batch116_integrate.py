from pathlib import Path
import os,re
R=Path('Code2');C=R/'Code2.cpp';P=R/'PROGRESS.md';L=R/'RESIDUAL_LEDGER.md';A=R/'CODE2_CONTINUITY_ADDENDUM.md';K=R/'RESUME_CHECKPOINT.md';F=R/'.batch116.fragment.cpp'

def norm(s): return s.rstrip()+"\n"
def once(s,a,b):
    assert a in s,a
    return s.replace(a,b,1)

def integrate():
    c=C.read_text();f=F.read_text().rstrip()+"\n"
    assert 'constexpr u32 kVerifiedTranslatedBytes = 221820u;' in c
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4468u;' in c
    assert '// Batch 116:' not in c
    c=once(c,'constexpr u32 kVerifiedTranslatedBytes = 221820u;','constexpr u32 kVerifiedTranslatedBytes = 222129u;')
    c=once(c,'constexpr u32 kVerifiedNonCodeBytes = 4468u;','constexpr u32 kVerifiedNonCodeBytes = 4471u;')
    c=once(c,'+ 70u + 488u + 89u);','+ 70u + 488u + 89u + 309u);')
    c=once(c,'\n} // namespace code2\n','\n'+f+'\n} // namespace code2\n')
    C.write_text(norm(c))

    p=P.read_text()
    # Accounting-neutral Batch-115 wording correction: 3967C is tell/current-position,
    # implemented as 38407(handle,0,1), not close.
    p=p.replace('and returns the final close result.','and returns the final current-position result.',1)
    p=once(p,'object%20accounting-94.41%25-red','object%20accounting-94.54%25-red')
    p=once(p,'**Verified translated instruction bytes:** **221,820**','**Verified translated instruction bytes:** **222,129**')
    p=once(p,'**Proven non-code/data bytes:** **4,468**','**Proven non-code/data bytes:** **4,471**')
    p=once(p,'**Reconciled object bytes:** **226,288 / 239,698 = 94.4055% (94.41%)**','**Reconciled object bytes:** **226,600 / 239,698 = 94.5356% (94.54%)**')
    p=once(p,'**Unresolved object bytes:** **13,410**','**Unresolved object bytes:** **13,098**')
    p+=('\n\n## CODE.2 Batch 116 checkpoint\n\n'
        'Batch 116 reconciles `358D4..35A0B`: **309 translated instruction bytes + 3 proven alignment bytes**. '
        'Cumulative accounting is **222,129 translated + 4,471 non-code = 226,600 / 239,698 = 94.5356% (94.54%)**; '
        '**13,098 bytes** remain unresolved.\n\n'
        '`358D4` preserves the flag-bit-0 split between caller-provided/direct source and a `37E77(source,200h)` opened source. Its four `35878` calls retain '
        'their exact offsets and lengths: 4 bytes at `3Ch`, 2 bytes at the decoded table offset, `ACh` bytes at that same offset, then `18h` bytes per record. '
        'The 2-byte identifier is compared against runtime string `1604`; validation failure executes `380F8(handle)` even in direct-source mode. '
        'The record loop advances by `35878`\'s returned next position and accumulates each record\'s first dword, returning total+0Fh. '
        '`35A09..35A0B` is an unreferenced LEA separator; `35A0C` has independent callers.\n')
    P.write_text(norm(p))

    l=L.read_text()
    assert '35A09..35A0B | alignment | 3' not in l
    l=once(l,'<!-- RESIDUAL-RANGES-END -->','35A09..35A0B | alignment | 3\n<!-- RESIDUAL-RANGES-END -->')
    l=once(l,'**Total proven non-code/data:** **4,468 bytes**.','**Total proven non-code/data:** **4,471 bytes**.')
    l+=('\nBatch 116 evidence: `35A09..35A0B` decodes exactly as `LEA EAX,[EAX+0]`; fresh whole-object direct-control-flow and raw-address scans find no incoming reference, '
        'while `35A0C` is independently called from `27DCA` and `28369`.\n')
    L.write_text(norm(l))

    a=A.read_text()
    a=a.replace('`38407/38106/3967C` seek-read-close ordering.','`38407/38106/3967C` seek-read-tell ordering.',1)
    a+=('\n## CODE.2 Batch 116 checkpoint\n\n'
        'Batch 116 reconciles `358D4..35A0B`: **309 translated + 3 proven alignment bytes**, bringing cumulative accounting to '
        '**226,600 / 239,698 = 94.5356% (94.54%)** with **13,098 unresolved bytes**. Preserve stack-local ownership for the four `35878` destinations rather than '
        'flattening SS/ES locals into DGROUP; preserve validation-failure cleanup even under direct-source mode and the unsigned record-count loop. '
        'Batch 115 documentation is corrected accounting-neutrally: `3967C` is current-position/tell, so that branch is seek-read-tell. Next frontier: `35A0C`.\n')
    A.write_text(norm(a))

def verify():
    c=C.read_text();p=P.read_text();l=L.read_text();a=A.read_text()
    for x in ['Runtime116Boundary','app116_read','function_358d4','call_35878_stack','call_27c70_stack','static_assert(0x35A09u-0x358D4u==309u);']:
        assert x in c,x
    assert 'constexpr u32 kVerifiedTranslatedBytes = 222129u;' in c
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4471u;' in c
    assert '226,600 / 239,698 = 94.5356% (94.54%)' in p
    assert '**13,098 bytes** remain unresolved' in p
    assert '35A09..35A0B | alignment | 3' in l
    assert '**Total proven non-code/data:** **4,471 bytes**.' in l
    assert 'seek-read-tell ordering' in a
    assert 'current-position result' in p
    assert 'CODE.2 Batch 116 checkpoint' in a
    body=l.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split('<!-- RESIDUAL-RANGES-END -->',1)[0]
    total=0
    for line in body.splitlines():
        m=re.match(r'^([0-9A-F]+)\.\.([0-9A-F]+) \| [^|]+ \| (\d+)$',line.strip())
        if m:
            lo,hi,n=int(m.group(1),16),int(m.group(2),16),int(m.group(3))
            assert hi-lo+1==n,(line,hi-lo+1,n)
            total+=n
    assert total==4471,total

def checkpoint():
    sha=os.environ['BATCH_SHA']
    K.write_text(norm(f'''# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `{sha}` — `Integrate CODE.2 decompilation batch 116`.
- Verified translated instruction bytes: **222,129**.
- Proven non-code/data bytes: **4,471**.
- Reconciled object bytes: **226,600 / 239,698 = 94.5356% (94.54%)**.
- Unresolved object bytes: **13,098**.
- Last integrated ownership fold: `358D4..35A0B`.
- Next independently decoded instruction boundary: **`35A0C`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x35990,0x35A70)` — 224 bytes; SHA-256 `4470ab5305088033d53eca6b57aa66c2ea04bc2eecbd1688058daf896f2aafaa`.
- Narrow `[0x359D0,0x35A30)` — 96 bytes; SHA-256 `8e6b572c5e9d26585f64d832de919ce5fef1636cedc4ca1475266494a6534108`.
- Narrow bytes: `005055e8a0feffff83c4144603bc24ac0000003b74244472d7f68424e000000001750955e8ff26000083c4048d470f81c4cc0000005d5f5ec38d400056575581ecd00200008b9c24e0020000a174580300c78424b4020000000000008984249c`.

Interpretation: `358D4..35A08` is the translated compact loader and `35A09..35A0B` is proven alignment. Batch 115's alternate I/O path is documented correctly as seek-read-tell. `35A0C` begins a larger independently called loader.
'''))

if __name__=='__main__':
    import sys
    {'integrate':integrate,'verify':verify,'checkpoint':checkpoint}[sys.argv[1]]()
