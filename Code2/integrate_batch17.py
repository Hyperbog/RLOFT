from pathlib import Path
import re
import base64
import zlib

cpp_path=Path('Code2/Code2.cpp')
blob_path=Path('Code2/batch17.zb64')
ledger_path=Path('Code2/RESIDUAL_LEDGER.md')
progress_path=Path('Code2/PROGRESS.md')
addendum_path=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md')

cpp=cpp_path.read_text()
inc=zlib.decompress(base64.b64decode(blob_path.read_text().strip())).decode().rstrip()+"\n\n"
ledger=ledger_path.read_text()
progress=progress_path.read_text()
addendum=addendum_path.read_text()

assert 'Batch 17: 0B518..0C6D6' not in cpp
assert '## CODE.2 Batch 17 checkpoint' not in progress
assert '## CODE.2 Batch 17 checkpoint' not in addendum

cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 52825u;',
                'constexpr u32 kVerifiedTranslatedBytes = 56834u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 475u;',
                'constexpr u32 kVerifiedNonCodeBytes = 1009u;',1)

old_assert='''static_assert(kVerifiedTranslatedBytes == 49376u +
              (0x0A8A0u-0x0A794u) + (0x0AA2Du-0x0A8A0u) +
              (0x0ABC4u-0x0AA30u) + (0x0AC2Cu-0x0ABC4u) +
              (0x0AD24u-0x0AC2Cu) + (0x0AE2Du-0x0AD24u) +
              (0x0AED7u-0x0AE30u) + (0x0B02Cu-0x0AED8u) +
              (0x0B40Eu-0x0B02Cu) + (0x0B516u-0x0B410u));'''
new_assert='''static_assert(kVerifiedTranslatedBytes == 52825u +
              (0x0B6F1u-0x0B518u) +
              (0x0B73Du-0x0B6F4u) +
              (0x0C6D7u-0x0B950u));'''
assert old_assert in cpp
cpp=cpp.replace(old_assert,new_assert,1)

# Batch 16 modeled 1C77C as one-argument even though CODE.2 keeps the older
# 0320h push live across 1C924 and therefore supplies it as argument two.
assert 'void (*call_1c77c)(u32)=nullptr;' in cpp
cpp=cpp.replace('void (*call_1c77c)(u32)=nullptr;',
                'void (*call_1c77c)(u32,u32)=nullptr;',1)
assert 'if(b.call_1c77c)b.call_1c77c(token);' in cpp
cpp=cpp.replace('if(b.call_1c77c)b.call_1c77c(token);',
                'if(b.call_1c77c)b.call_1c77c(token,0x320u);',1)

marker='\n} // namespace code2\n'
assert cpp.endswith(marker)
cpp=cpp[:-len(marker)]+'\n'+inc+'} // namespace code2\n'
cpp_path.write_text(cpp)

residual_insert='''0B6F1..0B6F3 | alignment | 3
0B73D..0B73F | alignment | 3
0B740..0B94F | table | 528
'''
ledger_marker='<!-- RESIDUAL-RANGES-END -->'
assert residual_insert.strip() not in ledger
assert ledger_marker in ledger
ledger=ledger.replace(ledger_marker,residual_insert+ledger_marker,1)
ledger=ledger.replace('**Total proven non-code/data:** **475 bytes**.',
                      '**Total proven non-code/data:** **1,009 bytes**.',1)
ledger += '''\n\nBatch 17 evidence: `0B6F1..0B6F3` and `0B73D..0B73F` decode as `LEA EAX,[EAX+0]` compiler separators between directly reconstructed routine boundaries. A full object-1 direct relative branch/call-target scan found no incoming target to either separator. `0B740..0B94F` is the exact 132-entry CS dword table selected by command values `00h..83h` at `0B988`; all 132 entries were decoded and reconciled, comprising 65 explicit handler targets and the shared default target `0C6C2`. `0C6D7..0C703` remains unresolved rather than being classified from appearance alone.\n'''
ledger_path.write_text(ledger)

