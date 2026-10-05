from pathlib import Path
import os,re
ROOT=Path('Code2')
CPP=ROOT/'Code2.cpp'; LED=ROOT/'RESIDUAL_LEDGER.md'; PROG=ROOT/'PROGRESS.md'; ADD=ROOT/'CODE2_CONTINUITY_ADDENDUM.md'; CK=ROOT/'RESUME_CHECKPOINT.md'; FRAG=ROOT/'.batch73.fragment.cpp'
OLD_T=204632; OLD_N=4212; ADD_T=1025; ADD_N=3; NEW_T=205657; NEW_N=4215; TOTAL=239698; RECON=209872; UNRES=29826
ledger_lines='''318B1..318B3 | alignment | 3\n'''
progress='''\n## Batch 73 — perspective-row renderer\n\nAdded **1,025 translated instruction bytes** and **3 directly proven alignment bytes**, bringing cumulative accounting to **205,657 translated + 4,215 non-code = 209,872 / 239,698 = 87.5568% (87.56%)**. Unresolved is **29,826 bytes**.\n\nInstruction ownership is `314B0..318B0`; `318B1..318B3` is the compiler LEA alignment before the next independent routine. A full object-1 direct CALL/JMP/Jcc target scan found no incoming target into those three bytes.\n\nFidelity notes: record zero is processed unconditionally while later records terminate on word0==0; the random decoration prepass reuses the two arguments left on the cdecl stack by `30CA2`, yielding the exact effective `29838(0,4,sprite,random_set,8)` call; the initial and post-prepass RNG seeds are `C658` and the live tick from `30700`; signed projection uses the encoded `(delta<<7)+12h` then signed divide by 24h; and failed visibility tests try +168h wrap, then -168h wrap without a final visibility rejection. The next independent routine begins at **`318B4`**.\n'''
addendum='''\n## CODE.2 Batch 73 checkpoint\n\nBatch 73 added **1,025 translated instruction bytes** plus **3 directly proven alignment bytes**, reconciling through `318B3`. Cumulative accounting is **205,657 translated + 4,215 non-code = 209,872 / 239,698 = 87.5568% (87.56%)**; unresolved is **29,826 bytes**.\n\nThe fold is the complete `314B0..318B0` perspective-row renderer. Preserve unconditional record-zero processing, the zero-word sentinel only on later records, cdecl stack reuse around `30CA2`/`29838`, RNG reseeding, signed wrapped projection arithmetic, and the three-position horizontal wrap search. The next independent routine begins at `318B4`.\n'''

def norm(s): return s.rstrip()+"\n"

def integrate():
    cpp=CPP.read_text(); frag=FRAG.read_text().rstrip()+"\n"
    assert 'constexpr u32 kVerifiedTranslatedBytes = 204632u;' in cpp
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4212u;' in cpp
    assert '// Batch 73:' not in cpp and '==1025u);' in frag
    cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 204632u;','constexpr u32 kVerifiedTranslatedBytes = 205657u;',1)
    cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 4212u;','constexpr u32 kVerifiedNonCodeBytes = 4215u;',1)
    old='+ 4070u + 1843u + 1852u);'; assert old in cpp
    cpp=cpp.replace(old,'+ 4070u + 1843u + 1852u + 1025u);',1)
    marker='\n} // namespace code2\n'; assert cpp.endswith(marker)
    cpp=cpp[:-len(marker)]+'\n\n'+frag+marker
    CPP.write_text(norm(cpp))

    led=LED.read_text(); mark='<!-- RESIDUAL-RANGES-END -->'; assert mark in led and '318B1..318B3 | alignment | 3' not in led
    led=led.replace(mark,ledger_lines+mark,1)
    led=led.replace('**Total proven non-code/data:** **4,212 bytes**.','**Total proven non-code/data:** **4,215 bytes**.',1)
    LED.write_text(norm(led))

    prog=PROG.read_text(); assert '## Batch 73 —' not in prog
    prog=re.sub(r'object%20accounting-[0-9.]+%25','object%20accounting-87.56%25',prog,count=1)
    prog=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*','**Verified translated instruction bytes:** **205,657**',prog,count=1)
    prog=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*','**Proven non-code/data bytes:** **4,215**',prog,count=1)
    prog=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*','**Reconciled object bytes:** **209,872 / 239,698 = 87.5568% (87.56%)**',prog,count=1)
    prog=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*','**Unresolved object bytes:** **29,826**',prog,count=1)
    PROG.write_text(norm(prog.rstrip()+progress))
    add=ADD.read_text(); assert '## CODE.2 Batch 73 checkpoint' not in add
    ADD.write_text(norm(add.rstrip()+addendum))

