from pathlib import Path
import base64,gzip
root=Path('Code2')
code_p=root/'Code2.cpp'; payload_p=root/'.batch35_fragment.cpp.gz.b64'
ledger_p=root/'RESIDUAL_LEDGER.md'; progress_p=root/'PROGRESS.md'; add_p=root/'CODE2_CONTINUITY_ADDENDUM.md'
code=code_p.read_text()
assert 'Batch 35:' not in code
frag=gzip.decompress(base64.b64decode(payload_p.read_text().strip())).decode().rstrip()+"\n\n"
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 111780u;','constexpr u32 kVerifiedTranslatedBytes = 114822u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 2200u;','constexpr u32 kVerifiedNonCodeBytes = 2246u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 109352u +
              (0x19C72u-0x19888u) + (0x19CC3u-0x19C80u) +
              (0x19CCAu-0x19CC4u) + (0x19CF4u-0x19CCCu) +
              (0x19D16u-0x19CF4u) + (0x1A064u-0x19D18u) +
              (0x1A0AEu-0x1A064u) + (0x1A1CEu-0x1A0B0u) +
              (0x1A1F2u-0x1A1D0u) + (0x1A21Du-0x1A1F4u));'''
new='''static_assert(kVerifiedTranslatedBytes == 111780u +
              ((0x1AE30u-0x1A220u)-46u));'''
assert old in code
code=code.replace(old,new,1)
marker='} // namespace code2'
pos=code.rfind(marker); assert pos>=0
code=code[:pos]+frag+code[pos:]
code_p.write_text(code)

ledger=ledger_p.read_text()
assert '1A285..1A287' not in ledger
insert='''1A285..1A287 | alignment | 3
1A342..1A343 | alignment | 2
1A3B7..1A3B7 | alignment | 1
1A50D..1A50F | alignment | 3
1A5AF..1A5AF | alignment | 1
1AACA..1AACB | alignment | 2
1AC16..1AC17 | alignment | 2
1AC18..1AC37 | table | 32
'''
ledger=ledger.replace('<!-- RESIDUAL-RANGES-END -->',insert+'<!-- RESIDUAL-RANGES-END -->',1)
ledger=ledger.replace('**Total proven non-code/data:** **2,200 bytes**.','**Total proven non-code/data:** **2,246 bytes**.',1)
ledger += '''\n\nBatch 35 evidence: `1AC18..1AC37` is the exact eight-entry CS dword table consumed by the indirect jump at `1ADB3`, with targets `1ADBB,1ADCB,1ADD0,1ADC6,1ADDB,1ADE0,1ADED,1AE00`. `1A285..1A287`, `1A342..1A343`, `1A3B7`, `1A50D..1A50F`, `1A5AF`, `1AACA..1AACB`, and `1AC16..1AC17` are bounded NOP/LEA/MOV-EAX,EAX compiler separators between directly reconstructed ownership blocks. A full object-1 direct relative control-flow target scan found no incoming call/jump target to any Batch 35 residual byte. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=progress.replace('object%20accounting-47.55%25-red','object%20accounting-48.84%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **111,780**','**Verified translated instruction bytes:** **114,822**',1)
progress=progress.replace('**Proven non-code/data bytes:** **2,200**','**Proven non-code/data bytes:** **2,246**',1)
progress=progress.replace('**Reconciled object bytes:** **113,980 / 239,698 = 47.5515% (47.55%)**','**Reconciled object bytes:** **117,068 / 239,698 = 48.8398% (48.84%)**',1)
progress=progress.replace('**Unresolved object bytes:** **125,718**','**Unresolved object bytes:** **122,630**',1)
progress_p.write_text(progress)

add=add_p.read_text().rstrip()+'''\n\n\n## CODE.2 Batch 35 checkpoint\n\nBatch 35 added **3,042 translated bytes** plus **46 proven non-code/data bytes**. Cumulative accounting is **114,822 translated + 2,246 non-code = 117,068 / 239,698 = 48.8398% (48.84%)**; unresolved is **122,630 bytes**.\n\nThe sequential frontier now reaches `1AE2F`; the next routine begins at `1AE30`. Preserve `1A220`'s byte-only `CF48` toggle followed by a signed-word test, `1A288`'s non-contiguous shared cold tail at `1AB6C..1AC0B`, and `1A3B8`'s two distinct cursor-clamp regions. The page/item helpers deliberately differ between active `XOR SI,1` and fallback `XOR AL,1` paths, while `1AACC/1AB20` perform the encoded word `DEC` before testing for zero. `1AC38` owns the exact eight-entry table at `1AC18..1AC37`; preserve its two hit-map families, `A6BC=index+3` update, and axis-specific `1F7F8` dispatch values. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_p.write_text(add)
