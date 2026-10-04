from pathlib import Path
import base64, os, re

ROOT=Path('.')
CPP=ROOT/'Code2/Code2.cpp'
LED=ROOT/'Code2/RESIDUAL_LEDGER.md'
PROG=ROOT/'Code2/PROGRESS.md'
ADD=ROOT/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
PAYLOAD=ROOT/'Code2/.batch64.fragment.b64'
CHK=ROOT/'Code2/RESUME_CHECKPOINT.md'

OLD_T=181039
NEW_T=184044
OLD_N=3773
NEW_N=3780
NEW_R=187824
DEN=239698
NEW_U=51874

ROWS='''2BB9F..2BB9F | alignment | 1\n2BDF1..2BDF3 | alignment | 3\n2BFEA..2BFEB | alignment | 2\n2C22F..2C22F | alignment | 1\n'''

PROGRESS_BLOCK='''\n\n## CODE.2 Batch 64 checkpoint\n\nBatch 64 added **3,005 translated instruction bytes** plus **7 directly proven alignment bytes**, reconciling through `2C293`. Cumulative accounting is **184,044 translated + 3,780 non-code = 187,824 / 239,698 = 78.3586% (78.36%)**; unresolved is **51,874 bytes**.\n\nInstruction ownership spans `2B6D0..2BB9E`, `2BBA0..2BDF0`, `2BDF4..2BFE9`, `2BFEC..2C22E`, and `2C230..2C293`. Preserve the per-iteration signed record-count reloads, the `190h` target-cursor cap, descriptor/metadata reloads after the encoded opaque/random calls, the temporary `7D00h` screen-space shifts around `21BF0`, and the compiler's signed truncation-toward-zero division sequence in `2C230`. The next independent routine begins at `2C294`.\n'''

ADD_BLOCK='''\n\n## CODE.2 Batch 64 checkpoint\n\nBatch 64 adds **3,005 translated bytes** and **7 proven alignment bytes**, bringing cumulative accounting to **184,044 translated + 3,780 non-code = 187,824 / 239,698 = 78.3586% (78.36%)**; unresolved is **51,874 bytes**. The sequential frontier is now `2C294`. Preserve per-loop signed count reloads in the four record-population passes, the exact descriptor families (`22230`, `255DC`, `255D8`, `22234`), callback-sensitive metadata reloads, `2B2F4` mode ownership, the `7D00h`/`20h` temporary rectangle transformation, and the encoded signed-division sequence in `2C230`.\n'''

EVIDENCE='''\n\nBatch 64 evidence: the mandatory Batch-63 overlap `2B650..2B72F` and narrow window were regenerated from immutable CODE.2 and matched the checkpoint hashes before forward accounting. `2BB9F` and `2C22F` decode as one-byte NOP separators, `2BDF1..2BDF3` as `LEA EAX,[EAX+0]`, and `2BFEA..2BFEB` as `MOV EAX,EAX`. Each lies between directly reconstructed routine boundaries. A full object-1 direct relative branch/call target scan found no incoming target to any of these seven bytes, and an exact 32-bit absolute-address scan of object 1 found no reference to any of them.\n'''

def one_replace(text, old, new, label):
    n=text.count(old)
    assert n==1, f'{label}: expected exactly one occurrence of {old!r}, got {n}'
    return text.replace(old,new,1)

def load_fragment():
    raw=base64.b64decode(PAYLOAD.read_text().strip())
    text=raw.decode('utf-8')
    assert '// Batch 64: 2B6D0..2C293' in text
    assert '==3005u);' in text
    return text

def update_cpp():
    cpp=CPP.read_text()
    assert 'struct Application64Boundary' not in cpp
    frag=load_fragment().rstrip()+"\n\n"
    cpp=one_replace(cpp, f'constexpr u32 kVerifiedTranslatedBytes = {OLD_T}u;', f'constexpr u32 kVerifiedTranslatedBytes = {NEW_T}u;', 'translated constant')
    cpp=one_replace(cpp, f'constexpr u32 kVerifiedNonCodeBytes = {OLD_N}u;', f'constexpr u32 kVerifiedNonCodeBytes = {NEW_N}u;', 'noncode constant')
    pat=re.compile(r'static_assert\(kVerifiedTranslatedBytes ==(?P<body>.*?)\);', re.S)
    matches=list(pat.finditer(cpp))
    assert len(matches)==1, f'cumulative translated assertion count={len(matches)}'
    m=matches[0]
    body=m.group('body')
    assert '3005u' not in body
    repl='static_assert(kVerifiedTranslatedBytes =='+body.rstrip()+' + 3005u);'
    cpp=cpp[:m.start()]+repl+cpp[m.end():]
    marker='} // namespace code2'
    pos=cpp.rfind(marker)
    assert pos!=-1
    cpp=cpp[:pos]+frag+cpp[pos:]
    CPP.write_text(cpp)

def update_ledger():
    led=LED.read_text()
    assert '2BB9F..2BB9F | alignment | 1' not in led
    end='<!-- RESIDUAL-RANGES-END -->'
    assert led.count(end)==1
    led=led.replace(end,ROWS+end,1)
    led,n=re.subn(r'\*\*Total proven non-code/data:\*\* \*\*[0-9,]+ bytes\*\*\.', '**Total proven non-code/data:** **3,780 bytes**.', led, count=1)
    assert n==1
    led=led.rstrip()+EVIDENCE+'\n'
    LED.write_text(led)

