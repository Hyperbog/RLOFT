from pathlib import Path
import base64,zlib,re
root=Path('.')
code_p=root/'Code2/Code2.cpp'
ledger_p=root/'Code2/RESIDUAL_LEDGER.md'
progress_p=root/'Code2/PROGRESS.md'
addendum_p=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
frag=zlib.decompress(base64.b64decode((root/'Code2/batch44.zb64').read_text())).decode().rstrip()+"\n\n"
code=code_p.read_text()
assert 'Batch 44:' not in code
assert 'constexpr u32 kVerifiedTranslatedBytes = 130855u;' in code
assert 'constexpr u32 kVerifiedNonCodeBytes = 2465u;' in code

# Batch-43 boundary correction discovered while decoding the identical stack
# pattern at 1F6D6: 2E6F0 reads one cdecl argument. The preceding pushed zero
# survives the call and becomes 30508's fifth argument.
old_sig='u32 (*call_2e6f0)(u32,u32)=nullptr;'
new_sig='u32 (*call_2e6f0)(u32)=nullptr;'
assert code.count(old_sig)==1
code=code.replace(old_sig,new_sig,1)
old_call='''const u32 text=b.call_2e6f0?b.call_2e6f0(load_le32(dg(m,0x21A8Au+timer_off)),0u):0u;\n            // The caller removes only the first 2E6F0 argument; its pushed zero\n            // remains as the fifth 30508 argument.'''
new_call='''const u32 text=b.call_2e6f0?b.call_2e6f0(load_le32(dg(m,0x21A8Au+timer_off))):0u;\n            // The encoded pre-pushed zero is not a 2E6F0 argument; it survives\n            // the call as the fifth 30508 argument.'''
assert old_call in code
code=code.replace(old_call,new_call,1)

old_assert='''static_assert(kVerifiedTranslatedBytes == 128548u +\n              (0x1E533u-0x1E4A0u) + (0x1E592u-0x1E534u) +\n              (0x1E74Au-0x1E594u) + (0x1E75Bu-0x1E74Cu) +\n              (0x1E8C8u-0x1E75Cu) + (0x1E930u-0x1E8C8u) +\n              (0x1E99Fu-0x1E930u) + (0x1EA04u-0x1E9A0u) +\n              (0x1EA78u-0x1EA04u) + (0x1EA8Eu-0x1EA78u) +\n              (0x1EDACu-0x1EA90u));'''
new_assert='''static_assert(kVerifiedTranslatedBytes == 130855u +\n              (0x1EEFBu-0x1EDACu) + (0x1F11Fu-0x1EF7Cu) +\n              (0x1F1C6u-0x1F120u) + (0x1F3FAu-0x1F1C8u) +\n              (0x1F493u-0x1F3FCu) + (0x1F7B0u-0x1F494u) +\n              (0x1F7F6u-0x1F7B0u));'''
assert old_assert in code
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 130855u;',
                  'constexpr u32 kVerifiedTranslatedBytes = 133354u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 2465u;',
                  'constexpr u32 kVerifiedNonCodeBytes = 2602u;',1)
code=code.replace(old_assert,new_assert,1)

assert code.rstrip().endswith('} // namespace code2')
code=code.rsplit('} // namespace code2',1)[0]+frag+'} // namespace code2\n'
code_p.write_text(code)

ledger=ledger_p.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert marker in ledger and '1EEFB..1EEFB' not in ledger
ranges='''1EEFB..1EEFB | alignment | 1\n1EEFC..1EF7B | table | 128\n1F11F..1F11F | alignment | 1\n1F1C6..1F1C7 | alignment | 2\n1F3FA..1F3FB | alignment | 2\n1F493..1F493 | alignment | 1\n1F7F6..1F7F7 | alignment | 2\n'''
ledger=ledger.replace(marker,ranges+marker,1)
ledger=ledger.replace('**Total proven non-code/data:** **2,465 bytes**.',
                      '**Total proven non-code/data:** **2,602 bytes**.',1)
