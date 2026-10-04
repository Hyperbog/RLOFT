from pathlib import Path
import base64,zlib,re
root=Path('.')
code_p=root/'Code2/Code2.cpp'
ledger_p=root/'Code2/RESIDUAL_LEDGER.md'
progress_p=root/'Code2/PROGRESS.md'
addendum_p=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
frag=zlib.decompress(base64.b64decode((root/'Code2/batch45.zb64').read_text())).decode().rstrip()+"\n\n"
code=code_p.read_text()
assert 'Batch 45:' not in code
assert 'constexpr u32 kVerifiedTranslatedBytes = 133354u;' in code
assert 'constexpr u32 kVerifiedNonCodeBytes = 2602u;' in code
old_assert='''static_assert(kVerifiedTranslatedBytes == 130855u +\n              (0x1EEFBu-0x1EDACu) + (0x1F11Fu-0x1EF7Cu) +\n              (0x1F1C6u-0x1F120u) + (0x1F3FAu-0x1F1C8u) +\n              (0x1F493u-0x1F3FCu) + (0x1F7B0u-0x1F494u) +\n              (0x1F7F6u-0x1F7B0u));'''
new_assert='''static_assert(kVerifiedTranslatedBytes == 133354u +\n              (0x1F809u-0x1F7F8u) + (0x1F81Eu-0x1F80Cu) +\n              (0x1F857u-0x1F820u) + (0x1F8E0u-0x1F86Cu) +\n              (0x1F8FBu-0x1F8E0u) + (0x1FB80u-0x1F8FCu) +\n              (0x1FC12u-0x1FB80u) + (0x20294u-0x1FC14u) +\n              (0x202EDu-0x20294u));'''
assert old_assert in code
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 133354u;',
                  'constexpr u32 kVerifiedTranslatedBytes = 136130u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 2602u;',
                  'constexpr u32 kVerifiedNonCodeBytes = 2634u;',1)
code=code.replace(old_assert,new_assert,1)
assert code.rstrip().endswith('} // namespace code2')
code=code.rsplit('} // namespace code2',1)[0]+frag+'} // namespace code2\n'
code_p.write_text(code)

ledger=ledger_p.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert marker in ledger and '1F809..1F80B' not in ledger
ranges='''1F809..1F80B | alignment | 3\n1F81E..1F81F | alignment | 2\n1F857..1F857 | alignment | 1\n1F858..1F86B | table | 20\n1F8FB..1F8FB | alignment | 1\n1FC12..1FC13 | alignment | 2\n202ED..202EF | alignment | 3\n'''
ledger=ledger.replace(marker,ranges+marker,1)
ledger=ledger.replace('**Total proven non-code/data:** **2,602 bytes**.',
                      '**Total proven non-code/data:** **2,634 bytes**.',1)
ledger+='''\nBatch 45 evidence: `1F858..1F86B` is the exact five-dword CS table consumed by the `C018 <= 4` indirect jump at `1F88B`; entries 0..4 target `1F8C5,1F8BB,1F8B1,1F8A7,1F893`, respectively. `1F809..1F80B`, `1F81E..1F81F`, `1F857`, `1F8FB`, `1FC12..1FC13`, and `202ED..202EF` decode as explicit LEA/MOV-EAX-EAX/NOP compiler separators. A full object-1 direct relative CALL/JMP/conditional target scan found no incoming code target into any of these 32 bytes.\n'''
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=re.sub(r'object%20accounting-[0-9.]+%25-red','object%20accounting-57.89%25-red',progress,count=1)
progress=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*',
                '**Verified translated instruction bytes:** **136,130**',progress,count=1)
progress=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*',
                '**Proven non-code/data bytes:** **2,634**',progress,count=1)
progress=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[^\n]+',
                '**Reconciled object bytes:** **138,764 / 239,698 = 57.8912% (57.89%)**',progress,count=1)
progress=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*',
                '**Unresolved object bytes:** **100,934**',progress,count=1)
progress=progress.rstrip()+'''\n\n## Batch 45 — movement state, modal input, and main game loop\n\nAdded **2,776 translated instruction bytes** plus **32 directly proven table/alignment bytes**, reconciling `1F7F8..202EF`. The five-entry CS table at `1F858..1F86B` belongs to `1F86C`; the next routine begins at `202F0`.\n\nFidelity notes: `1F86C` preserves signed tick expiry and state-4's `2Dh` call / `C018=-1` / fall-through `2Eh..31h` ordering. `1F8E0` uses the prior routine's shared `RET` when the mode is unchanged and otherwise falls directly into `1F8FC`. `1FC14` leaves `258FE` live at entry, dispatches on AX only, keeps the partial-EDX/low-word movement arithmetic, preserves the selected-axis decay exemption when direction*value is nonnegative, and models the `IMUL; SHR 3; CWDE` fixed-point conversions exactly. The loop has no encoded return.\n'''
progress_p.write_text(progress)

add=addendum_p.read_text()
assert '## CODE.2 Batch 45 checkpoint' not in add
add+='''\n\n## CODE.2 Batch 45 checkpoint\n\nBatch 45 added **2,776 translated bytes** plus **32 proven non-code/data bytes**. Cumulative accounting is **136,130 translated + 2,634 non-code = 138,764 / 239,698 = 57.8912% (57.89%)**; unresolved is **100,934 bytes**.\n\nThe sequential frontier now reaches `202EF`; the next routine begins at `202F0`. Preserve the five-entry `1F858..1F86B` table, `1F86C` state-4 fall-through through scripts `2Dh..31h`, `1F8E0`'s backward shared-RET / forward fall-through ownership, `1FB80`'s release-then-press polling, and `1FC14`'s non-returning main loop. In `1FC14`, only `258FC` is initialized at entry; `258FE` remains live, key dispatch compares AX, damping is skipped for the selected axis when direction*value >= 0, and the `C6E8/C6EC` scaling uses low-32 IMUL + logical SHR + CWDE semantics. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.\n'''
addendum_p.write_text(add)

translated=sum([0x1F809-0x1F7F8,0x1F81E-0x1F80C,0x1F857-0x1F820,
                0x1F8E0-0x1F86C,0x1F8FB-0x1F8E0,0x1FB80-0x1F8FC,
                0x1FC12-0x1FB80,0x20294-0x1FC14,0x202ED-0x20294])
assert translated==2776
assert 3+2+1+20+1+2+3==32
assert 136130+2634==138764
assert 239698-138764==100934
assert 'store_le16(dg(m,0x258FCu),0xFFFFu); // 258FE is deliberately left live.' in code
assert 'if(static_cast<s32>(direction)*static_cast<s32>(value)>=0)continue;' in code
assert 'function_002a8(m,*b.dispatch,0x2D);' in code
assert 'store_le32(dg(m,0xC018u),0xFFFFFFFFu);' in code
assert '[[noreturn]] void function_1fc14' in code
print('Batch 45 applied and verified')
