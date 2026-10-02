from pathlib import Path

# Residual pass 37: close two arithmetic-bounded exclusions without guessing subtype.
#
# Batch 12's preserved 6060..6183 VGA/interrupt span is 292 bytes inclusive.
# Its exact translated assertions are 6060..60B1 (82), 60B4..6100 (77),
# and 6104..6183 (128) = 287 translated bytes.  Therefore the only bytes
# excluded by that original fold are 60B2..60B3 and 6101..6103 (5 bytes).
#
# Batch 17's sprite family A8F4..AE0A is 1,303 bytes inclusive.  Its exact
# translated ranges A8F4..AA1F (300), AA20..AC0E (495), and AC10..AE0A
# (507) total 1,302 bytes, leaving exactly AC0F as the one excluded byte.
# Preserved evidence does not uniquely identify padding-vs-data subtype, so
# all six bytes are conservatively recorded as generic `noncode`.

cpp=Path('Code1/Code1.cpp').read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes=69365u;' in cpp
assert 'static_assert(0x60B2u-0x6060u==82u);' in cpp
assert 'static_assert(0x6101u-0x60B4u==77u);' in cpp
assert 'static_assert(0x6184u-0x6104u==128u);' in cpp
assert 'static_assert(0xAA20u-0xA8F4u==300u);' in cpp
assert 'static_assert(0xAC0Fu-0xAA20u==495u);' in cpp
assert 'static_assert(0xAE0Bu-0xAC10u==507u);' in cpp

ledger_path=Path('Code1/RESIDUAL_LEDGER.md')
ledger=ledger_path.read_text()
assert '- explicitly classified residual below: **721 bytes**' in ledger
assert '  - embedded tables: **122 bytes**' in ledger
assert '  - alignment/zero/padding: **479 bytes**' in ledger
assert '  - proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **120 bytes**' in ledger
assert '- residual still requiring classification: **316 bytes**' in ledger
for r in ('060B2..060B3','06101..06103','0AC0F..0AC0F'):
    assert r not in ledger

anchor='061BF..061BF | alignment | 1\n'
assert anchor in ledger
ledger=ledger.replace(anchor,
    '060B2..060B3 | noncode | 2\n'
    '06101..06103 | noncode | 3\n'+anchor,1)
anchor='0AE5A..0AE5B | alignment | 2\n'
assert anchor in ledger
ledger=ledger.replace(anchor,'0AC0F..0AC0F | noncode | 1\n'+anchor,1)

ledger=ledger.replace('- explicitly classified residual below: **721 bytes**','- explicitly classified residual below: **727 bytes**',1)
ledger=ledger.replace('  - proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **120 bytes**','  - proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **126 bytes**',1)
ledger=ledger.replace('- residual still requiring classification: **316 bytes**','- residual still requiring classification: **310 bytes**',1)
# Repair the stale pass-35 accounting sentence left at the bottom during pass 36.
assert 'Do not add these 713 bytes to `kVerifiedTranslatedBytes`' in ledger
assert 'Completion requires the remaining 324 bytes' in ledger
ledger=ledger.replace('Do not add these 713 bytes to `kVerifiedTranslatedBytes`','Do not add these 727 bytes to `kVerifiedTranslatedBytes`',1)
ledger=ledger.replace('Completion requires the remaining 324 bytes','Completion requires the remaining 310 bytes',1)
ledger_path.write_text(ledger)

section='''\n\n## 2026-10-02 residual classification pass 37\n\nTwo preserved integration folds close six additional residual bytes by exact arithmetic without assuming an alignment subtype. Batch 12's `6060..6183` span contains **292 bytes**; its three exact translated intervals `6060..60B1` (**82**), `60B4..6100` (**77**), and `6104..6183` (**128**) total **287**, proving `60B2..60B3` and `6101..6103` (**5 bytes**) were outside translated instructions. Batch 17's contiguous sprite family `A8F4..AE0A` contains **1,303 bytes**; its exact translated ranges `A8F4..AA1F` (**300**), `AA20..AC0E` (**495**), and `AC10..AE0A` (**507**) total **1,302**, proving `AC0F` is the sole excluded byte. All six are classified conservatively as generic non-code because the preserved repository evidence does not uniquely recover a padding-vs-data subtype. Classified non-code rises from **721** to **727 bytes** and unresolved residual falls from **316** to **310 bytes**. Verified translated code remains **69,365 / 70,402 = 98.53%**; no translated coverage is added or double-counted.\n'''
for name in ('Code1/PROGRESS.md','Code1/STAGING_LEDGER.md','Code1/DECOMPILATION_CONTINUITY.md'):
    p=Path(name); text=p.read_text()
    assert 'residual classification pass 37' not in text.lower()
    p.write_text(text+section)

print('residual pass37: +6 generic non-code bytes; unresolved 310; verified scalar unchanged')
