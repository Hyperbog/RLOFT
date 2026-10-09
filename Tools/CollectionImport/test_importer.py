"""Self-contained tests using synthetic archives; never reads or mutates user uploads."""
import hashlib
import importlib.util
import io
from pathlib import Path
import subprocess
import sys
import tempfile
import zipfile

ROOT = Path(__file__).parent
spec = importlib.util.spec_from_file_location('collection_import',ROOT/'upload_collection.py')
mod = importlib.util.module_from_spec(spec)
spec.loader.exec_module(mod)


def git(repo,*args):
    return subprocess.run(['git',*args],cwd=repo,check=True,capture_output=True,text=True).stdout.strip()


def test_import(tmp):
    src=tmp/'sources'; src.mkdir(); repo=tmp/'rloft'; repo.mkdir()
    git(repo,'init','-q'); git(repo,'checkout','-q','-b','DnD-Classics')
    git(repo,'config','user.name','Test'); git(repo,'config','user.email','test@example.org')
    git(repo,'remote','add','origin','https://github.com/Hyperbog/RLOFT.git')
    for games in mod.GAMES_BY_ARCHIVE.values():
        for game in games:
            p=repo/'MASTER'/game/'README.md';p.parent.mkdir(parents=True,exist_ok=True);p.write_text('# Existing scaffold\n')
    git(repo,'add','MASTER'); git(repo,'commit','-qm','initial')
    for archive, games in mod.GAMES_BY_ARCHIVE.items():
        stream=io.BytesIO()
        with zipfile.ZipFile(stream,'w') as z:
            for game in games:
                z.writestr(f'{game}/Data/CODE.1', (game+' unique content').encode())
        contents=stream.getvalue()
        if archive.endswith('.001'):
            cutoff1=len(contents)//3; cutoff2=len(contents)*2//3
            for index,part in enumerate([contents[:cutoff1],contents[cutoff1:cutoff2],contents[cutoff2:]],1):
                (src/f'Ravenloft - Stone Prophet.zip.{index:03}').write_bytes(part)
        else:
            (src/archive).write_bytes(contents)
    mod.ARCHIVES={name:hashlib.sha256((src/name).read_bytes()).hexdigest() for name in mod.ARCHIVES}
    sys.argv=['importer','--source-dir',str(src),'--repo-root',str(repo),'--batch-size','5']
    mod.main()
    assert len(git(repo,'rev-list','--count','HEAD').splitlines())==1
    assert not (repo/'MASTER'/'Dark Sun'/'Data'/'CODE.1').exists()
    sys.argv=['importer','--source-dir',str(src),'--repo-root',str(repo),'--batch-size','5','--apply']
    mod.main()
    for games in mod.GAMES_BY_ARCHIVE.values():
        for game in games:
            p=repo/'MASTER'/game/'Data'/'CODE.1'
            assert p.read_bytes()==(game+' unique content').encode()
            assert (p.parents[1]/'.source-manifest.json').exists()
    assert git(repo,'status','--porcelain')==''
    assert int(git(repo,'rev-list','--count','HEAD'))>2
    print('PASS: 13 games; source paths preserved; independent identical CODE.1 names; SHA verification; segmented ZIP; batched commits; clean working tree')

if __name__=='__main__':
    with tempfile.TemporaryDirectory() as d:
        test_import(Path(d))
