import importlib.util
from pathlib import Path
import unittest
import xml.etree.ElementTree as ET
spec = importlib.util.spec_from_file_location('repair', Path(__file__).resolve().parents[1]/'scripts/repair_stock_es.py')
repair = importlib.util.module_from_spec(spec)
spec.loader.exec_module(repair)
class RepairTests(unittest.TestCase):
    def test_preserve_existing_and_idempotent(self):
        data=b'<?xml version="1.0"?><systemList>\n<!-- Preserve -->\n<system><name>nes</name><path>/roms/nes</path></system>\n</systemList>'
        app=ET.fromstring('<system><name>fake-mobile-games</name><path>/sdcard/fake-mobile-games/games</path></system>')
        patched=repair.patch_xml(data,app)
        self.assertEqual(repair.patch_xml(patched,app),patched)
        self.assertIn(b'<!-- Preserve -->',patched)
        self.assertIn(b'<system><name>nes</name><path>/roms/nes</path></system>',patched)
        self.assertEqual(len(ET.fromstring(patched)),2)
    def test_update_only_own_category(self):
        data=b'<systemList><system><name>fake-mobile-games</name><path>old</path></system><system><name>nes</name></system></systemList>'
        app=ET.fromstring('<system><name>fake-mobile-games</name><path>new</path></system>')
        result=ET.fromstring(repair.patch_xml(data,app))
        self.assertEqual([n.findtext('name')for n in result],['nes','fake-mobile-games'])
        self.assertEqual(result[1].findtext('path'),'new')
    def test_invalid_xml(self):
        with self.assertRaises(ET.ParseError):repair.patch_xml(b'not xml',ET.Element('system'))
if __name__=='__main__':unittest.main()
