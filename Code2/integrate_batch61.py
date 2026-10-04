from pathlib import Path
import base64,zlib,re
root=Path('.')
cpp=root/'Code2/Code2.cpp'; ledger=root/'Code2/RESIDUAL_LEDGER.md'; progress=root/'Code2/PROGRESS.md'; addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; payload=root/'Code2/batch61.zb64'
c=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 176964u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 3720u;' in c
legacy_start='\nstatic void g2_clipped_sprite('
legacy_end='static_assert(0x2AA9Bu-0x2A8A0u==507u);\n'
assert c.count(legacy_start)==1 and c.count(legacy_end)==1
start=c.index(legacy_start)
end=c.index(legacy_end,start)+len(legacy_end)
c=c[:start]+'\n'+c[end:]
assert not re.search(r'^void function_2a6b0\(',c,re.M)
append=zlib.decompress(base64.b64decode(payload.read_text())).decode()
assert 'function_2a6b0' in append and 'function_2ac10' in append and '==1453u);' in append
assert '*dg(m,0x2A816u)=transparent;' in append and '*dg(m,0x2AA12u)=static_cast<u8>(delta);' in append
c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 176964u;','constexpr u32 kVerifiedTranslatedBytes = 178417u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 3720u;','constexpr u32 kVerifiedNonCodeBytes = 3755u;',1)
pat=r'(static_assert\(kVerifiedTranslatedBytes == 159155u \+\s*\(0x273E7u-0x26018u\) \+ 2797u \+ 2032u \+ 3214u \+ 1281u \+ 939u \+ 1048u \+ 1427u)(\);)'
c,n=re.subn(pat,r'\1 + 1453u\2',c,count=1); assert n==1,n
needle='\n} // namespace code2'; assert c.count(needle)==1
c=c.replace(needle,'\n\n'+append.rstrip()+'\n'+needle,1); cpp.write_text(c)
rows="""2A89F..2A89F | alignment | 1
2AA9B..2AA9B | alignment | 1
2AAEA..2AAEB | alignment | 2
2AB36..2AB37 | alignment | 2
2AB95..2AB97 | alignment | 3
2ABDB..2ABDB | alignment | 1
2ABF3..2ABF3 | alignment | 1
2AC68..2AC6F | padding | 8
2AC70..2AC7F | table | 16"""
l=ledger.read_text(); marker='<!-- RESIDUAL-RANGES-END -->'; assert l.count(marker)==1
block=l.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split(marker,1)[0]
assert '2A89F..2A89F |' not in block and '2AC70..2AC7F |' not in block
l=l.replace(marker,rows+'\n'+marker,1)
l += """\n\nBatch 61 evidence: the mandatory Batch-60 overlap `2A650..2A730` and narrow window were regenerated from immutable CODE.2 and matched the checkpoint hashes before forward accounting. Instruction ownership covers `2A6B0..2AC67`. `2AC70..2AC7F` is a directly proven 16-byte table: `2C2A8` loads its exact address and copies four dwords. Only `2AC68..2AC6F` is zero-fill padding; the remaining listed bytes are Watcom NOP/XCHG separators with no incoming control-flow target. Preserve writable-code transparency/subtraction immediates at `2A816`, `2AA0D`, and `2AA12`, the ROL-vs-SHL mask distinction and DX-only row count in `2A8A0`, the register-ABI ownership of `2AAEC`, VGA page-base arithmetic in `2AB6C`, and retrace/CRTC register ordering through `2AC10`.\n"""; ledger.write_text(l.rstrip()+'\n')
p=progress.read_text()
p=re.sub(r'object%20accounting-[0-9.]+%25-red','object%20accounting-76.00%25-red',p,count=1)
p=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*','**Verified translated instruction bytes:** **178,417**',p,count=1)
p=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*','**Proven non-code/data bytes:** **3,755**',p,count=1)
p=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*','**Reconciled object bytes:** **182,172 / 239,698 = 76.0006% (76.00%)**',p,count=1)
p=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*','**Unresolved object bytes:** **57,526**',p,count=1)
p += """\n\n## CODE.2 Batch 61 checkpoint\n\nBatch 61 added **1,453 translated instruction bytes** plus **35 directly proven alignment/padding/table bytes**, reconciling through `2AC7F`. Cumulative accounting is **178,417 translated + 3,755 non-code = 182,172 / 239,698 = 76.0006% (76.00%)**; unresolved is **57,526 bytes**.\n\nThe sequential frontier is now `2AC80`, the next independent application predicate. Preserve `2A6B0/2A8A0` clipped-plane asymmetries and writable-code bytes, `2AAEC`'s register ABI, the direct `2AC70` table ownership, and low-level VGA/retrace/CRTC ordering.\n"""; progress.write_text(p.rstrip()+'\n')
a=addendum.read_text(); a += """\n\n## CODE.2 Batch 61 checkpoint\nBatch 61 adds **1,453 translated bytes** and **35 proven alignment/padding/table bytes**, bringing cumulative accounting to **178,417 translated + 3,755 non-code = 182,172 / 239,698 = 76.0006% (76.00%)**. The sequential frontier is `2AC80`; preserve the self-modifying bytes at `2A816`, `2AA0D`, and `2AA12`, the ROL/SHL and 32-/16-bit clipped-blitter asymmetries, `2AAEC`'s register ABI, the directly referenced `2AC70` table, and VGA retrace/CRTC ordering.\n"""; addendum.write_text(a.rstrip()+'\n')
