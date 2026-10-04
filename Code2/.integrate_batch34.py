from pathlib import Path
import base64,gzip

root=Path('Code2')
code_p=root/'Code2.cpp'
payload_p=root/'.batch34_fragment.cpp.gz.b64'
ledger_p=root/'RESIDUAL_LEDGER.md'
progress_p=root/'PROGRESS.md'
add_p=root/'CODE2_CONTINUITY_ADDENDUM.md'

code=code_p.read_text()
assert 'Batch 34:' not in code
frag=gzip.decompress(base64.b64decode(payload_p.read_text().strip())).decode().rstrip()+"\n\n"
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 109352u;','constexpr u32 kVerifiedTranslatedBytes = 111780u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 2172u;','constexpr u32 kVerifiedNonCodeBytes = 2200u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 106569u +
              (0x191BEu-0x18D90u) + (0x192CCu-0x191C0u) +
              (0x19375u-0x192CCu) + (0x1942Bu-0x19378u) +
              (0x19453u-0x1942Cu) + (0x197B8u-0x19464u) +
              (0x19886u-0x197B8u));'''
new='''static_assert(kVerifiedTranslatedBytes == 109352u +
              (0x19C72u-0x19888u) + (0x19CC3u-0x19C80u) +
              (0x19CCAu-0x19CC4u) + (0x19CF4u-0x19CCCu) +
              (0x19D16u-0x19CF4u) + (0x1A064u-0x19D18u) +
              (0x1A0AEu-0x1A064u) + (0x1A1CEu-0x1A0B0u) +
              (0x1A1F2u-0x1A1D0u) + (0x1A21Du-0x1A1F4u));'''
assert old in code
code=code.replace(old,new,1)
marker='} // namespace code2'
pos=code.rfind(marker)
assert pos>=0
code=code[:pos]+frag+code[pos:]
code_p.write_text(code)

ledger=ledger_p.read_text()
assert '19C72..19C7F' not in ledger
insert='''19C72..19C7F | padding | 14
19CC3..19CC3 | alignment | 1
19CCA..19CCB | alignment | 2
19D16..19D17 | alignment | 2
1A0AE..1A0AF | alignment | 2
1A1CE..1A1CF | alignment | 2
1A1F2..1A1F3 | alignment | 2
1A21D..1A21F | alignment | 3
'''
ledger=ledger.replace('<!-- RESIDUAL-RANGES-END -->',insert+'<!-- RESIDUAL-RANGES-END -->',1)
ledger=ledger.replace('**Total proven non-code/data:** **2,172 bytes**.','**Total proven non-code/data:** **2,200 bytes**.',1)
ledger += ('\n\nBatch 34 evidence: `19C72..19C7F` is fourteen zero bytes bounded by the directly '
           'reconstructed `19888` return and the `19C80` entry; a full object-1 direct relative '
           'control-flow scan found no incoming target and no absolute data reference into the range. '
           '`19CC3` is `NOP`; `19CCA..19CCB`, `19D16..19D17`, `1A0AE..1A0AF`, '
           '`1A1CE..1A1CF`, and `1A1F2..1A1F3` decode as `MOV EAX,EAX`; '
           '`1A21D..1A21F` decodes as `LEA EAX,[EAX+0]`. None has an incoming direct relative '
           'control-flow target. Known zero islands `006B9..006BF`, `0380D..0380F`, and '
           '`03A64..03A77` remain unresolved.\n')
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=progress.replace('object%20accounting-46.53%25-red','object%20accounting-47.55%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **109,352**','**Verified translated instruction bytes:** **111,780**',1)
progress=progress.replace('**Proven non-code/data bytes:** **2,172**','**Proven non-code/data bytes:** **2,200**',1)
progress=progress.replace('**Reconciled object bytes:** **111,524 / 239,698 = 46.5269% (46.53%)**','**Reconciled object bytes:** **113,980 / 239,698 = 47.5515% (47.55%)**',1)
progress=progress.replace('**Unresolved object bytes:** **128,174**','**Unresolved object bytes:** **125,718**',1)
progress_p.write_text(progress)

add=add_p.read_text().rstrip()+'''\n\n\n## CODE.2 Batch 34 checkpoint\n\nBatch 34 added **2,428 translated bytes** plus **28 proven non-code/data bytes**. Cumulative accounting is **111,780 translated + 2,200 non-code = 113,980 / 239,698 = 47.5515% (47.55%)**; unresolved is **125,718 bytes**.\n\nThe sequential frontier now reaches `1A21F`; the next ownership unit begins at `1A220` and falls forward into the large `1A288` mouse-update routine. Preserve `19888`'s staged menu/cinematic sequence, including the saved `C02C/C02B` bytes, four unsigned timed waits, ESC promotion to status 1, and unconditional `19464` cleanup. Preserve `19C80`'s strict unsigned `now > A790` delayed-click expiry and the `19CCC/19CF4` 14-level stack that saves only the low byte of `A7E8` and zero-extends it on restore. `19D18` keeps the wrapped coordinate scale, the bit-8 clear only when creating the delayed-click latch or consuming an early second click, the three high-flag +/-1000 axis tests, and the cursor redraw/state-save path. `1A0B0` initializes eight `FFFFh` words at `9E21..9E2F` and installs mask `FFFFh`; `1A1D0` uses the shared prior RET when disabled and otherwise tail-jumps `33B44`; `1A1F4` stores the full button word at `9E35` but returns only its low two bits. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_p.write_text(add)
