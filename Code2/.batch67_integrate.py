from pathlib import Path
import os, re

ROOT=Path('.')
CPP=ROOT/'Code2/Code2.cpp'
LED=ROOT/'Code2/RESIDUAL_LEDGER.md'
PROG=ROOT/'Code2/PROGRESS.md'
ADD=ROOT/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
PAYLOAD=ROOT/'Code2/.batch67.fragment.cpp'
CHK=ROOT/'Code2/RESUME_CHECKPOINT.md'

OLD_T=191324
NEW_T=193485
OLD_N=3820
NEW_N=3903
NEW_R=197388
DEN=239698
NEW_U=42310

ROWS='''2DFE9..2DFEB | alignment | 3
2DFEC..2E00B | table | 32
2E0E3..2E0E3 | alignment | 1
2E11D..2E11F | alignment | 3
2E339..2E33F | padding | 7
2E349..2E34B | alignment | 3
2E35F..2E35F | alignment | 1
2E373..2E373 | alignment | 1
2E435..2E437 | alignment | 3
2E4A1..2E4A3 | alignment | 3
2E50D..2E50F | alignment | 3
2E536..2E537 | alignment | 2
2E561..2E563 | alignment | 3
2E58D..2E58F | alignment | 3
2E5D3..2E5D3 | alignment | 1
2E613..2E613 | alignment | 1
2E6EE..2E6EF | alignment | 2
2E709..2E70B | alignment | 3
2E725..2E727 | alignment | 3
2E74A..2E74B | alignment | 2
2E756..2E757 | alignment | 2
2E7EF..2E7EF | alignment | 1
'''

PROGRESS_BLOCK='''## CODE.2 Batch 67 checkpoint

Batch 67 added **2,161 translated instruction bytes** plus **83 directly proven non-code/data bytes**, reconciling through `2E7EF`. Cumulative accounting is **193,485 translated + 3,903 non-code = 197,388 / 239,698 = 82.3486% (82.35%)**; unresolved is **42,310 bytes**.

Instruction ownership spans `2DF2C..2DFE8`, `2E00C..2E0E2`, `2E0E4..2E11C`, `2E120..2E338`, `2E340..2E348`, `2E34C..2E35E`, `2E360..2E372`, `2E374..2E434`, `2E438..2E4A0`, `2E4A4..2E50C`, `2E510..2E535`, `2E538..2E560`, `2E564..2E58C`, `2E590..2E5D2`, `2E5D4..2E612`, `2E614..2E6ED`, `2E6F0..2E708`, `2E70C..2E724`, `2E728..2E749`, `2E74C..2E755`, `2E758..2E7EE`. Preserve `2DF2C`'s signed-high-word clamp condition and exact eight-entry CS table, `2E120`'s 16-bit NEG/MOVSX movement path, nested page-state semantics in `2E590/2E5D4`, and `2E614`'s encoded dead `0x1F0` branch after BL bit 2 is cleared. The next independent routine begins at `2E7F0`.
'''

ADD_BLOCK='''## CODE.2 Batch 67 checkpoint

Batch 67 adds **2,161 translated bytes** and **83 proven non-code/data bytes**, bringing cumulative accounting to **193,485 translated + 3,903 non-code = 197,388 / 239,698 = 82.3486% (82.35%)**; unresolved is **42,310 bytes**. The sequential frontier is now `2E7F0`. Preserve the exact `2DFEC` eight-entry CS table owned by `2DF2C`, the signed-high-word-only distance clamp, `2E120`'s word-sized negation before sign extension, page-stack state in `2E590/2E5D4`, and `2E614`'s intentionally dead post-mask return branch.
'''

EVIDENCE='''Batch 67 evidence: the mandatory Batch-66 overlap `2DEB0..2DF8F` and narrow window `2DF00..2DF5F` were regenerated from immutable CODE.2 and matched the checkpoint SHA-256 values exactly before forward accounting. `2DFEC..2E00B` is the exact eight-entry CS dword table consumed by the bounded indirect jump at `2E033`. The remaining classified bytes decode as compiler separators (`LEA EAX,[EAX+0]`, `MOV EAX,EAX`, `NOP`) except the seven zero bytes at `2E339..2E33F`. A full object-1 direct relative CALL/JMP/Jcc target scan found no incoming target to any classified separator/padding byte; direct absolute-address checks found no owned data reference except the proven `2DFEC` table reference from its dispatcher.
'''

