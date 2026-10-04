from pathlib import Path
import base64,zlib,re
root=Path('.')
cpp=root/'Code2/Code2.cpp'; ledger=root/'Code2/RESIDUAL_LEDGER.md'; progress=root/'Code2/PROGRESS.md'; addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; payload=root/'Code2/batch57.zb64'
c=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 172269u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 3651u;' in c
assert 'function_29414' not in c
append=zlib.decompress(base64.b64decode(payload.read_text())).decode()
assert 'function_29414' in append and 'function_298f0' in append and '== 1281u);' in append
c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 172269u;','constexpr u32 kVerifiedTranslatedBytes = 173550u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 3651u;','constexpr u32 kVerifiedNonCodeBytes = 3678u;',1)
pat=r'(static_assert\(kVerifiedTranslatedBytes == 159155u \+\s*\(0x273E7u-0x26018u\) \+ 2797u \+ 2032u \+ 3214u)(\);)'
c,n=re.subn(pat,r'\1 + 1281u\2',c,count=1); assert n==1,n
needle='\n} // namespace code2'; assert c.count(needle)==1
c=c.replace(needle,'\n\n'+append.rstrip()+'\n'+needle,1); cpp.write_text(c)
rows="""29563..29563 | alignment | 1
296BF..296BF | alignment | 1
29735..29737 | alignment | 3
2975B..2975B | alignment | 1
2978A..2978B | alignment | 2
297CE..297CF | alignment | 2
29802..29803 | alignment | 2
298EF..298EF | alignment | 1
29922..2992F | padding | 14"""
l=ledger.read_text(); marker='<!-- RESIDUAL-RANGES-END -->'; assert l.count(marker)==1
block=l.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split(marker,1)[0]
assert '29563..29563 |' not in block and '29922..2992F |' not in block
l=l.replace(marker,rows+'\n'+marker,1)
l += """\n\nBatch 57 evidence: mandatory Batch-56 overlap hashes were reproduced from immutable CODE.2 before forward work. The instruction-owned fold is `29414..29921`. The eight short inter-routine separators decode as NOP, MOV EAX,EAX, or LEA EAX,[EAX]; `29922..2992F` is fourteen zero bytes. A full object-1 direct branch/call-target scan plus exact dword scan for both object-relative and +10000h virtual addresses found no incoming target or absolute reference into any of these 27 bytes. Preserve the two clip paths' partial-register/global side effects, signed clipping tests, `296C0` signed truncation-to-zero division-by-four sequence, the 16-byte `29738` REP copy, and EFD4 scratch-depth increment/decrement ownership in the wrapper family.\n"""
ledger.write_text(l.rstrip()+'\n')
p=progress.read_text()
p=re.sub(r'object%20accounting-[0-9.]+%25-red','object%20accounting-73.94%25-red',p,count=1)
p=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*','**Verified translated instruction bytes:** **173,550**',p,count=1)
p=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*','**Proven non-code/data bytes:** **3,678**',p,count=1)
p=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*','**Reconciled object bytes:** **177,228 / 239,698 = 73.9380% (73.94%)**',p,count=1)
p=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*','**Unresolved object bytes:** **62,470**',p,count=1)
p += """\n\n## CODE.2 Batch 57 checkpoint\n\nBatch 57 added **1,281 translated instruction bytes** plus **27 directly proven alignment/padding bytes**, reconciling `29414..2992F`. Cumulative accounting is **173,550 translated + 3,678 non-code = 177,228 / 239,698 = 73.9380% (73.94%)**; unresolved is **62,470 bytes**.\n\nThe sequential frontier is now `29930`, the independent VGA mode/setup prologue. Preserve `29414/29564` clipping globals `EF69..EF6F`, their full-vs-clipped return codes 2/1/0, `296C0`'s signed truncation-to-zero quartering and BL-only alignment effect, `29738`'s exact 16-byte copy, and the EFD4 nested scratch-record ownership in `2978C/29804/2986C/298B4`.\n"""
progress.write_text(p.rstrip()+'\n')
a=addendum.read_text(); a += """\n\n## CODE.2 Batch 57 checkpoint\nBatch 57 adds **1,281 translated bytes** and **27 proven alignment/padding bytes**, bringing cumulative accounting to **173,550 translated + 3,678 non-code = 177,228 / 239,698 = 73.9380% (73.94%)**. The sequential frontier is `29930`; preserve the clipped/full renderer distinction, `EF69..EF6F` low-word global writes, `296C0` signed division behavior, and EFD4 scratch-depth balancing.\n"""; addendum.write_text(a.rstrip()+'\n')
