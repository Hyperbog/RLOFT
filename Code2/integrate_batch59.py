from pathlib import Path
import base64,zlib,re
root=Path('.')
cpp=root/'Code2/Code2.cpp'; ledger=root/'Code2/RESIDUAL_LEDGER.md'; progress=root/'Code2/PROGRESS.md'; addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; payload=root/'Code2/batch59.zb64'
c=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 174489u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 3695u;' in c
assert '// Batch 59: 29CEC..2A113 planar line drawing' not in c
append=zlib.decompress(base64.b64decode(payload.read_text())).decode()
assert 'function_29cec' in append and 'function_2a104' in append and '==1048u);' in append
c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 174489u;','constexpr u32 kVerifiedTranslatedBytes = 175537u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 3695u;','constexpr u32 kVerifiedNonCodeBytes = 3711u;',1)
pat=r'(static_assert\(kVerifiedTranslatedBytes == 159155u \+\s*\(0x273E7u-0x26018u\) \+ 2797u \+ 2032u \+ 3214u \+ 1281u \+ 939u)(\);)'
c,n=re.subn(pat,r'\1 + 1048u\2',c,count=1); assert n==1,n
needle='\n} // namespace code2'; assert c.count(needle)==1
c=c.replace(needle,'\n\n'+append.rstrip()+'\n'+needle,1); cpp.write_text(c)
rows="""29EAA..29EAB | alignment | 2
29F3E..29F3F | alignment | 2
29FE9..29FEB | alignment | 3
29FFA..29FFB | alignment | 2
2A01D..2A01F | alignment | 3
2A075..2A077 | alignment | 3
2A103..2A103 | alignment | 1"""
l=ledger.read_text(); marker='<!-- RESIDUAL-RANGES-END -->'; assert l.count(marker)==1
block=l.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split(marker,1)[0]
assert '29EAA..29EAB |' not in block and '2A103..2A103 |' not in block
l=l.replace(marker,rows+'\n'+marker,1)
l += """\n\nBatch 59 evidence: the Batch-58 overlap `29C80..29D60` and narrow window `29CC0..29D20` were regenerated from immutable CODE.2 and matched the committed checkpoint hashes before forward accounting. Instruction-owned units cover `29CEC..2A113`. The seven listed gaps are exact Watcom XCHG/NOP separators; a full-object direct branch/call-target scan found no incoming target into them, and the apparent dword match for `29FE9` is only the displacement bytes of the unrelated near jump at `17EED`, not an absolute reference. Preserve `29CEC`'s partial-register plane-mask rotation and signed error path, the low-16 sign tests in `29EAC/29F40`, the CX-only shift in `2A020`, and `2A078`'s save/select/restore page sequence before planar mode-1 copying.\n"""; ledger.write_text(l.rstrip()+'\n')
p=progress.read_text()
p=re.sub(r'object%20accounting-[0-9.]+%25-red','object%20accounting-74.78%25-red',p,count=1)
p=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*','**Verified translated instruction bytes:** **175,537**',p,count=1)
p=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*','**Proven non-code/data bytes:** **3,711**',p,count=1)
p=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*','**Reconciled object bytes:** **179,248 / 239,698 = 74.7808% (74.78%)**',p,count=1)
p=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*','**Unresolved object bytes:** **60,450**',p,count=1)
p += """\n\n## CODE.2 Batch 59 checkpoint\n\nBatch 59 added **1,048 translated instruction bytes** plus **16 directly proven alignment bytes**, reconciling through `2A113`. Cumulative accounting is **175,537 translated + 3,711 non-code = 179,248 / 239,698 = 74.7808% (74.78%)**; unresolved is **60,450 bytes**.\n\nThe sequential frontier is now `2A114`. Preserve `29CEC`'s line-drawing fast paths and plane-mask/error rotations, `29EAC/29F40` low-word sign tests with full-register arithmetic, `2A020`'s CX-only count shift, and `2A078`'s page save/select/restore before mode-1 planar copies.\n"""; progress.write_text(p.rstrip()+'\n')
a=addendum.read_text(); a += """\n\n## CODE.2 Batch 59 checkpoint\nBatch 59 adds **1,048 translated bytes** and **16 proven alignment bytes**, bringing cumulative accounting to **175,537 translated + 3,711 non-code = 179,248 / 239,698 = 74.7808% (74.78%)**. The sequential frontier is `2A114`; preserve the partial-register plane-mask and signed Bresenham paths in `29CEC`, low-word sign gates in `29EAC/29F40`, CX-only shift semantics in `2A020`, and page save/select/restore behavior in `2A078`.\n"""; addendum.write_text(a.rstrip()+'\n')
