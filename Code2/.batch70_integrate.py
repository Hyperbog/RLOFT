from pathlib import Path
import os,re

ROOT=Path('.')
CPP=ROOT/'Code2/Code2.cpp'
PROG=ROOT/'Code2/PROGRESS.md'
LED=ROOT/'Code2/RESIDUAL_LEDGER.md'
CHK=ROOT/'Code2/RESUME_CHECKPOINT.md'
FRAG=ROOT/'Code2/.batch70.fragment.cpp'
WF=ROOT/'.github/workflows/integrate-code2-batch70.yml'
OLD_T=196867
OLD_N=3993
ADD_T=4070
ADD_N=66
NEW_T=200937
NEW_N=4059
DEN=239698
RECON=204996
UNRES=34702
PCT='85.5226'
RANGES=[
('2F5F1','2F5F3','alignment',3),('2F839','2F83B','alignment',3),('2F85F','2F85F','alignment',1),
('2F96A','2F96B','alignment',2),('2F9AE','2F9AF','alignment',2),('2F9E1','2F9E3','alignment',3),
('2FA2F','2FA2F','alignment',1),('2FB01','2FB03','alignment',3),('2FB66','2FB67','alignment',2),
('2FC46','2FC47','alignment',2),('2FD3B','2FD3B','alignment',1),('2FD8A','2FD8B','alignment',2),
('2FD8C','2FD9B','table',16),('3004D','3004F','alignment',3),('30249','3024B','alignment',3),
('30286','30287','alignment',2),('302C9','302CB','alignment',3),('30336','30337','alignment',2),
('30352','30353','alignment',2),('303C7','303C7','alignment',1),('30457','30457','alignment',1),
('30507','30507','alignment',1),('3058B','3058B','alignment',1),('30592','30593','alignment',2),
('3059A','3059B','alignment',2),('305A6','305A7','alignment',2),
]

def one(text,old,new):
    assert text.count(old)==1,(old,text.count(old)); return text.replace(old,new,1)

def normalize(text): return text.rstrip()+"\n"

def integrate():
    cpp=CPP.read_text(); frag=FRAG.read_text().rstrip()+"\n"
    assert 'Batch 70: 2F580..305A5' in frag
    assert '==4070u);' in frag
    assert 'function_2f580' not in cpp
    cpp=one(cpp,'constexpr u32 kVerifiedTranslatedBytes = 196867u;','constexpr u32 kVerifiedTranslatedBytes = 200937u;')
    cpp=one(cpp,'constexpr u32 kVerifiedNonCodeBytes = 3993u;','constexpr u32 kVerifiedNonCodeBytes = 4059u;')
    cpp=one(cpp,'+ 2161u + 2173u + 1209u);','+ 2161u + 2173u + 1209u + 4070u);')
    marker='\n} // namespace code2\n'
    assert cpp.count(marker)==1
    cpp=cpp.replace(marker,'\n'+frag+marker,1)
    CPP.write_text(normalize(cpp))

    led=LED.read_text()
    begin='<!-- RESIDUAL-RANGES-BEGIN -->'; end='<!-- RESIDUAL-RANGES-END -->'
    assert begin in led and end in led
    for a,b,k,n in RANGES:
        row=f'{a}..{b} | {k} | {n}'
        assert row not in led
    rows=''.join(f'{a}..{b} | {k} | {n}\n' for a,b,k,n in RANGES)
    led=led.replace(end,rows+end,1)
    led=re.sub(r'\*\*Total proven non-code/data:\*\* \*\*[0-9,]+ bytes\*\*\.',
               '**Total proven non-code/data:** **4,059 bytes**.',led,count=1)
    LED.write_text(normalize(led))

    prog=PROG.read_text()
    prog=re.sub(r'object%20accounting-[0-9.]+%25', 'object%20accounting-85.52%25', prog, count=1)
    prog=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*', '**Verified translated instruction bytes:** **200,937**', prog, count=1)
    prog=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*', '**Proven non-code/data bytes:** **4,059**', prog, count=1)
    prog=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*', '**Reconciled object bytes:** **204,996 / 239,698 = 85.5226% (85.52%)**', prog, count=1)
    prog=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*', '**Unresolved object bytes:** **34,702**', prog, count=1)
    appendix='''\n\n## Batch 70 — expanded presentation, buffered resource decode, image materialization, and text helpers\n\nAdded **4,070 translated instruction bytes** and **66 proven non-code/data bytes**, bringing cumulative accounting to **200,937 translated + 4,059 non-code = 204,996 / 239,698 = 85.5226% (85.52%)**.\n\n- `2F580..2F5F0` — presentation/error wrapper around the interactive prompt path and backend restore.\n- `2F5F4..2F838` — centered prompt/input presenter, including the encoded incoming-ESI return on delay-only mode, page restoration, and mode-5 auxiliary state.\n- `2F83C..2F95F` — presentation flags plus deferred prompt construction; `2F860` deliberately falls through into the independently callable `2F868`.\n- `2F960..2FD89` — 1 KiB buffered stream reader, byte/RLE reader, callback-driven row assembly, and raw/RLE planar loaders.\n- `2FD8C..2FD9B` — exact four-entry CS jump table owned by `2FD9C`: `2FEBC,2FF32,2FFA4,2FFAB`.\n- `2FD9C..30248` — image loader/materializer and capture/descriptor wrappers, preserving signed `IDIV`, low-word allocation truncation in `30070`, and `30050`'s upper-EAX preservation across AX-only multiply.\n- `3024C..305A5` — text mutation, metrics, character/string rendering, cursor state, centering, and optional masked draw path. `303C8` and `30458` retain the binary's shared-return behavior semantically without double-counting the earlier tail bytes.\n\nProven separators/table bytes total **66**; a full object-1 disassembly reference scan found no incoming reference into any separator. The only reference into the non-code ranges is the expected indirect `CS:[2FD8C + index*4]` jump-table use by `2FD9C`.\n\nSequential frontier: **`305A8`**. Semantic audit remains not started.\n'''
    prog=prog.rstrip()+appendix
    PROG.write_text(normalize(prog))

