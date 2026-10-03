from pathlib import Path
p=Path('Code1/PROGRESS.md')
text=p.read_text()
p.write_text('\n'.join(line.rstrip() for line in text.splitlines())+'\n')
print('normalized CODE.1 progress whitespace for hardening 10o')
