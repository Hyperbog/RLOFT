"""Bridge tests preserve original-game proof boundaries."""
from __future__ import annotations
import json
import sys
import tempfile
import unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'ResearchInfra'))
from export_workflow_context import export_packet
import index

class BridgeTests(unittest.TestCase):
    def test_real_source_packet_without_original_claim(self):
        # Use actual canonical C++ source names; no DOS original binaries needed.
        if not (ROOT/'Code1/Code1.cpp').exists():
            self.skipTest('canonical source absent in test-only bundle')
        source=index.build(ROOT)
        self.assertTrue(source['functions'])
        key=source['functions'][0]['id']
        r=export_packet(source,ROOT,key,max_chars=1600)
        self.assertFalse(r['original_binary_match'])
        self.assertFalse(r['native_windows_parity'])
        self.assertEqual(r['target'],key)
        self.assertTrue(r['source']['source_sha256'])
        self.assertIn('unresolved',r['static_calls'])

    def test_source_hash_guard_prevents_stale_context(self):
        if not (ROOT/'Code1/Code1.cpp').exists():
            self.skipTest('canonical source absent in test-only bundle')
        source=index.build(ROOT)
        source['sources'][0]['sha256']='0'*64
        # Choose a key from that module, so stale hash must be rejected.
        module=source['sources'][0]['module']
        key=next(f['id'] for f in source['functions'] if f['module']==module)
        with self.assertRaisesRegex(ValueError,'source changed since indexing'):
            export_packet(source,ROOT,key)

if __name__=='__main__':unittest.main()
