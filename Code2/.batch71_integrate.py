from pathlib import Path
import os, re

ROOT=Path('Code2')
CPP=ROOT/'Code2.cpp'; LED=ROOT/'RESIDUAL_LEDGER.md'; PROG=ROOT/'PROGRESS.md'; ADD=ROOT/'CODE2_CONTINUITY_ADDENDUM.md'; CK=ROOT/'RESUME_CHECKPOINT.md'; FRAG=ROOT/'.batch71.fragment.cpp'
OLD_T=200937; OLD_N=4059; ADD_T=1843; ADD_N=85; NEW_T=202780; NEW_N=4144; TOTAL=239698; RECON=206924; UNRES=32774
ledger_lines='''306F8..306FF | padding | 8
30706..30707 | alignment | 2
30736..30737 | alignment | 2
30745..30747 | alignment | 3
307C8..307EF | table | 40
308B9..308BB | alignment | 3
308F5..308F7 | alignment | 3
30ABA..30ABB | alignment | 2
30AD7..30AD7 | alignment | 1
30AEB..30AEB | alignment | 1
30B19..30B1B | alignment | 3
30B2B..30B2B | alignment | 1
30B3E..30B3F | alignment | 2
30C37..30C37 | alignment | 1
30C91..30C93 | alignment | 3
30D26..30D2F | padding | 10
'''
progress='''\n## Batch 71 — text layout, keyboard/IRQ state, vector lifecycle, RNG, and memmove\n\nAdded **1,843 translated instruction bytes** and **85 directly proven non-code/data bytes**, bringing cumulative accounting to **202,780 translated + 4,144 non-code = 206,924 / 239,698 = 86.3270% (86.33%)**. Unresolved is **32,774 bytes**.\n\nInstruction ownership spans `305A8..306F7`, `30700..30705`, `30708..30735`, `30738..30744`, `30748..307C7`, `307F0..308B8`, `308BC..308F4`, `308F8..30AB9`, `30ABC..30AD6`, `30AD8..30AEA`, `30AEC..30B18`, `30B1C..30B2A`, `30B2C..30B3D`, `30B40..30C36`, `30C38..30C90`, and `30C94..30D25`. The exact ten-entry CS direction table is `307C8..307EF`; all other newly proven non-code bytes are direct compiler alignment or zero padding with no incoming direct control-flow reference.\n\nFidelity notes: `305A8` intentionally indexes glyph widths with signed characters while `30630` uses unsigned character indices; `308F8` keeps the encoded signed `JLE` split at scan code `80h` and the CODE.2-specific `19h` path that stores word `1` at `1457C`; `307F0` preserves the exact ten-way direction dispatch; vector install/uninstall retains saved offset/selector pairs; and `30CD4` preserves the backward-copy overlap condition and forward-copy behavior. The next independent routine begins at **`30D30`**.\n'''
addendum='''\n## CODE.2 Batch 71 checkpoint\n\nBatch 71 added **1,843 translated instruction bytes** plus **85 directly proven non-code/data bytes**, reconciling through `30D2F`. Cumulative accounting is **202,780 translated + 4,144 non-code = 206,924 / 239,698 = 86.3270% (86.33%)**; unresolved is **32,774 bytes**.\n\nThe fold covers text layout through `306F7`, the tick/keyboard interrupt and queue/vector family through `30C90`, RNG state through `30CD3`, and the overlap-safe move at `30CD4..30D25`. Preserve the signed-vs-unsigned glyph-index distinction, the keyboard handler's signed `80h` split and CODE.2-only `19h -> word [1457C]=1` branch, the exact `307C8` direction table, selector-bearing saved vectors, and overlap-copy direction. The next independent routine begins at `30D30`.\n'''

def integrate():
    cpp=CPP.read_text()
    assert 'constexpr u32 kVerifiedTranslatedBytes = 200937u;' in cpp
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4059u;' in cpp
    assert '// Batch 71:' not in cpp
    frag=FRAG.read_text()
    cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 200937u;','constexpr u32 kVerifiedTranslatedBytes = 202780u;',1)
    cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 4059u;','constexpr u32 kVerifiedNonCodeBytes = 4144u;',1)
    marker='\n} // namespace code2\n'
    assert cpp.endswith(marker)
    cpp=cpp[:-len(marker)]+'\n\n'+frag.rstrip()+marker
    CPP.write_text(cpp)

    led=LED.read_text(); mark='<!-- RESIDUAL-RANGES-END -->'
    assert '306F8..306FF | padding | 8' not in led and mark in led
    led=led.replace(mark,ledger_lines+mark,1)
    led=led.replace('**Total proven non-code/data:** **4,059 bytes**.','**Total proven non-code/data:** **4,144 bytes**.',1)
    LED.write_text(led)

    prog=PROG.read_text(); assert '## Batch 71 —' not in prog; PROG.write_text(prog.rstrip()+progress+'\n')
    add=ADD.read_text(); assert '## CODE.2 Batch 71 checkpoint' not in add; ADD.write_text(add.rstrip()+addendum+'\n')