def write1(path:Path,text:str):
    path.write_text(text.rstrip('\n')+'\n')

def append_section(text:str,block:str)->str:
    return text.rstrip('\n')+'\n\n'+block.strip('\n')+'\n'

def one_replace(text,old,new,label):
    n=text.count(old)
    assert n==1, f'{label}: expected one occurrence of {old!r}, got {n}'
    return text.replace(old,new,1)

def load_fragment():
    text=PAYLOAD.read_text()
    assert '// Batch 67: 2DF2C..2E7EE' in text
    assert '==2161u);' in text
    return text.rstrip()+'\n\n'

def update_cpp():
    cpp=CPP.read_text()
    assert 'struct Application67Boundary' not in cpp
    cpp=one_replace(cpp,f'constexpr u32 kVerifiedTranslatedBytes = {OLD_T}u;',f'constexpr u32 kVerifiedTranslatedBytes = {NEW_T}u;','translated constant')
    cpp=one_replace(cpp,f'constexpr u32 kVerifiedNonCodeBytes = {OLD_N}u;',f'constexpr u32 kVerifiedNonCodeBytes = {NEW_N}u;','noncode constant')
    pat=re.compile(r'static_assert\(kVerifiedTranslatedBytes ==(?P<body>.*?)\);',re.S)
    matches=list(pat.finditer(cpp))
    assert len(matches)==1, f'cumulative translated assertion count={len(matches)}'
    m=matches[0]
    body=m.group('body')
    assert '2161u' not in body
    repl='static_assert(kVerifiedTranslatedBytes =='+body.rstrip()+' + 2161u);'
    cpp=cpp[:m.start()]+repl+cpp[m.end():]
    marker='} // namespace code2'
    pos=cpp.rfind(marker)
    assert pos!=-1
    cpp=cpp[:pos]+load_fragment()+cpp[pos:]
    write1(CPP,cpp)

def update_ledger():
    led=LED.read_text()
    assert '2DFE9..2DFEB | alignment | 3' not in led
    end='<!-- RESIDUAL-RANGES-END -->'
    assert led.count(end)==1
    led=led.replace(end,ROWS+end,1)
    led,n=re.subn(r'\*\*Total proven non-code/data:\*\* \*\*[0-9,]+ bytes\*\*\.', '**Total proven non-code/data:** **3,903 bytes**.',led,count=1)
    assert n==1
    led=append_section(led,EVIDENCE)
    write1(LED,led)

def update_progress():
    p=PROG.read_text()
    assert '## CODE.2 Batch 67 checkpoint' not in p
    p,n=re.subn(r'object%20accounting-[0-9.]+%25-red','object%20accounting-82.35%25-red',p,count=1)
    assert n==1
    p=one_replace(p,'**Verified translated instruction bytes:** **191,324**','**Verified translated instruction bytes:** **193,485**','progress translated')
    p=one_replace(p,'**Proven non-code/data bytes:** **3,820**','**Proven non-code/data bytes:** **3,903**','progress noncode')
    p=one_replace(p,'**Reconciled object bytes:** **195,144 / 239,698 = 81.4124% (81.41%)**','**Reconciled object bytes:** **197,388 / 239,698 = 82.3486% (82.35%)**','progress reconciled')
    p=one_replace(p,'**Unresolved object bytes:** **44,554**','**Unresolved object bytes:** **42,310**','progress unresolved')
    write1(PROG,append_section(p,PROGRESS_BLOCK))

def update_addendum():
    a=ADD.read_text()
    assert '## CODE.2 Batch 67 checkpoint' not in a
    write1(ADD,append_section(a,ADD_BLOCK))

