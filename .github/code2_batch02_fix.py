from pathlib import Path
p=Path('Code2/PROGRESS.md')
s=p.read_text()
for old,new in [
    ('**Verified translated instruction bytes:** **8,632**  \n','**Verified translated instruction bytes:** **8,632**\n'),
    ('**Proven non-code/data bytes:** **58**  \n','**Proven non-code/data bytes:** **58**\n'),
    ('**Reconciled object bytes:** **8,690 / 239,698 = 3.6254% (3.63%)**  \n','**Reconciled object bytes:** **8,690 / 239,698 = 3.6254% (3.63%)**\n'),
    ('**Unresolved object bytes:** **231,008**  \n','**Unresolved object bytes:** **231,008**\n'),
]:
    assert old in s
    s=s.replace(old,new,1)
p.write_text(s)
