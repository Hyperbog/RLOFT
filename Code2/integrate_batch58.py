from pathlib import Path
import base64,zlib,re
root=Path('.')
cpp=root/'Code2/Code2.cpp'; ledger=root/'Code2/RESIDUAL_LEDGER.md'; progress=root/'Code2/PROGRESS.md'; addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; payload=root/'Code2/batch58.zb64'
c=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 173550u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 3678u;' in c
assert 'u32 function_29930(' not in c
append=zlib.decompress(base64.b64decode(payload.read_text())).decode()
assert 'function_29930' in append and 'function_29ca0' in append and '==939u);' in append
c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 173550u;','constexpr u32 kVerifiedTranslatedBytes = 174489u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 3678u;','constexpr u32 kVerifiedNonCodeBytes = 3695u;',1)
pat=r'(static_assert\(kVerifiedTranslatedBytes == 159155u \+\s*\(0x273E7u-0x26018u\) \+ 2797u \+ 2032u \+ 3214u \+ 1281u)(\);)'
c,n=re.subn(pat,r'\1 + 939u\2',c,count=1); assert n==1,n
needle='\n} // namespace code2'; assert c.count(needle)==1
c=c.replace(needle,'\n\n'+append.rstrip()+'\n'+needle,1); cpp.write_text(c)
rows="""29A9F..29A9F | alignment | 1
29AA2..29AA3 | alignment | 2
29B19..29B1B | alignment | 3
29B33..29B36 | table | 4
29B3E..29B3F | alignment | 2
29C9E..29C9F | alignment | 2
29CE9..29CEB | alignment | 3"""
l=ledger.read_text(); marker='<!-- RESIDUAL-RANGES-END -->'; assert l.count(marker)==1
block=l.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split(marker,1)[0]
assert '29A9F..29A9F |' not in block and '29B33..29B36 |' not in block
l=l.replace(marker,rows+'\n'+marker,1)
l += """\n\nBatch 58 evidence: Batch-57 overlap at `298D0..299B0` was regenerated from immutable CODE.2 and matched the checkpoint hashes before forward accounting. The instruction-owned units cover `29930..29CE8`, including the address-distinct two-byte tail wrapper at `29AA0` and shared executable epilogue `29B37..29B3D`. `29B33..29B36` is the exact four-byte CS table referenced directly by `29B24`; the remaining listed bytes are Watcom NOP/XCHG alignment with no incoming target or independent absolute reference. Preserve VGA latch reads before masked writes, `DEC DI` low-word-only stride semantics in `29B40`, its AL-only middle-byte counter in the nonzero raster-op path, and the 192-byte retrace-bounded palette chunks in `29CA0`.\n"""; ledger.write_text(l.rstrip()+'\n')
p=progress.read_text()
p=re.sub(r'object%20accounting-[0-9.]+%25-red','object%20accounting-74.34%25-red',p,count=1)
p=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*','**Verified translated instruction bytes:** **174,489**',p,count=1)
p=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*','**Proven non-code/data bytes:** **3,695**',p,count=1)
p=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*','**Reconciled object bytes:** **178,184 / 239,698 = 74.3369% (74.34%)**',p,count=1)
p=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*','**Unresolved object bytes:** **61,514**',p,count=1)
p += """\n\n## CODE.2 Batch 58 checkpoint\n\nBatch 58 added **939 translated instruction bytes** plus **17 directly proven alignment/table bytes**, reconciling through `29CEB`. Cumulative accounting is **174,489 translated + 3,695 non-code = 178,184 / 239,698 = 74.3369% (74.34%)**; unresolved is **61,514 bytes**.\n\nThe sequential frontier is now `29CEC`, the next independent raster primitive. Preserve `29930`'s BIOS/VGA register ordering and descriptor writes, the direct `29AA0 -> 29B1C` wrapper, the unbounded CS-byte lookup at `29B33`, `29B40`'s shared `29B37` epilogue plus partial-register mask/counter behavior, and `29CA0`'s retrace wait before every <=192-byte DAC chunk.\n"""; progress.write_text(p.rstrip()+'\n')
a=addendum.read_text(); a += """\n\n## CODE.2 Batch 58 checkpoint\nBatch 58 adds **939 translated bytes** and **17 proven alignment/table bytes**, bringing cumulative accounting to **174,489 translated + 3,695 non-code = 178,184 / 239,698 = 74.3369% (74.34%)**. The sequential frontier is `29CEC`; preserve the VGA register/BIOS ordering, `29B33` direct CS table, `29B40` latch/mask and partial-register behavior, and `29CA0` retrace-bounded DAC streaming.\n"""; addendum.write_text(a.rstrip()+'\n')
