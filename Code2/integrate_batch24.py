from pathlib import Path
import re
root=Path('.')
cppp=root/'Code2/Code2.cpp'
ledgerp=root/'Code2/RESIDUAL_LEDGER.md'
progp=root/'Code2/PROGRESS.md'
contp=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
inc=(root/'Code2/batch24.inc.cpp').read_text()
cpp=cppp.read_text()
old_constants='''constexpr u32 kVerifiedTranslatedBytes = 78245u;\nconstexpr u32 kVerifiedNonCodeBytes = 1459u;'''
new_constants='''constexpr u32 kVerifiedTranslatedBytes = 81609u;\nconstexpr u32 kVerifiedNonCodeBytes = 1499u;'''
assert old_constants in cpp
cpp=cpp.replace(old_constants,new_constants,1)
old_assert='''static_assert(kVerifiedTranslatedBytes == 75071u +\n              (0x110B4u-0x10FACu) + (0x1118Bu-0x110B4u) +\n              (0x11268u-0x1118Cu) + (0x112EAu-0x11268u) +\n              (0x113A9u-0x112ECu) + (0x113F2u-0x113ACu) +\n              (0x1162Eu-0x11404u) + (0x117A0u-0x11630u) +\n              (0x11AD8u-0x117A0u) + (0x11C20u-0x11AD8u) +\n              (0x11C2Cu-0x11C20u));'''
new_assert='''static_assert(kVerifiedTranslatedBytes == 78245u +\n              (0x11FAEu-0x11C3Cu) + (0x1202Au-0x11FB0u) +\n              (0x12097u-0x1202Cu) + (0x120BBu-0x12098u) +\n              (0x12125u-0x120BCu) + (0x12165u-0x12128u) +\n              (0x1246Fu-0x12180u) + (0x12723u-0x12470u) +\n              (0x12986u-0x12724u));'''
assert old_assert in cpp
cpp=cpp.replace(old_assert,new_assert,1)
marker='\n} // namespace code2\n'
assert cpp.endswith(marker)
cpp=cpp[:-len(marker)] + inc + marker
cppp.write_text(cpp)

ledger=ledgerp.read_text()
res='''11FAE..11FAF | alignment | 2\n1202A..1202B | alignment | 2\n12097..12097 | alignment | 1\n120BB..120BB | alignment | 1\n12125..12127 | alignment | 3\n12165..12167 | alignment | 3\n12168..1217F | table | 24\n1246F..1246F | alignment | 1\n12723..12723 | alignment | 1\n12986..12987 | alignment | 2\n'''
end='<!-- RESIDUAL-RANGES-END -->'
assert end in ledger and '11FAE..11FAF' not in ledger
ledger=ledger.replace(end,res+end,1)
ledger=ledger.replace('**Total proven non-code/data:** **1,459 bytes**.','**Total proven non-code/data:** **1,499 bytes**.',1)
ledger += '''\n\nBatch 24 evidence: `11FAE..11FAF`, `1202A..1202B`, and `12986..12987` decode as `MOV EAX,EAX`; `12097`, `120BB`, `1246F`, and `12723` are explicit `NOP` separators; `12125..12127` and `12165..12167` decode as `LEA EAX,[EAX+0]`. A full object-1 direct relative control-flow target scan found no incoming target to any Batch 24 separator. `12168..1217F` is the exact six-entry CS dword table consumed at `121ED`, with targets `121F5,1220C,122B0,12282,12299,1226B` for item types `3..8`. The older zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledgerp.write_text(ledger)

prog=progp.read_text()
prog=prog.replace('object%20accounting-33.25%25-red','object%20accounting-34.67%25-red',1)
prog=prog.replace('**Verified translated instruction bytes:** **78,245**','**Verified translated instruction bytes:** **81,609**',1)
prog=prog.replace('**Proven non-code/data bytes:** **1,459**','**Proven non-code/data bytes:** **1,499**',1)
prog=prog.replace('**Reconciled object bytes:** **79,704 / 239,698 = 33.2518% (33.25%)**','**Reconciled object bytes:** **83,108 / 239,698 = 34.6720% (34.67%)**',1)
prog=prog.replace('**Unresolved object bytes:** **159,994**','**Unresolved object bytes:** **156,590**',1)
progp.write_text(prog)

cont=contp.read_text()
cont += '''\n\n## CODE.2 Batch 24 checkpoint\n\nBatch 24 added **3,364 translated bytes** plus **40 proven non-code/data bytes**. Cumulative accounting is **81,609 translated + 1,499 non-code = 83,108 / 239,698 = 34.6720% (34.67%)**; unresolved is **156,590 bytes**.\n\nThe sequential frontier now reaches `12985`; `12986..12987` is proven `MOV EAX,EAX` alignment and the next routine begins at `12988`. Preserve `11C3C`'s encoded grid geometry, same-container rejection, capacity arithmetic, and byte-shift variant semantics; `12180`'s six-way type table plus empty-cursor hitbox scan and slot-0F container close/reopen behavior; `12470`'s complete 248h actor-block swap, four-byte panel-state swap, and 3-bit actor-index rewrite in both record families; and `12724`'s sparse status-bit scan, which stops after fourteen displayed statuses or bit `02000000h`, followed by its two distinct modal wait phases. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
contp.write_text(cont)
print('Batch 24 integrated')
