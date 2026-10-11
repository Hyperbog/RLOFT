"""Pure synthetic tests for original machine-to-reconstructed source CALL overlay."""
import unittest
import compare_call_evidence as cmp
import original_cfg
import index


class CallEvidenceTests(unittest.TestCase):
    def test_original_only_and_source_only_are_not_false_defect_claims(self):
        f={'id':'code2:35418','calls':['code2:39589','code2:88888']}
        src={'schema':index.SCHEMA,'functions':[f]}
        w={'id':'code2:35418','source_status':'matched_symbol_name_only',
           'flow':{'edges':[{'source':0x35623,'target':0x395e2,'kind':'call'}],
                   'external_targets':[{'source':0x3562a,'target':0x39589,'kind':'call'}],
                   'indirect_edges':[]}}
        result=cmp.compare_witness(w,src)
        self.assertEqual(result['shared_target_names'],['code2:39589'])
        self.assertEqual(result['original_only_target_names'],['code2:395e2'])
        self.assertEqual(result['cpp_only_target_names'],['code2:88888'])
        self.assertFalse(result['differences_are_defects'])
        all_results=cmp.compare_all({'schema':original_cfg.SCHEMA,
                                     'authority':'authenticated_original_LE_bytes_independent_static_decode',
                                     'witnesses':[w]},src)
        self.assertEqual(all_results['original_direct_call_sites'],2)
        self.assertFalse(all_results['original_dos_parity'])

    def test_missing_or_ambiguous_source_never_gets_invented(self):
        src={'schema':index.SCHEMA,'functions':[]}
        w={'id':'code2:395e2','source_status':'not_indexed_in_source',
           'flow':{'edges':[],'external_targets':[], 'indirect_edges':[{'address':1,'kind':'indirect_call'}]}}
        result=cmp.compare_witness(w,src)
        self.assertEqual(result['status'],'SOURCE_SYMBOL_NOT_UNIQUELY_INDEXED')
        self.assertEqual(result['indirect_original_calls'],[{'address':1,'kind':'indirect_call'}])
        with self.assertRaises(ValueError):
            cmp.compare_all({'schema':'unknown','witnesses':[w]},src)

    def test_reject_stale_original_source_index_and_traversal(self):
        import tempfile
        from pathlib import Path
        from hashlib import sha256
        with tempfile.TemporaryDirectory() as td:
            root=Path(td)
            (root/'Code2').mkdir()
            content=b'original source'
            (root/'Code2'/'Code2.cpp').write_bytes(content)
            indexed={'schema':index.SCHEMA,'sources':[{
                'path':'Code2/Code2.cpp','sha256':sha256(content).hexdigest()}]}
            cmp.verify_source_index_freshness(indexed,root)
            (root/'Code2'/'Code2.cpp').write_bytes(b'changed source')
            with self.assertRaises(ValueError):
                cmp.verify_source_index_freshness(indexed,root)
            indexed['sources'][0]['path']='../CODE.2'
            with self.assertRaises(ValueError):
                cmp.verify_source_index_freshness(indexed,root)


if __name__=='__main__':unittest.main()
