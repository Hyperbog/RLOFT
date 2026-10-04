from pathlib import Path
import os, re

ROOT=Path('.')
CPP=ROOT/'Code2/Code2.cpp'
LED=ROOT/'Code2/RESIDUAL_LEDGER.md'
PROG=ROOT/'Code2/PROGRESS.md'
ADD=ROOT/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
PAYLOAD=ROOT/'Code2/.batch65.fragment.cpp'
CHK=ROOT/'Code2/RESUME_CHECKPOINT.md'

OLD_T=184044
NEW_T=187462
OLD_N=3780
NEW_N=3802
NEW_R=191264
DEN=239698
NEW_U=48434

ROWS='''2C7F7..2C7FF | alignment | 9
2C8E2..2C8E3 | alignment | 2
2C9D9..2C9DB | alignment | 3
2CAD3..2CAD3 | alignment | 1
2CC85..2CC87 | alignment | 3
2CEB6..2CEB7 | alignment | 2
2D002..2D003 | alignment | 2
'''

PROGRESS_BLOCK='''## CODE.2 Batch 65 checkpoint

Batch 65 added **3,418 translated instruction bytes** plus **22 directly proven alignment bytes**, reconciling through `2D003`. Cumulative accounting is **187,462 translated + 3,802 non-code = 191,264 / 239,698 = 79.7937% (79.79%)**; unresolved is **48,434 bytes**.

Instruction ownership spans `2C294..2C7F6`, `2C800..2C8E1`, `2C8E4..2C9D8`, `2C9DC..2CA5B`, `2CA5C..2CAD2`, `2CAD4..2CC84`, `2CC88..2CEB5`, and `2CEB8..2D001`. Preserve `2C294`'s dynamic `F03C/F03E` clip tests and compiler-order rectangle restore, callback-sensitive target/source reloads, `2CAD4`'s independent party-slot branches, and `2CEB8`'s low-word-only `255D4 -> 255D0` transition copy plus post-callback `258EC` reload. The next independent routine begins at `2D004`.
'''

ADD_BLOCK='''## CODE.2 Batch 65 checkpoint

Batch 65 adds **3,418 translated bytes** and **22 proven alignment bytes**, bringing cumulative accounting to **187,462 translated + 3,802 non-code = 191,264 / 239,698 = 79.7937% (79.79%)**; unresolved is **48,434 bytes**. The sequential frontier is now `2D004`. Preserve `2C294`'s non-linear saved-rectangle restore order, its dynamically re-read clip globals and render-kind fields, the callback-sensitive metadata/source reloads in `2C8E4`, independent transition branches in `2CAD4`, and `2CEB8`'s word-only transition writes and explicit `258EC` reload after `2E510`.
'''

