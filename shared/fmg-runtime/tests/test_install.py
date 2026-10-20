import importlib.util
from pathlib import Path
import tempfile
import subprocess
import shutil
import unittest
import xml.etree.ElementTree as ET

spec = importlib.util.spec_from_file_location('installer', Path(__file__).resolve().parents[1] / 'scripts/install_sd.py')
installer = importlib.util.module_from_spec(spec)
spec.loader.exec_module(installer)

class InstallTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        self.sd = self.root / 'SD'
        self.sd.mkdir()
        self.binary = self.root / 'arm'
        header = bytearray(20)
        header[:6] = b'\x7fELF\x01\x01'
        header[18] = 40
        self.binary.write_bytes(header)  # fixture ONLY, never shipped as executable
    def tearDown(self):
        self.tmp.cleanup()
    def test_merge_backup_and_idempotence(self):
        conf = self.sd / 'es_systems.cfg'
        conf.write_text('<systemList><system><name>nes</name></system></systemList>')
        dst = installer.install(self.sd, self.binary, '/sdcard', 'es_systems.cfg')
        installer.install(self.sd, self.binary, '/sdcard', 'es_systems.cfg')
        tree = ET.parse(conf)
        self.assertEqual([x.findtext('name') for x in tree.getroot()], ['nes','fake-mobile-games'])
        self.assertEqual(len(list(self.sd.glob('*.fmg-backup-*'))), 2)
        self.assertEqual(len(list((dst / 'games').glob('*.fmg'))), 7)
        self.assertIn('"%ROM%"',tree.getroot()[1].findtext('command'))
    def test_launcher_and_manifest_whitelist(self):
        dst = installer.install(self.sd, self.binary, '/sdcard')
        binary = dst / 'bin/fake-mobile-games'
        binary.write_text('#!/bin/sh\nprintf \'%s\\n\' "$@" > "$FMG_SAVE_DIR/args"\n')
        binary.chmod(0o755)
        manifest = dst / 'games/boca.fmg'
        result = subprocess.run(['/bin/sh', str(dst/'launch.sh'), str(manifest)], capture_output=True)
        self.assertEqual(result.returncode, 0)
        self.assertEqual((dst/'saves/args').read_text().splitlines(), ['--game','boca'])
        manifest.write_text('boca; touch INJECTED\n')
        result = subprocess.run(['/bin/sh', str(dst/'launch.sh'), str(manifest)], capture_output=True)
        self.assertEqual(result.returncode, 2)
        self.assertFalse((dst/'INJECTED').exists())
    def test_launcher_menu(self):
        dst = installer.install(self.sd, self.binary, '/sdcard')
        binary = dst/'bin/fake-mobile-games'
        binary.write_text('#!/bin/sh\n[ "$#" = 0 ]\n')
        binary.chmod(0o755)
        result = subprocess.run(['/bin/sh',str(dst/'launch.sh')],capture_output=True)
        self.assertEqual(result.returncode,0)
    def test_invalid_binary(self):
        self.binary.write_bytes(b'macOS')
        with self.assertRaises(ValueError): installer.install(self.sd,self.binary,'/sdcard')
    def test_paths(self):
        for path in ['../outside.cfg', '/tmp/outside.cfg']:
            with self.assertRaises(ValueError): installer.install(self.sd,self.binary,'/sdcard',path)
        with self.assertRaises(ValueError): installer.install(self.sd,self.binary,'/sdcard/$(id)')
    def test_bad_xml_before_copy(self):
        (self.sd / 'es.cfg').write_text('<bad>')
        with self.assertRaises(ET.ParseError): installer.install(self.sd,self.binary,'/sdcard','es.cfg')
        self.assertFalse((self.sd / 'fake-mobile-games').exists())
    def test_preserve_save_and_controls(self):
        dst=installer.install(self.sd,self.binary,'/userdata')
        (dst/'saves/game-0.sav').write_text('save')
        (dst/'controls.sh').write_text('custom')
        installer.install(self.sd,self.binary,'/userdata')
        self.assertEqual((dst/'saves/game-0.sav').read_text(),'save')
        self.assertEqual((dst/'controls.sh').read_text(),'custom')

if __name__ == '__main__': unittest.main()
