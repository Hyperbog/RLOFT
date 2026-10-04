from pathlib import Path
import os, re

ROOT=Path('.')
CPP=ROOT/'Code2/Code2.cpp'
LED=ROOT/'Code2/RESIDUAL_LEDGER.md'
PROG=ROOT/'Code2/PROGRESS.md'
ADD=ROOT/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
PAYLOAD=ROOT/'Code2/.batch68.fragment.cpp'
CHK=ROOT/'Code2/RESUME_CHECKPOINT.md'

OLD_T=193485
NEW_T=195658
OLD_N=3903
NEW_N=3974
NEW_R=199632
DEN=239698
NEW_U=40066

ROWS='''2E9CB..2E9CB | alignment | 1
2EA27..2EA27 | alignment | 1
2EA9A..2EA9B | alignment | 2
2EB26..2EB27 | alignment | 2
2EB5E..2EB5F | alignment | 2
2EC73..2EC73 | alignment | 1
2EC95..2EC97 | alignment | 3
2EC98..2ECB3 | table | 28
2EFB3..2EFB3 | alignment | 1
2EFB4..2EFCF | table | 28
2F0B2..2F0B3 | alignment | 2
'''

PROGRESS_BLOCK='''## CODE.2 Batch 68 checkpoint

Batch 68 added **2,173 translated instruction bytes** plus **71 directly proven non-code/data bytes**, reconciling through `2F0B3`. Cumulative accounting is **195,658 translated + 3,974 non-code = 199,632 / 239,698 = 83.2848% (83.28%)**; unresolved is **40,066 bytes**.

Instruction ownership spans `2E7F0..2E9CA`, `2E9CC..2EA26`, `2EA28..2EA99`, `2EA9C..2EB25`, `2EB28..2EB5D`, `2EB60..2EC72`, `2EC74..2EC83`, `2EC84..2EC94`, `2ECB4..2EFB2`, and `2EFD0..2F0B1`. The exact CS tables `2EC98..2ECB3` and `2EFB4..2EFCF` are owned by their adjacent dispatchers. Preserve uninitialized high bits when `2E7F0` initializes item byte 0, `2ECB4`'s out-of-range error path continuing into the indexed access, low-byte selector arithmetic, callback/global reloads, and `2EFD0`'s type-dependent release calls. This batch also repairs the proven ABI at `28020` and `286C4`: both ignore their first stack argument and consume the stream from the second. The next independent routine begins at `2F0B4`.
'''

ADD_BLOCK='''## CODE.2 Batch 68 checkpoint

Batch 68 adds **2,173 translated bytes** and **71 proven non-code/data bytes**, bringing cumulative accounting to **195,658 translated + 3,974 non-code = 199,632 / 239,698 = 83.2848% (83.28%)**; unresolved is **40,066 bytes**. The sequential frontier is now `2F0B4`. Preserve the two exact seven-entry CS tables, `2E7F0`'s partial initialization of the nine-byte item records, `2ECB4`'s continue-after-error indexing and selector dispatch, and `2EFD0`'s low-nibble cleanup routing. Proven earlier ABI signatures at `28020` and `286C4` were corrected to retain the ignored first argument and stream-valued second argument.
'''