def update_progress():
    p=PROG.read_text()
    assert '## CODE.2 Batch 64 checkpoint' not in p
    p=one_replace(p,'object%20accounting-77.10%25-red','object%20accounting-78.36%25-red','badge')
    p=one_replace(p,'**Verified translated instruction bytes:** **181,039**','**Verified translated instruction bytes:** **184,044**','progress translated')
    p=one_replace(p,'**Proven non-code/data bytes:** **3,773**','**Proven non-code/data bytes:** **3,780**','progress noncode')
    p=one_replace(p,'**Reconciled object bytes:** **184,812 / 239,698 = 77.1020% (77.10%)**','**Reconciled object bytes:** **187,824 / 239,698 = 78.3586% (78.36%)**','progress reconciled')
    p=one_replace(p,'**Unresolved object bytes:** **54,886**','**Unresolved object bytes:** **51,874**','progress unresolved')
    PROG.write_text(p.rstrip()+PROGRESS_BLOCK+'\n')

def update_addendum():
    a=ADD.read_text()
    assert '## CODE.2 Batch 64 checkpoint' not in a
    ADD.write_text(a.rstrip()+ADD_BLOCK+'\n')

def verify():
    cpp=CPP.read_text(); led=LED.read_text(); p=PROG.read_text(); a=ADD.read_text()
    assert f'constexpr u32 kVerifiedTranslatedBytes = {NEW_T}u;' in cpp
    assert f'constexpr u32 kVerifiedNonCodeBytes = {NEW_N}u;' in cpp
    assert 'struct Application64Boundary' in cpp
    assert 'function_2b6d0' in cpp and 'function_2bba0' in cpp and 'function_2bdf4' in cpp and 'function_2bfec' in cpp and 'function_2c230' in cpp
    assert '==3005u);' in cpp
    m=re.search(r'static_assert\(kVerifiedTranslatedBytes ==(.*?)\);',cpp,re.S)
    assert m and '3005u' in m.group(1)
    assert all(row in led for row in ROWS.strip().splitlines())
    assert '**Total proven non-code/data:** **3,780 bytes**.' in led
    rm=re.search(r'<!-- RESIDUAL-RANGES-BEGIN -->\n(.*?)<!-- RESIDUAL-RANGES-END -->',led,re.S)
    assert rm
    total=0
    for line in rm.group(1).splitlines():
        line=line.strip()
        if not line: continue
        fields=[x.strip() for x in line.split('|')]
        assert len(fields)==3, line
        total += int(fields[2])
    assert total==NEW_N, total
    assert '187,824 / 239,698 = 78.3586% (78.36%)' in p
    assert '## CODE.2 Batch 64 checkpoint' in p and '## CODE.2 Batch 64 checkpoint' in a

def integrate():
    update_cpp(); update_ledger(); update_progress(); update_addendum(); verify()

def checkpoint():
    batch=os.environ['BATCH_SHA']
    text=f'''# CODE.2 resume checkpoint\n\nThis file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.\n\n## Last committed substantive decompilation state\n\n- Last substantive batch commit: `{batch}` — `Integrate CODE.2 decompilation batch 64`.\n- Verified translated instruction bytes: **184,044**.\n- Proven non-code/data bytes: **3,780**.\n- Reconciled object bytes: **187,824 / 239,698 = 78.3586% (78.36%)**.\n- Unresolved object bytes: **51,874**.\n- Last integrated instruction ownership fold: `2B6D0..2C293`.\n- Batch-64 compiler separators `2BB9F`, `2BDF1..2BDF3`, `2BFEA..2BFEB`, and `2C22F` are directly proven alignment and counted in the residual ledger.\n- The four population passes preserve per-iteration signed count reloads, callback-sensitive metadata reloads, and the `190h` cursor cap; `2C230` preserves the compiler's signed truncation-toward-zero division sequence.\n- Next independently decoded instruction boundary: **`2C294`**.\n\n## Immutable identity\n\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n- Object-1 file start: `0x3AF24`.\n- Object-1 length: `0x3A852`.\n- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.\n\n## Mandatory overlap anchor for next resume\n\nRe-disassemble at minimum `2C210..2C2EF` from freshly regenerated object 1 before doing new work.\n\n- Window: object-relative `[0x2C210, 0x2C2F0)` — 224 bytes.\n- SHA-256: `cf07d19e1c5c4d247d1670bcf93584511934d42810eff6ea27a59cca500d1b8a`.\n- Narrow boundary window: `[0x2C250, 0x2C2B0)` — 96 bytes.\n- Narrow SHA-256: `c1a91027965c1612cd9d298d9c8d3091f2964053f50039fb73deb32e79bf9672`.\n- Narrow boundary bytes:\n  `5514c1fa1f2bc2d1f8bb8400000029c38b45148b5514c1fa1fc1e2031bc2c1f80301c3895df88b45148b5514c1fa1f2bc2d1f80145f88b45f88945fc8b45fcc95f5e5bc35356575589e581ec40000000b9040000008d7ddcbe70ac0200f3a568`\n\nInterpretation to re-prove on resume, not merely trust: `2C230..2C293` is the final Batch-64 signed layout-scalar helper and returns at `2C293`; `2C294` begins the next independent routine with the four-register prologue, a `40h`-byte local frame, and a four-dword `REP MOVSD` copy from the Batch-61 table at `2AC70`.\n\n## Interrupted/in-progress state\n\n**UNCOUNTED / NOT COMMITTED:** no Batch 65 bytes are authorized by this checkpoint. Any analysis at or beyond `2C294` must be revalidated against the overlap before counting.\n\n## Resume rule\n\nDo not rework Batch 64 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2C294`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.\n'''
    CHK.write_text(text)

if __name__=='__main__':
    import sys
    assert len(sys.argv)==2
    {'integrate':integrate,'verify':verify,'checkpoint':checkpoint}[sys.argv[1]]()
