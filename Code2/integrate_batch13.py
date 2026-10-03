from pathlib import Path

cpp_p=Path('Code2/Code2.cpp')
prog_p=Path('Code2/PROGRESS.md')
led_p=Path('Code2/RESIDUAL_LEDGER.md')
cont_p=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md')
inc_p=Path('Code2/batch13.inc')

cpp=cpp_p.read_text()
prog=prog_p.read_text()
led=led_p.read_text()
cont=cont_p.read_text()
inc=inc_p.read_text().strip('\n')

assert 'constexpr u32 kVerifiedTranslatedBytes = 42612u;' in cpp
assert 'constexpr u32 kVerifiedNonCodeBytes = 384u;' in cpp
assert 'function_08cf4' not in cpp
assert 'function_08cf4' in inc and 'function_09418' in inc

cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 42612u;',
                'constexpr u32 kVerifiedTranslatedBytes = 44604u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 384u;',
                'constexpr u32 kVerifiedNonCodeBytes = 448u;',1)
old_assert='''static_assert(kVerifiedTranslatedBytes == 39537u +
              (0x0869Fu-0x080ACu) + (0x0871Bu-0x086C4u) +
              (0x0893Cu-0x0871Cu) + (0x08A7Du-0x0893Cu) +
              (0x08C88u-0x08A80u) + (0x08CD8u-0x08C88u));'''
new_assert='''static_assert(kVerifiedTranslatedBytes == 42612u +
              (0x0906Du-0x08CF4u) + (0x0934Au-0x09070u) +
              (0x09418u-0x09368u) + (0x094DDu-0x09418u));'''
assert old_assert in cpp
cpp=cpp.replace(old_assert,new_assert,1)
marker='\n} // namespace code2\n'
assert cpp.endswith(marker)
cpp=cpp[:-len(marker)]+'\n'+inc+'\n'+marker

prog=prog.replace('object%20accounting-17.94%25-red','object%20accounting-18.80%25-red',1)
prog=prog.replace('**Verified translated instruction bytes:** **42,612**','**Verified translated instruction bytes:** **44,604**',1)
prog=prog.replace('**Proven non-code/data bytes:** **384**','**Proven non-code/data bytes:** **448**',1)
prog=prog.replace('**Reconciled object bytes:** **42,996 / 239,698 = 17.9376% (17.94%)**',
                  '**Reconciled object bytes:** **45,052 / 239,698 = 18.7953% (18.80%)**',1)
prog=prog.replace('**Unresolved object bytes:** **196,702**','**Unresolved object bytes:** **194,646**',1)
prog += '''\n\n## Batch 13 — moving-record collision/effect family\n\nAdded **1,992 verified translated instruction bytes**:\n\n- `08CF4..0906C` — **889 bytes**: world-record collision/effect scanner, owner-derived multipliers, mode-specific type masks, and the type `58h`/`5Ah` special paths.\n- `09070..09349` — **730 bytes**: four-party moving-record hit propagation with the encoded early return for mode-zero types `57h`/`59h`, proximity box, resistance/special-state checks, and signed halving.\n- `09368..09417` — **176 bytes**: seven-way sound selection and distance/visibility gating.\n- `09418..094DC` — **197 bytes**: modal pointer-selected angle/value input with signed `IDIV` by three and single-step 360-degree wrap.\n\nAdded **64 directly proven non-code/data bytes**: the seven-entry table `08CD8..08CF3`, alignment at `0906D..0906F`, alignment at `0934A..0934B`, the seven-entry table `0934C..09367`, and alignment at `094DD..094DF`.\n\nFidelity notes: `08CF4` clears its return result on any geometric candidate even when the later mode filter suppresses the effect; `09070` clears bit 1 and returns for mode-zero types `57h`/`59h` before the proximity test; signed damage halving truncates toward zero; and `09418` performs only one encoded add/subtract wrap after the signed division.\n\nCumulative accounting: **44,604 translated + 448 proven non-code/data = 45,052 / 239,698 = 18.7953% (18.80%)**. Next sequential frontier: **`094E0`**.\n'''

res='''08CD8..08CF3 | table | 28
0906D..0906F | alignment | 3
0934A..0934B | alignment | 2
0934C..09367 | table | 28
094DD..094DF | alignment | 3
'''
marker='<!-- RESIDUAL-RANGES-END -->'
assert marker in led
led=led.replace(marker,res+marker,1)
assert '**Total proven non-code/data:** **384 bytes**.' in led
led=led.replace('**Total proven non-code/data:** **384 bytes**.',
                '**Total proven non-code/data:** **448 bytes**.',1)
led += '''\n\nBatch 13 evidence: `08CD8..08CF3` is the seven-entry CS table consumed by the indirect jump at `08E50`, with targets `08E58`, `08E77`, `08E94`, `08EAD`, `08EC5`, `08EDE`, `08EDE`. `0934C..09367` is the seven-entry CS table consumed at `093AB`, with targets `093B3`, `093BA`, `093CD`, `093CD`, `093C1`, `093CD`, `093C8`. `0906D..0906F` and `094DD..094DF` decode as `LEA EAX,[EAX+0]`; `0934A..0934B` decodes as `MOV EAX,EAX`. A full object-1 direct relative branch/call-target scan found no incoming target to any of these separator bytes. The known zero islands remain unresolved.\n'''

cont += '''\n\n## Batch 13 checkpoint — 08CD8..094DF\n\nBatch 13 adds **1,992 translated bytes** plus **64 directly proven non-code/data bytes**. Cumulative accounting is **44,604 translated + 448 non-code = 45,052 / 239,698 = 18.7953% (18.80%)**; unresolved is **194,646 bytes**.\n\nThe sequential application frontier now reaches `094DC`; `094DD..094DF` is proven alignment and the next routine begins at `094E0`. The two seven-entry CS tables at `08CD8..08CF3` and `0934C..09367` are fully decoded. Preserve these Batch 13 details: geometric contact in `08CF4` clears the result before the later type filter; mode-zero moving types `57h`/`59h` in `09070` clear bit 1 and return before proximity testing; damage halving is signed truncation toward zero; and `09418` uses signed division by three with the encoded single-step angular wrap. The zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''

cpp_p.write_text(cpp.rstrip()+'\n')
prog_p.write_text(prog.rstrip()+'\n')
led_p.write_text(led.rstrip()+'\n')
cont_p.write_text(cont.rstrip()+'\n')
