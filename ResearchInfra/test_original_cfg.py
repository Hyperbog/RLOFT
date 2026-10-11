"""No-media synthetic tests for bounded independent GNU x86 LE CFG evidence."""
from __future__ import annotations
import json
from pathlib import Path
import shutil
import unittest
import original_cfg as cfg


@unittest.skipUnless(shutil.which('objdump'), 'GNU binutils objdump required')
class OriginalCfgTests(unittest.TestCase):
    def test_exact_decoder_and_conditional_graph(self):
        # JE 0x105; NOP; JMP 0x106; INT3; RET
        raw = bytes.fromhex('74 03 90 eb 01 cc c3')
        ins = cfg.decode(raw,0x100)
        self.assertEqual([i['address'] for i in ins], [0x100,0x102,0x103,0x105,0x106])
        graph = cfg.graph(ins,0x100,0x107)
        self.assertEqual(graph['reachable_instruction_count'],5)
        self.assertFalse(graph['conflicts'])
        self.assertIn({'source':0x100,'target':0x105,'kind':'conditional'}, graph['edges'])
        self.assertEqual(len(graph['service_boundaries']),1)
        self.assertFalse(graph['indirect_edges'])

    def test_indirect_jump_cannot_be_invented(self):
        ins=cfg.decode(bytes.fromhex('ff e0 90'),0x200)
        graph=cfg.graph(ins,0x200,0x203)
        self.assertEqual(graph['reachable_instruction_count'],1)
        self.assertEqual(graph['indirect_edges'],[{'address':0x200,'kind':'indirect_jump','operand':'eax'}])
        self.assertEqual(graph['linear_decoded_but_not_reached'],1)

    def test_branch_into_instruction_does_not_claim_valid_block(self):
        ins=cfg.decode(bytes.fromhex('eb ff'),0x300)
        graph=cfg.graph(ins,0x300,0x302)
        self.assertEqual(graph['conflicts'][0]['reason'],'target_not_linear_instruction_boundary')

    def test_direct_external_call_and_no_execution_claim(self):
        ins=cfg.decode(bytes.fromhex('e8 05 00 00 00 c3'),0x400)
        graph=cfg.graph(ins,0x400,0x406)
        self.assertEqual(graph['external_targets'][0],{'source':0x400,'target':0x40A,'kind':'call'})
        self.assertEqual(graph['semantic_status'],'candidate_static_cfg_not_runtime_reachability')

    def test_missing_or_tampered_original_media_rejected(self):
        with self.assertRaises(ValueError):cfg.original_object(b'corrupt', 'code2')
        with self.assertRaises(ValueError):cfg.decode(b'',0x100)
        with self.assertRaises(ValueError):cfg.decode(b'\x90'*16385,0x100)

    def test_manifest_spans_are_unique_and_sha_pinned(self):
        m=json.loads(Path(__file__).with_name('original_witnesses.json').read_text())
        self.assertEqual(m['schema'],'rloft-original-witness-manifest-v1')
        keys=[(w['module'],w['start'],w['end']) for w in m['witnesses']]
        self.assertEqual(len(keys), len(set(keys)))
        self.assertEqual(len(keys),15)
        for w in m['witnesses']:
            self.assertEqual(len(w['sha256']),64)
            self.assertGreater(int(w['end'],16),int(w['start'],16))


if __name__=='__main__':unittest.main()
