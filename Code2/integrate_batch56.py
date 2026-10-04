from pathlib import Path
import base64,zlib,re,hashlib

root=Path('.')
cpp=root/'Code2/Code2.cpp'
ledger=root/'Code2/RESIDUAL_LEDGER.md'
progress=root/'Code2/PROGRESS.md'
addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
payload=root/'Code2/batch56.zb64'

c=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 169055u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 3577u;' in c
assert 'function_2873c' not in c
raw=zlib.decompress(base64.b64decode(payload.read_text()))
assert hashlib.sha256(raw).hexdigest() == 'b4c759a3b9e2d8f9e5d9b3f76fdba91f550f718c28c74f97d7af1345f7ba4234'
append=raw.decode()
assert 'function_2873c' in append and 'function_293ec' in append and '== 3214u);' in append
c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 169055u;','constexpr u32 kVerifiedTranslatedBytes = 172269u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 3577u;','constexpr u32 kVerifiedNonCodeBytes = 3651u;',1)
pat=(r'static_assert\(kVerifiedTranslatedBytes == 159155u \+\s*'
     r'\(0x273E7u-0x26018u\) \+ 2797u \+ 2032u\);')
rep=('static_assert(kVerifiedTranslatedBytes == 159155u +\n'
     '              (0x273E7u-0x26018u) + 2797u + 2032u + 3214u);')
c,n=re.subn(pat,rep,c,count=1)
assert n==1,n
needle='\n} // namespace code2'
assert c.count(needle)==1
c=c.replace(needle,'\n\n'+append.rstrip()+'\n'+needle,1)
cpp.write_text(c)

rows='''28789..2878B | alignment | 3
2885B..2885B | alignment | 1
28881..28883 | alignment | 3
288E2..288E3 | alignment | 2
288EE..288EF | alignment | 2
288FA..288FB | alignment | 2
289E3..289E3 | alignment | 1
28C51..28C53 | alignment | 3
28CE5..28CE7 | alignment | 3
28DC9..28DCB | alignment | 3
28E2E..28E2F | alignment | 2
28EED..28EEF | alignment | 3
28F55..28F57 | alignment | 3
29007..29007 | alignment | 1
29045..29047 | alignment | 3
2904E..2904F | alignment | 2
29056..29057 | alignment | 2
29095..29097 | alignment | 3
2909E..2909F | alignment | 2
290A6..290A7 | alignment | 2
290C3..290CF | padding | 13
290F2..290F3 | alignment | 2
29205..29207 | alignment | 3
2927D..2927F | alignment | 3
292A9..292AB | alignment | 3
2932B..2932B | alignment | 1
2939B..2939B | alignment | 1
293EA..293EB | alignment | 2'''
l=ledger.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert l.count(marker)==1
block=l.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split(marker,1)[0]
assert '28789..2878B |' not in block
l=l.replace(marker,rows+'\n'+marker,1)
l=re.sub(r'\*\*Total proven non-code/data:\*\* \*\*[0-9,]+ bytes\*\*\.',
         '**Total proven non-code/data:** **3,651 bytes**.',l,count=1)
l += '''\n\nBatch 56 evidence: the mandatory Batch-55 overlap `286E0..287C0` and narrow window `28720..28780` reproduced their recorded SHA-256 anchors exactly before forward accounting. The translated ownership units `2873C..29413` are instruction-bounded directly from immutable CODE.2. Each listed separator was checked against a full object-1 direct relative `CALL`/`JMP`/conditional-branch target scan and a raw object-relative/+10000h absolute dword-reference scan; none has an incoming target or reference. `290C3..290CF` is therefore counted as a 13-byte zero-fill pad on direct evidence, not appearance.\n\nPreserve `2873C`'s wrapped signed-IDIV scaling path, `2878C`'s immediate return on a zero mapping without setting the active byte, the current-slot guard oddity in `28AE0`/`289E4`, exact state values returned by `29058`, BX-only testing and high-half preservation in `290D0`, partial-byte mode storage in `290F4`, signed clipping differences between `292BC` and `2932C`, and the shared earlier RET plus tail transfer through `2E758` in `2939C/293C0/293EC`. The known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger.write_text(l.rstrip()+'\n')

p=progress.read_text()
p=re.sub(r'object%20accounting-[0-9.]+%25-red','object%20accounting-73.39%25-red',p,count=1)
p=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*','**Verified translated instruction bytes:** **172,269**',p,count=1)
p=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*','**Proven non-code/data bytes:** **3,651**',p,count=1)
p=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*','**Reconciled object bytes:** **175,920 / 239,698 = 73.3924% (73.39%)**',p,count=1)
p=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*','**Unresolved object bytes:** **63,778**',p,count=1)
p += '''\n\n## CODE.2 Batch 56 checkpoint\n\nBatch 56 added **3,214 translated instruction bytes** plus **74 directly proven alignment/padding bytes**, covering the complete ownership fold `2873C..29413`. Cumulative accounting is **172,269 translated + 3,651 non-code = 175,920 / 239,698 = 73.3924% (73.39%)**; unresolved is **63,778 bytes**.\n\nThe sequential frontier is now **`29414`**. Preserve the mode-3 wrapped signed scaling in `2873C`, `2878C`'s zero-mapping early exit, current-slot suppression in `28AE0`, state-value behavior in `29058`, BX-only/high-half semantics in `290D0`, `290F4`'s byte/word side effects, the clipping asymmetry of `292BC` versus `2932C`, and the shared-RET/tail-transfer ownership of `2939C/293C0/293EC`. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.\n'''
progress.write_text(p.rstrip()+'\n')

a=addendum.read_text()
a += '''\n\n## CODE.2 Batch 56 checkpoint\nBatch 56 adds **3,214 translated bytes** and **74 proven alignment/padding bytes**, bringing cumulative accounting to **172,269 translated + 3,651 non-code = 175,920 / 239,698 = 73.3924% (73.39%)**. The mandatory Batch-55 overlap hashes were reproduced exactly before forward work. The sequential frontier is `29414`; preserve `2878C`'s zero-map early exit, `28AE0`'s encoded current-slot guard interaction, `29058`'s non-boolean live-state return, BX-only/high-half behavior in `290D0`, `290F4`'s partial-width state updates, signed clipping distinctions, and the shared earlier RET/tail transfer in the final graphics wrappers.\n'''
addendum.write_text(a.rstrip()+'\n')
