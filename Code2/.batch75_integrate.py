from pathlib import Path
import os,re
ROOT=Path('Code2')
CPP=ROOT/'Code2.cpp'; LED=ROOT/'RESIDUAL_LEDGER.md'; PROG=ROOT/'PROGRESS.md'; ADD=ROOT/'CODE2_CONTINUITY_ADDENDUM.md'; CK=ROOT/'RESUME_CHECKPOINT.md'; FRAG=ROOT/'.batch75.fragment.cpp'
OLD_T=206586; OLD_N=4218; ADD_T=538; ADD_N=2; NEW_T=207124; NEW_N=4220; TOTAL=239698; RECON=211344; UNRES=28354
ledger_lines='''31E72..31E73 | alignment | 2\n'''
progress='''\n## Batch 75 — two-phase packed shading/lookup generator\n\nAdded **538 translated instruction bytes** and **2 directly proven alignment bytes**, bringing cumulative accounting to **207,124 translated + 4,220 non-code = 211,344 / 239,698 = 88.1709% (88.17%)**. Unresolved is **28,354 bytes**.\n\nInstruction ownership is `31C58..31E71`; `31E72..31E73` is the compiler `MOV EAX,EAX` alignment. A full object-1 direct CALL/JMP/Jcc target scan found no incoming target into those two bytes.\n\nFidelity notes: the first phase uses fixed shade/period values 3/4 with argument-dependent left/right widths, captures the signed `IDIV` quotient through AL before clamping, applies the same parity/high-byte rules as the adjacent generators, and publishes its final packed word to `FCFA`. The second phase rebuilds the 128-byte mirrored `FE94/FE14` bands with fixed 15/45 edge widths and the encoded rounded 7/69 interpolation. The next independent routine begins at **`31E74`**.\n'''
addendum='''\n## CODE.2 Batch 75 checkpoint\n\nBatch 75 added **538 translated instruction bytes** plus **2 directly proven alignment bytes**, reconciling through `31E73`. Cumulative accounting is **207,124 translated + 4,220 non-code = 211,344 / 239,698 = 88.1709% (88.17%)**; unresolved is **28,354 bytes**.\n\nThe fold is the complete two-phase packed lookup generator `31C58..31E71`. Preserve its AL-sized quotient capture, signed division and arithmetic half-rounding, parity-driven high-byte decrement, argument-dependent first-phase widths, and fixed mirrored 128-byte second phase. The next independent routine begins at `31E74`.\n'''
def norm(s): return s.rstrip()+"\n"
def integrate():
    cpp=CPP.read_text(); frag=FRAG.read_text().rstrip()+"\n"
    assert 'constexpr u32 kVerifiedTranslatedBytes = 206586u;' in cpp
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4218u;' in cpp
    assert '// Batch 75:' not in cpp and 'static_assert(0x31E72u-0x31C58u==538u);' in frag
    cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 206586u;','constexpr u32 kVerifiedTranslatedBytes = 207124u;',1)
    cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 4218u;','constexpr u32 kVerifiedNonCodeBytes = 4220u;',1)
    old='+ 1852u + 1025u + 929u);'; assert old in cpp
    cpp=cpp.replace(old,'+ 1852u + 1025u + 929u + 538u);',1)
    marker='\n} // namespace code2\n'; assert cpp.endswith(marker)
    CPP.write_text(norm(cpp[:-len(marker)]+'\n\n'+frag+marker))
    led=LED.read_text(); mark='<!-- RESIDUAL-RANGES-END -->'; assert mark in led and '31E72..31E73 | alignment | 2' not in led
    led=led.replace(mark,ledger_lines+mark,1).replace('**Total proven non-code/data:** **4,218 bytes**.','**Total proven non-code/data:** **4,220 bytes**.',1); LED.write_text(norm(led))
    prog=PROG.read_text(); assert '## Batch 75 —' not in prog
    prog=re.sub(r'object%20accounting-[0-9.]+%25','object%20accounting-88.17%25',prog,count=1)
    prog=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*','**Verified translated instruction bytes:** **207,124**',prog,count=1)
    prog=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*','**Proven non-code/data bytes:** **4,220**',prog,count=1)
    prog=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*','**Reconciled object bytes:** **211,344 / 239,698 = 88.1709% (88.17%)**',prog,count=1)
    prog=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*','**Unresolved object bytes:** **28,354**',prog,count=1)
    PROG.write_text(norm(prog.rstrip()+progress))
    add=ADD.read_text(); assert '## CODE.2 Batch 75 checkpoint' not in add; ADD.write_text(norm(add.rstrip()+addendum))