def ledger_sum(text):
    part=text.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split('<!-- RESIDUAL-RANGES-END -->',1)[0]
    total=0
    for ln in part.splitlines():
        m=re.match(r'^([0-9A-F]+)\.\.([0-9A-F]+) \| [^|]+ \| (\d+)$',ln.strip())
        if m:
            a,z,n=int(m.group(1),16),int(m.group(2),16),int(m.group(3)); assert z-a+1==n; total+=n
    return total

def verify():
    cpp=CPP.read_text(); led=LED.read_text(); prog=PROG.read_text(); add=ADD.read_text()
    assert 'constexpr u32 kVerifiedTranslatedBytes = 205657u;' in cpp
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4215u;' in cpp
    assert cpp.count('// Batch 73:')==1
    assert '+ 1843u + 1852u + 1025u);' in cpp
    assert 'static_assert(0x318B1u-0x314B0u==1025u);' in cpp
    for a in ['function_314b0','function_30cc4(m,function_30700(m))','i!=0u && load_le16(dg(m,rec))==0u','screen_for(0x168)','screen_for(-0x168)']:
        assert a in cpp,a
    assert ledger_sum(led)==4215,ledger_sum(led)
    assert '318B1..318B3 | alignment | 3' in led
    assert '**Total proven non-code/data:** **4,215 bytes**.' in led
    assert '209,872 / 239,698 = 87.5568% (87.56%)' in prog
    assert 'The next independent routine begins at **`318B4`**.' in prog
    assert 'CODE.2 Batch 73 checkpoint' in add

def checkpoint():
    sha=os.environ['BATCH_SHA']
    text=f'''# CODE.2 resume checkpoint\n\nThis file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.\n\n## Last committed substantive decompilation state\n\n- Last substantive batch commit: `{sha}` — `Integrate CODE.2 decompilation batch 73`.\n- Verified translated instruction bytes: **205,657**.\n- Proven non-code/data bytes: **4,215**.\n- Reconciled object bytes: **209,872 / 239,698 = 87.5568% (87.56%)**.\n- Unresolved object bytes: **29,826**.\n- Last integrated ownership fold: `314B0..318B3` (instructions through `318B0`, proven alignment through `318B3`).\n- Next independently decoded instruction boundary: **`318B4`**.\n\n## Immutable identity\n\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n- Object-1 file start: `0x3AF24`.\n- Object-1 length: `0x3A852`.\n- Address convention: all reconstruction addresses are object-relative offsets.\n\n## Mandatory overlap anchor for next resume\n\nRe-disassemble at minimum `31830..31913` from freshly regenerated object 1 before doing new work.\n\n- Window: object-relative `[0x31830, 0x31914)` — 228 bytes.\n- SHA-256: `TO_BE_FILLED`.\n- Narrow boundary window: `[0x31880, 0x318E0)` — 96 bytes.\n- Narrow SHA-256: `TO_BE_FILLED_NARROW`.\n- Narrow boundary bytes: `TO_BE_FILLED_BYTES`.\n\nInterpretation to re-prove on resume: `318B0` is the final Batch-73 `RET`; `318B1..318B3` is three-byte LEA alignment with no incoming direct reference; `318B4` begins the next independent routine.\n\n## Interrupted/in-progress state\n\n**UNCOUNTED / NOT COMMITTED:** no Batch 74 bytes are authorized by this checkpoint. Any analysis at or beyond `318B4` must be revalidated against the overlap before counting.\n\n## Resume rule\n\nDo not rework Batch 73 merely because the overlap is re-disassembled. Verify overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `318B4`.\n'''
    CK.write_text(norm(text))

if __name__=='__main__':
    import sys
    if len(sys.argv)!=2 or sys.argv[1] not in {'integrate','verify','checkpoint'}: raise SystemExit('usage: batch73_integrate.py integrate|verify|checkpoint')
    globals()[sys.argv[1]]()
