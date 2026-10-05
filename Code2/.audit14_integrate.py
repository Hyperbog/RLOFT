from pathlib import Path

progress = Path('Code2/PROGRESS.md')
p = progress.read_text()
p = p.replace('semantic%20audit-27.48%25-yellow', 'semantic%20audit-30.34%25-yellow')
p = p.replace('**Semantic audit:** **65,864 / 239,698 = 27.4779%**, closed through object offset `0x10157`; **6 semantic defect classes** found so far.', '**Semantic audit:** **72,732 / 239,698 = 30.3432%**, closed through object offset `0x11C2B`; **6 semantic defect classes** found so far.')
progress.write_text(p)

audit = Path('Code2/FINAL_SEMANTIC_AUDIT.md')
a = audit.read_text()
section = '''\n\n## Checkpoint 14 — `10158..11C2B`\n\n**6,868 bytes.** Fresh disassembly was regenerated from immutable `CODE.2`; both the full-file and object-1 SHA-256 identities matched the controlling hashes. The mandatory prior-frontier overlap `[0x10118,0x10218)` hashes to `9f3289d1fb1ccc4fe363b6e6dec1d07407875891ae0ce1e4de8e0209d8f67918`. The closed span `10158..11C2B` hashes to `72296e7f72a99824b437bec11a4d35b1cb08c5a43e6a71d019286fc3a3c14983`.\n\n**PASS with no new semantic correction.** The audit applied the established CODE.1 defect checklist to call-slot order, signed/unsigned branches, partial-register effects, truncation, global reloads across calls, and address-distinct control flow. The reconstructed families preserve their encoded widths and branch behavior; no additional semantic drift was established from the bytes.\n\nThe checkpoint closes at the RET at `11C2B`. `11C2C..11C3B` is a four-entry CS dispatch table consumed by the following executable family at `11C3C`; those table bytes are intentionally left to the next ownership-complete semantic checkpoint. Cumulative semantic audit coverage is **72,732 / 239,698 = 30.3432%**. Semantic defect classes remain **6**. Next frontier: **`11C2C`**.\n'''
if '## Checkpoint 14 — `10158..11C2B`' not in a:
    audit.write_text(a.rstrip() + section + '\n')

resume = Path('Code2/RESUME_CHECKPOINT.md')
r = resume.read_text()
section2 = '''\n\n## Semantic audit checkpoint 14 handoff\n- Object accounting remains **239,698 / 239,698**.\n- Semantic audit: **72,732 / 239,698 = 30.3432%**, closed through **`11C2B`**.\n- Semantic defect classes remain **6**; checkpoint 14 found no new defect.\n- Span `10158..11C2B` SHA-256: `72296e7f72a99824b437bec11a4d35b1cb08c5a43e6a71d019286fc3a3c14983`.\n- Mandatory overlap `[0x10118,0x10218)` SHA-256: `9f3289d1fb1ccc4fe363b6e6dec1d07407875891ae0ce1e4de8e0209d8f67918`.\n- Next semantic-audit frontier: **`11C2C`**; `11C2C..11C3B` is the next CS dispatch table and `11C3C` begins executable code.\n'''
if '## Semantic audit checkpoint 14 handoff' not in r:
    resume.write_text(r.rstrip() + section2 + '\n')
