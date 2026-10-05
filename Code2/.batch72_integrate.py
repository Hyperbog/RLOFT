from pathlib import Path
import os,re
ROOT=Path('Code2')
CPP=ROOT/'Code2.cpp'; LED=ROOT/'RESIDUAL_LEDGER.md'; PROG=ROOT/'PROGRESS.md'; ADD=ROOT/'CODE2_CONTINUITY_ADDENDUM.md'; CK=ROOT/'RESUME_CHECKPOINT.md'; FRAG=ROOT/'.batch72.fragment.cpp'
OLD_T=202780; OLD_N=4144; ADD_T=1852; ADD_N=68; NEW_T=204632; NEW_N=4212; TOTAL=239698; RECON=208844; UNRES=30854
ledger_lines='''30DE7..30DE7 | alignment | 1
30E2F..30E2F | alignment | 1
30E63..30E63 | alignment | 1
30F89..30F8B | alignment | 3
30FC9..30FCB | alignment | 3
31013..31013 | alignment | 1
31043..31043 | alignment | 1
31049..3104B | alignment | 3
311D5..311DF | padding | 11
31233..31233 | alignment | 1
31272..31273 | alignment | 2
3127A..3127B | alignment | 2
3129F..3129F | alignment | 1
312B2..312B3 | alignment | 2
312C6..312C7 | alignment | 2
312DA..312DB | alignment | 2
312E9..312EB | alignment | 3
31301..31303 | alignment | 3
31319..3131B | alignment | 3
31331..31333 | alignment | 3
3137D..3137F | alignment | 3
3138D..3138F | alignment | 3
313A9..313AB | alignment | 3
313C1..313C3 | alignment | 3
313EB..313EB | alignment | 1
31483..31483 | alignment | 1
314AB..314AF | padding | 5
'''
progress='''\n## Batch 72 — DPMI/runtime memory wrappers, allocator recovery, buffered output, and handle helpers\n\nAdded **1,852 translated instruction bytes** and **68 directly proven non-code/data bytes**, bringing cumulative accounting to **204,632 translated + 4,212 non-code = 208,844 / 239,698 = 87.1280% (87.13%)**. Unresolved is **30,854 bytes**.\n\nInstruction ownership spans `30D30..30DE6`, `30DE8..30E2E`, `30E30..30E62`, `30E64..30F88`, `30F8C..30FC8`, `30FCC..31012`, `31014..31048`, `3104C..311D4`, `311E0..31232`, `31234..31271`, `31274..31279`, `3127C..3129E`, `312A0..312B1`, `312B4..312C5`, `312C8..312D9`, `312DC..312E8`, `312EC..31300`, `31304..31318`, `3131C..31330`, `31334..3137C`, `31380..3138C`, `31390..313A8`, `313AC..313C0`, `313C4..313EA`, `313EC..31482`, and `31484..314AA`. All 68 intervening bytes are direct NOP/MOV-EAX-EAX/LEA alignment or zero padding; a full object-1 direct CALL/JMP/Jcc scan found no incoming target into any of them.\n\nFidelity notes: `30D80` and `30E64` retain signed truncation-toward-zero paragraph arithmetic after 32-bit wrap; `31274` preserves the incoming upper 24 EAX bits while replacing AL; `312EC`, `31304`, and `3131C` retain the binary's stack-induced `(third, first, second)` forwarding order; `3104C` keeps its one-byte recovery stage and repeats stage 2 indefinitely after it is reached; `31107` keeps byte-sized stream flag updates while testing the full dword at +0C; and `313FC` restores the saved drive only when pathname open fails. The next independent application routine begins at **`314B0`**.\n'''
addendum='''\n## CODE.2 Batch 72 checkpoint\n\nBatch 72 added **1,852 translated instruction bytes** plus **68 directly proven non-code/data bytes**, reconciling through `314AF`. Cumulative accounting is **204,632 translated + 4,212 non-code = 208,844 / 239,698 = 87.1280% (87.13%)**; unresolved is **30,854 bytes**.\n\nThe fold covers the `30D30` fill primitive, DPMI/memory wrappers through `31012`, allocator recovery and checked free logic, buffered byte output, DOS/string and handle wrappers, and drive-aware pathname opening through `314AA`. Preserve signed paragraph division, partial-AL return at `31274`, the argument permutation in the three I/O forwarding thunks, the staged allocator-recovery loop, byte-vs-dword stream flags, and failure-only drive restoration. The next independent application routine begins at `314B0`.\n'''

def norm(s): return s.rstrip()+"\n"

