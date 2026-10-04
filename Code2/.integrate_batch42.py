from pathlib import Path
import base64,zlib
root=Path('Code2')
code_p=root/'Code2.cpp'; payload_p=root/'batch42.zb64'
progress_p=root/'PROGRESS.md'; add_p=root/'CODE2_CONTINUITY_ADDENDUM.md'
code=code_p.read_text()
frag=zlib.decompress(base64.b64decode(payload_p.read_text().strip())).decode().rstrip()+"\n\n"
assert code.rstrip().endswith('} // namespace code2')
assert 'void function_1ce68(Machine& m,Application42Boundary& b) noexcept' not in code
code=code.rsplit('} // namespace code2',1)[0]+frag+'} // namespace code2\n'
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 122860u;',
                  'constexpr u32 kVerifiedTranslatedBytes = 128548u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 121232u +\n              (0x1C924u-0x1C77Cu) + (0x1C99Fu-0x1C924u) +\n              (0x1CD47u-0x1CA2Cu) + (0x1CE66u-0x1CD48u));'''
new='''static_assert(kVerifiedTranslatedBytes == 122860u +\n              (0x1E4A0u-0x1CE68u));'''
assert old in code
code=code.replace(old,new,1)
code_p.write_text(code)

progress=progress_p.read_text()
progress=progress.replace('object%20accounting-52.28%25-red','object%20accounting-54.65%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **122,860**','**Verified translated instruction bytes:** **128,548**',1)
progress=progress.replace('**Reconciled object bytes:** **125,316 / 239,698 = 52.2808% (52.28%)**','**Reconciled object bytes:** **131,004 / 239,698 = 54.6538% (54.65%)**',1)
progress=progress.replace('**Unresolved object bytes:** **114,382**','**Unresolved object bytes:** **108,694**',1)
progress=progress.rstrip()+'''\n\n## Batch 42 — command/event dispatcher\n\nAdded **5,688 translated instruction bytes** and no new non-code/data bytes. The complete `1CE68..1E49F` owner is reconciled; the next routine begins directly at `1E4A0`.\n\nFidelity notes: the original large unsigned compare tree over `BF44` is expressed as a switch without changing command behavior. All `1CD48` selector argument tuples and ordering are retained, including the live `BF44` reread on commands `11h/12h`, event-bit gates and mutations, command `2Ch`'s zero-valued live-ESI fallback criterion, command `33h`'s `14A60(111Ch)` modal actor selection, item predicates in `2Fh/4Fh/50h`, and the low-32 `ADD EAX,EAX` random text choice used by `4Ch/60h/61h/62h`. The special command `270Fh` is also preserved.\n'''
progress_p.write_text(progress)

add=add_p.read_text().rstrip()+'''\n\n## CODE.2 Batch 42 checkpoint\n\nBatch 42 intentionally exceeds the nominal 2–3 KB target because `1CE68..1E49F` is one compiler-owned command dispatcher with a shared selector tail at `1E488`; splitting it would create false ownership boundaries. It added **5,688 translated bytes** and no new non-code/data bytes. Cumulative accounting is **128,548 translated + 2,456 non-code = 131,004 / 239,698 = 54.6538% (54.65%)**; unresolved is **108,694 bytes**.\n\nThe sequential frontier now reaches `1E49F`; the next routine begins directly at `1E4A0`. Preserve the selector-call ordering, live `BF44` reread in the `11h/12h` path, direct byte event mutations (`22211h|=04h`, `22213h|=10h`, `22217h|=01h`), command `2Ch`'s second criterion of zero, inventory-test ordering in `2Fh`, and random text calculation `(r+r)>>15 + 6Fh`. Semantic audit remains not started.\n'''
add_p.write_text(add)
