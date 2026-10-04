from pathlib import Path
import base64,zlib,re

root=Path('.')
cpp=root/'Code2/Code2.cpp'
ledger=root/'Code2/RESIDUAL_LEDGER.md'
progress=root/'Code2/PROGRESS.md'
addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
payload=root/'Code2/batch54.zb64'

c=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 164226u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 3457u;' in c
assert 'function_273f0' not in c
append=zlib.decompress(base64.b64decode(payload.read_text())).decode()
assert '==2797u);' in append and 'function_27ecc' in append
c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 164226u;','constexpr u32 kVerifiedTranslatedBytes = 167023u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 3457u;','constexpr u32 kVerifiedNonCodeBytes = 3537u;',1)
needle='\n} // namespace code2'
assert c.count(needle)==1
c=c.replace(needle,'\n\n'+append.rstrip()+'\n'+needle,1)
cpp.write_text(c)

rows='''273E7..273EF | alignment | 9
274A2..274A3 | alignment | 2
2757E..2757F | alignment | 2
275E2..275E3 | alignment | 2
27635..27637 | alignment | 3
27739..2773B | alignment | 3
277E2..277EF | alignment | 14
27842..27843 | alignment | 2
27891..27893 | alignment | 3
27913..2791F | alignment | 13
279CE..279CF | alignment | 2
27A54..27A5F | alignment | 12
27A75..27A77 | alignment | 3
27A8E..27A8F | alignment | 2
27B83..27B83 | alignment | 1
27C6E..27C6F | alignment | 2
27D1C..27D1F | alignment | 4
27F23..27F23 | alignment | 1'''
l=ledger.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert l.count(marker)==1
block=l.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split(marker,1)[0]
assert '273E7..273EF |' not in block
l=l.replace(marker,rows+'\n'+marker,1)
l += '''\n\nBatch 54 evidence: the mandatory overlap revalidation reproduced the recorded `273A0..27480` and `273E0..27420` SHA-256 anchors exactly, proving Batch 53 ends at the `RET` at `273E6`. `273E7..273EF` is nine zero bytes with no incoming branch/call target and no absolute dword reference anywhere in object 1. The remaining Batch-54 separator rows decode as Watcom `XCHG EBX,EBX`, `NOP`, or zero-fill alignment between independently called routines; a full-object target/reference scan found no incoming target or embedded absolute reference to any listed separator byte.\n\nThe translated `273F0..27F22` ownership units are instruction-bounded directly from immutable CODE.2. Preserve the VGA register width/port ordering, plane-mask rotations, `27694` opposite-page copy split, `2773C` mode-specific sparse byte overlays, interrupt-frame SS/ESP swap in `27894`, filename retry on DOS error `50h`, stream cursor increments in `27A60/27A78`, PackBits-style row expansion in `27BD8`, the four-dword-at-a-time zero-detect behavior of `27C70`, and the one-time multimedia setup/teardown state at `EE40/EE6C/EE68`.\n'''
ledger.write_text(l.rstrip()+'\n')

p=progress.read_text()
p=re.sub(r'object%20accounting-[0-9.]+%25-red','object%20accounting-71.16%25-red',p,count=1)
p=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*','**Verified translated instruction bytes:** **167,023**',p,count=1)
p=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*','**Proven non-code/data bytes:** **3,537**',p,count=1)
p=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*','**Reconciled object bytes:** **170,560 / 239,698 = 71.1562% (71.16%)**',p,count=1)
p=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*','**Unresolved object bytes:** **69,138**',p,count=1)
p += '''\n\n## CODE.2 Batch 54 checkpoint\n\nBatch 54 added **2,797 translated instruction bytes** plus **80 directly proven alignment/padding bytes**, reconciling the prior nine-byte frontier gap and the complete low-level block `273F0..27F22`. Cumulative accounting is **167,023 translated + 3,537 non-code = 170,560 / 239,698 = 71.1562% (71.16%)**; unresolved is **69,138 bytes**.\n\nThe sequential frontier now reaches `27F23`; `27F23` is proven `NOP` alignment and the next instruction-owned routine begins at `27F24` with its nonstandard register-save prologue. Preserve `273F0`'s plane-mask rotation plus ADC-driven VGA-byte advance, `274A4`'s unsigned multiply/divide followed by signed-16 wrap, `27580/275E4/27638/27694`'s exact read-map/write-mask widths, `2773C`'s sparse mode overlays, the `27894` interrupt frame's SS/ESP swap and reentry latch, `279D0`'s DOS error-50h filename retry, `27AEC/27BD8`'s encoded stream-cursor ownership, `27C70`'s dword-at-a-time zero detection, and `27D20/27ECC`'s one-time multimedia init/teardown state. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.\n'''
progress.write_text(p.rstrip()+'\n')

a=addendum.read_text()
a += '''\n\n## CODE.2 Batch 54 checkpoint\nBatch 54 adds **2,797 translated bytes** and **80 proven alignment/padding bytes**, bringing cumulative accounting to **167,023 translated + 3,537 non-code = 170,560 / 239,698 = 71.1562% (71.16%)**. The resume-overlap hashes from Batch 53 were reproduced exactly before any forward accounting. The sequential frontier is `27F24`; preserve the low-level VGA plane/register ordering, `27894` interrupt stack/selector swap, screenshot filename retry semantics, stream cursor/RLE ownership, `27C70` dword zero-detect loads, and `27D20/27ECC` multimedia state lifecycle.\n'''
addendum.write_text(a.rstrip()+'\n')
