from pathlib import Path
import base64,zlib,re
root=Path('.')
code_p=root/'Code2/Code2.cpp'
ledger_p=root/'Code2/RESIDUAL_LEDGER.md'
progress_p=root/'Code2/PROGRESS.md'
addendum_p=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
frag=zlib.decompress(base64.b64decode((root/'Code2/batch43.zb64').read_text())).decode()
code=code_p.read_text()
assert 'Batch 43:' not in code
assert 'constexpr u32 kVerifiedTranslatedBytes = 128548u;' in code
assert 'constexpr u32 kVerifiedNonCodeBytes = 2456u;' in code
old='''static_assert(kVerifiedTranslatedBytes == 122860u +\n              (0x1E4A0u-0x1CE68u));'''
new='''static_assert(kVerifiedTranslatedBytes == 128548u +\n              (0x1E533u-0x1E4A0u) + (0x1E592u-0x1E534u) +\n              (0x1E74Au-0x1E594u) + (0x1E75Bu-0x1E74Cu) +\n              (0x1E8C8u-0x1E75Cu) + (0x1E930u-0x1E8C8u) +\n              (0x1E99Fu-0x1E930u) + (0x1EA04u-0x1E9A0u) +\n              (0x1EA78u-0x1EA04u) + (0x1EA8Eu-0x1EA78u) +\n              (0x1EDACu-0x1EA90u));'''
assert old in code
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 128548u;',
                  'constexpr u32 kVerifiedTranslatedBytes = 130855u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 2456u;',
                  'constexpr u32 kVerifiedNonCodeBytes = 2465u;',1)
code=code.replace(old,new,1)
assert code.rstrip().endswith('} // namespace code2')
body=code.rsplit('} // namespace code2',1)[0]
if not body.endswith('\n'): body+='\n'
code=body+frag
if not code.endswith('\n'): code+='\n'
code+='} // namespace code2\n'
code_p.write_text(code)

ledger=ledger_p.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert marker in ledger and '1E533..1E533' not in ledger
ranges='''1E533..1E533 | alignment | 1\n1E592..1E593 | alignment | 2\n1E74A..1E74B | alignment | 2\n1E75B..1E75B | alignment | 1\n1E99F..1E99F | alignment | 1\n1EA8E..1EA8F | alignment | 2\n'''
ledger=ledger.replace(marker,ranges+marker,1)
ledger=ledger.replace('**Total proven non-code/data:** **2,456 bytes**.',
                      '**Total proven non-code/data:** **2,465 bytes**.',1)
evidence='''\nBatch 43 evidence: `1E533`, `1E592..1E593`, `1E74A..1E74B`, `1E75B`, `1E99F`, and `1EA8E..1EA8F` decode as explicit compiler NOP/MOV-EAX-EAX separators between directly reconstructed routine boundaries. A full object-1 direct relative CALL/JMP/conditional-branch target scan found no incoming target to any separator byte.\n'''
ledger+=evidence
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=re.sub(r'object%20accounting-[0-9.]+%25-red','object%20accounting-55.62%25-red',progress,count=1)
progress=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*',
                '**Verified translated instruction bytes:** **130,855**',progress,count=1)
progress=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*',
                '**Proven non-code/data bytes:** **2,465**',progress,count=1)
progress=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[^\n]+',
                '**Reconciled object bytes:** **133,320 / 239,698 = 55.6200% (55.62%)**',progress,count=1)
progress=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*',
                '**Unresolved object bytes:** **106,378**',progress,count=1)
progress_p.write_text(progress)

add=addendum_p.read_text()
assert '## CODE.2 Batch 43 checkpoint' not in add
add+='''\n\n## CODE.2 Batch 43 checkpoint\n\nBatch 43 added **2,307 translated bytes** plus **9 proven non-code/data bytes**. Cumulative accounting is **130,855 translated + 2,465 non-code = 133,320 / 239,698 = 55.6200% (55.62%)**; unresolved is **106,378 bytes**.\n\nThe sequential frontier now reaches `1EDAB`; the next routine begins at `1EDAC`. Preserve `1E4A0/1E534`'s unsigned timer expiry tests, `1E594`'s phase-gated early return and dual timed-text damage paths, `1E75C`'s signed tick-delta comparisons and 438h/21Ch cadence, `1E930/1E9A0`'s partial-register return values that retain pointer high words, and `1EA90`'s status/item renderer including the stack-carried zero that becomes `30508`'s fifth argument. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
addendum_p.write_text(add)

# Exact accounting and anchor checks before the workflow compiler pass.
assert sum([0x1E533-0x1E4A0,0x1E592-0x1E534,0x1E74A-0x1E594,
            0x1E75B-0x1E74C,0x1E8C8-0x1E75C,0x1E930-0x1E8C8,
            0x1E99F-0x1E930,0x1EA04-0x1E9A0,0x1EA78-0x1EA04,
            0x1EA8E-0x1EA78,0x1EDAC-0x1EA90])==2307
assert 'return (ptr&0xFFFF0000u)|static_cast<u32>(*dg(m,ptr+1u)&3u);' in code
assert 'const u32 eax=(ptr&0xFFFF0000u)|static_cast<u32>(*dg(m,ptr+6u));' in code
assert 'if(static_cast<s32>(now)>static_cast<s32>(deadline))' in code
assert 'b.call_30508(0x120u,pos,0x10u,text,0u)' in code
print('Batch 43 applied and verified')
