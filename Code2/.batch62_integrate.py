from pathlib import Path
import base64
import os
import re
import sys


def ledger_sum(text: str) -> int:
    begin='<!-- RESIDUAL-RANGES-BEGIN -->'
    end='<!-- RESIDUAL-RANGES-END -->'
    block=text.split(begin,1)[1].split(end,1)[0]
    total=0
    for line in block.splitlines():
        m=re.match(r'^([0-9A-F]+)\.\.([0-9A-F]+) \| [^|]+ \| (\d+)$',line.strip())
        if m:
            a,b,n=int(m.group(1),16),int(m.group(2),16),int(m.group(3))
            assert b-a+1==n,(line,b-a+1,n)
            total+=n
    return total


def integrate() -> None:
    fragment=base64.b64decode(Path('Code2/.batch62.fragment.b64').read_text().strip()).decode()
    cpp_path=Path('Code2/Code2.cpp')
    cpp=cpp_path.read_text()
    assert 'constexpr u32 kVerifiedTranslatedBytes = 178417u;' in cpp
    assert 'constexpr u32 kVerifiedNonCodeBytes = 3755u;' in cpp
    assert '// Batch 62: 2AC80..2B085' not in cpp
    old_formula=(
        'static_assert(kVerifiedTranslatedBytes == 159155u +\n'
        '              (0x273E7u-0x26018u) + 2797u + 2032u + 3214u + 1281u + 939u + 1048u + 1427u + 1453u);'
    )
    new_formula=(
        'static_assert(kVerifiedTranslatedBytes == 159155u +\n'
        '              (0x273E7u-0x26018u) + 2797u + 2032u + 3214u + 1281u + 939u + 1048u + 1427u + 1453u + 1022u);'
    )
    assert old_formula in cpp
    cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 178417u;',
                    'constexpr u32 kVerifiedTranslatedBytes = 179439u;',1)
    cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 3755u;',
                    'constexpr u32 kVerifiedNonCodeBytes = 3765u;',1)
    cpp=cpp.replace(old_formula,new_formula,1)
    close='\n} // namespace code2\n'
    assert cpp.count(close)==1
    cpp=cpp.replace(close,fragment+close,1)
    cpp_path.write_text(cpp)

    ledger_path=Path('Code2/RESIDUAL_LEDGER.md')
    ledger=ledger_path.read_text()
    assert ledger_sum(ledger)==3755,ledger_sum(ledger)
    end='<!-- RESIDUAL-RANGES-END -->'
    rows=(
        '2ACDA..2ACDB | alignment | 2\n'
        '2AD15..2AD17 | alignment | 3\n'
        '2ADDD..2ADDF | alignment | 3\n'
        '2B086..2B087 | alignment | 2\n'
    )
    for row in rows.splitlines():
        assert row not in ledger
    ledger=ledger.replace(end,rows+end,1)
    assert ledger_sum(ledger)==3765,ledger_sum(ledger)
    ledger=re.sub(r'\*\*Total proven non-code/data:\*\* \*\*[0-9,]+ bytes\*\*\.',
                  '**Total proven non-code/data:** **3,765 bytes**.',ledger,count=1)
    evidence=(
        '\n\nBatch 62 evidence: `2ACDA..2ACDB` and `2B086..2B087` decode as `MOV EAX,EAX`; '
        '`2AD15..2AD17` and `2ADDD..2ADDF` decode as `LEA EAX,[EAX+0]`. Each lies between directly '
        'reconstructed routine boundaries. A fresh full-object direct relative `CALL`/`JMP`/conditional-branch '
        'target scan found no incoming target to any of these ten bytes. Instruction ownership is `2AC80..2ACD9`, '
        '`2ACDC..2AD14`, `2AD18..2ADDC`, `2ADE0..2AE2F`, and `2AE30..2B085`; the next independent routine '
        'begins at `2B088`.\n'
    )
    assert 'Batch 62 evidence:' not in ledger
    ledger_path.write_text(ledger.rstrip()+evidence)

    progress_path=Path('Code2/PROGRESS.md')
    progress=progress_path.read_text()
    replacements={
        'object%20accounting-76.00%25-red':'object%20accounting-76.43%25-red',
        '**Verified translated instruction bytes:** **178,417**':'**Verified translated instruction bytes:** **179,439**',
        '**Proven non-code/data bytes:** **3,755**':'**Proven non-code/data bytes:** **3,765**',
        '**Reconciled object bytes:** **182,172 / 239,698 = 76.0006% (76.00%)**':'**Reconciled object bytes:** **183,204 / 239,698 = 76.4312% (76.43%)**',
        '**Unresolved object bytes:** **57,526**':'**Unresolved object bytes:** **56,494**',
    }
    for old,new in replacements.items():
        assert old in progress,old
        progress=progress.replace(old,new,1)
    checkpoint=(
        '\n\n## CODE.2 Batch 62 checkpoint\n\n'
        'Batch 62 added **1,022 translated instruction bytes** plus **10 directly proven alignment bytes**, '
        'reconciling through `2B087`. Cumulative accounting is **179,439 translated + 3,765 non-code = '
        '183,204 / 239,698 = 76.4312% (76.43%)**; unresolved is **56,494 bytes**.\n\n'
        'Instruction ownership spans `2AC80..2ACD9`, `2ACDC..2AD14`, `2AD18..2ADDC`, `2ADE0..2AE2F`, '
        'and `2AE30..2B085`. The four compiler separators total ten bytes and have no direct incoming control-flow '
        'target. `2AE30` retains the entry-time opposite-page record base but reloads the `C050`-selected 16-bit '
        'count on every loop test, including after the opaque `1C1DC` probes reached through `2AD18`. The next '
        'independent routine begins at `2B088`.\n'
    )
    assert '## CODE.2 Batch 62 checkpoint' not in progress
    progress_path.write_text(progress.rstrip()+checkpoint)

    add_path=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md')
    add=add_path.read_text()
    note=(
        '\n\n## Batch 62 checkpoint\n\n'
        'Batch 62 adds **1,022 translated bytes** and **10 proven non-code/alignment bytes**, bringing cumulative '
        'accounting to **179,439 translated + 3,765 non-code = 183,204 / 239,698 = 76.4312% (76.43%)**. The '
        'sequential frontier is now `2B088`. Preserve the signed DWORD comparisons in `2ACDC`/`2AD18`/`2ADE0`, '
        'the two real resource-state calls from `2AD18`, the `C050` low-bit page toggle, the entry-stable record base '
        'but per-iteration count reload in `2AE30`, the second load of record word `+8` after rectangle acceptance, '
        'and the address-distinct inline predicate copies at `2AEBD` and `2B01D`.\n'
    )
    assert '## Batch 62 checkpoint' not in add
    add_path.write_text(add.rstrip()+note)


