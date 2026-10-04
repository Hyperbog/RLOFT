from pathlib import Path
import base64,zlib,re

root=Path('.')
cpp=root/'Code2/Code2.cpp'
ledger=root/'Code2/RESIDUAL_LEDGER.md'
progress=root/'Code2/PROGRESS.md'
addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
payload=root/'Code2/batch55.zb64'

c=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 167023u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 3537u;' in c
assert 'function_27f24(' not in c
append=zlib.decompress(base64.b64decode(payload.read_text())).decode()
assert '== 2032u);' in append and 'function_2871c' in append
c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 167023u;','constexpr u32 kVerifiedTranslatedBytes = 169055u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 3537u;','constexpr u32 kVerifiedNonCodeBytes = 3577u;',1)
pat=r'static_assert\(kVerifiedTranslatedBytes == 159155u \+\s*\(0x273E7u-0x26018u\) \+ 2797u\);'
repl='static_assert(kVerifiedTranslatedBytes == 159155u +\n              (0x273E7u-0x26018u) + 2797u + 2032u);'
c,n=re.subn(pat,repl,c,count=1)
assert n==1,n
needle='\n} // namespace code2'
assert c.count(needle)==1
c=c.replace(needle,'\n\n'+append.rstrip()+'\n'+needle,1)
cpp.write_text(c)

rows='''27FEE..27FEF | alignment | 2
2801D..2801F | alignment | 3
28076..28077 | alignment | 2
28095..28097 | alignment | 3
280DB..280DB | alignment | 1
280E6..280E7 | alignment | 2
2810F..2810F | alignment | 1
28165..28167 | alignment | 3
28203..2820F | alignment | 13
282BA..282BB | alignment | 2
286C1..286C3 | alignment | 3
2871A..2871B | alignment | 2
28739..2873B | alignment | 3'''
l=ledger.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert l.count(marker)==1
block=l.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split(marker,1)[0]
assert '27FEE..27FEF |' not in block
l=l.replace(marker,rows+'\n'+marker,1)
l += '''\n\nBatch 55 evidence: `27FEE..27FEF`, `2801D..2801F`, `28076..28077`, `28095..28097`, `280DB`, `280E6..280E7`, `2810F`, `28165..28167`, `282BA..282BB`, `286C1..286C3`, `2871A..2871B`, and `28739..2873B` decode as explicit Watcom MOV-EAX,EAX / LEA-EAX,[EAX+0] / NOP separators between independently decoded routines. `28203..2820F` is a thirteen-byte zero-fill separator after the RET at `28202` and before the register-save prologue at `28210`. A fresh full-object direct-control-flow scan found no CALL/JMP/Jcc target into any listed byte, and a raw scan found no dword reference to any listed object-relative or `+10000h` virtual address.\n\nThe translated `27F24..28738` ownership units are instruction-bounded directly from immutable CODE.2. Preserve `27F24`'s REP-MOVSD/low-CL remainder copy, the signed priority arbitration in `28110/28168`, `28210`'s ten-byte stack filename copy and error fall-through, `282BC`'s repeated stack-slot argument walk, the live/stale ESI behavior around `EEF8`, the encoded `28564` repeated close from one selector address, the explicit `EE8C` global reload in `28670`, and the stack-local length reads in `28020/286C4`.\n'''
ledger.write_text(l.rstrip()+'\n')

p=progress.read_text()
p=re.sub(r'object%20accounting-[0-9.]+%25-red','object%20accounting-72.02%25-red',p,count=1)
p=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*','**Verified translated instruction bytes:** **169,055**',p,count=1)
p=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*','**Proven non-code/data bytes:** **3,577**',p,count=1)
p=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*','**Reconciled object bytes:** **172,632 / 239,698 = 72.0206% (72.02%)**',p,count=1)
p=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*','**Unresolved object bytes:** **67,066**',p,count=1)
p += '''\n\n## CODE.2 Batch 55 checkpoint\n\nBatch 55 added **2,032 translated instruction bytes** plus **40 directly proven alignment/padding bytes**, covering the complete multimedia queue/resource family `27F24..28738` and its inter-routine separators through `2873B`. Cumulative accounting is **169,055 translated + 3,577 non-code = 172,632 / 239,698 = 72.0206% (72.02%)**; unresolved is **67,066 bytes**.\n\nThe sequential frontier is now `2873C`. Preserve `27F24`'s count-preserving REP copy and DS selector arguments, `28110`'s signed existing-vs-incoming priority comparison and `28168`'s tie-to-second-slot selection, `28210`'s exact ten-byte stack filename copy plus fall-through after the error formatter, and `282BC`'s device-type-3 setup. In particular, do not normalize its live ESI behavior: when `EEF8` is zero, ESI still holds the freed file buffer; when the later `EE80` allocation fails, `28564` doubles that live ESI once and closes the same word at `[EEDC+ESI]` eight times. `28670` reloads the `EE8C` base only on a match before adding the record dword. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.\n'''
progress.write_text(p.rstrip()+'\n')

a=addendum.read_text()
a += '''\n\n## CODE.2 Batch 55 checkpoint\nBatch 55 adds **2,032 translated bytes** and **40 proven alignment/padding bytes**, bringing cumulative accounting to **169,055 translated + 3,577 non-code = 172,632 / 239,698 = 72.0206% (72.02%)**. The sequential frontier is `2873C`. Preserve `27F24` REP-copy remainder semantics, signed queue priority arbitration, the `28210` stack-copy/error fall-through, `282BC`'s four-argument stack walk into `366BB/366C5`, its stale/live ESI behavior and repeated-close oddity at `28564`, and `28670`'s explicit `EE8C` reload on the match path.\n'''
addendum.write_text(a.rstrip()+'\n')
