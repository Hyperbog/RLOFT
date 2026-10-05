from pathlib import Path
import os,re
R=Path('Code2');C=R/'Code2.cpp';L=R/'RESIDUAL_LEDGER.md';P=R/'PROGRESS.md';A=R/'CODE2_CONTINUITY_ADDENDUM.md';K=R/'RESUME_CHECKPOINT.md';F=R/'.batch84.fragment.cpp'
def norm(s): return s.rstrip()+"\n"
def integrate():
    c=C.read_text(); f=F.read_text().rstrip()+"\n"
    assert 'constexpr u32 kVerifiedTranslatedBytes = 209871u;' in c
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4257u;' in c
    assert '// Batch 84:' not in c
    c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 209871u;','constexpr u32 kVerifiedTranslatedBytes = 211397u;',1)
    c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 4257u;','constexpr u32 kVerifiedNonCodeBytes = 4259u;',1)
    pat=r'(static_assert\(kVerifiedTranslatedBytes ==[\s\S]*?)(\);)'
    m=re.search(pat,c); assert m
    assert '+ 1526u' not in m.group(1)
    repl=m.group(1).rstrip()+' + 1526u'+m.group(2)
    c=c[:m.start()]+repl+c[m.end():]
    marker='\n} // namespace code2\n'; assert c.endswith(marker)
    C.write_text(norm(c[:-len(marker)]+'\n\n'+f+marker))

    l=L.read_text(); end='<!-- RESIDUAL-RANGES-END -->'; assert end in l
    assert '32F4A..32F4B' not in l
    l=l.replace(end,'32F4A..32F4B | alignment | 2\n'+end,1)
    l=l.replace('**Total proven non-code/data:** **4,257 bytes**.','**Total proven non-code/data:** **4,259 bytes**.',1)
    L.write_text(norm(l))

    p=P.read_text()
    p=re.sub(r'object%20accounting-[0-9.]+%25','object%20accounting-89.97%25',p,count=1)
    p=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*','**Verified translated instruction bytes:** **211,397**',p,count=1)
    p=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*','**Proven non-code/data bytes:** **4,259**',p,count=1)
    p=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*','**Reconciled object bytes:** **215,656 / 239,698 = 89.9699% (89.97%)**',p,count=1)
    p=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*','**Unresolved object bytes:** **24,042**',p,count=1)
    P.write_text(norm(p.rstrip()+'''\n## Batch 84 — world/grid sweep state machine\n\nAdded **1,526 translated instruction bytes** and **2 proven alignment bytes**, bringing accounting to **211,397 translated + 4,259 non-code = 215,656 / 239,698 = 89.9699% (89.97%)**. Unresolved is **24,042 bytes**. `32954` preserves the four angular quadrant setups, the dynamically patched signed `JLE/JGE` sweep predicates at `32C1A` and `32D04`, the low-word-only `ADD AX` path, cached `1E930` bounds, and distinct left/right collision exits through `32F8C`, `33124`, `332B4`, and `333BC`. The shared sweep/outer control flow remains one ownership unit through `32F49`; `32F4A..32F4B` is unreferenced `XCHG EBX,EBX` alignment. Next frontier: **`32F4C`**.\n'''))

    a=A.read_text()
    A.write_text(norm(a.rstrip()+'''\n## CODE.2 Batch 84 checkpoint\n\nBatch 84 reconciles `32954..32F4B`: **1,526 translated + 2 alignment bytes**. Coverage is **215,656 / 239,698 = 89.9699% (89.97%)**, with **24,042 unresolved bytes**. Preserve the four region-specific step vectors and `356D8` argument pairs, dynamic opcode writes at `32C1A`/`32D04`, unsigned range tests where encoded, low-word-only arithmetic at the right sweep, and the shared backward sweep/outer-loop edges. Next frontier is `32F4C`.\n'''))
def verify():
    c=C.read_text(); l=L.read_text(); p=P.read_text()
    assert 'constexpr u32 kVerifiedTranslatedBytes = 211397u;' in c
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4259u;' in c
    m=re.search(r'static_assert\(kVerifiedTranslatedBytes ==[\s\S]*?\);',c); assert m and '+ 1526u);' in m.group(0)
    for x in ['function_32954','code_store8(0x32C1Au','code_store8(0x32D04u','enum Sweep','app84_add_ax','call_332b4','call_333bc']:
        assert x in c,x
    assert '32F4A..32F4B | alignment | 2' in l
    assert '215,656 / 239,698 = 89.9699% (89.97%)' in p
def checkpoint():
    sha=os.environ['BATCH_SHA']
    K.write_text(norm(f'''# CODE.2 resume checkpoint\n\nDurable handoff anchor; live repository/binary evidence wins.\n\n## Last committed substantive decompilation state\n- Last substantive batch commit: `{sha}` — `Integrate CODE.2 decompilation batch 84`.\n- Verified translated instruction bytes: **211,397**.\n- Proven non-code/data bytes: **4,259**.\n- Reconciled object bytes: **215,656 / 239,698 = 89.9699% (89.97%)**.\n- Unresolved object bytes: **24,042**.\n- Last integrated ownership fold: `32954..32F4B`.\n- Next independently decoded instruction boundary: **`32F4C`**.\n\n## Immutable identity\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n\n## Mandatory overlap anchor for next resume\n- Window `[0x32EC0,0x32FA0)` — 224 bytes; SHA-256 `4b32e9af1a5996bbd7577c4ff66cb7ac8d2445f921fe7d2f16ec4072f9004a97`.\n- Narrow `[0x32F20,0x32F80)` — 96 bytes; SHA-256 `367439d0111ea61333ffc433b4b1b5f4368573639edd7bcf5c4840fca1c86754`.\n- Narrow bytes: `f0550200a1e848020066b80080a3e8550200e8850400000fbf0df25502000fbf1dea550200e968faffff87db506b05fcc600001c05185a0100c70000000000c7400493000000c7400893000000c7400c00000000c7401000000000c740140000`.\n\nInterpretation: `32F45..32F49` is the final Batch-84 jump back into shared sweep/outer control flow; `32F4A..32F4B` is unreferenced `XCHG EBX,EBX` alignment; `32F4C` begins the next independent helper. No Batch 85 bytes are counted.\n'''))
if __name__=='__main__':
    import sys
    globals()[sys.argv[1]]()