def integrate():
    cpp=CPP.read_text(); frag=FRAG.read_text().rstrip()+"\n"
    assert 'constexpr u32 kVerifiedTranslatedBytes = 202780u;' in cpp
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4144u;' in cpp
    assert '// Batch 72:' not in cpp and '==1852u);' in frag
    cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 202780u;','constexpr u32 kVerifiedTranslatedBytes = 204632u;',1)
    cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 4144u;','constexpr u32 kVerifiedNonCodeBytes = 4212u;',1)
    old='+ 2161u + 2173u + 1209u + 4070u + 1843u);'
    assert old in cpp
    cpp=cpp.replace(old,'+ 2161u + 2173u + 1209u + 4070u + 1843u + 1852u);',1)
    marker='\n} // namespace code2\n'; assert cpp.endswith(marker)
    cpp=cpp[:-len(marker)]+'\n\n'+frag+marker
    CPP.write_text(norm(cpp))

    led=LED.read_text(); mark='<!-- RESIDUAL-RANGES-END -->'; assert mark in led
    for ln in ledger_lines.strip().splitlines(): assert ln not in led
    led=led.replace(mark,ledger_lines+mark,1)
    led=led.replace('**Total proven non-code/data:** **4,144 bytes**.','**Total proven non-code/data:** **4,212 bytes**.',1)
    LED.write_text(norm(led))

    prog=PROG.read_text(); assert '## Batch 72 —' not in prog
    prog=re.sub(r'object%20accounting-[0-9.]+%25','object%20accounting-87.13%25',prog,count=1)
    prog=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*','**Verified translated instruction bytes:** **204,632**',prog,count=1)
    prog=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*','**Proven non-code/data bytes:** **4,212**',prog,count=1)
    prog=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*','**Reconciled object bytes:** **208,844 / 239,698 = 87.1280% (87.13%)**',prog,count=1)
    prog=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*','**Unresolved object bytes:** **30,854**',prog,count=1)
    PROG.write_text(norm(prog.rstrip()+progress))

    add=ADD.read_text(); assert '## CODE.2 Batch 72 checkpoint' not in add
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
    assert 'constexpr u32 kVerifiedTranslatedBytes = 204632u;' in cpp
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4212u;' in cpp
    assert cpp.count('// Batch 72:')==1
    assert '+ 4070u + 1843u + 1852u);' in cpp
    assert '==1852u);' in cpp
    for a in ['app72_div16_trunc','function_3104c','function_31107','m.cpu.eax=(m.cpu.eax&0xFFFFFF00u)','call_38106?b.call_38106(d,a,c)','function_313fc']:
        assert a in cpp,a
    assert ledger_sum(led)==4212,ledger_sum(led)
    for ln in ledger_lines.strip().splitlines(): assert ln in led,ln
    assert '**Total proven non-code/data:** **4,212 bytes**.' in led
    assert '208,844 / 239,698 = 87.1280% (87.13%)' in prog
    assert 'The next independent application routine begins at **`314B0`**.' in prog
    assert 'CODE.2 Batch 72 checkpoint' in add

def checkpoint():
    sha=os.environ['BATCH_SHA']
    text=f'''# CODE.2 resume checkpoint\n\nThis file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.\n\n## Last committed substantive decompilation state\n\n- Last substantive batch commit: `{sha}` — `Integrate CODE.2 decompilation batch 72`.\n- Verified translated instruction bytes: **204,632**.\n- Proven non-code/data bytes: **4,212**.\n- Reconciled object bytes: **208,844 / 239,698 = 87.1280% (87.13%)**.\n- Unresolved object bytes: **30,854**.\n- Last integrated ownership fold: `30D30..314AF` (instructions through `314AA`, proven alignment/padding through `314AF`).\n- Next independently decoded instruction boundary: **`314B0`**.\n\n## Immutable identity\n\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n- Object-1 file start: `0x3AF24`.\n- Object-1 length: `0x3A852`.\n- Address convention: all reconstruction addresses are object-relative offsets.\n\n## Mandatory overlap anchor for next resume\n\nRe-disassemble at minimum `31430..3150F` from freshly regenerated object 1 before doing new work.\n\n- Window: object-relative `[0x31430, 0x31510)` — 224 bytes.\n- SHA-256: `6f4b9f178a7cf0c68848b214c1180280a1fa9b0d4dee32020529522bf87ae5e7`.\n- Narrow boundary window: `[0x31480, 0x314E0)` — 96 bytes.\n- Narrow SHA-256: `923273f978aafc806aa9a183c2a1ccea069c79d9b6beb434bb3561db55b8ba96`.\n- Narrow boundary bytes:\n  `5e5bc390ff742404e8ab25000083c404c38b54240485d27508e8d070000031c0c352e8b968000083c404c300000000005356575589e581ec200000008b451401c00fbf8050b702008945f48b55148d149500000000a164b7020001d08b008945`\n\nInterpretation to re-prove on resume: `314AA` is the final Batch-72 `RET`; `314AB..314AF` is five zero padding bytes with no incoming direct reference; `314B0` begins the next independent application routine.\n\n## Interrupted/in-progress state\n\n**UNCOUNTED / NOT COMMITTED:** no Batch 73 bytes are authorized by this checkpoint. Any analysis at or beyond `314B0` must be revalidated against the overlap before counting.\n\n## Resume rule\n\nDo not rework Batch 72 merely because the overlap is re-disassembled. Verify overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `314B0`.\n'''
    CK.write_text(norm(text))

if __name__=='__main__':
    import sys
    if len(sys.argv)!=2 or sys.argv[1] not in {'integrate','verify','checkpoint'}: raise SystemExit('usage: batch72_integrate.py integrate|verify|checkpoint')
    globals()[sys.argv[1]]()
