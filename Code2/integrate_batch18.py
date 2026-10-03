from pathlib import Path
import base64, zlib

cppp=Path('Code2/Code2.cpp')
cpp=cppp.read_text()
inc=zlib.decompress(base64.b64decode(Path('Code2/batch18.zb64').read_text())).decode()

cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 56834u;',
                'constexpr u32 kVerifiedTranslatedBytes = 60401u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 1009u;',
                'constexpr u32 kVerifiedNonCodeBytes = 1147u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 52825u +\n              (0x0B6F1u-0x0B518u) +\n              (0x0B73Du-0x0B6F4u) +\n              (0x0C6D7u-0x0B950u));'''
new='''static_assert(kVerifiedTranslatedBytes == 56834u +\n              (0x0C7C1u-0x0C704u) + (0x0C80Bu-0x0C7C4u) +\n              (0x0C90Du-0x0C80Cu) + (0x0C99Fu-0x0C910u) +\n              (0x0CA36u-0x0C9A0u) + (0x0CADAu-0x0CA38u) +\n              (0x0CBC6u-0x0CADCu) + (0x0CCF1u-0x0CBC8u) +\n              (0x0CD41u-0x0CCF4u) + (0x0CEE4u-0x0CD44u) +\n              (0x0CF2Au-0x0CEE4u) + (0x0CF6Du-0x0CF2Cu) +\n              (0x0D097u-0x0CF88u) + (0x0D1DFu-0x0D0B0u) +\n              (0x0D4E8u-0x0D1E0u) + (0x0D53Eu-0x0D4E8u));'''
if old not in cpp: raise SystemExit('old accounting assert not found')
cpp=cpp.replace(old,new,1)
marker='\n} // namespace code2\n'
if marker not in cpp: raise SystemExit('namespace marker not found')
cpp=cpp.replace(marker,'\n'+inc.rstrip()+'\n'+marker,1)
cppp.write_text(cpp)

ledp=Path('Code2/RESIDUAL_LEDGER.md')
led=ledp.read_text()
ranges='''0C6D7..0C6DF | alignment | 9\n0C6E0..0C703 | table | 36\n0C7C1..0C7C3 | alignment | 3\n0C80B..0C80B | alignment | 1\n0C90D..0C90F | alignment | 3\n0C99F..0C99F | alignment | 1\n0CA36..0CA37 | alignment | 2\n0CADA..0CADB | alignment | 2\n0CBC6..0CBC7 | alignment | 2\n0CCF1..0CCF3 | alignment | 3\n0CD41..0CD43 | alignment | 3\n0CF2A..0CF2B | alignment | 2\n0CF6D..0CF6F | alignment | 3\n0CF70..0CF87 | table | 24\n0D097..0D097 | alignment | 1\n0D098..0D0AF | table | 24\n0D1DF..0D1DF | alignment | 1\n0D53E..0D53F | alignment | 2\n0D540..0D54F | table | 16\n'''
end='<!-- RESIDUAL-RANGES-END -->'
if end not in led: raise SystemExit('ledger marker missing')
led=led.replace(end,ranges+end,1)
led=led.replace('**Total proven non-code/data:** **1,009 bytes**.',
                '**Total proven non-code/data:** **1,147 bytes**.',1)
led += '''\n\nBatch 18 evidence: `0C6E0..0C703` is the exact nine-dword source copied by `REP MOVSD` at `0CBD5`, with values `0,1,0,1,0,2,3,4,5`; the preceding zero bytes `0C6D7..0C6DF` lie between the directly reconstructed `0B950` return and that consumed data object. `0CF70..0CF87` is the six-entry CS table selected at `0D025` with targets `0D02C,0D02C,0D02C,0D04A,0D03B,0D053`. `0D098..0D0AF` is the six-entry CS table selected at `0D127`, targeting `0D12E,0D12E,0D12E,0D152,0D142,0D15D`. `0D540..0D54F` is the four-entry CS table consumed at `0D69A`, targeting alternating `0D6AF/0D6A2`. All other Batch 18 ranges are explicit compiler NOP/LEA/MOV-EAX,EAX separators between directly reconstructed routines. A full object-1 direct relative control-flow scan found no incoming target to any Batch 18 separator or table byte.\n'''
ledp.write_text(led)

progp=Path('Code2/PROGRESS.md')
prog=progp.read_text()
repls={
'object%20accounting-24.13%25-red':'object%20accounting-25.68%25-red',
'**Verified translated instruction bytes:** **56,834**':'**Verified translated instruction bytes:** **60,401**',
'**Proven non-code/data bytes:** **1,009**':'**Proven non-code/data bytes:** **1,147**',
'**Reconciled object bytes:** **57,843 / 239,698 = 24.1316% (24.13%)**':'**Reconciled object bytes:** **61,548 / 239,698 = 25.6773% (25.68%)**',
'**Unresolved object bytes:** **181,855**':'**Unresolved object bytes:** **178,150**',
}
for a,z in repls.items():
    if a not in prog: raise SystemExit('progress token missing: '+a)
    prog=prog.replace(a,z,1)
prog += '''\n\n## Batch 18 — party/stat checks, transition support, damage and level rollback\n\nBatch 18 adds **3,567 translated instruction bytes** plus **138 directly proven non-code/data bytes**. Cumulative accounting is **60,401 translated + 1,147 non-code = 61,548 / 239,698 = 25.6773% (25.68%)**; unresolved is **178,150 bytes**.\n\nThe sequential code frontier is now `0D550`. This batch closes `0C704..0D53D` and proves the intervening compiler separators plus the data/jump tables at `0C6E0..0C703`, `0CF70..0CF87`, `0D098..0D0AF`, and `0D540..0D54F`. Preserve `0CD44`'s non-consuming `219F7` countdown before the two consuming damage buffers, `0CF88`'s three-track average and encoded divide-error edge, `0D0B0`'s class-indexed six-way table, and `0D1E0`'s tie-preserving random track selection (only a strict unique maximum overrides the initial random track). The next directly decoded routine begins at `0D550`. Known earlier zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
progp.write_text(prog)

addp=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md')
add=addp.read_text()
add += '''\n\n## CODE.2 Batch 18 checkpoint\n\nBatch 18 added **3,567 translated bytes** plus **138 proven non-code/data bytes**. Cumulative accounting is **60,401 translated + 1,147 non-code = 61,548 / 239,698 = 25.6773% (25.68%)**; unresolved is **178,150 bytes**.\n\nThe sequential code frontier is `0D550`. The formerly unresolved `0C6D7..0C703` is now closed: nine padding bytes precede the exact nine-dword data block copied by `0CBD5`. Preserve the `0CD44` countdown-vs-damage-buffer distinction, `0CF88` divide edge, class-table behavior in `0D0B0`, and random-tie behavior in `0D1E0`. The next routine at `0D550` owns the four-entry CS table `0D540..0D54F`, which is already directly proven and accounted.\n'''
addp.write_text(add)
