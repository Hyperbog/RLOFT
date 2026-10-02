from pathlib import Path
import re

ledger = Path('Code1/RESIDUAL_LEDGER.md').read_text()
match = re.search(r'<!-- RESIDUAL-RANGES-BEGIN -->\n(.*?)\n<!-- RESIDUAL-RANGES-END -->', ledger, re.S)
assert match, 'missing residual range block'

ranges = []
by_category = {}
for raw in match.group(1).splitlines():
    raw = raw.strip()
    if not raw:
        continue
    m = re.fullmatch(r'([0-9A-F]{5})\.\.([0-9A-F]{5}) \| (alignment|table) \| ([0-9]+)', raw)
    assert m, f'bad residual ledger line: {raw}'
    start, end = int(m.group(1), 16), int(m.group(2), 16)
    category, stated = m.group(3), int(m.group(4))
    assert 0 <= start <= end < 0x11302
    assert end - start + 1 == stated
    ranges.append((start, end + 1, category))
    by_category[category] = by_category.get(category, 0) + stated

ranges.sort()
for previous, current in zip(ranges, ranges[1:]):
    assert previous[1] <= current[0], f'overlap: {previous} vs {current}'

total = sum(end - start for start, end, _ in ranges)
assert len(ranges) == 116, len(ranges)
assert total == 448, total
assert by_category == {'alignment': 330, 'table': 118}, by_category

verified = 69652
denominator = 0x11302
residual = denominator - verified
assert residual == 750
assert residual - total == 302

progress_path = Path('Code1/PROGRESS.md')
progress = progress_path.read_text()
assert 'Current verified translated bytes: **69,652**.' in progress
assert '**Verified conversion:** 98.93%' in progress
progress_section = '''\n\n## 2026-10-02 residual classification pass 21\n\nThe verified translated-code counter remains **69,652 / 70,402 = 98.93%**; no unverified bytes were promoted to translated coverage in this pass. `RESIDUAL_LEDGER.md` now records **448 of the 750 conservative residual bytes** as explicitly established non-code from current repository evidence: **118 bytes of embedded jump/address/lookup tables** and **330 bytes of alignment, zero-fill, NOP/LEA/MOV-self padding**. This leaves **302 bytes** requiring individual classification or verified translation.\n\nThe residual audit also proved that the raw `static_assert` interval gaps are not a coverage ledger: the unified C++ has **64,729 unique bytes represented by those asserts** and **5,673 bytes outside them**, while `kVerifiedTranslatedBytes` is 69,652 because several verified regions are represented without one range assert per machine interval. Do not treat the 5,673-byte diagnostic gap total as untranslated code. The badge stays red and the denominator remains 70,402 until the remaining 302 bytes are resolved without overlap or double-counting.\n'''
if '## 2026-10-02 residual classification pass 21' not in progress:
    progress += progress_section
progress_path.write_text(progress)

continuity_path = Path('Code1/DECOMPILATION_CONTINUITY.md')
continuity = continuity_path.read_text()
continuity_section = '''\n\n## 2026-10-02 final residual-classification checkpoint\n\nVerified translated code remains **69,652 / 70,402 = 98.93%**. The new `Code1/RESIDUAL_LEDGER.md` is the authoritative curated residual-classification ledger for already-established non-code bytes. It contains **116 non-overlapping ranges totaling 448 bytes**: **118 table bytes** plus **330 alignment/zero/padding bytes**. Because the conservative residual before classification is 750 bytes, **302 bytes remain to classify or translate**.\n\nDo not derive remaining coverage from `static_assert` gaps. A repository audit found 64,729 unique bytes enclosed by current range asserts and 5,673 bytes outside them, but many verified regions intentionally lack one assert per machine interval. Use the verified counter plus `RESIDUAL_LEDGER.md`, and investigate the remaining 302 bytes individually. The next priority is to classify undocumented inter-function gaps (especially the small gaps around the already-integrated self-modifying blitters and the F7xx/F8xx resource helpers), then sweep the remaining one-to-three-byte islands. Never promote a gap merely because it resembles compiler alignment; require current direct evidence or preserved authoritative disassembly evidence.\n'''
if '## 2026-10-02 final residual-classification checkpoint' not in continuity:
    continuity += continuity_section
continuity_path.write_text(continuity)

staging_path = Path('Code1/STAGING_LEDGER.md')
staging = staging_path.read_text()
staging_section = '''\n\n## Residual-classification checkpoint\n`RESIDUAL_LEDGER.md` now preserves 448 explicitly classified non-code bytes (118 table + 330 alignment/zero/padding). This is classification only, not translated coverage. Verified code remains 69,652 bytes and 302 conservative residual bytes still require classification or translation. The active staging pool remains zero.\n'''
if '## Residual-classification checkpoint' not in staging:
    staging += staging_section
staging_path.write_text(staging)
