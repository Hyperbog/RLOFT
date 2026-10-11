"""Deterministic, original-game-free checks for bulk original CFG survey."""
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch
import bulk_cfg
import index


def fixture():
    return {'schema':index.SCHEMA,'sources':[],
            'functions':[
                {'id':'code1:00000','module':'code1','offset':0},
                {'id':'code1:00004#variant1','module':'code1','offset':4},
                {'id':'code1:00004#variant2','module':'code1','offset':4},
                {'id':'code2:00001','module':'code2','offset':1}]}


class BulkSurveyTests(unittest.TestCase):
    def test_partition_and_ambiguous_address(self):
        xs, bad=bulk_cfg.ranges_from_index(fixture(),{'code1':b'\x90'*12,'code2':b'\x90'*8},
                                           span_limit=6,total_limit=50,max_labels=8)
        self.assertFalse(bad)
        self.assertEqual([(x['module'],x['start'],x['end_exclusive']) for x in xs],
                         [('code1',0,4),('code1',4,10),('code2',1,7)])
        self.assertEqual(len(xs[1]['source_ids']),2)
        self.assertTrue(all(not x['source_symbol_is_proven_function_boundary'] for x in xs))
        self.assertTrue(xs[1]['truncated_by_cap'])

    def test_seed_and_byte_caps_refuse_silent_sampling(self):
        with self.assertRaises(ValueError):
            bulk_cfg.ranges_from_index(fixture(),{'code1':b'\0'*12,'code2':b'\0'*8},
                                       span_limit=6,total_limit=9,max_labels=8)
        with self.assertRaises(ValueError):
            bulk_cfg.ranges_from_index(fixture(),{'code1':b'\0'*12,'code2':b'\0'*8},
                                       span_limit=6,total_limit=50,max_labels=2)
        with self.assertRaises(ValueError):
            bulk_cfg.ranges_from_index(fixture(),{'code1':b'\0'*12,'code2':b'\0'*8},
                                       span_limit=1025,total_limit=50,max_labels=8)

    def test_outside_labels_not_fabricated(self):
        d=fixture()
        d['functions'].append({'id':'code2:fffff','module':'code2','offset':0xfffff})
        xs,bad=bulk_cfg.ranges_from_index(d,{'code1':b'\0'*12,'code2':b'\0'*8},
                                         span_limit=6,total_limit=50,max_labels=8)
        self.assertEqual(len(bad),1)
        self.assertEqual(bad[0]['reason'],'source_label_outside_authentic_code_object')
        self.assertEqual(len(xs),3)


    def test_verified_source_noncode_caps_windows_and_excludes_bad_seeds(self):
        protected={'code1':[{'start':2,'end_exclusive':4},
                            {'start':6,'end_exclusive':9}]}
        specs, rejects=bulk_cfg.ranges_from_index(
            fixture(),{'code1':b'\x90'*12,'code2':b'\x90'*8},
            span_limit=6,total_limit=50,max_labels=8,protected=protected)
        # Original C++ function_00000 stops before the known alignment bytes.
        self.assertEqual((specs[0]['start'],specs[0]['end_exclusive']), (0,2))
        self.assertTrue(specs[0]['cut_at_declared_noncode'])
        # Candidate 0x4 is allowed but stops at protected byte 0x6.
        self.assertEqual((specs[1]['start'],specs[1]['end_exclusive']),(4,6))
        self.assertEqual(specs[2]['module'],'code2')
        # Newly asserted seed landing inside source-declared padding is dropped.
        fixture_doc=fixture()
        fixture_doc['functions'].append({'id':'code1:00007','module':'code1','offset':7})
        specs,rejects=bulk_cfg.ranges_from_index(
            fixture_doc,{'code1':b'\x90'*12,'code2':b'\x90'*8},
            span_limit=6,total_limit=50,max_labels=8,protected=protected)
        self.assertEqual([r['reason'] for r in rejects],['source_seed_inside_declared_noncode'])

    def test_invalid_protection_is_not_silently_accepted(self):
        with self.assertRaises(ValueError):
            bulk_cfg.ranges_from_index(
                fixture(),{'code1':b'\x90'*12,'code2':b'\x90'*8},
                span_limit=6,total_limit=50,max_labels=8,
                protected={'code1':[{'start':4,'end_exclusive':7},
                                    {'start':6,'end_exclusive':8}]})

    def test_authentication_failure_precedes_decode(self):
        with tempfile.TemporaryDirectory() as td:
            root=Path(td)
            (root/'CODE.1').write_bytes(b'WRONG')
            (root/'CODE.2').write_bytes(b'WRONG')
            with self.assertRaises(ValueError):
                bulk_cfg.inspect(root,fixture())


if __name__=='__main__':
    unittest.main()
