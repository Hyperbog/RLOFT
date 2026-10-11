"""Synthetic archive safety tests; no historical compiler archive required."""
from __future__ import annotations
import io
from pathlib import Path
import sys
import tarfile
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parent))
from provision_watcom10 import (ARCHIVE_SHA256, ProvisionError, extract_verified,
                                safe_members, verify_archive)


def archive(entries) -> bytes:
    data = io.BytesIO()
    with tarfile.open(fileobj=data, mode='w:gz') as tf:
        for name, content, tag in entries:
            info = tarfile.TarInfo(name)
            if tag == 'dir':
                info.type = tarfile.DIRTYPE
                info.size = 0
                tf.addfile(info)
            elif tag == 'symlink':
                info.type = tarfile.SYMTYPE
                info.linkname = '/outside'
                tf.addfile(info)
            else:
                info.size = len(content)
                tf.addfile(info, io.BytesIO(content))
    return data.getvalue()


class ProvisionTests(unittest.TestCase):
    def read_members(self, entries):
        with tarfile.open(fileobj=io.BytesIO(archive(entries)), mode='r:gz') as tf:
            return safe_members(tf)

    def test_benign_archive_and_safe_directory(self):
        entries = [('bin', b'', 'dir'), ('bin/wcc386', b'fixture', 'file')]
        self.assertEqual(len(self.read_members(entries)), 2)
        with tempfile.TemporaryDirectory() as d:
            p = Path(d)/'fixture.tar.gz'
            p.write_bytes(archive(entries))
            target = Path(d)/'unpacked'
            result = extract_verified(p, target, verified={'sha256': ARCHIVE_SHA256})
            self.assertEqual((target/'bin'/'wcc386').read_bytes(), b'fixture')
            self.assertFalse(result['compiler_executed'])
            with self.assertRaises(FileExistsError):
                extract_verified(p, target, verified={'sha256': ARCHIVE_SHA256})

    def test_real_release_posix_relative_root_and_names(self):
        entries = [('.', b'', 'dir'), ('./binnt', b'', 'dir'),
                   ('./binnt/wcc386.exe', b'MZ-fixture', 'file')]
        self.assertEqual(len(self.read_members(entries)), 2)
        with tempfile.TemporaryDirectory() as d:
            file = Path(d) / 'release.tar.gz'
            file.write_bytes(archive(entries))
            target = Path(d) / 'tools'
            result = extract_verified(file, target, verified={'sha256': ARCHIVE_SHA256})
            self.assertEqual(result['members'], 2)
            self.assertEqual((target / 'binnt' / 'wcc386.exe').read_bytes(), b'MZ-fixture')

    def test_dot_root_file_and_dotdot_aliases_rejected(self):
        with self.assertRaisesRegex(ProvisionError, 'root may only'):
            self.read_members([('.', b'bad', 'file')])
        for name in ('./../evil', './binnt/../wrong', 'binnt/./tool'):
            with self.subTest(name=name):
                with self.assertRaisesRegex(ProvisionError, 'unsafe archive member'):
                    self.read_members([(name,b'x','file')])

    def test_unverified_hash_rejected(self):
        with tempfile.TemporaryDirectory() as d:
            file = Path(d)/'archive.tar.gz'
            file.write_bytes(b'false')
            with self.assertRaisesRegex(ProvisionError, 'size mismatch'):
                verify_archive(file)
            with self.assertRaisesRegex(ProvisionError, 'authenticated'):
                extract_verified(file, Path(d)/'target', verified={})

    def test_reject_directory_traversal(self):
        for path in ('../evil', '/etc/passwd', 'C:/Windows/a', 'a/../b', 'a\\b'):
            with self.subTest(path=path):
                with self.assertRaisesRegex(ProvisionError, 'unsafe archive member'):
                    self.read_members([(path, b'x', 'file')])

    def test_reject_symlinks(self):
        with self.assertRaisesRegex(ProvisionError, 'special archive members'):
            self.read_members([('evil', b'', 'symlink')])

    def test_reject_case_collisions(self):
        with self.assertRaisesRegex(ProvisionError, 'case-colliding'):
            self.read_members([('BIN/WCC386', b'a', 'file'), ('bin/wcc386', b'b', 'file')])

    def test_refuse_too_many_archive_members(self):
        from unittest.mock import patch
        from provision_watcom10 import MAX_FILES
        with patch('provision_watcom10.MAX_FILES', 1):
            with self.assertRaisesRegex(ProvisionError, 'member count'):
                self.read_members([('a', b'a', 'file'), ('b', b'b', 'file')])
        self.assertGreater(MAX_FILES, 1)

    def test_refuse_unpacked_size_cap(self):
        from unittest.mock import patch
        with patch('provision_watcom10.MAX_UNPACKED', 5):
            with self.assertRaisesRegex(ProvisionError, 'unpacked size cap'):
                self.read_members([('a', b'123', 'file'), ('b', b'456', 'file')])


if __name__ == '__main__':
    unittest.main()