ledger+='''\nBatch 44 evidence: `1EEFC..1EF7B` is the exact 32-dword CS dispatch table consumed by the bounded `C6C8 <= 1Fh` indirect jump at `1EFAC`; decoded targets are `1F041,1F025,1F02F,1F07B,1F08D,1F0E0,1F0CC,1F106,1F053,1EFBB,1F106,1EFF3,1F09C,1F09C,1F09C,1F106,1F0F4,1F0F4,1F106,1F106,1F106,1F106,1EFB4,1EFBB,1EFBB,1EFBB,1EFBB,1F106,1F0B4,1F0B4,1F106,1EFBB`. `1EEFB`, `1F11F`, `1F1C6..1F1C7`, `1F3FA..1F3FB`, `1F493`, and `1F7F6..1F7F7` decode as explicit NOP/MOV-EAX-EAX separators. A full object-1 direct relative CALL/JMP/conditional target scan, excluding the table bytes' meaningless linear disassembly, found no incoming code target into any of these 137 bytes.\n'''
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=re.sub(r'object%20accounting-[0-9.]+%25-red','object%20accounting-56.72%25-red',progress,count=1)
progress=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*',
                '**Verified translated instruction bytes:** **133,354**',progress,count=1)
progress=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*',
                '**Proven non-code/data bytes:** **2,602**',progress,count=1)
progress=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[^\n]+',
                '**Reconciled object bytes:** **135,956 / 239,698 = 56.7197% (56.72%)**',progress,count=1)
progress=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*',
                '**Unresolved object bytes:** **103,742**',progress,count=1)
progress=progress.rstrip()+'''\n\n## Batch 44 — terrain cycling, encounter selection, and party-panel refresh\n\nAdded **2,499 translated instruction bytes** plus **137 directly proven table/alignment bytes**, reconciling `1EDAC..1F7F7`. The `1EEFC..1EF7B` region is the exact 32-entry CS table owned by `1EF7C`; the next routine begins at `1F7F8`.\n\nFidelity notes: `1EDAC` performs at most one mapping replacement per lane before cycling the two `242h..244h` / `245h..247h` triplets. `1EF7C` retains unsigned selector bounds and low-32 random scaling. `1F1C8` keeps the truncation-toward-zero signed divisions used to derive `FF50` and the asymmetric `C026/C027/C028` update paths. `1F494` preserves the high-bit actor path's direct jump to `1F745` (skipping `22F40` and both `1EA90` calls), signed HP-bar `IDIV`, and the pre-pushed zero that becomes `30508`'s fifth argument. The same stack pattern proves the Batch-43 `2E6F0` boundary takes one argument; that signature is corrected here without changing byte accounting.\n'''
progress_p.write_text(progress)

add=addendum_p.read_text()
assert '## CODE.2 Batch 44 checkpoint' not in add
add+='''\n\n## CODE.2 Batch 44 checkpoint\n\nBatch 44 added **2,499 translated bytes** plus **137 proven non-code/data bytes**. Cumulative accounting is **133,354 translated + 2,602 non-code = 135,956 / 239,698 = 56.7197% (56.72%)**; unresolved is **103,742 bytes**.\n\nThe sequential frontier now reaches `1F7F7`; the next routine begins at `1F7F8`. The exact 32-entry table at `1EEFC..1EF7B` belongs to `1EF7C`. Preserve `1EDAC`'s one-replacement-per-lane mapping behavior, `1EF7C`'s phase-dependent random sound ranges, `1F120`'s repeated live `29048` calls, `1F1C8`'s signed truncation and asymmetric presentation paths, `1F3FC`'s flag-priority order, and `1F494`'s dead/high-bit direct jump that bypasses the common actor-status refresh. Direct inspection of `2E6F0` (`PUSH arg; CALL 373BF`) proves it has one cdecl input; Batch 43's boundary signature was corrected accordingly while retaining the encoded pre-pushed zero as `30508`'s fifth argument. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.\n'''
addendum_p.write_text(add)

# Local accounting and semantic anchors before the workflow compiler pass.
translated=sum([0x1EEFB-0x1EDAC,0x1F11F-0x1EF7C,0x1F1C6-0x1F120,
                0x1F3FA-0x1F1C8,0x1F493-0x1F3FC,0x1F7B0-0x1F494,
                0x1F7F6-0x1F7B0])
assert translated==2499
assert 1+128+1+2+2+1+2==137
assert 133354+2602==135956
assert 239698-135956==103742
assert 'case 22u:\n                sound=0u;' in code
assert 'if(static_cast<s32>(remaining)>0)' in code
assert 'if((flags&0x8000u)!=0u)' in code
assert 'skipping 22F40 and both 1EA90 calls' in code
assert 'b.app43->call_2e6f0(load_le32(dg(m,0x21A92u+actor)))' in code
print('Batch 44 applied and verified')
