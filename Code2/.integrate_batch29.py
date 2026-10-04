from pathlib import Path
import re

root=Path('.')
code_path=root/'Code2/Code2.cpp'
prog_path=root/'Code2/PROGRESS.md'
ledger_path=root/'Code2/RESIDUAL_LEDGER.md'
cont_path=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
frag_path=root/'Code2/.batch29_fragment.cpp'

code=code_path.read_text()
frag=frag_path.read_text()
assert 'void function_15e38(Machine& m,Application29Boundary& b,u32 choice) noexcept' not in code
assert code.rstrip().endswith('} // namespace code2')
code=code.rstrip()
code=code[:code.rfind('} // namespace code2')].rstrip()+"\n\n"+frag.rstrip()+"\n\n} // namespace code2\n"
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 94703u;','constexpr u32 kVerifiedTranslatedBytes = 97832u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 1893u;','constexpr u32 kVerifiedNonCodeBytes = 1916u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 92402u +\n              (0x15DF5u-0x15534u) + (0x15E21u-0x15DF8u) +\n              (0x15E37u-0x15E24u));'''
new='''static_assert(kVerifiedTranslatedBytes == 94703u +\n              (0x15F7Eu-0x15E38u) + (0x16034u-0x15F7Eu) +\n              (0x1608Au-0x16034u) + (0x1624Bu-0x1608Cu) +\n              (0x1635Cu-0x1624Cu) + (0x163C4u-0x1635Cu) +\n              (0x16400u-0x163C4u) + (0x16524u-0x16400u) +\n              (0x16806u-0x16524u) + (0x1686Bu-0x16810u) +\n              (0x168C7u-0x1686Cu) + (0x16931u-0x168C8u) +\n              (0x1698Fu-0x16934u) + (0x16A57u-0x16990u) +\n              (0x16A85u-0x16A58u));'''
assert old in code
code=code.replace(old,new,1)
code_path.write_text(code)

prog=prog_path.read_text()
prog=prog.replace('object%20accounting-40.30%25-red','object%20accounting-41.61%25-red',1)
prog=prog.replace('**Verified translated instruction bytes:** **94,703**','**Verified translated instruction bytes:** **97,832**',1)
prog=prog.replace('**Proven non-code/data bytes:** **1,893**','**Proven non-code/data bytes:** **1,916**',1)
prog=prog.replace('**Reconciled object bytes:** **96,596 / 239,698 = 40.2990% (40.30%)**','**Reconciled object bytes:** **99,748 / 239,698 = 41.6140% (41.61%)**',1)
prog=prog.replace('**Unresolved object bytes:** **143,102**','**Unresolved object bytes:** **139,950**',1)
prog_path.write_text(prog)

ledger=ledger_path.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert marker in ledger
entries='''1608A..1608B | alignment | 2\n1624B..1624B | alignment | 1\n16806..1680F | noncode | 10\n1686B..1686B | alignment | 1\n168C7..168C7 | alignment | 1\n16931..16933 | alignment | 3\n1698F..1698F | alignment | 1\n16A57..16A57 | alignment | 1\n16A85..16A87 | alignment | 3\n'''
for line in entries.strip().splitlines(): assert line not in ledger
ledger=ledger.replace(marker,entries+marker,1)
ledger=ledger.replace('**Total proven non-code/data:** **1,893 bytes**.','**Total proven non-code/data:** **1,916 bytes**.',1)
ledger += '''\n\nBatch 29 evidence: `1608A..1608B`, `1624B`, `1686B`, `168C7`, `16931..16933`, `1698F`, `16A57`, and `16A85..16A87` decode as explicit Watcom no-op separators between directly reconstructed routine boundaries. `16806..1680F` is an exact ten-byte zero-filled gap bounded by the `16524` return at `16805` and the directly decoded `16810` entry; a full object-1 textual reference scan found no direct relative control-flow target or absolute memory/immediate reference to any byte in the gap. No Batch 29 residual byte is counted from appearance alone. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger_path.write_text(ledger)

cont=cont_path.read_text()
cont += '''\n\n## CODE.2 Batch 29 checkpoint\n\nBatch 29 added **3,129 translated bytes** plus **23 proven non-code/data bytes**. Cumulative accounting is **97,832 translated + 1,916 non-code = 99,748 / 239,698 = 41.6140% (41.61%)**; unresolved is **139,950 bytes**.\n\nThe sequential frontier now reaches `16A84`; `16A85..16A87` is proven `LEA EAX,[EAX+0]` alignment and the next routine begins at `16A88`. Preserve `16400`'s non-contiguous ownership of the backward blocks at `15F7E..16033` and `163C4`'s fall-through into that renderer; `16034`'s unsigned deadline comparison and save/restore of `7FEC`; `1608C`'s permanently `-1` reset selection plus dword-width `C050` XOR after a completed choice; `1624C`'s five-item spawn and encoded `(count-index+1)*16h` record copy without decrementing `258A6`; `16524`'s live-EBX second argument to `1C160(4Eh, EBX)`, selective graphics-state restore, and exact `D680/BF38` byte restoration; the zero-count handle leaks in `16810` and `168C8`; the two-byte/full-dword count behavior in `1686C/16934`; and `16A58`'s stack-slot alias that replaces only the low 16 bits of the handle value before returning it. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
cont_path.write_text(cont)
