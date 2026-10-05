from pathlib import Path
import os,re
ROOT=Path('Code2')
CPP=ROOT/'Code2.cpp'; LED=ROOT/'RESIDUAL_LEDGER.md'; PROG=ROOT/'PROGRESS.md'; ADD=ROOT/'CODE2_CONTINUITY_ADDENDUM.md'; CK=ROOT/'RESUME_CHECKPOINT.md'; FRAG=ROOT/'.batch74.fragment.cpp'
OLD_T=205657; OLD_N=4215; ADD_T=929; ADD_N=3; NEW_T=206586; NEW_N=4218; TOTAL=239698; RECON=210804; UNRES=28894
ledger_lines='''31A12..31A13 | alignment | 2
31C57..31C57 | alignment | 1
'''
progress='''\n## Batch 74 — packed shading lookup generators\n\nAdded **929 translated instruction bytes** and **3 directly proven alignment bytes**, bringing cumulative accounting to **206,586 translated + 4,218 non-code = 210,804 / 239,698 = 87.9457% (87.95%)**. Unresolved is **28,894 bytes**.\n\nInstruction ownership is `318B4..31A11` and `31A14..31C56`; `31A12..31A13` is the compiler `MOV EAX,EAX` alignment and `31C57` is a NOP. A full object-1 direct CALL/JMP/Jcc target scan found no incoming target into either separator.\n\nFidelity notes: both generators retain signed `IDIV` behavior and arithmetic-right-shift rounding, write only the low byte of intermediate quotients into signed-byte temporaries, apply the parity decrement before the low-intensity and 8/9 collapse rules, and pack the two signed bytes into the published word table. `318B4` mirrors the capped low byte into `FE14/FE94`; `31A14` preserves byte-width subtraction of the fade adjustment, its `F954` per-argument scale divisor, and the two bounded lookup-band writes. The next independent routine begins at **`31C58`**.\n'''
addendum='''\n## CODE.2 Batch 74 checkpoint\n\nBatch 74 added **929 translated instruction bytes** plus **3 directly proven alignment bytes**, reconciling through `31C57`. Cumulative accounting is **206,586 translated + 4,218 non-code = 210,804 / 239,698 = 87.9457% (87.95%)**; unresolved is **28,894 bytes**.\n\nThe fold covers the paired packed shading/lookup generators `318B4..31A11` and `31A14..31C56`. Preserve signed division, arithmetic-shift half rounding, AL-sized quotient capture, parity-driven high-byte decrement, byte-width fade subtraction, `F954` scaling, and the exact lookup-band ranges. The next independent routine begins at `31C58`.\n'''

def norm(s): return s.rstrip()+"\n"

def integrate():
    cpp=CPP.read_text(); frag=FRAG.read_text().rstrip()+"\n"
    assert 'constexpr u32 kVerifiedTranslatedBytes = 205657u;' in cpp
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4215u;' in cpp
    assert '// Batch 74:' not in cpp
    assert 'static_assert(0x31A12u-0x318B4u==350u);' in frag
    assert 'static_assert(0x31C57u-0x31A14u==579u);' in frag
    cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 205657u;','constexpr u32 kVerifiedTranslatedBytes = 206586u;',1)
    cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 4215u;','constexpr u32 kVerifiedNonCodeBytes = 4218u;',1)
    old='+ 1843u + 1852u + 1025u);'; assert old in cpp
    cpp=cpp.replace(old,'+ 1843u + 1852u + 1025u + 929u);',1)
    marker='\n} // namespace code2\n'; assert cpp.endswith(marker)
    cpp=cpp[:-len(marker)]+'\n\n'+frag+marker
    CPP.write_text(norm(cpp))

    led=LED.read_text(); mark='<!-- RESIDUAL-RANGES-END -->'; assert mark in led
    for ln in ledger_lines.strip().splitlines(): assert ln not in led
    led=led.replace(mark,ledger_lines+mark,1)
    led=led.replace('**Total proven non-code/data:** **4,215 bytes**.','**Total proven non-code/data:** **4,218 bytes**.',1)
    LED.write_text(norm(led))

    prog=PROG.read_text(); assert '## Batch 74 —' not in prog
    prog=re.sub(r'object%20accounting-[0-9.]+%25','object%20accounting-87.95%25',prog,count=1)
    prog=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*','**Verified translated instruction bytes:** **206,586**',prog,count=1)
    prog=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*','**Proven non-code/data bytes:** **4,218**',prog,count=1)
    prog=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*','**Reconciled object bytes:** **210,804 / 239,698 = 87.9457% (87.95%)**',prog,count=1)
    prog=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*','**Unresolved object bytes:** **28,894**',prog,count=1)
    PROG.write_text(norm(prog.rstrip()+progress))

    add=ADD.read_text(); assert '## CODE.2 Batch 74 checkpoint' not in add
    ADD.write_text(norm(add.rstrip()+addendum))

