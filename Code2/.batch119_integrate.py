from pathlib import Path
import os,re
R=Path('Code2');C=R/'Code2.cpp';P=R/'PROGRESS.md';A=R/'CODE2_CONTINUITY_ADDENDUM.md';K=R/'RESUME_CHECKPOINT.md';F=R/'.batch119.fragment.cpp'
def norm(s): return s.rstrip()+"\n"
def integrate():
 c=C.read_text();f=F.read_text().rstrip()+"\n"
 assert 'constexpr u32 kVerifiedTranslatedBytes = 223532u;' in c
 # Batch 118 updated the durable accounting documents but omitted this matching Code2.cpp constant.
 assert 'constexpr u32 kVerifiedNonCodeBytes = 4472u;' in c
 assert '// Batch 119:' not in c
 c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 223532u;','constexpr u32 kVerifiedTranslatedBytes = 223874u;',1)
 c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 4472u;','constexpr u32 kVerifiedNonCodeBytes = 4502u;',1)
 m=re.search(r'(static_assert\(kVerifiedTranslatedBytes ==[\s\S]*?)(\);)',c);assert m
 c=c[:m.start()]+m.group(1).rstrip()+' + 342u'+m.group(2)+c[m.end():]
 mark='\n} // namespace code2\n';assert c.endswith(mark)
 C.write_text(norm(c[:-len(mark)]+'\n\n'+f+mark))
 p=P.read_text()
 p=p.replace('object%20accounting-95.13%25-red','object%20accounting-95.28%25-red',1)
 p=p.replace('**Verified translated instruction bytes:** **223,532**','**Verified translated instruction bytes:** **223,874**',1)
 p=p.replace('**Reconciled object bytes:** **228,034 / 239,698 = 95.1339% (95.13%)**','**Reconciled object bytes:** **228,376 / 239,698 = 95.2766% (95.28%)**',1)
 p=p.replace('**Unresolved object bytes:** **11,664**','**Unresolved object bytes:** **11,322**',1)
 p += '''\n\n## Batch 119 — late runtime helper island `39526..3967B`\n\nBatch 119 adds **342 translated instruction bytes** across four independently bounded runtime helpers: `39526..39588`, `39589..395E1`, `395E2..3962C`, and `3962D..3967B`. `39526` is the ASCII-only length-bounded case-insensitive comparator used by the environment lookup at `352EE`; it preserves unsigned byte subtraction and terminates on the second operand's NUL. `39589` preserves the rounded dynamic-stack request, strict unsigned allocation threshold, global `12650` update, and subsequent runtime callback chain. `395E2` and `3962D` preserve the packed six-byte callback records, priority selection/restart behavior, status-byte writeback, and callback EDX register payload. This batch also repairs the accounting-neutral Batch 118 omission in `Code2.cpp` by synchronizing `kVerifiedNonCodeBytes` from 4,472 to the already-proven/documented 4,502. Cumulative accounting is **223,874 translated + 4,502 non-code = 228,376 / 239,698 = 95.2766% (95.28%)**; unresolved is **11,322 bytes**.\n'''
 P.write_text(norm(p))
 a=A.read_text();a += '''\n\n## CODE.2 Batch 119 checkpoint\n\nBatch 119 reconciles the late runtime island `39526..3967B`: **342 translated bytes**. Preserve `39526`'s ASCII-only folding and unsigned-byte difference; `39589`'s strict unsigned stack-allocation threshold and `12650` update; and the packed six-byte callback-table selection/restart semantics in `395E2/3962D`, including the full EDX payload passed through indirect callbacks. The batch also repairs the accounting-neutral Batch 118 omission in `Code2.cpp`: `kVerifiedNonCodeBytes` now matches the already-proven/documented **4,502** bytes. Cumulative accounting is **223,874 translated + 4,502 non-code = 228,376 / 239,698 = 95.2766% (95.28%)**; unresolved is **11,322 bytes**. Continue from the residual complement rather than assuming sequential ownership after `3967B`, because `3967C` and later runtime helpers already have integrated ownership.\n''';A.write_text(norm(a))
def verify():
 c=C.read_text();p=P.read_text()
 assert 'constexpr u32 kVerifiedTranslatedBytes = 223874u;' in c
 assert 'constexpr u32 kVerifiedNonCodeBytes = 4502u;' in c
 for x in ['Runtime119Boundary','function_39526','function_39589','function_395e2','function_3962d','static_assert(0x3967Cu-0x3962Du==79u);']:
  assert x in c,x
 assert '228,376 / 239,698 = 95.2766% (95.28%)' in p
 assert '**Unresolved object bytes:** **11,322**' in p
def checkpoint():
 sha=os.environ['BATCH_SHA']
 K.write_text(norm(f'''# CODE.2 resume checkpoint\n\nDurable handoff anchor; live repository/binary evidence wins.\n\n## Last committed substantive decompilation state\n- Last substantive batch commit: `{sha}` — `Integrate CODE.2 decompilation batch 119`.\n- Verified translated instruction bytes: **223,874**.\n- Proven non-code/data bytes: **4,502**.\n- Reconciled object bytes: **228,376 / 239,698 = 95.2766% (95.28%)**.\n- Unresolved object bytes: **11,322**.\n- Last integrated ownership fold: `39526..3967B`.\n- Next work: select another independently proven range from the unresolved complement; `3967C` and later runtime helpers already have ownership.\n\n## Immutable identity\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n\n## Mandatory overlap anchor for next resume\n- Window `[0x394E0,0x395B0)` — 208 bytes; SHA-256 `c9ac1da39ecdfe4cbde0e42bf05cc75bf1c3330e5a996e204e31beaadfd6bfce`.\n- Narrow `[0x39500,0x39590)` — 144 bytes; SHA-256 `cc29d7509bd62c692e4d2562e4f80f11f31a3c577eb50f02b5c5372ade3cfa3d`.\n- The overlap bytes are verification only and must not be recounted.\n\n## Batch 119 interpretation\n- `39526..39588`: ASCII-only length-bounded case-insensitive comparator used by `352EE`.\n- `39589..395E1`: dynamic-stack/runtime initialization helper; strict unsigned threshold is preserved.\n- `395E2..3962C`: four-record packed callback-table drain, selecting the lowest eligible priority and restarting after status writeback.\n- `3962D..3967B`: one-record callback-table drain with lower/upper priority gating and preserved full EDX callback payload.\n- Batch 118's 30-byte non-code proof was already in the ledger/docs; Batch 119 synchronizes the previously stale `Code2.cpp` constant to 4,502.\n'''))
if __name__=='__main__':
 import sys
 {'integrate':integrate,'verify':verify,'checkpoint':checkpoint}[sys.argv[1]]()
