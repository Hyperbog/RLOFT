from pathlib import Path
p=Path('Code2/PROGRESS.md')
s=p.read_text()
old='![Semantic audit](https://img.shields.io/badge/semantic%20audit-not%20started-lightgrey)'
new='![Semantic audit](https://img.shields.io/badge/semantic%20audit-0.28%25-yellow)'
if old not in s:
    raise SystemExit('semantic audit badge target not found')
p.write_text(s.replace(old,new,1))
