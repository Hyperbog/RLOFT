from pathlib import Path
import base64,zlib,re
root=Path('.')
cpp=root/'Code2/Code2.cpp'; ledger=root/'Code2/RESIDUAL_LEDGER.md'; progress=root/'Code2/PROGRESS.md'; addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; payload=root/'Code2/batch60.zb64'
c=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 175537u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 3711u;' in c
assert not re.search(r'^void function_2a114\(',c,re.M)
append=zlib.decompress(base64.b64decode(payload.read_text())).decode()
assert 'function_2a114' in append and 'function_2a584' in append and '==1427u);' in append
assert '*dg(m,0x2A4F6u)=d;' in append and 'store_le32(dg(m,0x2A60Bu),table);' in append
c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 175537u;','constexpr u32 kVerifiedTranslatedBytes = 176964u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 3711u;','constexpr u32 kVerifiedNonCodeBytes = 3720u;',1)
pat=r'(static_assert\(kVerifiedTranslatedBytes == 159155u \+\s*\(0x273E7u-0x26018u\) \+ 2797u \+ 2032u \+ 3214u \+ 1281u \+ 939u \+ 1048u)(\);)'
c,n=re.subn(pat,r'\1 + 1427u\2',c,count=1); assert n==1,n
needle='\n} // namespace code2'; assert c.count(needle)==1
c=c.replace(needle,'\n\n'+append.rstrip()+'\n'+needle,1); cpp.write_text(c)
rows="""2A177..2A177 | alignment | 1
2A1E5..2A1E7 | alignment | 3
2A256..2A257 | alignment | 2
2A305..2A307 | alignment | 3"""
l=ledger.read_text(); marker='<!-- RESIDUAL-RANGES-END -->'; assert l.count(marker)==1
block=l.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split(marker,1)[0]
assert '2A177..2A177 |' not in block and '2A305..2A307 |' not in block
l=l.replace(marker,rows+'\n'+marker,1)
l += """\n\nBatch 60 evidence: the mandatory Batch-59 overlap `2A0B0..2A190` and its narrow window were regenerated from immutable CODE.2 and matched the checkpoint hashes before forward accounting. Instruction ownership covers nine complete routines at `2A114..2A6AF`; the four listed separator ranges are Watcom NOP/XCHG alignment with no incoming branch/call target. Preserve the plane-major read/write asymmetry in `2A114/2A178`, 16-bit swap-counter wrap in `2A258`, self-modifying immediate bytes `2A4F6/2A54F` in `2A478`, self-modifying table displacements `2A60B/2A672` in `2A584`, and BL-only transparent row-byte counts in the blitter family.\n"""; ledger.write_text(l.rstrip()+'\n')
p=progress.read_text()
p=re.sub(r'object%20accounting-[0-9.]+%25-red','object%20accounting-75.38%25-red',p,count=1)
p=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*','**Verified translated instruction bytes:** **176,964**',p,count=1)
p=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*','**Proven non-code/data bytes:** **3,720**',p,count=1)
p=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*','**Reconciled object bytes:** **180,684 / 239,698 = 75.3799% (75.38%)**',p,count=1)
p=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*','**Unresolved object bytes:** **59,014**',p,count=1)
p += """\n\n## CODE.2 Batch 60 checkpoint\n\nBatch 60 added **1,427 translated instruction bytes** plus **9 directly proven alignment bytes**, reconciling through `2A6AF`. Cumulative accounting is **176,964 translated + 3,720 non-code = 180,684 / 239,698 = 75.3799% (75.38%)**; unresolved is **59,014 bytes**.\n\nThe sequential frontier is now `2A6B0`, the next independent clipped blitter. Preserve plane-major transfer ownership, partial-register loop counts, and the writable-code immediates/displacements in `2A478` and `2A584`.\n"""; progress.write_text(p.rstrip()+'\n')
a=addendum.read_text(); a += """\n\n## CODE.2 Batch 60 checkpoint\nBatch 60 adds **1,427 translated bytes** and **9 proven alignment bytes**, bringing cumulative accounting to **176,964 translated + 3,720 non-code = 180,684 / 239,698 = 75.3799% (75.38%)**. The sequential frontier is `2A6B0`; preserve the plane-major `2A114/2A178` asymmetry, 16-bit wrap in `2A258`, transparent BL-only byte counts, and self-modifying code operands at `2A4F6`, `2A54F`, `2A60B`, and `2A672`.\n"""; addendum.write_text(a.rstrip()+'\n')
