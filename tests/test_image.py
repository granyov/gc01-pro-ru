import importlib.util, struct, unittest
from pathlib import Path
spec=importlib.util.spec_from_file_location('image',Path(__file__).parents[1]/'tools/image.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
class ImageTests(unittest.TestCase):
    def raw(self):
        b=bytearray(256);struct.pack_into('<II',b,0,0x20005000,m.BASE+241);return b
    def test_crc_vectors(self):
        self.assertEqual(m.crc_stm32(bytes(4)),0xc704dd7b)
        self.assertEqual(m.crc_stm32(bytes.fromhex('ffffffff')),0)
    def test_image(self):
        b=m.pack(self.raw());self.assertEqual(len(b),m.SIZE)
        self.assertEqual(struct.unpack_from('<II',b,0x20),(m.STATE,1024))
        self.assertEqual(m.crc_stm32(b[:-4]),struct.unpack_from('<I',b,len(b)-4)[0])
    def test_reject_wrong_target(self):
        for offset,value in [(0,0x20010000),(0,0x20004fff),(4,0x08000101),(4,m.BASE+240),(8,0x08000101)]:
            b=self.raw();struct.pack_into('<I',b,offset,value)
            with self.assertRaises(ValueError):m.pack(b)
    def test_reject_oversize(self):
        with self.assertRaises(ValueError):m.pack(self.raw()+bytes(m.SIZE))
    def test_corruption(self):
        b=m.pack(self.raw());b[250]^=1
        self.assertNotEqual(m.crc_stm32(b[:-4]),struct.unpack_from('<I',b,len(b)-4)[0])
