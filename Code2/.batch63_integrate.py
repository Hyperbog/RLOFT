from pathlib import Path
import base64, os, re, sys

def ledger_sum(text):
    block=text.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split('<!-- RESIDUAL-RANGES-END -->',1)[0]
    total=0
    for line in block.splitlines():
        m=re.match(r'^([0-9A-F]+)\.\.([0-9A-F]+) \| [^|]+ \| (\d+)$',line.strip())
        if m:
            a,b,n=int(m.group(1),16),int(m.group(2),16),int(m.group(3))
            assert b-a+1==n,(line,b-a+1,n)
            total+=n
    return total

def integrate():
    fragment=base64.b64decode(Path('Code2/.batch63.fragment.b64').read_text().strip()).decode()
    cpp_path=Path('Code2/Code2.cpp'); cpp=cpp_path.read_text()
    assert 'constexpr u32 kVerifiedTranslatedBytes = 179439u;' in cpp
    assert 'constexpr u32 kVerifiedNonCodeBytes = 3765u;' in cpp
    assert '// Batch 63: 2B088..2B6CC' not in cpp
    cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 179439u;','constexpr u32 kVerifiedTranslatedBytes = 181039u;',1)
    cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 3765u;','constexpr u32 kVerifiedNonCodeBytes = 3773u;',1)
    pat=r'(static_assert\(kVerifiedTranslatedBytes == 159155u \+.*?\+ 1022u)\);'
    cpp,n=re.subn(pat,r'\1 + 1600u);',cpp,count=1,flags=re.S)
    assert n==1,n
    close='\n} // namespace code2\n'; assert cpp.count(close)==1
    cpp=cpp.replace(close,fragment+close,1); cpp_path.write_text(cpp)

    led_path=Path('Code2/RESIDUAL_LEDGER.md'); led=led_path.read_text()
    assert ledger_sum(led)==3765,ledger_sum(led)
    rows=('2B186..2B187 | alignment | 2\n'
          '2B1BA..2B1BB | alignment | 2\n'
          '2B2F3..2B2F3 | alignment | 1\n'
          '2B6CD..2B6CF | alignment | 3\n')
    for row in rows.splitlines(): assert row not in led
    led=led.replace('<!-- RESIDUAL-RANGES-END -->',rows+'<!-- RESIDUAL-RANGES-END -->',1)
    assert ledger_sum(led)==3773,ledger_sum(led)
    led=re.sub(r'\*\*Total proven non-code/data:\*\* \*\*[0-9,]+ bytes\*\*\.',
               '**Total proven non-code/data:** **3,773 bytes**.',led,count=1)
    assert 'Batch 63 evidence:' not in led
    led=led.rstrip()+'''\n\nBatch 63 evidence: `2B186..2B187` and `2B1BA..2B1BB` decode as `MOV EAX,EAX`; `2B2F3` is `NOP`; `2B6CD..2B6CF` is `LEA EAX,[EAX+0]`. Each lies between directly reconstructed routine boundaries. Fresh full-object scans found no direct relative `CALL`/`JMP`/conditional-branch target and no raw absolute dword reference to any of these eight separator bytes. Instruction ownership is `2B088..2B185`, `2B188..2B1B9`, `2B1BC..2B2F2`, and `2B2F4..2B6CC`; the next independent routine begins at `2B6D0`.\n'''
    led_path.write_text(led)

    prog_path=Path('Code2/PROGRESS.md'); prog=prog_path.read_text()
    reps={
      'object%20accounting-76.43%25-red':'object%20accounting-77.10%25-red',
      '**Verified translated instruction bytes:** **179,439**':'**Verified translated instruction bytes:** **181,039**',
      '**Proven non-code/data bytes:** **3,765**':'**Proven non-code/data bytes:** **3,773**',
      '**Reconciled object bytes:** **183,204 / 239,698 = 76.4312% (76.43%)**':'**Reconciled object bytes:** **184,812 / 239,698 = 77.1020% (77.10%)**',
      '**Unresolved object bytes:** **56,494**':'**Unresolved object bytes:** **54,886**',
    }
    for a,b in reps.items(): assert a in prog,a; prog=prog.replace(a,b,1)
    assert '## CODE.2 Batch 63 checkpoint' not in prog
    prog=prog.rstrip()+'''\n\n## CODE.2 Batch 63 checkpoint\n\nBatch 63 added **1,600 translated instruction bytes** plus **8 directly proven alignment bytes**, reconciling through `2B6CF`. Cumulative accounting is **181,039 translated + 3,773 non-code = 184,812 / 239,698 = 77.1020% (77.10%)**; unresolved is **54,886 bytes**.\n\nThe fold owns `2B088..2B185`, `2B188..2B1B9`, `2B1BC..2B2F2`, and `2B2F4..2B6CC`. Preserve grouped-key value propagation in `2B088`, signed range tests in `2B188`, the signed max-distance rounding in `2B1BC`, and `2B2F4`'s signed IDIV quotient/remainder behavior, mutable `2B608` word cache, partial-register heading extraction, and mode-indexed `27100` tail addition. The next independent routine begins at `2B6D0`.\n'''
    prog_path.write_text(prog)

    add_path=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md'); add=add_path.read_text()
    assert '## Batch 63 checkpoint' not in add
    add=add.rstrip()+'''\n\n## Batch 63 checkpoint\n\nBatch 63 adds **1,600 translated bytes** and **8 proven alignment bytes**, bringing cumulative accounting to **181,039 translated + 3,773 non-code = 184,812 / 239,698 = 77.1020% (77.10%)**. The sequential frontier is now `2B6D0`. Preserve `2B088`'s cross-entry carried value and signed neighborhood tables, `2B1BC`'s signed comparison after the runtime-style absolute value and `+8000h` rounding, and `2B2F4`'s mutable `2B608` cache, signed `IDIV` quotient/remainders, heading normalization loops, low-word additions, and final `27100 + mode*2` word adjustment.\n'''
    add_path.write_text(add)