progress=progress.replace('object%20accounting-22.24%25-red','object%20accounting-24.13%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **52,825**','**Verified translated instruction bytes:** **56,834**',1)
progress=progress.replace('**Proven non-code/data bytes:** **475**','**Proven non-code/data bytes:** **1,009**',1)
progress=progress.replace('**Reconciled object bytes:** **53,300 / 239,698 = 22.2363% (22.24%)**',
                          '**Reconciled object bytes:** **57,843 / 239,698 = 24.1316% (24.13%)**',1)
progress=progress.replace('**Unresolved object bytes:** **186,398**','**Unresolved object bytes:** **181,855**',1)
progress += '''\n\n## Batch 17 — nearby-world effects and 132-way effect dispatcher\n\nSource identity and object-1 SHA-256 were reverified before this batch. Added **4,009 translated instruction bytes** and **534 directly proven non-code/data bytes**, bringing cumulative accounting to **56,834 translated + 1,009 non-code = 57,843 / 239,698 = 24.1316% (24.13%)**; unresolved is **181,855 bytes**.\n\n### Newly translated ranges\n\n- `0B518..0B6F0` — **473 bytes**: nearby-world effect scan with the exact rank-indexed stack alias, signed two's-complement distance behavior, descriptor gates, mutable threshold/offset/counter aliases, and `E284` counter argument.\n- `0B6F4..0B73C` — **73 bytes**: world-record descriptor-marking pass with signed record count and explicit restoration of `22230`.\n- `0B950..0C6D6` — **3,463 bytes**: complete command/effect dispatcher for command values through `83h`, including all 65 explicit handlers, the shared default tail, modal selection exits, timed-status installation, projectile launch variants, and the `58h` call back into `0B518`.\n\n### Newly proven non-code/data\n\n- `0B6F1..0B6F3` — **3 bytes**, `LEA EAX,[EAX+0]` alignment.\n- `0B73D..0B73F` — **3 bytes**, `LEA EAX,[EAX+0]` alignment.\n- `0B740..0B94F` — **528 bytes**, 132-entry CS dword jump table indexed at `0B988`; every entry was decoded and reconciled.\n\n### Fidelity notes\n\n`0B518` preserves the encoded rank-indexed write aliases: ranks 6, 7, and 8 can overwrite the loop counter, table offset, and threshold respectively; rank zero aliases the stale dword immediately below the local array. The x86 absolute-value sequence is modeled in 32-bit two's-complement form so `INT_MIN` retains its encoded signed-compare behavior. `0B950` preserves the command-`18h/19h/1Ah/29h/5Ch` no-selection result of `FFFFFFFFh`, the distinct command-`7Ah` zero result, and command `58h`'s forced `FFFFFFFEh` result.\n\nDirect audit also corrected an older Batch 16 boundary model: the call to `1C77C` at `0AD15` receives `(token, 0320h)`, not only `token`; Batch 17's `1Eh` handler likewise calls it as `(slot, 031Dh)`. This correction changes no byte accounting.\n\nThe sequential accounting frontier is now `0C6D7`. Bytes `0C6D7..0C703` remain deliberately unresolved; the next directly decoded routine begins at `0C704`. The older zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` also remain unresolved.\n'''
progress_path.write_text(progress)

addendum += '''\n\n## CODE.2 Batch 17 checkpoint\n\nBatch 17 added **4,009 translated bytes** plus **534 proven non-code/data bytes**. Cumulative accounting is **56,834 translated + 1,009 non-code = 57,843 / 239,698 = 24.1316% (24.13%)**; unresolved is **181,855 bytes**.\n\nThe sequential accounting frontier is `0C6D7`; `0C6D7..0C703` remains deliberately unresolved, while the next directly decoded routine begins at `0C704`. Preserve `0B518`'s rank-indexed stack aliases (including counter/offset/threshold writes), 32-bit two's-complement absolute-value behavior, and the `E284` loop-counter argument. `0B950` owns the fully reconciled 132-entry table at `0B740..0B94F` and preserves its distinct no-selection return values and command-`58h` forced `FFFFFFFEh`. This batch also corrects Batch 16's `1C77C` boundary to preserve its second `0320h` argument; byte accounting is unchanged by that correction.\n'''
addendum_path.write_text(addendum)