EVIDENCE='''Batch 68 evidence: the mandatory Batch-67 overlap `2E770..2E84F` and narrow window `2E7C0..2E81F` were regenerated from immutable CODE.2 and matched the recorded checkpoint hashes before forward accounting. `2EC98..2ECB3` is the exact seven-entry CS dword table consumed by the bounded indirect jump at `2EDF8`; `2EFB4..2EFCF` is the exact seven-entry table consumed by the bounded indirect jump at `2F019`. The remaining classified bytes decode as compiler `NOP`, `MOV EAX,EAX`, or `LEA EAX,[EAX+0]` separators. A full object-1 direct relative CALL/JMP/Jcc target scan found no incoming target to any separator byte, and no classified separator address occurs as a 32-bit absolute pointer in object 1. Direct disassembly of `28020` and `286C4` independently proves both routines read their stream from stack argument two while ignoring argument one; the source signatures were repaired without changing byte accounting.
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
    assert '// Batch 68: 2E7F0..2F0B1' in text
    assert '==2173u);' in text
    return text.rstrip()+'\n\n'

def update_cpp():
    cpp=CPP.read_text()
    assert 'struct Application68Boundary' not in cpp
    cpp=one_replace(cpp,'[[nodiscard]] u32 function_28020(Machine& m,Application55Boundary& b,u32 stream) noexcept {','[[nodiscard]] u32 function_28020(Machine& m,Application55Boundary& b,u32 /*ignored*/,u32 stream) noexcept {','28020 ABI')
    cpp=one_replace(cpp,'[[nodiscard]] u32 function_286c4(Machine& m,Application55Boundary& b,u32 stream) noexcept {','[[nodiscard]] u32 function_286c4(Machine& m,Application55Boundary& b,u32 /*ignored*/,u32 stream) noexcept {','286c4 ABI')
    cpp=one_replace(cpp,f'constexpr u32 kVerifiedTranslatedBytes = {OLD_T}u;',f'constexpr u32 kVerifiedTranslatedBytes = {NEW_T}u;','translated constant')
    cpp=one_replace(cpp,f'constexpr u32 kVerifiedNonCodeBytes = {OLD_N}u;',f'constexpr u32 kVerifiedNonCodeBytes = {NEW_N}u;','noncode constant')
    pat=re.compile(r'static_assert\(kVerifiedTranslatedBytes ==(?P<body>.*?)\);',re.S)
    matches=list(pat.finditer(cpp))
    assert len(matches)==1, f'cumulative translated assertion count={len(matches)}'
    m=matches[0]
    body=m.group('body')
    assert '2173u' not in body
    repl='static_assert(kVerifiedTranslatedBytes =='+body.rstrip()+' + 2173u);'
    cpp=cpp[:m.start()]+repl+cpp[m.end():]
    marker='} // namespace code2'
    pos=cpp.rfind(marker)
    assert pos!=-1
    cpp=cpp[:pos]+load_fragment()+cpp[pos:]
    write1(CPP,cpp)

def update_ledger():
    led=LED.read_text()
    assert '2E9CB..2E9CB | alignment | 1' not in led
    end='<!-- RESIDUAL-RANGES-END -->'
    assert led.count(end)==1
    led=led.replace(end,ROWS+end,1)
    led,n=re.subn(r'\*\*Total proven non-code/data:\*\* \*\*[0-9,]+ bytes\*\*\.', '**Total proven non-code/data:** **3,974 bytes**.',led,count=1)
    assert n==1
    led=append_section(led,EVIDENCE)
    write1(LED,led)

def update_progress():
    p=PROG.read_text()
    assert '## CODE.2 Batch 68 checkpoint' not in p
    p,n=re.subn(r'object%20accounting-[0-9.]+%25-red','object%20accounting-83.28%25-red',p,count=1)
    assert n==1
    p=one_replace(p,'**Verified translated instruction bytes:** **193,485**','**Verified translated instruction bytes:** **195,658**','progress translated')
    p=one_replace(p,'**Proven non-code/data bytes:** **3,903**','**Proven non-code/data bytes:** **3,974**','progress noncode')
    p=one_replace(p,'**Reconciled object bytes:** **197,388 / 239,698 = 82.3486% (82.35%)**','**Reconciled object bytes:** **199,632 / 239,698 = 83.2848% (83.28%)**','progress reconciled')
    p=one_replace(p,'**Unresolved object bytes:** **42,310**','**Unresolved object bytes:** **40,066**','progress unresolved')
    write1(PROG,append_section(p,PROGRESS_BLOCK))

def update_addendum():
    a=ADD.read_text()
    assert '## CODE.2 Batch 68 checkpoint' not in a
    write1(ADD,append_section(a,ADD_BLOCK))

def verify():
    cpp=CPP.read_text(); led=LED.read_text(); p=PROG.read_text(); a=ADD.read_text()
    assert f'constexpr u32 kVerifiedTranslatedBytes = {NEW_T}u;' in cpp
    assert f'constexpr u32 kVerifiedNonCodeBytes = {NEW_N}u;' in cpp
    assert 'struct Application68Boundary' in cpp
    assert 'function_28020(Machine& m,Application55Boundary& b,u32 /*ignored*/,u32 stream)' in cpp
    assert 'function_286c4(Machine& m,Application55Boundary& b,u32 /*ignored*/,u32 stream)' in cpp
    funcs=['function_2e7f0','function_2e9cc','function_2ea28','function_2ea9c','function_2eb28','function_2eb60','function_2ec74','function_2ec84','function_2ecb4','function_2efd0']
    assert all(x in cpp for x in funcs)
    assert '==2173u);' in cpp
    m=re.search(r'static_assert\(kVerifiedTranslatedBytes ==(.*?)\);',cpp,re.S)
    assert m and '2173u' in m.group(1)
    assert all(row in led for row in ROWS.strip().splitlines())
    assert '**Total proven non-code/data:** **3,974 bytes**.' in led
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
    assert '199,632 / 239,698 = 83.2848% (83.28%)' in p
    assert '## CODE.2 Batch 68 checkpoint' in p and '## CODE.2 Batch 68 checkpoint' in a

def integrate():
    update_cpp(); update_ledger(); update_progress(); update_addendum(); verify()

def checkpoint():
    batch=os.environ['BATCH_SHA']
    text=f'''# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `{batch}` — `Integrate CODE.2 decompilation batch 68`.
