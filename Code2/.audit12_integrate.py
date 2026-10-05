from pathlib import Path

p = Path('Code2/FINAL_SEMANTIC_AUDIT.md')
s = p.read_text()
block = '''\n\n## Checkpoint 12 — `0E578..0F166`\n\n**3,055 bytes.** Fresh immutable-object disassembly resumed at the exact checkpoint-11 frontier and was extended through the complete routine ending at `0F166`; `0F167` is the next separator/entry family and is intentionally excluded. Span SHA-256: `54d15727c287a998ed5e4d163dce62533489eb5a658c5bc8a1afad926be3a110`. Mandatory overlap `[0x0E538,0x0E638)` SHA-256: `7a9a3da4ebbc8913a710c160374b20b1e24bfb2cb85420d9a1600380db7e4243`. Immutable CODE.2 and Object-1 hashes were revalidated before review.\n\n**PASS with no new semantic correction.** This checkpoint closes the remainder of the reconstruction-batch-20 gameplay/state family rather than stopping at the old accounting boundary inside the final scoring routine. Audit emphasis was on the CODE.1 defect classes most likely to recur here: signed byte/word promotion from the 0x248-byte party records, exact bit tests and byte clears in the `21A12h` state field, stack-slot order across nested helper calls, sentinel `-1` indices, repeated global/table reloads, and the distinction between boolean predicates and helpers returning exact classifier indices. The final `0EF34..0F166` score/priority routine preserves its signed table lookups, optional target-record contribution, `24478h` mode probes, exact `FE44h` classifier values, saved state-bit tests, conditional cap at six, `0x80` penalty, and final `F2D4h(...,46h)` decrement. No normalization of those values to booleans was found in the reconstruction.\n\nCumulative semantic audit is now **61,783 / 239,698 = 25.7754%**, closed through `0F166`, with **6 semantic defect classes** found so far. Next frontier: **`0F167`**.\n'''
if '## Checkpoint 12 — `0E578..0F166`' not in s:
    s += block
    p.write_text(s)

p = Path('Code2/PROGRESS.md')
s = p.read_text()
s = s.replace('semantic%20audit-24.50%25-yellow', 'semantic%20audit-25.78%25-yellow')
s = s.replace('**Semantic audit:** **58,728 / 239,698 = 24.5008%**, closed through object offset `0x0E577`; **6 semantic defect classes** found so far.', '**Semantic audit:** **61,783 / 239,698 = 25.7754%**, closed through object offset `0x0F166`; **6 semantic defect classes** found so far.')
p.write_text(s)

p = Path('Code2/RESUME_CHECKPOINT.md')
s = p.read_text()
block = '''\n\n## Semantic audit checkpoint 12 handoff\n- Object accounting remains **239,698 / 239,698**.\n- Semantic audit: **61,783 / 239,698 = 25.7754%**, closed through **`0F166`**.\n- Semantic defect classes remain **6**; checkpoint 12 found no new defect.\n- Span `0E578..0F166` SHA-256: `54d15727c287a998ed5e4d163dce62533489eb5a658c5bc8a1afad926be3a110`.\n- Mandatory overlap `[0x0E538,0x0E638)` SHA-256: `7a9a3da4ebbc8913a710c160374b20b1e24bfb2cb85420d9a1600380db7e4243`.\n- Next semantic-audit frontier: **`0F167`**.\n'''
if '## Semantic audit checkpoint 12 handoff' not in s:
    s += block
    p.write_text(s)
