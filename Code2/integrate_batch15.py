from pathlib import Path

cpp_p=Path('Code2/Code2.cpp')
prog_p=Path('Code2/PROGRESS.md')
led_p=Path('Code2/RESIDUAL_LEDGER.md')
cont_p=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md')
frag_p=Path('Code2/batch15.inc')

cpp=cpp_p.read_text()
prog=prog_p.read_text()
led=led_p.read_text()
cont=cont_p.read_text()
frag=frag_p.read_text().rstrip()+"\n"

assert 'constexpr u32 kVerifiedTranslatedBytes = 46240u;' in cpp
assert 'constexpr u32 kVerifiedNonCodeBytes = 456u;' in cpp
assert 'static_assert(kVerifiedTranslatedBytes == 44604u +\n              (0x095BDu-0x094E0u) + (0x096E0u-0x095C0u) +\n              (0x0978Du-0x096E0u) + (0x09B4Au-0x09790u));' in cpp
assert 'function_09b4c' not in cpp

cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 46240u;',
                'constexpr u32 kVerifiedTranslatedBytes = 49376u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 456u;',
                'constexpr u32 kVerifiedNonCodeBytes = 464u;',1)
cpp=cpp.replace(
'''static_assert(kVerifiedTranslatedBytes == 44604u +
              (0x095BDu-0x094E0u) + (0x096E0u-0x095C0u) +
              (0x0978Du-0x096E0u) + (0x09B4Au-0x09790u));''',
'''static_assert(kVerifiedTranslatedBytes == 46240u +
              (0x09D8Fu-0x09B4Cu) + (0x0A254u-0x09D90u) +
              (0x0A5BCu-0x0A254u) + (0x0A609u-0x0A5BCu) +
              (0x0A667u-0x0A60Cu) + (0x0A791u-0x0A668u));''',1)
marker='\n} // namespace code2\n'
assert cpp.endswith(marker)
cpp=cpp[:-len(marker)]+'\n'+frag+marker

prog=prog.replace('object%20accounting-19.48%25-red','object%20accounting-20.79%25-red',1)
prog=prog.replace('**Verified translated instruction bytes:** **46,240**','**Verified translated instruction bytes:** **49,376**',1)
prog=prog.replace('**Proven non-code/data bytes:** **456**','**Proven non-code/data bytes:** **464**',1)
prog=prog.replace('**Reconciled object bytes:** **46,696 / 239,698 = 19.4812% (19.48%)**',
                  '**Reconciled object bytes:** **49,840 / 239,698 = 20.7928% (20.79%)**',1)
prog=prog.replace('**Unresolved object bytes:** **193,002**','**Unresolved object bytes:** **189,858**',1)
prog += '''\n\n## Batch 15 — party-effect animation, resolver, and modal apply flow\n\nAdded **3,136 verified translated instruction bytes**:\n\n- `09B4C..09D8E` — **579 bytes**: descriptor-driven party-panel animation, including the all-party mode and visual-5 second draw.\n- `09D90..0A253` — **1,220 bytes**: type-specific effect resolver with signed 16-bit current/max comparisons, message-only failure exits, partial-word status-bit clears, and the encoded `7Bh` recovery path.\n- `0A254..0A5BB` — **872 bytes**: modal four-party target/apply loop preserving its positional eight-argument interface, eligibility tests, optional animation, timer installation, resolver fallback, and input-state restoration.\n- `0A5BC..0A608` — **77 bytes**: save/suppress `D681`/`D682` while dirtying all four party panels.\n- `0A60C..0A666` — **91 bytes**: restore/normalize saved input-state bytes and dirty all four panels.\n- `0A668..0A790` — **297 bytes**: global party animation plus `21A14` bit-20 / `21A6A` timer installation for non-`8000h` party slots.\n\nAdded **8 directly proven alignment bytes** at `09D8F`, `0A609..0A60B`, `0A667`, and `0A791..0A793`. Full-object direct relative branch/call-target scanning found no incoming target to any separator byte.\n\nFidelity notes: `09D90` compares the wrapped 16-bit current/max words as signed 16-bit values; the `7Bh` branch has three distinct message-failure exits and a byte-width `21917` decrement; `0A254` preserves the two-phase input poll, effect-specific target eligibility, and the split between direct status/timer installation and `09D90` fallback.\n\nCumulative accounting: **49,376 translated + 464 proven non-code/data = 49,840 / 239,698 = 20.7928% (20.79%)**. Next sequential frontier: **`0A794`**.\n'''

insert='''09D8F..09D8F | alignment | 1\n0A609..0A60B | alignment | 3\n0A667..0A667 | alignment | 1\n0A791..0A793 | alignment | 3\n'''
end='<!-- RESIDUAL-RANGES-END -->'
assert end in led
led=led.replace(end,insert+end,1)
led=led.replace('**Total proven non-code/data:** **456 bytes**.','**Total proven non-code/data:** **464 bytes**.',1)
led += '''\n\nBatch 15 evidence: `09D8F` and `0A667` decode as explicit `NOP`; `0A609..0A60B` and `0A791..0A793` decode as `LEA EAX,[EAX+0]`. Each lies between directly reconstructed function boundaries, and a full object-1 direct relative branch/call-target scan found no incoming target to any byte in these ranges. The known zero islands remain unresolved.\n'''

cont += '''\n\n## CODE.2 Batch 15 checkpoint\n\nBatch 15 added **3,136 translated bytes** plus **8 proven alignment bytes**. Cumulative accounting is **49,376 translated + 464 non-code = 49,840 / 239,698 = 20.7928% (20.79%)**; unresolved is **189,858 bytes**.\n\nThe sequential frontier now reaches `0A790`; `0A791..0A793` is proven compiler alignment and the next routine begins at `0A794`. Preserve the signed-16 current/max comparison in `09D90`, its distinct `7Bh` message/recovery exits, the positional argument behavior in `0A254`, and the save/normalize semantics of `D681`/`D682`. The zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''

cpp_p.write_text(cpp)
prog_p.write_text(prog.rstrip()+"\n")
led_p.write_text(led.rstrip()+"\n")
cont_p.write_text(cont.rstrip()+"\n")
