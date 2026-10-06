import struct
import unittest

import rloft_anm as anm


class DecoderTests(unittest.TestCase):
    def test_type2_literal_full_frame(self):
        # 320 literal pixels per row encoded as 127 + 127 + 66 byte runs.
        row = bytes([126]) + bytes([1]) * 127
        row += bytes([126]) + bytes([2]) * 127
        row += bytes([65]) + bytes([3]) * 66
        decoded = anm.decode_type2(row * anm.HEIGHT)
        self.assertEqual(len(decoded), anm.FRAME_PIXELS)
        self.assertEqual(decoded[:127], bytes([1]) * 127)
        self.assertEqual(decoded[127:254], bytes([2]) * 127)
        self.assertEqual(decoded[254:320], bytes([3]) * 66)

    def test_type2_repeat_full_frame(self):
        # Repeat op 0x81 emits 128 pixels because count = 0x101 - op.
        row = bytes([0x81, 7, 0x81, 7, 0xC1, 7])
        # 128 + 128 + 64 = 320.
        decoded = anm.decode_type2(row * anm.HEIGHT)
        self.assertEqual(decoded, bytes([7]) * anm.FRAME_PIXELS)

    def test_type3_sparse_delta(self):
        framebuffer = bytearray(anm.FRAME_PIXELS)
        # outer=1, x += 5, groups=1, y += 10, run=3, colors 4/5/6
        payload = struct.pack("<HHHHHBBB", 1, 5, 1, 10, 3, 4, 5, 6)
        anm.apply_type3(framebuffer, payload)
        self.assertEqual(framebuffer[10 * anm.WIDTH + 5], 4)
        self.assertEqual(framebuffer[11 * anm.WIDTH + 5], 5)
        self.assertEqual(framebuffer[12 * anm.WIDTH + 5], 6)

    def test_type8_planar_mapping(self):
        payload = bytearray(anm.RAW_PLANAR_SIZE)
        plane_size = 0x5000
        payload[0] = 10
        payload[plane_size] = 11
        payload[2 * plane_size] = 12
        payload[3 * plane_size] = 13
        framebuffer = bytearray(anm.FRAME_PIXELS)
        anm.apply_type8(framebuffer, payload)
        base = anm.RAW_PLANAR_Y * anm.WIDTH
        self.assertEqual(list(framebuffer[base : base + 4]), [10, 11, 12, 13])

    def test_palette_expansion(self):
        payload = bytes([0, 31, 63]) * 256
        palette = anm.vga_palette_to_rgb(payload)
        self.assertEqual(palette[:3], bytes([0, 125, 255]))


if __name__ == "__main__":
    unittest.main()
