"""Certified source-range parser and bounded decoder protection regression."""
import unittest
import protected_noncode


class ProtectedNoncodeTests(unittest.TestCase):
    def test_accept_source_classification_and_totals(self):
        raw=b'prefix\n<!-- RESIDUAL-RANGES-BEGIN -->\n00003..00004 | alignment | 2\n0000A..0000C | table | 3\n<!-- RESIDUAL-RANGES-END -->\n'
        j=protected_noncode.parse_ledger(raw,code_size=40,expected_total=5)
        self.assertEqual(j['counts'],{'alignment':2,'table':3,'noncode':0})
        self.assertEqual(j['ranges'][1]['end_exclusive'],13)

    def test_reject_overlaps_or_wrong_declared_counts(self):
        p=b'<!-- RESIDUAL-RANGES-BEGIN -->\n00003..00004 | alignment | 2\n00004..00006 | table | 3\n<!-- RESIDUAL-RANGES-END -->'
        with self.assertRaises(ValueError):
            protected_noncode.parse_ledger(p,code_size=40,expected_total=None)
        with self.assertRaises(ValueError):
            protected_noncode.parse_ledger(p.replace(b'00004..00006',b'00008..0000A'),code_size=40,expected_total=9)

    def test_no_unknown_categories_or_out_of_bounds(self):
        q=b'<!-- RESIDUAL-RANGES-BEGIN -->\n0000B..0000D | unknown | 3\n<!-- RESIDUAL-RANGES-END -->'
        with self.assertRaises(ValueError):
            protected_noncode.parse_ledger(q,code_size=40,expected_total=None)
        with self.assertRaises(ValueError):
            protected_noncode.parse_ledger(q.replace(b'unknown',b'noncode'),code_size=12,expected_total=None)

    def test_duplicate_marker_rejected(self):
        q=b'<!-- RESIDUAL-RANGES-BEGIN -->\n<!-- RESIDUAL-RANGES-BEGIN -->\n'
        with self.assertRaises(ValueError):
            protected_noncode.parse_ledger(q)