- Verified translated instruction bytes: **195,658**.
- Proven non-code/data bytes: **3,974**.
- Reconciled object bytes: **199,632 / 239,698 = 83.2848% (83.28%)**.
- Unresolved object bytes: **40,066**.
- Last integrated ownership fold: `2E7F0..2F0B3` (instructions through `2F0B1`, proven tables/separators through `2F0B3`).
- Proven ABI repair in this batch: `28020` and `286C4` retain an ignored first stack argument and consume their stream from the second.
- Next independently decoded instruction boundary: **`2F0B4`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2F030..2F10F` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2F030, 0x2F110)` — 224 bytes.
- SHA-256: `a5e30b22aeb2f4c40ae1ff1e585cd312de4a7e2f2e3a76d5e3abfa844e1c27cf`.
- Narrow boundary window: `[0x2F080, 0x2F0E0)` — 96 bytes.
- Narrow SHA-256: `d3386e4b85e390dccef48af30d43af9658e2c7f69c743388d8eb5f00ca2889ab`.
- Narrow boundary bytes:
  `048b5c240801d08d14dd0000000001da8b580cc6441a02008b580c66c7441a0300008b580cc7441a0500000000ff48045bc38bc05356575583ec148b35b8f0000031edb9ffffffff892c24833db4f00000000f8e7c000000896c240489f00344`

Interpretation to re-prove on resume, not merely trust: `2F0B1` is the final Batch-68 `RET`; `2F0B2..2F0B3` is a `MOV EAX,EAX` separator; `2F0B4` begins the next independent four-register-prologue routine with a `14h` local frame, loads `F0B8`, initializes EBP=0 and ECX=-1, then begins a signed scan of the slot/item arrays.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 69 bytes are authorized by this checkpoint. Any analysis at or beyond `2F0B4` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 68 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2F0B4`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
'''
    write1(CHK,text)

if __name__=='__main__':
    import sys
    assert len(sys.argv)==2
    {'integrate':integrate,'verify':verify,'checkpoint':checkpoint}[sys.argv[1]]()
