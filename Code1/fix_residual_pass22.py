from pathlib import Path
import re

ledger_path = Path('Code1/RESIDUAL_LEDGER.md')
ledger = ledger_path.read_text()
new_ranges = [
    (0x04B17, 0x04B17),
    (0x04C7F, 0x04C7F),
    (0x0522F, 0x0522F),
    (0x0550B, 0x0550B),
    (0x055DB, 0x055DB),
    (0x057DB, 0x057DB),
    (0x06303, 0x06303),
    (0x0782F, 0x0782F),
    (0x07843, 0x07843),
    (0x07B73, 0x07B73),
    (0x07D7F, 0x07D7F),
    (0x07E03, 0x07E03),
    (0x07E43, 0x07E43),
    (0x07E6B, 0x07E6B),
]

block_re = re.compile(r'(<!-- RESIDUAL-RANGES-BEGIN -->\n)(.*?)(\n<!-- RESIDUAL-RANGES-END -->)', re.S)
m = block_re.search(ledger)
assert m
rows = []
for raw in m.group(2).splitlines():
    mm = re.fullmatch(r'([0-9A-F]{5})\.\.([0-9A-F]{5}) \| (alignment|table) \| ([0-9]+)', raw.strip())
    assert mm, raw
    a, b = int(mm.group(1), 16), int(mm.group(2), 16)
    rows.append((a, b, mm.group(3)))
existing = {(a, b) for a, b, _ in rows}
for a, b in new_ranges:
    assert (a, b) not in existing
    rows.append((a, b, 'alignment'))
rows.sort()
for x, y in zip(rows, rows[1:]):
    assert x[1] < y[0], (x, y)
new_block = '\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a, b, cat in rows)
ledger = block_re.sub(lambda mm: mm.group(1) + new_block + mm.group(3), ledger)
ledger = ledger.replace('explicitly classified residual below: **448 bytes**', 'explicitly classified residual below: **462 bytes**')
ledger = ledger.replace('alignment/zero/padding: **330 bytes**', 'alignment/zero/padding: **344 bytes**')
ledger = ledger.replace('residual still requiring classification: **302 bytes**', 'residual still requiring classification: **288 bytes**')
ledger = ledger.replace('Do not add these 448 bytes', 'Do not add these 462 bytes')
ledger = ledger.replace('Completion requires the remaining 302 bytes', 'Completion requires the remaining 288 bytes')
assert len(rows) == 131
assert sum(b-a+1 for a,b,_ in rows) == 462
assert sum(b-a+1 for a,b,c in rows if c=='table') == 118
assert sum(b-a+1 for a,b,c in rows if c=='alignment') == 344
ledger_path.write_text(ledger)

progress_path = Path('Code1/PROGRESS.md')
progress = progress_path.read_text()
section = '''\n\n## 2026-10-02 residual classification pass 22\n\nNo translated-code coverage was added: the verified counter remains **69,652 / 70,402 = 98.93%**. Fourteen one-byte islands already had explicit current-source alignment comments but were absent from `RESIDUAL_LEDGER.md`: `4B17`, `4C7F`, `522F`, `550B`, `55DB`, `57DB`, `6303`, `782F`, `7843`, `7B73`, `7D7F`, `7E03`, `7E43`, and `7E6B`. They are now classified as alignment. The curated non-code ledger therefore increases from **448 to 462 bytes** (118 table + 344 alignment/zero/padding), reducing the conservative unresolved residual from **302 to 288 bytes**. The progress badge remains red because translated coverage is unchanged and the remaining bytes still require proof.\n'''
if '## 2026-10-02 residual classification pass 22' not in progress:
    progress += section
progress_path.write_text(progress)

continuity_path = Path('Code1/DECOMPILATION_CONTINUITY.md')
continuity = continuity_path.read_text()
section = '''\n\n## 2026-10-02 residual pass 22 checkpoint\n\nFourteen current-source one-byte alignment comments were missing from the curated residual ledger and are now recorded: `4B17`, `4C7F`, `522F`, `550B`, `55DB`, `57DB`, `6303`, `782F`, `7843`, `7B73`, `7D7F`, `7E03`, `7E43`, `7E6B`. Classified non-code is now **462 bytes** and the unresolved conservative residual is **288 bytes**. Verified translated code remains **69,652 bytes (98.93%)**. Continue from repository evidence only; do not infer padding for the remaining islands without an explicit current or preserved integration record.\n'''
if '## 2026-10-02 residual pass 22 checkpoint' not in continuity:
    continuity += section
continuity_path.write_text(continuity)

staging_path = Path('Code1/STAGING_LEDGER.md')
staging = staging_path.read_text()
section = '''\n\n## Residual pass 22\nFourteen explicitly commented one-byte alignment islands were added to `RESIDUAL_LEDGER.md`. Classified non-code is 462 bytes; unresolved conservative residual is 288 bytes. Verified translated code remains 69,652 bytes and active staging remains zero.\n'''
if '## Residual pass 22' not in staging:
    staging += section
staging_path.write_text(staging)
