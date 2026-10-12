"""Game-media-free regression for true partially file-backed termination record."""
import unittest
import code2_callback_tables as table


class Code2CallbackTableTests(unittest.TestCase):
    def test_original_termination_target_high_byte_is_bss_zero_fill(self):
        data=bytearray(table.VIRTUAL_DATA_SIZE)
        records=[
            (0x12720,0x3968E),(0x12726,0x3A1B3),
            (0x1272C,0x3A2AD),(0x12732,0x3A16E),
            (0x12738,0x3A228)]
        for off,target in records:
            data[off:off+6]=bytes([0,0x20])+target.to_bytes(4,'little')
        init=table.table_records(bytes(data),4,0x12720,table.PHYSICAL_DATA_SIZE)
        term=table.table_records(bytes(data),1,0x12738,table.PHYSICAL_DATA_SIZE)
        self.assertEqual([r['target'] for r in init],list(table.INIT_TARGETS))
        self.assertEqual([r['file_backed_bytes'] for r in init],[6]*4)
        self.assertEqual(term[0]['target'],0x3A228)
        self.assertEqual((term[0]['file_backed_bytes'],term[0]['zero_filled_bytes']),(5,1))
        self.assertEqual(data[table.PHYSICAL_DATA_SIZE-1],0x03)
        self.assertEqual(data[table.PHYSICAL_DATA_SIZE],0x00)

    def test_wrong_source_media_and_data_geometry_rejected(self):
        with self.assertRaises(ValueError):
            table.reconstruct_data(b'not original CODE2')
        with self.assertRaises(ValueError):
            table.table_records(b'\0'*50,1,0,40)
        with self.assertRaises(ValueError):
            table.table_records(bytes(table.VIRTUAL_DATA_SIZE),0,0,40)


if __name__=='__main__':
    unittest.main()
