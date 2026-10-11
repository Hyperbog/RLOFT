"""Pure synthetic DOSDis v1 schema acceptance and provenance regression."""
import copy
import unittest
from unittest.mock import patch

import dosdis_bridge as bridge
import original_cfg


class DosDisBridgeTests(unittest.TestCase):
    def doc(self):
        return {'format':'dosdis-analysis-report','schemaVersion':1,
                'analysisStatus':'candidate-unverified',
                'source':{'sha256':original_cfg.ORIGINALS['code2']['sha256'],
                          'sourceBinaryIncluded':False},
                'instructions':[{'Address':{'Object':1,'Offset':0x100},
                                 'Length':2,'Assembly':'jmp 102h'}],
                'edges':[{'Source':{'Object':1,'Offset':0x100},
                          'Destination':{'Object':1,'Offset':0x102},
                          'Kind':'jump','Evidence':'decoded-flow'}]}

    def test_exact_schema_and_direct_edges(self):
        i,e=bridge.parse_report(self.doc(),'code2',0x3A852)
        self.assertEqual(i[0x100]['Length'],2)
        self.assertEqual(e[0x100],{('jump',0x102)})

    def test_reject_wrong_game_hash(self):
        d=self.doc();d['source']['sha256']='0'*64
        with self.assertRaises(ValueError):bridge.parse_report(d,'code2',0x3A852)

    def test_reject_fake_acceptance_and_attached_binary(self):
        d=self.doc();d['analysisStatus']='DOS_ACCEPTED'
        with self.assertRaises(ValueError):bridge.parse_report(d,'code2',0x3A852)
        d=self.doc();d['source']['sourceBinaryIncluded']=True
        with self.assertRaises(ValueError):bridge.parse_report(d,'code2',0x3A852)

    def test_reject_wrong_object_and_bounds_and_duplicates(self):
        d=self.doc();d['instructions'][0]['Address']['Object']=2
        with self.assertRaises(ValueError):bridge.parse_report(d,'code2',0x3A852)
        d=self.doc();d['instructions'].append(copy.deepcopy(d['instructions'][0]))
        with self.assertRaises(ValueError):bridge.parse_report(d,'code2',0x3A852)
        d=self.doc();d['instructions'][0]['Length']=16
        with self.assertRaises(ValueError):bridge.parse_report(d,'code2',0x3A852)
        d=self.doc();d['edges'][0]['Destination']['Offset']=0x3A852
        with self.assertRaises(ValueError):bridge.parse_report(d,'code2',0x3A852)

    def test_reject_original_mismatch_before_reconciliation(self):
        with self.assertRaises(ValueError):
            bridge.compare(b'NOT AN ORIGINAL','code2',{'schema':'rloft-original-witness-manifest-v1'},self.doc())

    def test_missing_observation_is_unknown_not_conflict(self):
        # Real original bytes used for execution of decode are not needed in this
        # pure-schema suite. The positive optional media path is separate.
        i,e=bridge.parse_report(self.doc(),'code2',0x3A852)
        self.assertNotIn(0x200,i)


if __name__=='__main__':
    unittest.main()
