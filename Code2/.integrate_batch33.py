from pathlib import Path
import base64,gzip
root=Path('Code2')
code_p=root/'Code2.cpp'; ledger_p=root/'RESIDUAL_LEDGER.md'; progress_p=root/'PROGRESS.md'; add_p=root/'CODE2_CONTINUITY_ADDENDUM.md'
payload_p=root/'.batch33_fragment.cpp.gz.b64'
code=code_p.read_text()
assert 'Batch 33:' not in code
frag=gzip.decompress(base64.b64decode(payload_p.read_text().strip())).decode().rstrip()+"\n\n"
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 106569u;','constexpr u32 kVerifiedTranslatedBytes = 109352u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 2147u;','constexpr u32 kVerifiedNonCodeBytes = 2172u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 103537u +
              (0x18D8Cu-0x181B4u));'''
new='''static_assert(kVerifiedTranslatedBytes == 106569u +
              (0x191BEu-0x18D90u) + (0x192CCu-0x191C0u) +
              (0x19375u-0x192CCu) + (0x1942Bu-0x19378u) +
              (0x19453u-0x1942Cu) + (0x197B8u-0x19464u) +
              (0x19886u-0x197B8u));'''
assert old in code
code=code.replace(old,new,1)
marker='} // namespace code2'
pos=code.rfind(marker); assert pos>=0
code=code[:pos]+frag+code[pos:]
code_p.write_text(code)

ledger=ledger_p.read_text()
assert '191BE..191BF' not in ledger
insert='''191BE..191BF | alignment | 2
19375..19377 | alignment | 3
1942B..1942B | alignment | 1
19453..19453 | alignment | 1
19454..19463 | table | 16
19886..19887 | alignment | 2
'''
ledger=ledger.replace('<!-- RESIDUAL-RANGES-END -->',insert+'<!-- RESIDUAL-RANGES-END -->',1)
ledger=ledger.replace('**Total proven non-code/data:** **2,147 bytes**.','**Total proven non-code/data:** **2,172 bytes**.',1)
ledger += ('\n\nBatch 33 evidence: `191BE..191BF`, `19375..19377`, `1942B`, `19453`, and '
           '`19886..19887` decode as explicit compiler no-op separators between directly reconstructed '
           'routine boundaries; a full object-1 direct relative branch/call target scan found no incoming '
           'target to any separator byte. `19454..19463` is the exact four-dword CS jump table indexed '
           'by `EDI` at `194AD`, with decoded targets `194B5`, `19529`, `19644`, and `19688`. Known '
           'zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n')
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=progress.replace('object%20accounting-45.36%25-red','object%20accounting-46.53%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **106,569**','**Verified translated instruction bytes:** **109,352**',1)
progress=progress.replace('**Proven non-code/data bytes:** **2,147**','**Proven non-code/data bytes:** **2,172**',1)
progress=progress.replace('**Reconciled object bytes:** **108,716 / 239,698 = 45.3554% (45.36%)**','**Reconciled object bytes:** **111,524 / 239,698 = 46.5269% (46.53%)**',1)
progress=progress.replace('**Unresolved object bytes:** **130,982**','**Unresolved object bytes:** **128,174**',1)
progress_p.write_text(progress)

add=add_p.read_text().rstrip()+'''\n\n\n## CODE.2 Batch 33 checkpoint\n\nBatch 33 added **2,783 translated bytes** plus **25 proven non-code/data bytes**. Cumulative accounting is **109,352 translated + 2,172 non-code = 111,524 / 239,698 = 46.5269% (46.53%)**; unresolved is **128,174 bytes**.\n\nThe sequential frontier now reaches `19887`; `19886..19887` is proven `MOV EAX,EAX` alignment and the next routine begins at `19888`. Preserve `18D90`'s one-shot `F050+72h` latch, redraw-only-on-mutation input loop, low-byte-plus-one `165C` character-class lookup, and three-failure `205B4(-1)` path. Preserve `191C0`'s x86 two's-complement ABS plus low-32 IMUL/signed-IDIV interpolation, `192CC`'s tail jump to `1A9F8`, and the exact four-entry CS table at `19454..19463`. `19464` keeps unsigned dispatch gating for the `-1` hover sentinel and signed mouse-threshold comparisons. `197B8` preserves the later dword load from `C050` with only AL XORed before `2A020`, so bytes `C051..C053` remain live. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_p.write_text(add)
