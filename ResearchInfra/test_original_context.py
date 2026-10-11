"""No original media needed; test that combined evidence never implies parity."""
import tempfile
from pathlib import Path
import unittest
import index
import original_cfg
import original_context as context


class OriginalContextTests(unittest.TestCase):
    def test_distinct_source_and_machine_evidence(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp)
            for folder in ('Code1','Code2'):(root/folder).mkdir()
            (root/'Code1'/'Code1.cpp').write_text('void function_7f50(){}')
            (root/'Code2'/'Code2.cpp').write_text('void function_35418(){function_395e2();}')
            src=index.build(root)
            witness={'id':'code2:35418','original_span_sha256':'f'*64,
                     'original_file_sha256':'a'*64,'source_index_matches':['code2:35418'],
                     'flow':{'basic_blocks':[{'start':0x35418}],'indirect_edges':[{'address':1}],
                             'semantic_status':'candidate_static_cfg_not_runtime_reachability'}}
            machine={'schema':original_cfg.SCHEMA,'decoder':'GNU objdump (GNU Binutils) 2.44',
                     'witnesses':[witness]}
            registry={'schema':'rloft-project-registry-v1','primary':[
                {'branch':'code1-decompilation'},{'branch':'demo-v1'},
                {'branch':'disassembler-foundation'},{'branch':'DnD-Classics'}]}
            p=context.packet(machine,src,root,registry,'code2:35418')
            self.assertFalse(p['original_identity_verified_at_packet_generation'])
            self.assertFalse(p['compiler_output_matching'])
            self.assertFalse(p['original_dos_execution_parity'])
            self.assertFalse(p['native_engine_gameplay_parity'])
            self.assertEqual(len(p['cross_project_review_lanes']),4)
            self.assertTrue(all(not x['confirmed_direct_dependency'] for x in p['cross_project_review_lanes']))
            (root/'Code2'/'Code2.cpp').write_text('modified')
            with self.assertRaises(ValueError):context.packet(machine,src,root,registry,'code2:35418')

    def test_mismatched_target_and_registry_fail(self):
        sources={'schema':index.SCHEMA}
        machine={'schema':original_cfg.SCHEMA,'witnesses':[]}
        with self.assertRaises(ValueError):context.packet(machine,sources,Path('.'),{'schema':'wrong'},'code1:00000')
        with self.assertRaises(ValueError):context.packet(machine,sources,Path('.'),{'schema':'rloft-project-registry-v1'},'code1:00000')


if __name__=='__main__':unittest.main()