EVIDENCE='''Batch 65 evidence: the mandatory Batch-64 overlap `2C210..2C2EF` and narrow window `2C250..2C2AF` were regenerated from immutable CODE.2 and matched the recorded checkpoint SHA-256 values exactly before forward accounting. `2C7F7..2C7FF` is a nine-byte zero-fill separator; `2C8E2..2C8E3`, `2CEB6..2CEB7`, and `2D002..2D003` decode as `MOV EAX,EAX`; `2C9D9..2C9DB` and `2CC85..2CC87` decode as `LEA EAX,[EAX+0]`; `2CAD3` is `NOP`. A full object-1 direct relative CALL/JMP/Jcc target scan found no incoming target to any of these 22 bytes, and exact 32-bit object-relative absolute-address scans found no reference to any of them.
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
    assert '// Batch 65: 2C294..2D001' in text
    assert '==3418u);' in text
    return text.rstrip()+'\n\n'

def update_cpp():
    cpp=CPP.read_text()
    assert 'struct Application65Boundary' not in cpp
    cpp=one_replace(cpp,f'constexpr u32 kVerifiedTranslatedBytes = {OLD_T}u;',f'constexpr u32 kVerifiedTranslatedBytes = {NEW_T}u;','translated constant')
    cpp=one_replace(cpp,f'constexpr u32 kVerifiedNonCodeBytes = {OLD_N}u;',f'constexpr u32 kVerifiedNonCodeBytes = {NEW_N}u;','noncode constant')
    pat=re.compile(r'static_assert\(kVerifiedTranslatedBytes ==(?P<body>.*?)\);',re.S)
    matches=list(pat.finditer(cpp))
    assert len(matches)==1, f'cumulative translated assertion count={len(matches)}'
    m=matches[0]
    body=m.group('body')
    assert '3418u' not in body
    repl='static_assert(kVerifiedTranslatedBytes =='+body.rstrip()+' + 3418u);'
    cpp=cpp[:m.start()]+repl+cpp[m.end():]
    marker='} // namespace code2'
    pos=cpp.rfind(marker)
    assert pos!=-1
    cpp=cpp[:pos]+load_fragment()+cpp[pos:]
    write1(CPP,cpp)

def update_ledger():
    led=LED.read_text()
    assert '2C7F7..2C7FF | alignment | 9' not in led
    end='<!-- RESIDUAL-RANGES-END -->'
    assert led.count(end)==1
    led=led.replace(end,ROWS+end,1)
    led,n=re.subn(r'\*\*Total proven non-code/data:\*\* \*\*[0-9,]+ bytes\*\*\.', '**Total proven non-code/data:** **3,802 bytes**.',led,count=1)
    assert n==1
    led=append_section(led,EVIDENCE)
    write1(LED,led)

def update_progress():
    p=PROG.read_text()
    assert '## CODE.2 Batch 65 checkpoint' not in p
    p=one_replace(p,'object%20accounting-78.36%25-red','object%20accounting-79.79%25-red','badge')
    p=one_replace(p,'**Verified translated instruction bytes:** **184,044**','**Verified translated instruction bytes:** **187,462**','progress translated')
    p=one_replace(p,'**Proven non-code/data bytes:** **3,780**','**Proven non-code/data bytes:** **3,802**','progress noncode')
    p=one_replace(p,'**Reconciled object bytes:** **187,824 / 239,698 = 78.3586% (78.36%)**','**Reconciled object bytes:** **191,264 / 239,698 = 79.7937% (79.79%)**','progress reconciled')
    p=one_replace(p,'**Unresolved object bytes:** **51,874**','**Unresolved object bytes:** **48,434**','progress unresolved')
    write1(PROG,append_section(p,PROGRESS_BLOCK))

def update_addendum():
    a=ADD.read_text()
    assert '## CODE.2 Batch 65 checkpoint' not in a
    write1(ADD,append_section(a,ADD_BLOCK))

def verify():
    cpp=CPP.read_text(); led=LED.read_text(); p=PROG.read_text(); a=ADD.read_text()
    assert f'constexpr u32 kVerifiedTranslatedBytes = {NEW_T}u;' in cpp
    assert f'constexpr u32 kVerifiedNonCodeBytes = {NEW_N}u;' in cpp
    assert 'struct Application65Boundary' in cpp
    funcs=['function_2c294','function_2c800','function_2c8e4','function_2c9dc','function_2ca5c','function_2cad4','function_2cc88','function_2ceb8']
    assert all(x in cpp for x in funcs)
    assert '==3418u);' in cpp
    m=re.search(r'static_assert\(kVerifiedTranslatedBytes ==(.*?)\);',cpp,re.S)
    assert m and '3418u' in m.group(1)
    assert all(row in led for row in ROWS.strip().splitlines())
    assert '**Total proven non-code/data:** **3,802 bytes**.' in led
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
    assert '191,264 / 239,698 = 79.7937% (79.79%)' in p
    assert '## CODE.2 Batch 65 checkpoint' in p and '## CODE.2 Batch 65 checkpoint' in a

def integrate():
    update_cpp(); update_ledger(); update_progress(); update_addendum(); verify()

def checkpoint():
    batch=os.environ['BATCH_SHA']
    text=f'''# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `{batch}` — `Integrate CODE.2 decompilation batch 65`.
- Verified translated instruction bytes: **187,462**.
- Proven non-code/data bytes: **3,802**.
- Reconciled object bytes: **191,264 / 239,698 = 79.7937% (79.79%)**.
- Unresolved object bytes: **48,434**.
- Last integrated instruction ownership fold: `2C294..2D003` (instructions through `2D001`, proven separator through `2D003`).
- Batch-65 compiler separators `2C7F7..2C7FF`, `2C8E2..2C8E3`, `2C9D9..2C9DB`, `2CAD3`, `2CC85..2CC87`, `2CEB6..2CEB7`, and `2D002..2D003` are directly proven alignment and counted in the residual ledger.
- Preserve `2C294`'s dynamic clip/global reloads and compiler-order rectangle restore, `2CAD4`'s independent party branches, and `2CEB8`'s low-word transition copy plus post-callback `258EC` reload.
- Next independently decoded instruction boundary: **`2D004`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2CFF0..2D0CF` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2CFF0, 0x2D0D0)` — 224 bytes.
- SHA-256: `9f68d05e36bfe5d05de60305f4e46354216dc65453c4d631fc38e1d78bc43796`.
- Narrow boundary window: `[0x2D000, 0x2D060)` — 96 bytes.
- Narrow SHA-256: `f45147203dc64635216a67d07a730ad3cfb44f12803e208f987fff1c976d115d`.
- Narrow boundary bytes:
  `5bc38bc05356575589e581ec100000006a3ee8c5f1feff83c40485c00f84e00000008b451401c00fbf80225602003d420200007c138b451401c00fbf80225602003d470200007e05e9b0000000833dc8c60000087409833dc8c60000127502eb`

Interpretation to re-prove on resume, not merely trust: `2D001` is the final Batch-65 `RET`; `2D002..2D003` is a `MOV EAX,EAX` compiler separator; `2D004` begins the next independent routine with the four-register prologue, a `10h` local frame, and `PUSH 3Eh / CALL 1C1DCh` before routing on `[25622 + argument*2]` and `C6C8`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 66 bytes are authorized by this checkpoint. Any analysis at or beyond `2D004` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 65 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2D004`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
'''
    write1(CHK,text)

if __name__=='__main__':
    import sys
    assert len(sys.argv)==2
    {'integrate':integrate,'verify':verify,'checkpoint':checkpoint}[sys.argv[1]]()
