from pathlib import Path

progress = Path('Code2/PROGRESS.md')
p = progress.read_text()
p = p.replace('semantic%20audit-27.48%25-yellow', 'semantic%20audit-31.77%25-yellow')
p = p.replace('semantic%20audit-30.34%25-yellow', 'semantic%20audit-31.77%25-yellow')
p = p.replace('**Semantic audit:** **65,864 / 239,698 = 27.4779%**, closed through object offset `0x10157`; **6 semantic defect classes** found so far.', '**Semantic audit:** **76,152 / 239,698 = 31.7700%**, closed through object offset `0x12987`; **6 semantic defect classes** found so far.')
p = p.replace('**Semantic audit:** **72,732 / 239,698 = 30.3432%**, closed through object offset `0x11C2B`; **6 semantic defect classes** found so far.', '**Semantic audit:** **76,152 / 239,698 = 31.7700%**, closed through object offset `0x12987`; **6 semantic defect classes** found so far.')
progress.write_text(p)

audit = Path('Code2/FINAL_SEMANTIC_AUDIT.md')
a = audit.read_text()
section14 = '''\n\n## Checkpoint 14 — `10158..11C2B`\n\n**6,868 bytes.** Fresh disassembly was regenerated from immutable `CODE.2`; both the full-file and object-1 SHA-256 identities matched the controlling hashes. The mandatory prior-frontier overlap `[0x10118,0x10218)` hashes to `9f3289d1fb1ccc4fe363b6e6dec1d07407875891ae0ce1e4de8e0209d8f67918`. The closed span `10158..11C2B` hashes to `72296e7f72a99824b437bec11a4d35b1cb08c5a43e6a71d019286fc3a3c14983`.\n\n**PASS with no new semantic correction.** The audit applied the established CODE.1 defect checklist to call-slot order, signed/unsigned branches, partial-register effects, truncation, global reloads across calls, and address-distinct control flow. The reconstructed families preserve their encoded widths and branch behavior; no additional semantic drift was established from the bytes.\n\nThe checkpoint closes at the RET at `11C2B`. `11C2C..11C3B` is a four-entry CS dispatch table consumed by the following executable family at `11C3C`. Cumulative semantic audit coverage is **72,732 / 239,698 = 30.3432%**. Semantic defect classes remain **6**.\n'''
section15 = '''\n\n## Checkpoint 15 — `11C2C..12987`\n\n**3,420 bytes.** The mandatory overlap `[0x11BEC,0x11CAC)` hashes to `c75a68863cec721ed8a14ec26f3669f37d4b189ee3ad19dec2e958589a89a523`. Fresh immutable-object bytes for `11C2C..12987` hash to `79ca61ce712932c29a4e06bcc47ddfddea7807114eac8b8705179ef6b6c61326`. The first 16 bytes are the four-entry CS dispatch table used by `11C3C`; executable decoding begins at `11C3C`.\n\n**PASS with no new semantic correction.** `11C3C` retains the encoded grid geometry, same-container rejection, capacity arithmetic, signed actor/container fields, and byte-shift variant semantics. The six-way type dispatch around `12168` remains a CS table rather than fall-through code; the `12180` family preserves its empty-cursor hitbox scan and slot-0F container close/reopen behavior. `12470` preserves the complete `248h` actor-block swap, the separate four-byte panel-state swap, and the three-bit actor-index rewrite in both record families. `12724` preserves the sparse status-bit scan, stopping after fourteen displayed statuses or bit `02000000h`, followed by its two distinct modal wait phases.\n\nHigh-risk checks included signed `MOVSX` actor fields, unsigned byte fields, signed `IDIV` coordinate arithmetic, AL-only shift/update behavior, page-global reloads around opaque calls, and address-distinct wrappers. No CODE.1 defect class reproduced in this span. `12986..12987` is the encoded `MOV EAX,EAX` alignment; `12988` begins the next independent routine. Cumulative semantic audit coverage is **76,152 / 239,698 = 31.7700%**. Semantic defect classes remain **6**. Next frontier: **`12988`**.\n'''
if '## Checkpoint 14 — `10158..11C2B`' not in a:
    a = a.rstrip() + section14
if '## Checkpoint 15 — `11C2C..12987`' not in a:
    a = a.rstrip() + section15
    
audit.write_text(a.rstrip() + '\n')

resume = Path('Code2/RESUME_CHECKPOINT.md')
r = resume.read_text()
section14r = '''\n\n## Semantic audit checkpoint 14 handoff\n- Object accounting remains **239,698 / 239,698**.\n- Semantic audit: **72,732 / 239,698 = 30.3432%**, closed through **`11C2B`**.\n- Semantic defect classes remain **6**; checkpoint 14 found no new defect.\n- Span `10158..11C2B` SHA-256: `72296e7f72a99824b437bec11a4d35b1cb08c5a43e6a71d019286fc3a3c14983`.\n- Mandatory overlap `[0x10118,0x10218)` SHA-256: `9f3289d1fb1ccc4fe363b6e6dec1d07407875891ae0ce1e4de8e0209d8f67918`.\n- Next semantic-audit frontier: **`11C2C`**.\n'''
section15r = '''\n\n## Semantic audit checkpoint 15 handoff\n- Object accounting remains **239,698 / 239,698**.\n- Semantic audit: **76,152 / 239,698 = 31.7700%**, closed through **`12987`**.\n- Semantic defect classes remain **6**; checkpoint 15 found no new defect.\n- Span `11C2C..12987` SHA-256: `79ca61ce712932c29a4e06bcc47ddfddea7807114eac8b8705179ef6b6c61326`.\n- Mandatory overlap `[0x11BEC,0x11CAC)` SHA-256: `c75a68863cec721ed8a14ec26f3669f37d4b189ee3ad19dec2e958589a89a523`.\n- `11C2C..11C3B` is a four-entry CS table; `12986..12987` is `MOV EAX,EAX` alignment.\n- Next semantic-audit frontier: **`12988`**.\n'''
if '## Semantic audit checkpoint 14 handoff' not in r:
    r = r.rstrip() + section14r
if '## Semantic audit checkpoint 15 handoff' not in r:
    r = r.rstrip() + section15r
resume.write_text(r.rstrip() + '\n')
