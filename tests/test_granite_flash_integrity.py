"""Compile the actual native/default Granite lighting path against unlock states."""
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


def function(path, signature):
    return re.search(re.escape(signature) + r"\n\{.*?\n\}", path.read_text(), re.S).group()


class GraniteFlashIntegrity(unittest.TestCase):
    def test_native_unlock_and_manual_flash_survive_ambient_resume(self):
        overworld = ROOT / 'src/overworld.c'
        native = '\n'.join(function(overworld, signature) for signature in (
            'void SetDefaultFlashLevel(void)', 'void SetFlashLevel(s32 flashLevel)',
            'u8 GetFlashLevel(void)',
        ))
        helper = function(ROOT / 'src/field_specials.c', 'void SetGraniteCaveFlashLevel(void)')
        harness = r'''
#include <assert.h>
typedef int s32;
typedef unsigned char u8;
#define FLAG_SYS_USE_FLASH 1
#define FIELD_MOVE_FLASH 1
static struct { int cave; } gMapHeader;
static struct { u8 flashLevel; } save;
static __typeof__(save) *gSaveBlock1Ptr = &save;
static const int gMaxFlashLevel = 8;
static int unlocked, manuallyUsed;
static int FlagGet(int flag) { return manuallyUsed; }
static int IsFieldMoveUnlocked(int move) { return unlocked; }
'''
        checks = r'''
int main(void) {
    gMapHeader.cave = 1;
    save.flashLevel = 1;
    SetGraniteCaveFlashLevel();
    assert(save.flashLevel == 4);
    unlocked = 1;
    save.flashLevel = 4; /* Old cartridge's stale ambient level on Continue. */
    SetGraniteCaveFlashLevel();
    assert(save.flashLevel == 1);
    SetGraniteCaveFlashLevel();
    assert(save.flashLevel == 1);
    unlocked = 0;
    manuallyUsed = 1;
    SetGraniteCaveFlashLevel();
    assert(save.flashLevel == 1);
    gMapHeader.cave = 0;
    SetGraniteCaveFlashLevel();
    assert(save.flashLevel == 0);
}
'''
        compiler = shutil.which('cc') or shutil.which('clang')
        self.assertIsNotNone(compiler)
        with tempfile.TemporaryDirectory(prefix='ec-granite-flash-') as directory:
            source = Path(directory) / 'check.c'
            binary = Path(directory) / 'check'
            for label, body in (
                ('current', helper),
                ('old_resume_override', 'void SetGraniteCaveFlashLevel(void) { SetFlashLevel(4); }'),
            ):
                source.write_text(harness + native + body + checks)
                subprocess.run([compiler, '-std=gnu11', str(source), '-o', str(binary)], check=True, capture_output=True)
                result = subprocess.run([str(binary)], capture_output=True)
                if label == 'current':
                    self.assertEqual(result.returncode, 0, result.stderr.decode())
                else:
                    self.assertNotEqual(result.returncode, 0, 'old unconditional resume override escaped')

    def test_both_floors_share_helper_and_hole_callback_is_retained(self):
        for floor in ('GraniteCave_B1F', 'GraniteCave_B2F'):
            source = (ROOT / 'data/maps' / floor / 'scripts.inc').read_text()
            self.assertEqual(source.count('special SetGraniteCaveFlashLevel'), 1)
            self.assertNotIn('setflashlevel 4', source)
        first = (ROOT / 'data/maps/GraniteCave_B1F/scripts.inc').read_text()
        self.assertIn('setstepcallback STEP_CB_CRACKED_FLOOR', first)
        self.assertIn('setholewarp MAP_GRANITE_CAVE_B2F', first)


if __name__ == '__main__':
    unittest.main()
