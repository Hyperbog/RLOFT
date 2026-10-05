from pathlib import Path
import os,re
R=Path('Code2');C=R/'Code2.cpp';P=R/'PROGRESS.md';L=R/'RESIDUAL_LEDGER.md';A=R/'CODE2_CONTINUITY_ADDENDUM.md';K=R/'RESUME_CHECKPOINT.md';F=R/'.batch113.fragment.cpp'

def norm(s): return s.rstrip()+"\n"
def once(s,a,b):
    assert a in s,a
    return s.replace(a,b,1)

def integrate():
    c=C.read_text(); f=F.read_text().rstrip()+"\n"
    assert 'constexpr u32 kVerifiedTranslatedBytes = 221173u;' in c
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4438u;' in c
    assert '// Batch 113:' not in c
    c=once(c,'constexpr u32 kVerifiedTranslatedBytes = 221173u;','constexpr u32 kVerifiedTranslatedBytes = 221243u;')
    c=once(c,'constexpr u32 kVerifiedNonCodeBytes = 4438u;','constexpr u32 kVerifiedNonCodeBytes = 4449u;')
    c=once(c,'+ 1182u + 284u + 535u);','+ 1182u + 284u + 535u + 70u);')
    c=once(c,'\n} // namespace code2\n','\n'+f+'\n} // namespace code2\n')
    C.write_text(norm(c))

    p=P.read_text()
    p=once(p,'object%20accounting-94.12%25-red','object%20accounting-94.16%25-red')
    p=once(p,'**Verified translated instruction bytes:** **221,173**','**Verified translated instruction bytes:** **221,243**')
    p=once(p,'**Proven non-code/data bytes:** **4,438**','**Proven non-code/data bytes:** **4,449**')
    p=once(p,'**Reconciled object bytes:** **225,611 / 239,698 = 94.1230% (94.12%)**','**Reconciled object bytes:** **225,692 / 239,698 = 94.1568% (94.16%)**')
    p=once(p,'**Unresolved object bytes:** **14,087**','**Unresolved object bytes:** **14,006**')
    p+=('\n\n## CODE.2 Batch 113 checkpoint\n\n'
        'Batch 113 reconciles `3562F..3567F`: **70 translated instruction bytes + 11 proven non-code/data bytes**. '
        'Cumulative accounting is **221,243 translated + 4,449 non-code = 225,692 / 239,698 = 94.1568% (94.16%)**; '
        '**14,006 bytes** remain unresolved.\n\n'
        '`3562F` is the stack-sharing termination continuation (`POP EAX; JMP 35655`). `35632` is the separately entered console-output path used by `3A529`; '
        'it opens the embedded `"con"` name at `35414`, scans the caller message to NUL, writes it through DOS AH=40h, and joins the same AH=4Ch termination tail. '
        '`3566C` restores DS from the CS-qualified saved word at `35675`. `35675..35676` is runtime data written by Batch 112, and `35677..3567F` is zero alignment with no direct/raw references. '
        'The next independently called routine begins at `35680`.\n')
    P.write_text(norm(p))

    l=L.read_text()
    assert '35675..35676 | data | 2' not in l
    l=once(l,'<!-- RESIDUAL-RANGES-END -->','35675..35676 | runtime-data | 2\n35677..3567F | alignment | 9\n<!-- RESIDUAL-RANGES-END -->')
    l=once(l,'**Total proven non-code/data:** **4,438 bytes**.','**Total proven non-code/data:** **4,449 bytes**.')
    l+=('\nBatch 113 evidence: `3553A` writes DS to selector-qualified offset `35675`, while `3566C` reads that same word through CS; '
        'this proves `35675..35676` is runtime data. Fresh whole-object target/address scans find no control-flow or raw-address reference to `35677..3567F`; '
        'those nine bytes are all zero and sit immediately before the independently called `ENTER 0,0` routine at `35680`, proving alignment ownership.\n')
    L.write_text(norm(l))

    a=A.read_text()
    a+=('\n## CODE.2 Batch 113 checkpoint\n\n'
        'Batch 113 reconciles `3562F..3567F`: **70 translated + 11 proven non-code/data bytes**, bringing cumulative accounting to '
        '**225,692 / 239,698 = 94.1568% (94.16%)** with **14,006 unresolved bytes**. Preserve the three address-distinct runtime entries: '
        '`3562F` stack-sharing termination, `35632` DOS console-output/termination, and `3566C` CS-qualified DS restore. '
        '`35675..35676` is the saved DS word and `35677..3567F` is proven zero alignment. Next frontier: `35680`.\n')
    A.write_text(norm(a))

def verify():
    c=C.read_text();p=P.read_text();l=L.read_text();a=A.read_text()
    for x in ['Runtime113Boundary','function_3562f','function_35632','function_3566c','app113_terminate','static_assert(0x35675u-0x3562Fu==70u);','static_assert(0x35680u-0x35675u==11u);']:
        assert x in c,x
    assert 'constexpr u32 kVerifiedTranslatedBytes = 221243u;' in c
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4449u;' in c
    assert '225,692 / 239,698 = 94.1568% (94.16%)' in p
    assert '**14,006 bytes** remain unresolved' in p
    assert '35675..35676 | runtime-data | 2' in l and '35677..3567F | alignment | 9' in l
    assert '**Total proven non-code/data:** **4,449 bytes**.' in l
    assert 'CODE.2 Batch 113 checkpoint' in a
    body=l.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split('<!-- RESIDUAL-RANGES-END -->',1)[0]
    total=0
    for line in body.splitlines():
        m=re.match(r'^([0-9A-F]+)\.\.([0-9A-F]+) \| [^|]+ \| (\d+)$',line.strip())
        if m:
            lo,hi,n=int(m.group(1),16),int(m.group(2),16),int(m.group(3))
            assert hi-lo+1==n,(line,hi-lo+1,n)
            total+=n
    assert total==4449,total

def checkpoint():
    sha=os.environ['BATCH_SHA']
    K.write_text(norm(f'''# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `{sha}` — `Integrate CODE.2 decompilation batch 113`.
- Verified translated instruction bytes: **221,243**.
- Proven non-code/data bytes: **4,449**.
- Reconciled object bytes: **225,692 / 239,698 = 94.1568% (94.16%)**.
- Unresolved object bytes: **14,006**.
- Last integrated ownership fold: `3562F..3567F`.
- Next independently decoded instruction boundary: **`35680`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x35620,0x35700)` — 224 bytes; SHA-256 `1f7dfc7ad7f8d551f353c14f75af9cc9005a511f6909b6360028374a6297a189`.
- Narrow `[0x35660,0x356C0)` — 96 bytes; SHA-256 `4a1e5e83fb5ec8680492ba456bfc2cca91c1a56a6ce9f6dfbfcc13a8473b7e4b`.
- Narrow bytes: `e8c83f000058b44ccd218bc0662e8e1d75560300c30000000000000000000000c8000000665366566657668b5d0c668b750e66837d0a007d08f75d086681f60080668b450866f7e3668bfa668b450a66f7e36603c76683d2006683fe007d0a66`.

Interpretation: `3562F..35674` is the translated shared termination/console-output/DS-restore family. `35675..35676` is the selector-qualified saved-DS runtime word and `35677..3567F` is proven zero alignment. `35680` begins an independently called `ENTER 0,0` arithmetic helper.
'''))

if __name__=='__main__':
    import sys
    {'integrate':integrate,'verify':verify,'checkpoint':checkpoint}[sys.argv[1]]()