def verify():
    cpp=CPP.read_text(); led=LED.read_text(); p=PROG.read_text(); a=ADD.read_text()
    assert f'constexpr u32 kVerifiedTranslatedBytes = {NEW_T}u;' in cpp
    assert f'constexpr u32 kVerifiedNonCodeBytes = {NEW_N}u;' in cpp
    assert 'struct Application67Boundary' in cpp
    funcs=['function_2df2c','function_2e0e4','function_2e120','function_2e340','function_2e34c','function_2e360','function_2e374','function_2e404','function_2e438','function_2e488','function_2e4a4','function_2e4f4','function_2e510','function_2e538','function_2e564','function_2e590','function_2e5d4','function_2e608','function_2e614','function_2e6d0','function_2e6f0','function_2e70c','function_2e728','function_2e740','function_2e74c','function_2e758','function_2e768']
    assert all(x in cpp for x in funcs)
    assert '==2161u);' in cpp
    m=re.search(r'static_assert\(kVerifiedTranslatedBytes ==(.*?)\);',cpp,re.S)
    assert m and '2161u' in m.group(1)
    assert all(row in led for row in ROWS.strip().splitlines())
    assert '**Total proven non-code/data:** **3,903 bytes**.' in led
    rm=re.search(r'<!-- RESIDUAL-RANGES-BEGIN -->\n(.*?)<!-- RESIDUAL-RANGES-END -->',led,re.S)
    assert rm
    total=0
    for line in rm.group(1).splitlines():
        line=line.strip()
        if not line: continue
        fields=[x.strip() for x in line.split('|')]
        assert len(fields)==3,line
        total+=int(fields[2])
    assert total==NEW_N,total
    assert '197,388 / 239,698 = 82.3486% (82.35%)' in p
    assert '## CODE.2 Batch 67 checkpoint' in p and '## CODE.2 Batch 67 checkpoint' in a

def integrate():
    update_cpp(); update_ledger(); update_progress(); update_addendum(); verify()

def checkpoint():
    batch=os.environ['BATCH_SHA']
    text=f'''# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `{batch}` — `Integrate CODE.2 decompilation batch 67`.
- Verified translated instruction bytes: **193,485**.
- Proven non-code/data bytes: **3,903**.
- Reconciled object bytes: **197,388 / 239,698 = 82.3486% (82.35%)**.
- Unresolved object bytes: **42,310**.
- Last integrated ownership fold: `2DF2C..2E7EF` (instructions through `2E7EE`, proven table/separators through `2E7EF`).
- Next independently decoded instruction boundary: **`2E7F0`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2E770..2E84F` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2E770, 0x2E850)` — 224 bytes.
- SHA-256: `9d5cef77c3b7cd4983ef3c39500b7bd8dc6498a99156197bedb6c6d4c9764716`.
- Narrow boundary window: `[0x2E7C0, 0x2E820)` — 96 bytes.
- Narrow SHA-256: `e0b7a0a835e3dcd5df5595e990fe55768c3149b5d24263a4610dcbdd418c266d`.
- Narrow boundary bytes:
  `8d44240850e8222b000083c40c530fbf442404506861f00000e80e2b000083c40c53e8f52a000083c40483c4045bc3905356575583ec54e86cffffff8b44246cc1e00450e84328000083c404a3b8f000008b44246cc744245000000000a3b4f0`

Interpretation to re-prove on resume, not merely trust: `2E7EE` is the final Batch-67 `RET`; `2E7EF` is a `NOP` separator; `2E7F0` begins the next independent routine with `PUSH EBX/ESI/EDI/EBP`, a `54h` stack frame, calls `2E768`, allocates `arg*16` bytes via `3104C`, and stores the result at `F0B8`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 68 bytes are authorized by this checkpoint. Any analysis at or beyond `2E7F0` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 67 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2E7F0`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
'''
    write1(CHK,text)

if __name__=='__main__':
    import sys
    assert len(sys.argv)==2
    {'integrate':integrate,'verify':verify,'checkpoint':checkpoint}[sys.argv[1]]()