def checkpoint() -> None:
    batch=os.environ['BATCH_SHA']
    text=f'''# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `{batch}` — `Integrate CODE.2 decompilation batch 62`.
- Verified translated instruction bytes: **179,439**.
- Proven non-code/data bytes: **3,765**.
- Reconciled object bytes: **183,204 / 239,698 = 76.4312% (76.43%)**.
- Unresolved object bytes: **56,494**.
- Last integrated instruction ownership fold: `2AC80..2B085`.
- Batch-62 compiler separators `2ACDA..2ACDB`, `2AD15..2AD17`, `2ADDD..2ADDF`, and `2B086..2B087` are directly proven non-code/alignment and counted in the residual ledger.
- `2AE30` retains its entry-time record base but reloads the `C050`-selected 16-bit record count at every loop test; the two resource-state probes reached through `2AD18` remain opaque/address-distinct calls.
- Next independently decoded instruction boundary: **`2B088`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2B000..2B0DF` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2B000, 0x2B0E0)` — 224 bytes.
- SHA-256: `96140f0994fb0a075f5064ff8151e3d8be8049e6289f175229fcd83e1d537050`.
- Narrow boundary window: `[0x2B040, 0x2B0A0)` — 96 bytes.
- Narrow SHA-256: `5ccd80e3fb9008a17f98f1e426a9ffc4cd8514973fc0460dc4f88884e210b471`.
- Narrow boundary bytes:
  `09c745d401000000eb19817dd0910000007509c745d401000000eb07c745d400000000837dd4007409c745fc02000000eb0ce912feffffc745fc000000008b45fcc95f5e5bc38bc05356575589e581ec10000000c745f0ffffffffc745f80000`

Interpretation to re-prove on resume, not merely trust: `2B000..2B085` is the final tail of the Batch-62 rectangle/type classifier and returns at `2B085`; `2B086..2B087` is a `MOV EAX,EAX` compiler separator. `2B088` begins the next independent routine with the four-register prologue, a 10h-byte local frame, local `-10h = -1`, local `-8 = 0`, and a signed loop bound of `100h`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 63 bytes are authorized by this checkpoint. Any analysis at or beyond `2B088` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 62 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2B088`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
'''
    Path('Code2/RESUME_CHECKPOINT.md').write_text(text)


def verify() -> None:
    cpp=Path('Code2/Code2.cpp').read_text()
    assert 'constexpr u32 kVerifiedTranslatedBytes = 179439u;' in cpp
    assert 'constexpr u32 kVerifiedNonCodeBytes = 3765u;' in cpp
    assert '==1022u);' in cpp
    led=Path('Code2/RESIDUAL_LEDGER.md').read_text()
    assert ledger_sum(led)==3765,ledger_sum(led)
    assert '**Total proven non-code/data:** **3,765 bytes**.' in led


if __name__=='__main__':
    if len(sys.argv)!=2 or sys.argv[1] not in {'integrate','checkpoint','verify'}:
        raise SystemExit('usage: batch62_integrate.py integrate|checkpoint|verify')
    globals()[sys.argv[1]]()
