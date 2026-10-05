from pathlib import Path
import os
R=Path('Code2');C=R/'Code2.cpp';P=R/'PROGRESS.md';A=R/'CODE2_CONTINUITY_ADDENDUM.md';K=R/'RESUME_CHECKPOINT.md'
def norm(s): return s.rstrip()+"\n"
def patch():
    c=C.read_text()
    old='function_32468(m,0xF9D0u,start,count,factor,3u,target);'
    new='function_32468(m,0xF9D0u,start,end,factor,3u,target);'
    assert c.count(old)==2, c.count(old)
    c=c.replace(old,new)
    C.write_text(norm(c))
    p=P.read_text()
    marker='## Batch 80 fidelity correction — raw interpolation count'
    assert marker not in p
    P.write_text(norm(p.rstrip()+'''\n## Batch 80 fidelity correction — raw interpolation count\n\nFresh call-site reconciliation found that `32220` and `322EC` pass their raw third argument (`end`) as the third argument to `32468`, while the scratch-palette restore length remains the inclusive `(end-start+1)` span. The previously integrated C++ incorrectly passed that inclusive span to `32468`. Both calls now preserve the encoded asymmetry. Byte accounting and the `324CC` frontier are unchanged.\n'''))
    a=A.read_text()
    marker2='## CODE.2 Batch 80 post-commit fidelity correction'
    assert marker2 not in a
    A.write_text(norm(a.rstrip()+'''\n## CODE.2 Batch 80 post-commit fidelity correction\n\nDirect stack tracing at the `32468` call sites proved that `32220` and `322EC` pass the raw third argument as the interpolation count, while their restore copies use the separately computed inclusive `(end-start+1)` span. The reconstruction now preserves this asymmetry. Accounting remains **208,723 translated + 4,245 non-code = 212,968 / 239,698 = 88.8485% (88.85%)** and the next frontier remains `324CC`.\n'''))
def verify():
    c=C.read_text();p=P.read_text();a=A.read_text()
    good='function_32468(m,0xF9D0u,start,end,factor,3u,target);'
    bad='function_32468(m,0xF9D0u,start,count,factor,3u,target);'
    assert c.count(good)==2 and bad not in c
    assert 'const s32 count=app74_wrap_add' in c
    assert 'Batch 80 fidelity correction — raw interpolation count' in p
    assert 'Batch 80 post-commit fidelity correction' in a
    assert 'constexpr u32 kVerifiedTranslatedBytes = 208723u;' in c
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4245u;' in c
def checkpoint():
    sha=os.environ['CORRECTION_SHA']
    k=K.read_text()
    anchor='- Last substantive batch commit: `7bd336d65a38f3638339b8bee762875a60af730f` — `Integrate CODE.2 decompilation batch 80`.\n'
    assert anchor in k
    assert 'Post-batch fidelity correction:' not in k
    k=k.replace(anchor,anchor+f'- Post-batch fidelity correction: `{sha}` — preserve raw third-argument count in `32220`/`322EC` calls to `32468`.\n',1)
    K.write_text(norm(k))
if __name__=='__main__':
    import sys
    globals()[sys.argv[1]]()