def verify():
    cpp=CPP.read_text(); led=LED.read_text(); prog=PROG.read_text(); add=ADD.read_text()
    assert 'constexpr u32 kVerifiedTranslatedBytes = 202780u;' in cpp
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4144u;' in cpp
    assert cpp.count('// Batch 71:')==1
    assert 'kApp71DirectionJumpTable[10]' in cpp
    assert 'else if(code==0x19u){store_le16(dg(m,0x1457Cu),1u);' in cpp
    assert 'static_cast<s32>(scan)>0x80' in cpp
    assert '==1843u);' in cpp
    for line in ledger_lines.strip().splitlines(): assert line in led,line
    assert '**Total proven non-code/data:** **4,144 bytes**.' in led
    assert '206,924 / 239,698 = 86.3270% (86.33%)' in prog
    assert 'The next independent routine begins at **`30D30`**.' in prog
    assert 'CODE.2 Batch 71 checkpoint' in add

def checkpoint():
    sha=os.environ['BATCH_SHA']
    text=f'''# CODE.2 resume checkpoint\n\nThis file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.\n\n## Last committed substantive decompilation state\n\n- Last substantive batch commit: `{sha}` — `Integrate CODE.2 decompilation batch 71`.\n- Verified translated instruction bytes: **202,780**.\n- Proven non-code/data bytes: **4,144**.\n- Reconciled object bytes: **206,924 / 239,698 = 86.3270% (86.33%)**.\n- Unresolved object bytes: **32,774**.\n- Last integrated ownership fold: `305A8..30D2F` (instructions through `30D25`, proven table/alignment/padding through `30D2F`).\n- Next independently decoded instruction boundary: **`30D30`**.\n\n## Immutable identity\n\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n- Object-1 file start: `0x3AF24`.\n- Object-1 length: `0x3A852`.\n- Address convention: all reconstruction addresses are object-relative offsets.\n\n## Mandatory overlap anchor for next resume\n\nRe-disassemble at minimum `30CB0..30D8F` from freshly regenerated object 1 before doing new work.\n\n- Window: object-relative `[0x30CB0, 0x30D90)` — 224 bytes.\n- SHA-256: `e3a6c3c5a8290d8c5d8a420755d1304238bf125800b30fbad888bfcf7de40b93`.\n- Narrow boundary window: `[0x30D00, 0x30D60)` — 96 bytes.\n- Narrow SHA-256: `8882f49487c40880cb0d48469a1bcb6f0808660b8d1ae3de0050b18e9ac1b645`.\n- Narrow boundary bytes:\n  `66f3a511c9464766f3a407fc5f5ec38cda89c7068ec251c1e902f3a55983e103f3a4075f5ec3000000000000000000008b4424048b5424088b4c240c88d6c1e20888f2c1e20888f2e8836c00008b442404c35383ec0c89e050e8156d000083c4`\n\nInterpretation to re-prove on resume: `30D25` is the final Batch-71 `RET`; `30D26..30D2F` is ten zero padding bytes with no incoming direct reference; `30D30` begins the next independent memory-fill/runtime-wrapper routine.\n\n## Interrupted/in-progress state\n\n**UNCOUNTED / NOT COMMITTED:** no Batch 72 bytes are authorized by this checkpoint. Any analysis at or beyond `30D30` must be revalidated against the overlap before counting.\n\n## Resume rule\n\nDo not rework Batch 71 merely because the overlap is re-disassembled. Verify overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `30D30`.\n'''
    CK.write_text(text)

if __name__=='__main__':
    import sys
    if len(sys.argv)!=2 or sys.argv[1] not in {'integrate','verify','checkpoint'}: raise SystemExit('usage: batch71_integrate.py integrate|verify|checkpoint')
    globals()[sys.argv[1]]()