def ledger_sum(text):
    part=text.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split('<!-- RESIDUAL-RANGES-END -->',1)[0]
    total=0
    for ln in part.splitlines():
        m=re.match(r'^([0-9A-F]+)\.\.([0-9A-F]+) \| [^|]+ \| (\d+)$',ln.strip())
        if m:
            a,b,n=int(m.group(1),16),int(m.group(2),16),int(m.group(3))
            assert b-a+1==n,(ln,b-a+1,n); total+=n
    return total

def verify():
    cpp=CPP.read_text(); led=LED.read_text(); prog=PROG.read_text()
    assert 'constexpr u32 kVerifiedTranslatedBytes = 200937u;' in cpp
    assert 'constexpr u32 kVerifiedNonCodeBytes = 4059u;' in cpp
    assert 'function_2f580' in cpp and 'function_3059c' in cpp and 'kApp70DecodeJumpTable' in cpp
    assert '+ 2161u + 2173u + 1209u + 4070u);' in cpp
    assert ledger_sum(led)==4059,ledger_sum(led)
    assert '**Total proven non-code/data:** **4,059 bytes**.' in led
    assert '204,996 / 239,698 = 85.5226% (85.52%)' in prog
    assert 'Sequential frontier: **`305A8`**.' in prog

def checkpoint():
    sha=os.environ['BATCH_SHA']
    text=f'''# CODE.2 resume checkpoint\n\nThis file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.\n\n## Last committed substantive decompilation state\n\n- Last substantive batch commit: `{sha}` — `Integrate CODE.2 decompilation batch 70`.\n- Verified translated instruction bytes: **200,937**.\n- Proven non-code/data bytes: **4,059**.\n- Reconciled object bytes: **204,996 / 239,698 = 85.5226% (85.52%)**.\n- Unresolved object bytes: **34,702**.\n- Last integrated ownership fold: `2F580..305A7` (instructions through `305A5`, proven separators/table through `305A7`).\n- Next independently decoded instruction boundary: **`305A8`**.\n\n## Immutable identity\n\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n- Object-1 file start: `0x3AF24`.\n- Object-1 length: `0x3A852`.\n- Address convention: all reconstruction addresses are object-relative offsets.\n\n## Mandatory overlap anchor for next resume\n\nRe-disassemble at minimum `304F4..305D3` from freshly regenerated object 1 before doing new work.\n\n- Window: object-relative `[0x304F4, 0x305D4)` — 224 bytes.\n- SHA-256: `fe54ab4feef70a58dfb46463b018e3fa8e0beb02788c8fd6e71d4cad3fce7bd6`.\n- Narrow boundary window: `[0x30558, 0x305B8)` — 96 bytes.\n- Narrow SHA-256: `0e41c3d36817c5b55f329b207144b080b8d7617535d2b7cea4646c44859fe94a`.\n- Narrow boundary bytes:\n  `0fbe030fb64401044301c2ebee8b44241029d08b54240cd1f8ff74241801c2ff742418891580b60200e842feffff83c4085bc390a184b60200c38bc0a180b60200c38bc08b442404a36cf50000c38bc053837c241000745a6a00ff7424188b5c`\n\nInterpretation to re-prove on resume: `305A5` is the final Batch-70 `RET`; `305A6..305A7` is `MOV EAX,EAX` alignment; `305A8` begins the next independent `PUSH EBX` text-layout routine.\n\n## Interrupted/in-progress state\n\n**UNCOUNTED / NOT COMMITTED:** no Batch 71 bytes are authorized by this checkpoint. Any analysis at or beyond `305A8` must be revalidated against the overlap before counting.\n\n## Resume rule\n\nDo not rework Batch 70 merely because the overlap is re-disassembled. Verify overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `305A8`.\n'''
    CHK.write_text(normalize(text))

if __name__=='__main__':
    import sys
    {'integrate':integrate,'verify':verify,'checkpoint':checkpoint}[sys.argv[1]]()
