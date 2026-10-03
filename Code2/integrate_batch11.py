from pathlib import Path
import re

cpp_path=Path('Code2/Code2.cpp')
frag_path=Path('Code2/batch11.inc')
prog_path=Path('Code2/PROGRESS.md')
ledger_path=Path('Code2/RESIDUAL_LEDGER.md')
cont_path=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md')
workflow_path=Path('.github/workflows/integrate-code2-batch11.yml')

cpp=cpp_path.read_text()
frag=frag_path.read_text()
prog=prog_path.read_text()
ledger=ledger_path.read_text()
cont=cont_path.read_text()

assert 'constexpr u32 kVerifiedTranslatedBytes = 36959u;' in cpp
assert 'constexpr u32 kVerifiedNonCodeBytes = 305u;' in cpp
assert 'Batch 11:' not in cpp
assert '## CODE.2 Batch 11 checkpoint' not in cont

# Fresh byte-audit corrections made before canonical integration.
old='''    if(b.call_171fc)b.call_171fc(load_le32(dg(m,0xC6C8u)));\n    *dg(m,0xA79Du)=1u;'''
new='''    *dg(m,0xA79Du)=0u;\n    if(b.call_171fc)b.call_171fc(load_le32(dg(m,0xC6C8u)));\n    *dg(m,0xA79Du)=1u;'''
assert old in frag
frag=frag.replace(old,new,1)
frag=frag.replace('// original; the structured call here retains its return value while accounting\n// only for the 07874..07937 prefix in this batch.',
                  '// original; the structured call here preserves its side effects while accounting\n// only for the 07874..07937 prefix in this batch.',1)
frag=frag.replace('[[nodiscard]] u32 function_07874(Machine& m,Application11Boundary& b,Application10Boundary& a10,\n                                  Application09Boundary& a9) noexcept {',
                  'void function_07874(Machine& m,Application11Boundary& b,Application10Boundary& a10,\n                    Application09Boundary& a9) noexcept {',1)
frag=frag.replace('if(b.call_33a5a&&b.call_33a5a(0x668Cu)!=0u){if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x60BCu)));return 1u;}',
                  'if(b.call_33a5a&&b.call_33a5a(0x668Cu)!=0u){if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x60BCu)));return;}',1)
frag=frag.replace('if(b.call_33a5a&&b.call_33a5a(0xC020u)!=0u){if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x60BCu)));return 1u;}',
                  'if(b.call_33a5a&&b.call_33a5a(0xC020u)!=0u){if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x60BCu)));return;}',1)
frag=frag.replace('    return function_07938(m,a10,a9);\n}\nstatic_assert(0x07938u-0x07874u==196u);',
                  '    (void)function_07938(m,a10,a9);\n}\nstatic_assert(0x07938u-0x07874u==196u);',1)
assert '*dg(m,0xA79Du)=0u;' in frag
assert 'void function_07874(' in frag
assert '[[nodiscard]] u32 function_07874' not in frag

# Insert the verified reconstruction before the namespace close.
marker='\n} // namespace code2\n'
assert cpp.count(marker)==1
cpp=cpp.replace(marker,frag.rstrip()+marker,1)
cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 36959u;',
                'constexpr u32 kVerifiedTranslatedBytes = 39537u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 305u;',
                'constexpr u32 kVerifiedNonCodeBytes = 343u;',1)
old_assert='''static_assert(kVerifiedTranslatedBytes == 34466u +\n              (0x0706Eu-0x06CADu) + (0x0719Cu-0x07070u) +\n              (0x071B8u-0x0719Cu) + (0x07376u-0x071B8u) +\n              (0x0739Du-0x07378u) + (0x075C9u-0x073A0u) +\n              (0x079E0u-0x07938u));'''
new_assert='''static_assert(kVerifiedTranslatedBytes == 36959u +\n              (0x0779Fu-0x075CCu) + (0x07871u-0x077A0u) +\n              (0x07938u-0x07874u) + (0x07B94u-0x079E0u) +\n              (0x07D43u-0x07B94u) + (0x0803Cu-0x07D44u) +\n              (0x0808Bu-0x0803Cu));'''
assert old_assert in cpp
cpp=cpp.replace(old_assert,new_assert,1)

