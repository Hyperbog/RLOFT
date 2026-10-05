from pathlib import Path

cpp=Path('Code2/Code2.cpp')
s=cpp.read_text()
repls=[
('a3.call_2975c(x,y,p,0u,app04_scale15_4(r))','a3.call_2975c(app04_scale15_4(r),x,y,p,0u)',1),
('a3.call_2975c(0u,x,p,0u,app04_scale15_2(r)+9u)','a3.call_2975c(app04_scale15_2(r)+9u,0u,x,p,0u)',2),
('a3.call_2975c(0x180u,x,p,0u,app04_scale15_2(r)+5u)','a3.call_2975c(app04_scale15_2(r)+5u,0x180u,x,p,0u)',2),
('a3.call_2975c(x,0u,p,0u,app04_scale15_2(r)+0x0Cu)','a3.call_2975c(app04_scale15_2(r)+0x0Cu,x,0u,p,0u)',2),
('a3.call_2975c(x,0xF0u,p,0u,app04_scale15_2(r)+0x12u)','a3.call_2975c(app04_scale15_2(r)+0x12u,x,0xF0u,p,0u)',2),
('a3.call_2975c(x,0x138u,p,0u,app04_scale15_2(r)+0x0Eu)','a3.call_2975c(app04_scale15_2(r)+0x0Eu,x,0x138u,p,0u)',1),
]
for old,new,n in repls:
    if s.count(old)!=n: raise SystemExit(f'expected {n} occurrences: {old!r}, got {s.count(old)}')
    s=s.replace(old,new)
cpp.write_text(s)

a=Path('Code2/FINAL_SEMANTIC_AUDIT.md')
t=a.read_text()
t=t.replace('PASS with no new semantic correction.\n\n### `011A8..01226`','A later cdecl-stack-order review in checkpoint 4 found one repeated semantic defect in this already-closed span: the `30CA2h` result is pushed on top of four still-live arguments before `2975Ch`, so that result is **argument 0** to `2975Ch`, not argument 4. The affected scaled-border calls in `01834..01C07` were corrected during checkpoint 4.\n\n### `011A8..01226`',1)
marker='## CODE.1 cross-reference policy\n'
block='''## Checkpoint 4 — `01C4D..02D6F`\n\n**4,403 bytes.** Fresh immutable-object disassembly covered the text editor, border redraw, mouse-driven editor loops, packed-record deletion, status-frame query, cell-label generation, text serialization, three-plane map export, numbered resource load, and the `02D70` data-table boundary. Immutable source hashes matched exactly.\n\nOne repeated cdecl argument-order defect was found while applying the CODE.1 call-stack checklist. At `018B1..018CB` and the analogous `01F94..0203B` border loops, four arguments for `30CA2h` remain on the stack; after `30CA2h` returns, the scaled result is pushed and `2975Ch` is called without first removing those four slots. Therefore the scaled result is `2975Ch` argument 0, followed by the four pre-existing slots. The reconstruction had represented these calls in the opposite order, placing the scaled result in argument 4. All ten affected calls in `01834` and `01F6C` were corrected. This is one defect class with ten call sites.\n\nThe newly closed `01C4D..02D6F` region otherwise passes the semantic audit. Particular checks included signed text-length traversal, CR handling, signed mouse-coordinate bounds, IDIV-based line count, record deletion copy lengths, the 17h status-frame bit predicate, signed `/8` and `/5` label coordinate arithmetic, A..Z / space-A label rollover, CR/LF serialization, overlay precedence in block mode, signed map dimensions, allocation/free paths, numbered filename decimal construction, and stream read ordering. `02D70` begins an address table and is the next independent ownership family.\n\n'''
if marker not in t: raise SystemExit('audit marker missing')
t=t.replace(marker,block+marker,1)
t=t.replace('- Audited object bytes: **7,245 / 239,698 = 3.0226%**.\n- Semantic corrections found so far: **1**.\n- New corrections in checkpoint 3: **0**.\n- Current closed audited frontier: **`01C4C`**.\n- Next frontier: **`01C4D`**.','- Audited object bytes: **11,632 / 239,698 = 4.8528%**.\n- Semantic corrections found so far: **2 defect classes**.\n- New correction in checkpoint 4: **1 repeated cdecl argument-order defect affecting 10 call sites**.\n- Current closed audited frontier: **`02D6F`**.\n- Next frontier: **`02D70`**.',1)
a.write_text(t)

p=Path('Code2/PROGRESS.md')
u=p.read_text()
u=u.replace('semantic%20audit-3.02%25-yellow','semantic%20audit-4.85%25-yellow',1)
u=u.replace('**Semantic audit:** **7,245 / 239,698 = 3.0226%**, closed through object offset `0x01C4C`; **1 semantic correction** found so far.','**Semantic audit:** **11,632 / 239,698 = 4.8528%**, closed through object offset `0x02D6F`; **2 semantic defect classes** found so far.',1)
p.write_text(u)