def verify():
    cpp=Path('Code2/Code2.cpp').read_text(); led=Path('Code2/RESIDUAL_LEDGER.md').read_text()
    assert 'constexpr u32 kVerifiedTranslatedBytes = 181039u;' in cpp
    assert 'constexpr u32 kVerifiedNonCodeBytes = 3773u;' in cpp
    assert '==1600u);' in cpp
    assert ledger_sum(led)==3773,ledger_sum(led)
    assert '**Total proven non-code/data:** **3,773 bytes**.' in led

def checkpoint():
    batch=os.environ['BATCH_SHA']
    text=f'''# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `{batch}` — `Integrate CODE.2 decompilation batch 63`.
- Verified translated instruction bytes: **181,039**.
- Proven non-code/data bytes: **3,773**.
- Reconciled object bytes: **184,812 / 239,698 = 77.1020% (77.10%)**.
- Unresolved object bytes: **54,886**.
- Last integrated instruction ownership fold: `2B088..2B6CC`.
- Batch-63 compiler separators `2B186..2B187`, `2B1BA..2B1BB`, `2B2F3`, and `2B6CD..2B6CF` are directly proven alignment and counted in the residual ledger.
- `2B2F4` preserves signed IDIV quotient/remainder behavior and writes target word 0 into the mutable data-segment cache at `2B608 + q*2` on its mode-1/remainder-zero path.
- Next independently decoded instruction boundary: **`2B6D0`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2B650..2B72F` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2B650, 0x2B730)` — 224 bytes.
- SHA-256: `2c28862c66da6bb018cd50179f3ee6c6c531520d01a6d60ec703adcd4339512a`.
- Narrow boundary window: `[0x2B690, 0x2B6F0)` — 96 bytes.
- Narrow SHA-256: `fa599e86f0a27363cc7ce2f99829aaa381e2e218f92b5862eef013b5f0f92252`.
- Narrow boundary bytes:
  `c1fa1ff77d2489c38b45ec8b55ecc1fa1ff7fb89c28b45180fb7400401d08b5518668942048b451c01c0668b90007102008b451866015004c95f5e5bc38d40005356575589e581ec20000000c745e400000000c745e8000000000fbf05a25802`

Interpretation to re-prove on resume, not merely trust: `2B690..2B6CC` is the final divisor/phase tail of Batch 63 and returns at `2B6CC`; `2B6CD..2B6CF` is `LEA EAX,[EAX+0]` alignment. `2B6D0` begins the next independent routine with four saved registers, a 20h-byte local frame, zeroed locals `-1Ch/-18h`, and a signed-count scan of `258A2`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 64 bytes are authorized by this checkpoint. Any analysis at or beyond `2B6D0` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 63 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2B6D0`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
'''
    Path('Code2/RESUME_CHECKPOINT.md').write_text(text)

if __name__=='__main__':
    assert len(sys.argv)==2 and sys.argv[1] in {'integrate','verify','checkpoint'}
    globals()[sys.argv[1]]()