def ledger_sum(text):
    part=text.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split('<!-- RESIDUAL-RANGES-END -->',1)[0]; total=0
    for ln in part.splitlines():
        m=re.match(r'^([0-9A-F]+)\.\.([0-9A-F]+) \| [^|]+ \| (\d+)$',ln.strip())
        if m:
            a,z,n=int(m.group(1),16),int(m.group(2),16),int(m.group(3)); assert z-a+1==n; total+=n
    return total
def verify():
    cpp=CPP.read_text(); led=LED.read_text(); prog=PROG.read_text(); add=ADD.read_text()
    assert 'constexpr u32 kVerifiedTranslatedBytes = 207124u;' in cpp and 'constexpr u32 kVerifiedNonCodeBytes = 4220u;' in cpp
    assert cpp.count('// Batch 75:')==1 and '+ 1025u + 929u + 538u);' in cpp
    for a in ['function_31c58','middle2=0x81-left2-right2','0xFE94u+static_cast<u32>(i)','0xFE14u+static_cast<u32>(0x7F-i)']: assert a in cpp,a
    assert ledger_sum(led)==4220 and '31E72..31E73 | alignment | 2' in led
    assert '**Total proven non-code/data:** **4,220 bytes**.' in led
    assert '211,344 / 239,698 = 88.1709% (88.17%)' in prog and 'The next independent routine begins at **`31E74`**.' in prog
    assert 'CODE.2 Batch 75 checkpoint' in add
def checkpoint():
    sha=os.environ['BATCH_SHA']
    CK.write_text(norm(f'''# CODE.2 resume checkpoint\n\nThis file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.\n\n## Last committed substantive decompilation state\n\n- Last substantive batch commit: `{sha}` — `Integrate CODE.2 decompilation batch 75`.\n- Verified translated instruction bytes: **207,124**.\n- Proven non-code/data bytes: **4,220**.\n- Reconciled object bytes: **211,344 / 239,698 = 88.1709% (88.17%)**.\n- Unresolved object bytes: **28,354**.\n- Last integrated ownership fold: `31C58..31E73` (instructions through `31E71`, proven alignment at `31E72..31E73`).\n- Next independently decoded instruction boundary: **`31E74`**.\n\n## Immutable identity\n\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n- Object-1 file start: `0x3AF24`.\n- Object-1 length: `0x3A852`.\n- Address convention: all reconstruction addresses are object-relative offsets.\n\n## Mandatory overlap anchor for next resume\n\nRe-disassemble at minimum `31DF0..31ECF` from freshly regenerated object 1 before doing new work.\n\n- Window: object-relative `[0x31DF0, 0x31ED0)` — 224 bytes.\n- SHA-256: `d43121acce1172188763eff2bf76c739ef5b9ed7fca5c4854c7ccd18c9466fa0`.\n- Narrow boundary window: `[0x31E20, 0x31E80)` — 96 bytes.\n- Narrow SHA-256: `34455be5efb500028a1b1f9767873899ac1b14fba8ed573aeb0cfb76622c2253`.\n- Narrow boundary bytes:\n  `45f808eb1c8b45dc2b45ec406bd0078b45f4d1f801c289d0c1fa1ff77df48845f80fbe45f883f8087e04c645f8088a45f88b55dc888294fe0000ba7f0000002b55dc8a45f8888214fe0000eb8fc95f5e5bc38bc05356575589e581ec0c000000`.\n\nInterpretation to re-prove on resume: `31E71` is the final Batch-75 `RET`; `31E72..31E73` is two-byte `MOV EAX,EAX` alignment with no incoming direct reference; `31E74` begins the next independent routine.\n\n## Interrupted/in-progress state\n\n**UNCOUNTED / NOT COMMITTED:** no Batch 76 bytes are authorized by this checkpoint. Any analysis at or beyond `31E74` must be revalidated against the overlap before counting.\n\n## Resume rule\n\nDo not rework Batch 75 merely because the overlap is re-disassembled. Verify overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `31E74`.\n'''))
if __name__=='__main__':
 import sys
 if len(sys.argv)!=2 or sys.argv[1] not in {'integrate','verify','checkpoint'}: raise SystemExit('usage: batch75_integrate.py integrate|verify|checkpoint')
 globals()[sys.argv[1]]()
