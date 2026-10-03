from pathlib import Path

root = Path('Code2')
cpp_p = root / 'Code2.cpp'
prog_p = root / 'PROGRESS.md'
led_p = root / 'RESIDUAL_LEDGER.md'
cont_p = root / 'CODE2_CONTINUITY_ADDENDUM.md'
frag_p = root / 'batch16.inc'

cpp = cpp_p.read_text()
prog = prog_p.read_text()
led = led_p.read_text()
cont = cont_p.read_text()
frag = frag_p.read_text().rstrip() + '\n'

old_counter = 'constexpr u32 kVerifiedTranslatedBytes = 49376u;\nconstexpr u32 kVerifiedNonCodeBytes = 464u;'
new_counter = 'constexpr u32 kVerifiedTranslatedBytes = 52825u;\nconstexpr u32 kVerifiedNonCodeBytes = 475u;'
assert old_counter in cpp
cpp = cpp.replace(old_counter, new_counter, 1)

old_assert = '''static_assert(kVerifiedTranslatedBytes == 46240u +
              (0x09D8Fu-0x09B4Cu) + (0x0A254u-0x09D90u) +
              (0x0A5BCu-0x0A254u) + (0x0A609u-0x0A5BCu) +
              (0x0A667u-0x0A60Cu) + (0x0A791u-0x0A668u));'''
new_assert = '''static_assert(kVerifiedTranslatedBytes == 49376u +
              (0x0A8A0u-0x0A794u) + (0x0AA2Du-0x0A8A0u) +
              (0x0ABC4u-0x0AA30u) + (0x0AC2Cu-0x0ABC4u) +
              (0x0AD24u-0x0AC2Cu) + (0x0AE2Du-0x0AD24u) +
              (0x0AED7u-0x0AE30u) + (0x0B02Cu-0x0AED8u) +
              (0x0B40Eu-0x0B02Cu) + (0x0B516u-0x0B410u));'''
assert old_assert in cpp
cpp = cpp.replace(old_assert, new_assert, 1)

closing = '\n} // namespace code2\n'
assert cpp.endswith(closing)
cpp = cpp[:-len(closing)] + '\n' + frag + closing

repls = {
    'object%20accounting-20.79%25-red': 'object%20accounting-22.24%25-red',
    '**Verified translated instruction bytes:** **49,376**': '**Verified translated instruction bytes:** **52,825**',
    '**Proven non-code/data bytes:** **464**': '**Proven non-code/data bytes:** **475**',
    '**Reconciled object bytes:** **49,840 / 239,698 = 20.7928% (20.79%)**': '**Reconciled object bytes:** **53,300 / 239,698 = 22.2363% (22.24%)**',
    '**Unresolved object bytes:** **189,858**': '**Unresolved object bytes:** **186,398**',
}
for old,new in repls.items():
    assert old in prog
    prog = prog.replace(old,new,1)

prog += '''

## Batch 16 — timed status wrappers, object interactions, and projectile/world-record effects

Added **3,449 verified translated instruction bytes**:

- `0A794..0A89F` — **268 bytes**: one-slot animation wrapper and four-party bit-20 status propagation.
- `0A8A0..0AA2C` — **397 bytes**: timed bit-10 status installation with class-dependent cap and `1202C` result capture.
- `0AA30..0ABC3` — **404 bytes**: timed bit-20 status installation with class-dependent cap and byte-width stat increment.
- `0ABC4..0AC2B` — **104 bytes**: table-coordinate signed remainder helper and object flag set.
- `0AC2C..0AD23` — **248 bytes**: type-1/type-4 object interaction, including temporary bit-2 mutation and the `002A8` dispatch path.
- `0AD24..0AE2C` — **265 bytes**: selected/all-party status and event cancellation with `0871C`, `086C4`, and final `0893C` sweep.
- `0AE30..0AED6` — **167 bytes**: fixed-point probe loop with restoration of the saved global position.
- `0AED8..0B02B` — **340 bytes**: clipped 2x2 nearby-world-record effect processor with signed budget exhaustion.
- `0B02C..0B40D` — **994 bytes**: projectile/moving-record spawner, map collision scan, world-record effects, sound/redraw sequence, and type-wide cleanup.
- `0B410..0B515` — **262 bytes**: world-record to temporary moving-record conversion and exact redraw/page-flip sequence.

Added **11 directly proven alignment bytes** at `0AA2D..0AA2F`, `0AE2D..0AE2F`, `0AED7`, `0B40E..0B40F`, and `0B516..0B517`.

Fidelity notes: `0B02C` restores `C02F` and returns immediately if the first collision-cleared fixed-point position is already beyond the encoded high-word map bounds, thereby skipping its later sound/redraw cleanup. Its world-record scan is inclusive through `258A2`, the `0Bh` moving type has a distinct type-8 decrement/effect path, and guarded records force only the coordinate low words to `8000h`. `0B410` performs exactly two redraw pairs after sound `+21h`, followed by one final `28168` page flip before compacting the temporary record.

Cumulative accounting: **52,825 translated + 475 proven non-code/data = 53,300 / 239,698 = 22.2363% (22.24%)**. Next sequential frontier: **`0B518`**.
'''

marker = '<!-- RESIDUAL-RANGES-END -->'
assert marker in led
new_ranges = '''0AA2D..0AA2F | alignment | 3
0AE2D..0AE2F | alignment | 3
0AED7..0AED7 | alignment | 1
0B40E..0B40F | alignment | 2
0B516..0B517 | alignment | 2
'''
led = led.replace(marker, new_ranges + marker, 1)
assert '**Total proven non-code/data:** **464 bytes**.' in led
led = led.replace('**Total proven non-code/data:** **464 bytes**.', '**Total proven non-code/data:** **475 bytes**.', 1)
led += '''

Batch 16 evidence: `0AA2D..0AA2F` and `0AE2D..0AE2F` decode as compiler `LEA EAX,[EAX+0]` separators, `0AED7` is a one-byte `NOP`, and `0B40E..0B40F` / `0B516..0B517` decode as `MOV EAX,EAX`. A full object-1 direct relative branch/call-target scan found no incoming target to any byte in these separator ranges. The known zero islands remain unresolved.
'''

cont += '''

## CODE.2 Batch 16 checkpoint

Batch 16 added **3,449 translated bytes** plus **11 proven alignment bytes**. Cumulative accounting is **52,825 translated + 475 non-code = 53,300 / 239,698 = 22.2363% (22.24%)**; unresolved is **186,398 bytes**.

The sequential application frontier now reaches `0B515`; `0B516..0B517` is proven alignment and the next routine begins at `0B518`. Preserve the initial out-of-bounds hard return in `0B02C` (it skips the normal sound/redraw/type-cleanup tail), the inclusive world-record scan through `258A2`, the moving-type-`0Bh` special world-record path, and `0B410`'s exact two redraw pairs plus one final page flip after sound `+21h`.
'''

cpp_p.write_text(cpp.rstrip() + '\n')
prog_p.write_text(prog.rstrip() + '\n')
led_p.write_text(led.rstrip() + '\n')
cont_p.write_text(cont.rstrip() + '\n')