def ledger_sum(text):
    part=text.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split('<!-- RESIDUAL-RANGES-END -->',1)[0]
    total=0
    for ln in part.splitlines():
        m=re.match(r'^([0-9A-F]+)\.\.([0-9A-F]+) \| [^|]+ \| (\d+)$',ln.strip())
        if m:
            a,z,n=int(m.group(1),16),int(m.group(2),16),int(m.group(3)); assert z-a+1==n,(ln,z-a+1,n); total+=n
    return total

def verify():
    cpp=CPP.read_text(); led=LED.read_text(); prog=PROG.read_text(); add=ADD.read_text()
    assert 'constexpr u32 kVerifiedTranslatedBytes = 206586u;' in cpp
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4218u;' in cpp
    assert cpp.count('// Batch 74:')==1
    assert '+ 1852u + 1025u + 929u);' in cpp
    for a in ['function_318b4','function_31a14','app74_sar1','app74_pack','0xFE4Eu+static_cast<u32>(i)','0xFE14u+static_cast<u32>(0xB1-i)']:
        assert a in cpp,a
    assert ledger_sum(led)==4218,ledger_sum(led)
    for ln in ledger_lines.strip().splitlines(): assert ln in led,ln
    assert '**Total proven non-code/data:** **4,218 bytes**.' in led
    assert '210,804 / 239,698 = 87.9457% (87.95%)' in prog
    assert 'The next independent routine begins at **`31C58`**.' in prog
    assert 'CODE.2 Batch 74 checkpoint' in add

def checkpoint():
    sha=os.environ['BATCH_SHA']
    text=f'''# CODE.2 resume checkpoint\n\nThis file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.\n\n## Last committed substantive decompilation state\n\n- Last substantive batch commit: `{sha}` — `Integrate CODE.2 decompilation batch 74`.\n- Verified translated instruction bytes: **206,586**.\n- Proven non-code/data bytes: **4,218**.\n- Reconciled object bytes: **210,804 / 239,698 = 87.9457% (87.95%)**.\n- Unresolved object bytes: **28,894**.\n- Last integrated ownership fold: `318B4..31C57` (instructions through `31C56`, proven alignment at `31A12..31A13` and `31C57`).\n- Next independently decoded instruction boundary: **`31C58`**.\n\n## Immutable identity\n\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n- Object-1 file start: `0x3AF24`.\n- Object-1 length: `0x3A852`.\n- Address convention: all reconstruction addresses are object-relative offsets.\n\n## Mandatory overlap anchor for next resume\n\nRe-disassemble at minimum `31BD0..31CAF` from freshly regenerated object 1 before doing new work.\n\n- Window: object-relative `[0x31BD0, 0x31CB0)` — 224 bytes.\n- SHA-256: `d7beccad5fc4d39703a3ee9f346390e098c4186b1e6563c1e7920e67324e785b`.\n- Narrow boundary window: `[0x31C20, 0x31C80)` — 96 bytes.\n- Narrow SHA-256: `df34a6fcf0ee3fcda666c67e16af452f5fdfd6351734977da7db4bc75734dea2`.\n- Narrow boundary bytes:\n  `09817dd4b20000007c02eb188b55d483ea32b87f00000029d089c28a45f8888214fe0000e951feffff8b45dc66a3fafc0000c95f5e5bc3905356575589e581ec2c000000c745d403000000c745d8040000008b45d4408945e8a130c700004089`.\n\nInterpretation to re-prove on resume: `31C56` is the final Batch-74 `RET`; `31C57` is one NOP byte with no incoming direct reference; `31C58` begins the next independent routine.\n\n## Interrupted/in-progress state\n\n**UNCOUNTED / NOT COMMITTED:** no Batch 75 bytes are authorized by this checkpoint. Any analysis at or beyond `31C58` must be revalidated against the overlap before counting.\n\n## Resume rule\n\nDo not rework Batch 74 merely because the overlap is re-disassembled. Verify overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `31C58`.\n'''
    CK.write_text(norm(text))

if __name__=='__main__':
    import sys
    if len(sys.argv)!=2 or sys.argv[1] not in {'integrate','verify','checkpoint'}: raise SystemExit('usage: batch74_integrate.py integrate|verify|checkpoint')
    globals()[sys.argv[1]]()
