from pathlib import Path

p=Path('Code2/Code2.cpp')
s=p.read_text()
old='''    u32 (*call_30ca2)()=nullptr;\n    void (*call_3975c)(u32,u32,u32,u32,u32)=nullptr;'''
new='''    // At 0F715..0F736 four eventual 2975Ch arguments are already live on\n    // the cdecl stack when 30CA2h is called. Preserve those consumed slots.\n    u32 (*call_30ca2)(u32,u32,u32,u32)=nullptr;\n    void (*call_3975c)(u32,u32,u32,u32,u32)=nullptr;'''
assert old in s
s=s.replace(old,new,1)
old='''        else tile=2u+((b.call_30ca2?b.call_30ca2():0u)*3u>>15u);'''
new='''        else tile=2u+((b.call_30ca2?b.call_30ca2(0x62u,static_cast<u32>(x),3u,2u):0u)*3u>>15u);'''
assert old in s
s=s.replace(old,new,1)
p.write_text(s)

p=Path('Code2/FINAL_SEMANTIC_AUDIT.md')
s=p.read_text()
block='''\n\n## Checkpoint 13 — `0F167..10157`\n\n**4,081 bytes.** Fresh immutable-object disassembly closed the remainder of reconstruction batch 20 plus the complete batch-21 item-description/inventory-burden/character-panel family. Span SHA-256: `38351153807a25bb65b40217cceaf3774d10e27409adc20e7bc2e7e5899e5bf9`. Mandatory overlap `[0x0F127,0x0F227)` SHA-256: `769c85a2b667ab8b09d4c9dc98453f0531181b05d2017163d06c2117887f4ff0`. Immutable CODE.2 and Object-1 hashes were revalidated before review. `0F768..0F78F` remains the proven ten-entry CS table rather than executable fall-through.\n\nOne further instance of the already-known **cdecl live-stack argument defect class** was corrected in `0F66C`. At `0F715..0F71C`, the binary pushes four eventual `2975Ch` arguments (`62h`, x, `3`, `2`) before calling `30CA2h`; those slots remain live during the call. The reconstruction had modeled this `30CA2h` invocation as zero-argument. The boundary and call now preserve the four live stack slots, while the scaled random result is subsequently pushed as argument 0 to `2975Ch`. This is not a new defect class, so the total remains six.\n\nThe rest of the span passes the high-risk audit. The `0F168..0F215` address-distinct constant predicates remain separate; item-table access keeps signed 16-bit item IDs and signed subtype bytes; `0F5F8` preserves its byte/word promotions and table fallbacks; `0F66C` keeps signed centering/division and exact tile-selection order; `0F790` preserves type-specific text construction, signed subtype indexing, half-unit weight formatting, and the `item == 4Ah` exception. Burden/rank helpers `0FD84..0FECF` retain signed item sentinels, signed table-row arithmetic, and the binary's signed burden comparisons. Character-panel rendering preserves partial-register flag tests, signed HP/max-HP arithmetic and IDIV, exact overlay predicates, and the page-display wrappers' signed-word ID plus low-byte mode truncation.\n\nCumulative semantic audit is now **65,864 / 239,698 = 27.4779%**, closed through `10157`, with **6 semantic defect classes** found so far. Next frontier: **`10158`**.\n'''
assert '## Checkpoint 13 — `0F167..10157`' not in s
p.write_text(s+block)

p=Path('Code2/PROGRESS.md')
s=p.read_text().replace('semantic%20audit-25.78%25-yellow','semantic%20audit-27.48%25-yellow').replace('**Semantic audit:** **61,783 / 239,698 = 25.7754%**, closed through object offset `0x0F166`; **6 semantic defect classes** found so far.','**Semantic audit:** **65,864 / 239,698 = 27.4779%**, closed through object offset `0x10157`; **6 semantic defect classes** found so far.')
p.write_text(s)

p=Path('Code2/RESUME_CHECKPOINT.md')
s=p.read_text()
block='''\n\n## Semantic audit checkpoint 13 handoff\n- Object accounting remains **239,698 / 239,698**.\n- Semantic audit: **65,864 / 239,698 = 27.4779%**, closed through **`10157`**.\n- Semantic defect classes remain **6**; checkpoint 13 corrected another instance of the existing cdecl live-stack argument defect in `0F66C`.\n- Span `0F167..10157` SHA-256: `38351153807a25bb65b40217cceaf3774d10e27409adc20e7bc2e7e5899e5bf9`.\n- Mandatory overlap `[0x0F127,0x0F227)` SHA-256: `769c85a2b667ab8b09d4c9dc98453f0531181b05d2017163d06c2117887f4ff0`.\n- Next semantic-audit frontier: **`10158`**.\n'''
assert '## Semantic audit checkpoint 13 handoff' not in s
p.write_text(s+block)
