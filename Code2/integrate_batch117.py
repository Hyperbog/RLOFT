from pathlib import Path

cpp=Path('Code2/Code2.cpp')
text=cpp.read_text()
frag=Path('Code2/batch117_fragment.cpp').read_text()
marker='// -----------------------------------------------------------------------------\n// 35F88..369A6 — Watcom timer/event subsystem.'
assert marker in text
text=text.replace('constexpr u32 kVerifiedTranslatedBytes = 222129u;','constexpr u32 kVerifiedTranslatedBytes = 223532u;',1)
text=text.replace('constexpr u32 kVerifiedNonCodeBytes = 4471u;','constexpr u32 kVerifiedNonCodeBytes = 4472u;',1)
old_tail='+ 488u + 89u + 309u);'
assert old_tail in text
text=text.replace(old_tail,'+ 488u + 89u + 309u + 1403u);',1)
text=text.replace(marker,frag+'\n// 35F87 is the single zero alignment byte before 35F88.\nstatic_assert(0x35F88u-0x35F87u==1u);\n\n'+marker,1)
cpp.write_text(text)

progress=Path('Code2/PROGRESS.md')
p=progress.read_text()
p=p.replace('object%20accounting-94.54%25-red','object%20accounting-95.12%25-red',1)
p=p.replace('**Verified translated instruction bytes:** **222,129**','**Verified translated instruction bytes:** **223,532**',1)
p=p.replace('**Proven non-code/data bytes:** **4,471**','**Proven non-code/data bytes:** **4,472**',1)
p=p.replace('**Reconciled object bytes:** **226,600 / 239,698 = 94.5356% (94.54%)**','**Reconciled object bytes:** **228,004 / 239,698 = 95.1214% (95.12%)**',1)
p=p.replace('**Unresolved object bytes:** **13,098**','**Unresolved object bytes:** **11,694**',1)
p += '''\n\n## Batch 117 — 35A0C..35F87 large loader\n\nBatch 117 adds **1,403 translated instruction bytes** at `35A0C..35F86` plus the **1-byte proven alignment** at `35F87`. Cumulative accounting is **223,532 translated + 4,472 non-code = 228,004 / 239,698 = 95.1214% (95.12%)**; unresolved is **11,694 bytes**.\n\nThe mandatory resume overlap `[35990,35A70)` re-hashed to `4470ab5305088033d53eca6b57aa66c2ea04bc2eecbd1688058daf896f2aafaa`, exactly matching the Batch 116 checkpoint. `35A0C` is therefore confirmed as the next independent entry. The loader preserves the raw `02D0h` Watcom frame because the binary deliberately aliases its byte/word/dword fields: notably `35A1C` seeds a dword from `35874`, while `35AE1` overwrites only its low word. It also preserves the flag-1 memory/file dual path in `35878`, optional flag-4 allocation, 16-byte destination alignment gate, the two local dword tables, and the encoded relocation record validation before guest dword stores.\n\n`35F87` is a single zero alignment byte and `35F88` remains the already integrated timer/event entry. The sequential runtime frontier is therefore closed through `35F87`; remaining work is in unresolved islands elsewhere in object 1.\n'''
progress.write_text(p)

ledger=Path('Code2/RESIDUAL_LEDGER.md')
l=ledger.read_text()
l += '''\n\n## Batch 117 additions\n\n- `35F87..35F87` — **1 byte**, zero alignment separating the complete `35A0C..35F86` loader from the independently called `35F88` timer/event entry. Boundary is proven by fresh disassembly of both complete adjacent routines.\n\nCumulative proven non-code/data: **4,472 bytes**.\n'''
ledger.write_text(l)

add=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md')
a=add.read_text()
a += '''\n\n## CODE.2 Batch 117 checkpoint\n\nBatch 117 integrated `35A0C..35F86` (**1,403 translated bytes**) and classified `35F87` as **1 byte of alignment**. Cumulative accounting is **223,532 translated + 4,472 non-code = 228,004 / 239,698 = 95.1214% (95.12%)**; unresolved is **11,694 bytes**. The Batch 116 overlap hash matched exactly before forward work. Preserve the large loader's raw-frame aliasing, flag-1 memory/file split, flag-4 allocation path, 16-byte alignment rule, local base tables, and relocation validation/store ordering. `35F88` is already integrated; continue by closing remaining unresolved islands rather than double-counting the runtime tail.\n'''
add.write_text(a)