# Progress headline and durable batch record.
repls={
'object%20accounting-15.55%25-red':'object%20accounting-16.64%25-red',
'**Verified translated instruction bytes:** **36,959**':'**Verified translated instruction bytes:** **39,537**',
'**Proven non-code/data bytes:** **305**':'**Proven non-code/data bytes:** **343**',
'**Reconciled object bytes:** **37,264 / 239,698 = 15.5462% (15.55%)**':'**Reconciled object bytes:** **39,880 / 239,698 = 16.6376% (16.64%)**',
'**Unresolved object bytes:** **202,434**':'**Unresolved object bytes:** **199,818**',
}
for a,z in repls.items():
    assert a in prog
    prog=prog.replace(a,z,1)
prog=prog.rstrip()+'''\n\n## Batch 11 — save/load slot UI and confirmation workflows\n\nAdded **2,578 translated instruction bytes** plus **38 proven non-code/data bytes**. The sequential application frontier now reaches `0808A`; `0808B` is a one-byte compiler separator and `0808C..080AB` is an eight-entry CS dispatch table consumed by the next routine at `080AC`.\n\nTranslated ranges:\n- `075CC..0779E` — 467 bytes: 49-character filename editor with first-edit semantics.\n- `077A0..07870` — 209 bytes: live-game save-to-slot-00 workflow.\n- `07874..07937` — 196 bytes: slot-00 load preparation and fall-through into `07938`.\n- `079E0..07B93` — 436 bytes: interactive save-slot selector.\n- `07B94..07D42` — 431 bytes: interactive load-slot selector.\n- `07D44..0803B` — 760 bytes: randomized two-choice confirmation UI.\n- `0803C..0808A` — 79 bytes: modal message helper.\n\nFidelity notes: `077A0` clears `A79D` before `171FC` and sets it to one immediately before serialization; `07874` is modeled as a side-effect wrapper because its early exits do not establish a meaningful return; both slot selectors intentionally leak the ten panel handles on the encoded right-click return-2 path; `07D44` accepts x coordinates inclusively from `C8h` through `E0h`.\n'''

# Directly proven residual bytes.
insert='''0779F..0779F | alignment | 1\n07871..07873 | alignment | 3\n07D43..07D43 | alignment | 1\n0808B..0808B | alignment | 1\n0808C..080AB | table | 32\n'''
end='<!-- RESIDUAL-RANGES-END -->'
assert end in ledger
ledger=ledger.replace(end,insert+end,1)
assert '**Total proven non-code/data:** **305 bytes**.' in ledger
ledger=ledger.replace('**Total proven non-code/data:** **305 bytes**.',
                      '**Total proven non-code/data:** **343 bytes**.',1)
ledger=ledger.rstrip()+'''\n\nBatch 11 evidence: `0779F`, `07871..07873`, `07D43`, and `0808B` decode as explicit compiler no-op separators between directly reconciled boundaries, and a full object-1 direct relative branch/call scan found no incoming target to any separator byte. `0808C..080AB` is the eight-entry CS dword table used by the indirect dispatcher at `08392`; its decoded targets are `0839A`, `083AD`, `08472`, `08537`, `0854D`, `08587`, `085DD`, and `085FE`. The known zero islands remain unresolved.\n'''

cont=cont.rstrip()+'''\n\n## CODE.2 Batch 11 checkpoint\n\nBatch 11 added **2,578 translated bytes** plus **38 proven non-code/data bytes**. Cumulative accounting is **39,537 translated + 343 non-code = 39,880 / 239,698 = 16.6376% (16.64%)**; unresolved is **199,818 bytes**.\n\nThe sequential application frontier now reaches `0808A`. `0808C..080AB` is the directly decoded eight-entry CS table for the dispatcher beginning at `080AC`; continue there. Preserve the `077A0` `A79D` clear-before-`171FC` / set-before-save ordering, `07874` fall-through ownership of `07938`, and the intentional right-click handle leaks in `079E0` and `07B94`. The zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''

cpp_path.write_text(cpp)
prog_path.write_text(prog)
ledger_path.write_text(ledger)
cont_path.write_text(cont)

# The integrated commit must leave only durable canonical files.
frag_path.unlink()
Path(__file__).unlink()
workflow_path.unlink()
